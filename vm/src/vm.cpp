#include "vm.hpp"

#include "../../common/preamble.hpp"
#include "exceptions.hpp"
#include "io.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>

namespace reqvm {

vm::vm(const std::string& binary) {
    {
        namespace fs = std::filesystem;
        fs::path bin_path {binary};
        _binary.reserve(fs::file_size(bin_path));
    }
    std::ifstream the_binary {binary, std::ios::binary};
    std::uint8_t in;
    while (the_binary >> in) {
        _binary.push_back(in);
    }
}

auto vm::run() -> int {
    read_preamble();
    while (_regs.pc() <= _binary.size() && !_halted) {
        cycle(static_cast<common::opcode>(_binary[_regs.pc()]));
    }
    return static_cast<int>(_regs.ire());
}

auto vm::read_preamble() -> void {
    for (auto i = std::size_t {0}; i < sizeof(common::magic_byte_string); i++) {
        if (_binary[i] != common::magic_byte_string[i]) {
            throw preamble_error {};
        }
    }
    // read the version + the features
    _regs.jump_to(256);
}

auto vm::cycle(common::opcode op) -> void {
#define CHECK_LHS_REG(opcode, reg)                                             \
    do {                                                                       \
        if (registers::is_error_on_lhs((reg))) {                               \
            throw reqvm::invalid_register {                                    \
                "Invalid lhs operand for opcode '" #opcode "': ",              \
                static_cast<common::registers>(_binary[_regs.pc() + 1])};      \
        }                                                                      \
    } while (0)

#define MAKE_8_BYTE_VAL(val)                                                   \
    auto val = static_cast<std::uint64_t>(_binary[_regs.pc() + 1]) << 56       \
               | static_cast<std::uint64_t>(_binary[_regs.pc() + 2]) << 48     \
               | static_cast<std::uint64_t>(_binary[_regs.pc() + 3]) << 40     \
               | static_cast<std::uint64_t>(_binary[_regs.pc() + 4]) << 32     \
               | static_cast<std::uint64_t>(_binary[_regs.pc() + 5]) << 24     \
               | static_cast<std::uint64_t>(_binary[_regs.pc() + 6]) << 16     \
               | static_cast<std::uint64_t>(_binary[_regs.pc() + 7]) << 8      \
               | static_cast<std::uint64_t>(_binary[_regs.pc() + 8])

#define CHECK_AT_LEAST_8_BYTES(opcode)                                         \
    if (_binary.size() - _regs.pc() < 8) {                                     \
        throw bad_argument {                                                   \
            "Opcode '" #opcode                                                 \
            "c' is the last opcode in your binary and after"                   \
            " it there are less than 8 bytes, as such it is not possible to "  \
            "build its argument."};                                            \
    }

    using common::opcode;
    switch (op) {
    case opcode::noop: {
        // do nothing
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
        switch (io::parse_from_byte(_binary[_regs.pc() + 1])) {
        case io_op::getc: {
            auto reg = registers::parse_from_byte(_binary[_regs.pc() + 2]);
            if (registers::is_error_on_lhs(reg)) {
                throw invalid_register {
                    "Invalid lhs register for opcode 'io getc':",
                    static_cast<common::registers>(_binary[_regs.pc() + 2])};
            }
            _regs[reg] = io::getc();
            break;
        }
        case io_op::putc: {
            auto reg = registers::parse_from_byte(_binary[_regs.pc() + 2]);
            io::putc(_regs[reg]);
            break;
        }
        case io_op::put8c: {
            auto reg = registers::parse_from_byte(_binary[_regs.pc() + 2]);
            io::put8c(_regs[reg]);
            break;
        }
        case io_op::putn: {
            auto reg = registers::parse_from_byte(_binary[_regs.pc() + 2]);
            io::putn(_regs[reg]);
            break;
        }
        }
        break;
    }
    case opcode::add: {
        auto r1 = registers::parse_from_byte(_binary[_regs.pc() + 1]);
        CHECK_LHS_REG(add, r1);
        auto r2 = registers::parse_from_byte(_binary[_regs.pc() + 2]);
        _regs[r1] += _regs[r2];
        _regs.advance_pc(3);
        break;
    }
    case opcode::sub: {
        auto r1 = registers::parse_from_byte(_binary[_regs.pc() + 1]);
        CHECK_LHS_REG(sub, r1);
        auto r2 = registers::parse_from_byte(_binary[_regs.pc() + 2]);
        _regs[r1] -= _regs[r2];
        _regs.advance_pc(3);
        break;
    }
    case opcode::mul: {
        auto r1 = registers::parse_from_byte(_binary[_regs.pc() + 1]);
        CHECK_LHS_REG(mul, r1);
        auto r2 = registers::parse_from_byte(_binary[_regs.pc() + 2]);
        _regs[r1] *= _regs[r2];
        _regs.advance_pc(3);
        break;
    }
    case opcode::div: {
        auto r1 = registers::parse_from_byte(_binary[_regs.pc() + 1]);
        CHECK_LHS_REG(div, r1);
        auto r2 = registers::parse_from_byte(_binary[_regs.pc() + 2]);
        _regs[r1] /= _regs[r2];
        _regs.advance_pc(3);
        break;
    }
    case opcode::mod: {
        auto r1 = registers::parse_from_byte(_binary[_regs.pc() + 1]);
        CHECK_LHS_REG(mod, r1);
        auto r2 = registers::parse_from_byte(_binary[_regs.pc() + 2]);
        _regs[r1] %= _regs[r2];
        _regs.advance_pc(3);
        break;
    }
    case opcode::and_: {
        auto r1 = registers::parse_from_byte(_binary[_regs.pc() + 1]);
        CHECK_LHS_REG(and, r1);
        auto r2 = registers::parse_from_byte(_binary[_regs.pc() + 2]);
        _regs[r1] &= _regs[r2];
        _regs.advance_pc(3);
        break;
    }
    case opcode::or_: {
        auto r1 = registers::parse_from_byte(_binary[_regs.pc() + 1]);
        CHECK_LHS_REG(or, r1);
        auto r2 = registers::parse_from_byte(_binary[_regs.pc() + 2]);
        _regs[r1] |= _regs[r2];
        _regs.advance_pc(3);
        break;
    }
    case opcode::xor_: {
        auto r1 = registers::parse_from_byte(_binary[_regs.pc() + 1]);
        CHECK_LHS_REG(xor, r1);
        auto r2 = registers::parse_from_byte(_binary[_regs.pc() + 2]);
        _regs[r1] ^= _regs[r2];
        _regs.advance_pc(3);
        break;
    }
    case opcode::not_: {
        auto r1 = registers::parse_from_byte(_binary[_regs.pc() + 1]);
        if (registers::is_error_on_lhs(r1)) {
            throw invalid_register {
                "Invalid operand for opcode 'not': ",
                static_cast<common::registers>(_binary[_regs.pc() + 1])};
        }
        _regs[r1] = ~_regs[r1];
        _regs.advance_pc(2);
        break;
    }
    case opcode::lshft: {
        auto r1 = registers::parse_from_byte(_binary[_regs.pc() + 1]);
        CHECK_LHS_REG(lshft, r1);
        auto r2 = registers::parse_from_byte(_binary[_regs.pc() + 2]);
        _regs[r1] <<= _regs[r2];
        _regs.advance_pc(3);
        break;
    }
    case opcode::rshft: {
        auto r1 = registers::parse_from_byte(_binary[_regs.pc() + 1]);
        CHECK_LHS_REG(rhsft, r1);
        auto r2 = registers::parse_from_byte(_binary[_regs.pc() + 2]);
        _regs[r1] >>= _regs[r2];
        _regs.advance_pc(3);
        break;
    }
    case opcode::push: {
        auto r1 = registers::parse_from_byte(_binary[_regs.pc() + 1]);
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
        auto r1 = registers::parse_from_byte(_binary[_regs.pc() + 1]);
        if (registers::is_error_on_lhs(r1)) {
            throw invalid_register {
                "Invalid operand for opcode 'pop': ",
                static_cast<common::registers>(_binary[_regs.pc() + 1])};
        }
        _regs[r1] = _stack.pop(_regs);
        _regs.advance_pc(2);
        break;
    }
    case opcode::cmp: {
        auto r1 = registers::parse_from_byte(_binary[_regs.pc() + 1]);
        auto r2 = registers::parse_from_byte(_binary[_regs.pc() + 2]);

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
        // TODO: check the condition for jumping
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
        // TODO: check the condition for jumping
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
        // TODO: check the condition for jumping
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
        // TODO: check the condition for jumping
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
        // TODO: check the condition for jumping
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
        // TODO: check the condition for jumping
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
