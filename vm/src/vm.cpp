/*
 *  MIT License
 *
 *  Copyright (c) 2020 Mitca Dumitru
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "vm.hpp"

#include "../../common/preamble.hpp"
#include "exceptions.hpp"
#include "io.hpp"

#include <array>
#include <filesystem>

static inline auto is_version_compatible(std::uint16_t major,
                                         std::uint16_t minor,
                                         std::uint16_t patch) noexcept -> bool {
    using namespace common;
    if (major <= version::major) {
        return true;
    }
    if (minor <= version::minor) {
        return true;
    }
    if (patch <= version::patch) {
        return true;
    }
    return false;
}

namespace reqvm {

vm::vm(const std::string& binary) {
    auto path = std::filesystem::path {binary};
    _binary   = std::move(load_from(path));
}

auto vm::run() -> int {
    read_preamble();
    while (_regs.pc() <= _binary->size() && !_halted) {
        cycle(static_cast<common::opcode>((*_binary)[_regs.pc()]));
    }
    return static_cast<int>(_regs.ire());
}

auto vm::read_preamble() -> void {
    std::size_t i {0};
    for (; i < sizeof(common::magic_byte_string) - 1; i++) {
        if ((*_binary)[i] != common::magic_byte_string[i]) {
            throw preamble_error {preamble_error::kind::nonstandard_mbs};
        }
    }
    std::array<std::uint8_t, 10> version_string;
    for (; i < sizeof(common::magic_byte_string) + 9; i++) {
        version_string[i - (sizeof(common::magic_byte_string) - 1)] =
            (*_binary)[i];
    }
    if (version_string[0] != '!' || version_string[3] != ';'
        || version_string[6] != ';' || version_string[9] != ';') {
        throw preamble_error {preamble_error::kind::bad_version_serialization};
    }
    auto major =
        static_cast<std::uint16_t>(version_string[1]) << 8 | version_string[2];
    auto minor =
        static_cast<std::uint16_t>(version_string[4]) << 8 | version_string[5];
    auto patch =
        static_cast<std::uint16_t>(version_string[7]) << 8 | version_string[8];
    if (not ::is_version_compatible(major, minor, patch)) {
        throw preamble_error {preamble_error::kind::version_too_high};
    }
    // TODO: read ~~the version +~~ :^) the features
    _regs.jump_to(256);
}

auto vm::cycle(common::opcode op) -> void {
#define CHECK_LHS_REG(opcode, reg)                                             \
    do {                                                                       \
        if (registers::is_error_on_lhs((reg))) {                               \
            throw reqvm::invalid_register {                                    \
                "Invalid lhs operand for opcode '" #opcode "': ",              \
                static_cast<common::registers>((*_binary)[_regs.pc() + 1])};   \
        }                                                                      \
    } while (0)

#define MAKE_8_BYTE_VAL(val)                                                   \
    auto val = static_cast<std::uint64_t>((*_binary)[_regs.pc() + 1]) << 56    \
               | static_cast<std::uint64_t>((*_binary)[_regs.pc() + 2]) << 48  \
               | static_cast<std::uint64_t>((*_binary)[_regs.pc() + 3]) << 40  \
               | static_cast<std::uint64_t>((*_binary)[_regs.pc() + 4]) << 32  \
               | static_cast<std::uint64_t>((*_binary)[_regs.pc() + 5]) << 24  \
               | static_cast<std::uint64_t>((*_binary)[_regs.pc() + 6]) << 16  \
               | static_cast<std::uint64_t>((*_binary)[_regs.pc() + 7]) << 8   \
               | static_cast<std::uint64_t>((*_binary)[_regs.pc() + 8])

#define CHECK_AT_LEAST_8_BYTES(opcode)                                         \
    if (_binary->size() - _regs.pc() < 8) {                                    \
        throw bad_argument {                                                   \
            "Opcode '" #opcode                                                 \
            "c' is the last opcode in your binary and after"                   \
            " it there are less than 8 bytes, as such it is not possible to "  \
            "build its argument."};                                            \
    }

    using common::opcode;
    switch (op) {
    case opcode::noop: {
        // do nothing but advance pc
        _regs.advance_pc(1);
        break;
    }
    case opcode::call: {
        CHECK_AT_LEAST_8_BYTES(call);
        MAKE_8_BYTE_VAL(address);
        for (auto& gp : _regs.general_purpose()) {
            gp = 0;
        }
        _stack.push(_regs.pc() + 9, _regs);
        _regs.jump_to(address);
        break;
    }
    case opcode::ret: {
        auto ret_addr = _stack.pop(_regs);
        _regs.jump_to(ret_addr);
        break;
    }
    case opcode::io: {
        using common::io_op;
        switch (io::parse_from_byte((*_binary)[_regs.pc() + 1])) {
        case io_op::getc: {
            auto reg = registers::parse_from_byte((*_binary)[_regs.pc() + 2]);
            if (registers::is_error_on_lhs(reg)) {
                throw invalid_register {
                    "Invalid lhs register for opcode 'io getc':",
                    static_cast<common::registers>((*_binary)[_regs.pc() + 2])};
            }
            _regs[reg] = io::getc();
            break;
        }
        case io_op::putc: {
            auto reg = registers::parse_from_byte((*_binary)[_regs.pc() + 2]);
            io::putc(_regs[reg]);
            break;
        }
        case io_op::put8c: {
            auto reg = registers::parse_from_byte((*_binary)[_regs.pc() + 2]);
            io::put8c(_regs[reg]);
            break;
        }
        case io_op::putn: {
            auto reg = registers::parse_from_byte((*_binary)[_regs.pc() + 2]);
            io::putn(_regs[reg]);
            break;
        }
        }
        _regs.advance_pc(3);
        break;
    }
    case opcode::add: {
        auto r1 = registers::parse_from_byte((*_binary)[_regs.pc() + 1]);
        CHECK_LHS_REG(add, r1);
        auto r2 = registers::parse_from_byte((*_binary)[_regs.pc() + 2]);
        _regs[r1] += _regs[r2];
        _regs.advance_pc(3);
        break;
    }
    case opcode::sub: {
        auto r1 = registers::parse_from_byte((*_binary)[_regs.pc() + 1]);
        CHECK_LHS_REG(sub, r1);
        auto r2 = registers::parse_from_byte((*_binary)[_regs.pc() + 2]);
        _regs[r1] -= _regs[r2];
        _regs.advance_pc(3);
        break;
    }
    case opcode::mul: {
        auto r1 = registers::parse_from_byte((*_binary)[_regs.pc() + 1]);
        CHECK_LHS_REG(mul, r1);
        auto r2 = registers::parse_from_byte((*_binary)[_regs.pc() + 2]);
        _regs[r1] *= _regs[r2];
        _regs.advance_pc(3);
        break;
    }
    case opcode::div: {
        auto r1 = registers::parse_from_byte((*_binary)[_regs.pc() + 1]);
        CHECK_LHS_REG(div, r1);
        auto r2 = registers::parse_from_byte((*_binary)[_regs.pc() + 2]);
        _regs[r1] /= _regs[r2];
        _regs.advance_pc(3);
        break;
    }
    case opcode::mod: {
        auto r1 = registers::parse_from_byte((*_binary)[_regs.pc() + 1]);
        CHECK_LHS_REG(mod, r1);
        auto r2 = registers::parse_from_byte((*_binary)[_regs.pc() + 2]);
        _regs[r1] %= _regs[r2];
        _regs.advance_pc(3);
        break;
    }
    case opcode::and_: {
        auto r1 = registers::parse_from_byte((*_binary)[_regs.pc() + 1]);
        CHECK_LHS_REG(and, r1);
        auto r2 = registers::parse_from_byte((*_binary)[_regs.pc() + 2]);
        _regs[r1] &= _regs[r2];
        _regs.advance_pc(3);
        break;
    }
    case opcode::or_: {
        auto r1 = registers::parse_from_byte((*_binary)[_regs.pc() + 1]);
        CHECK_LHS_REG(or, r1);
        auto r2 = registers::parse_from_byte((*_binary)[_regs.pc() + 2]);
        _regs[r1] |= _regs[r2];
        _regs.advance_pc(3);
        break;
    }
    case opcode::xor_: {
        auto r1 = registers::parse_from_byte((*_binary)[_regs.pc() + 1]);
        CHECK_LHS_REG(xor, r1);
        auto r2 = registers::parse_from_byte((*_binary)[_regs.pc() + 2]);
        _regs[r1] ^= _regs[r2];
        _regs.advance_pc(3);
        break;
    }
    case opcode::not_: {
        auto r1 = registers::parse_from_byte((*_binary)[_regs.pc() + 1]);
        if (registers::is_error_on_lhs(r1)) {
            throw invalid_register {
                "Invalid operand for opcode 'not': ",
                static_cast<common::registers>((*_binary)[_regs.pc() + 1])};
        }
        _regs[r1] = ~_regs[r1];
        _regs.advance_pc(2);
        break;
    }
    case opcode::lshft: {
        auto r1 = registers::parse_from_byte((*_binary)[_regs.pc() + 1]);
        CHECK_LHS_REG(lshft, r1);
        auto r2 = registers::parse_from_byte((*_binary)[_regs.pc() + 2]);
        _regs[r1] <<= _regs[r2];
        _regs.advance_pc(3);
        break;
    }
    case opcode::rshft: {
        auto r1 = registers::parse_from_byte((*_binary)[_regs.pc() + 1]);
        CHECK_LHS_REG(rhsft, r1);
        auto r2 = registers::parse_from_byte((*_binary)[_regs.pc() + 2]);
        _regs[r1] >>= _regs[r2];
        _regs.advance_pc(3);
        break;
    }
    case opcode::push: {
        auto r1 = registers::parse_from_byte((*_binary)[_regs.pc() + 1]);
        _stack.push(_regs[r1], _regs);
        _regs.advance_pc(2);
        break;
    }
    case opcode::pushc: {
        CHECK_AT_LEAST_8_BYTES(pushc);
        MAKE_8_BYTE_VAL(val);
        _stack.push(val, _regs);
        _regs.advance_pc(9);
        break;
    }
    case opcode::pop: {
        auto r1 = registers::parse_from_byte((*_binary)[_regs.pc() + 1]);
        if (registers::is_error_on_lhs(r1)) {
            throw invalid_register {
                "Invalid operand for opcode 'pop': ",
                static_cast<common::registers>((*_binary)[_regs.pc() + 1])};
        }
        _regs[r1] = _stack.pop(_regs);
        _regs.advance_pc(2);
        break;
    }
    case opcode::cmp: {
        auto r1 = registers::parse_from_byte((*_binary)[_regs.pc() + 1]);
        auto r2 = registers::parse_from_byte((*_binary)[_regs.pc() + 2]);

        if (_regs[r1] < _regs[r2]) {
            _flags.cmp_flag = static_cast<std::uint64_t>(flags::cf::less);
        } else if (_regs[r1] > _regs[r2]) {
            _flags.cmp_flag = static_cast<std::uint64_t>(flags::cf::gr);
        } else {
            _flags.cmp_flag = static_cast<std::uint64_t>(flags::cf::eq);
        }

        _regs.advance_pc(3);
        break;
    }
    case opcode::jmp: {
        CHECK_AT_LEAST_8_BYTES(jmp);
        MAKE_8_BYTE_VAL(address);
        _regs.jump_to(address);
        break;
    }
#define U64(x) static_cast<std::uint64_t>(x)

    case opcode::jeq: {
        CHECK_AT_LEAST_8_BYTES(jeq);
        if (_flags.cmp_flag == U64(flags::cf::eq)) {
            MAKE_8_BYTE_VAL(address);
            _regs.jump_to(address);
        } else {
            _regs.advance_pc(9);
        }
        break;
    }
    case opcode::jneq: {
        CHECK_AT_LEAST_8_BYTES(jneq)
        if (_flags.cmp_flag != U64(flags::cf::eq)) {
            MAKE_8_BYTE_VAL(address);
            _regs.jump_to(address);
        } else {
            _regs.advance_pc(9);
        }
        break;
    }
    case opcode::jl: {
        CHECK_AT_LEAST_8_BYTES(jl)
        if (_flags.cmp_flag == U64(flags::cf::less)) {
            MAKE_8_BYTE_VAL(address);
            _regs.jump_to(address);
        } else {
            _regs.advance_pc(9);
        }
        break;
    }
    case opcode::jleq: {
        CHECK_AT_LEAST_8_BYTES(jleq)
        if (_flags.cmp_flag == U64(flags::cf::less)
            || _flags.cmp_flag == U64(flags::cf::eq)) {
            MAKE_8_BYTE_VAL(address);
            _regs.jump_to(address);
        } else {
            _regs.advance_pc(9);
        }
        break;
    }
    case opcode::jg: {
        CHECK_AT_LEAST_8_BYTES(jq)
        if (_flags.cmp_flag == U64(flags::cf::gr)) {
            MAKE_8_BYTE_VAL(address);
            _regs.jump_to(address);
        } else {
            _regs.advance_pc(9);
        }
        break;
    }
    case opcode::jgeq: {
        CHECK_AT_LEAST_8_BYTES(jgeq)
        if (_flags.cmp_flag == U64(flags::cf::gr)
            || _flags.cmp_flag == U64(flags::cf::eq)) {
            MAKE_8_BYTE_VAL(address);
            _regs.jump_to(address);
        } else {
            _regs.advance_pc(9);
        }
        break;
    }
#undef U64
    case opcode::halt: {
        _halted = true;
        break;
    }
    }

#undef CHECK_LHS_REG
#undef MAKE_8_BYTE_VAL
#undef CHECK_AT_LEAST_8_BYTES
}

}   // namespace reqvm
