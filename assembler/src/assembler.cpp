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

#include "assembler.hpp"

#include "../../common/preamble.hpp"
#include "logger.hpp"

#include <array>

#ifdef AGGRESIVE_LOGGING
#    include <magic_enum.hpp>

using namespace magic_enum::ostream_operators;
#endif

// This is an incredibly dumb hashing functions, we just make the bytes
// of the string be a std::uint64_t instead.
// Not cryptographically secure, but we don't need that.
// A current limitation is that an instruction can't be more than 8 characters
// long, but who needs that anyway?
static constexpr auto hash(const char* str, std::size_t len) -> std::uint64_t {
    if (len > 9) {
        return ~0ull;
    }

    char s[8] = {0};
    for (auto i = std::size_t {0}; i < len; i++) {
        s[i] = str[i];
    }

    return static_cast<std::uint64_t>(s[0]) << 56
           | static_cast<std::uint64_t>(s[1]) << 48
           | static_cast<std::uint64_t>(s[2]) << 40
           | static_cast<std::uint64_t>(s[3]) << 32
           | static_cast<std::uint64_t>(s[4]) << 24
           | static_cast<std::uint64_t>(s[5]) << 16
           | static_cast<std::uint64_t>(s[6]) << 8
           | static_cast<std::uint64_t>(s[7]);
}

static constexpr auto operator""_u64(const char* s, std::size_t l)
    -> std::uint64_t {
    return hash(s, l);
}

namespace reqvm {

assembler::assembler(const std::string& filename) : _file {filename} {
    auto output_filename = std::string {
        filename.begin(), filename.begin() + filename.find_last_of('.')};
    output_filename += ".reqvm";
    _out.open(output_filename, std::ios::binary);
}

auto assembler::run() -> int {
    std::string line;
    write_preamble();
    while (std::getline(_file, line)) {
        switch (line[0]) {
        case '%':
            // TODO: error because preprocessing should be done first
            break;
        case ';':
            // line starts with comment, ignore
            break;
        case '.': {
            auto label = get_label(line);
            if (_labels.find(label) != _labels.end()) {
                if (_labels[label][0] == 0) {
                    _labels[label][0] = _pc;
                }
            } else {
                _labels[label] = std::vector<std::uint64_t> {};
                _labels[label].push_back(_pc);
            }
            break;
        }
        case '\t': {
            if (line[1] == ';') {
                // TODO: add function to consume comments
                break;
            }
            auto op = get_opcode(line);
            switch (get_category(op)) {
            case opcode_category::nullary:
                emit(op);
                break;
            case opcode_category::unary_constant: {
                auto try_parse_num =
                    [](const std::string& line) -> std::uint64_t {
                    // Skip the instruction
                    std::size_t num_start = line.find_first_of(' ');
                    while (!isdigit(line[num_start])) {
                        num_start++;
                    }
                    auto num_candidate =
                        std::string {line.begin() + num_start, line.end()};
                    try {
                        return std::stoull(num_candidate, nullptr);
                    } catch (const std::out_of_range& e) {
                        // error
                        return 0;
                    }
                };
                auto num = try_parse_num(line);
                emit(op, num);
                break;
            }
            case opcode_category::unary_label: {
                std::size_t label_start = line.find_first_of(' ');
                while (!std::isalpha(line[label_start])) {
                    label_start++;
                }
                std::size_t label_end = label_start;
                while (std::isalpha(line[label_start])) {
                    label_end++;
                }
                emit(op, std::string {line.begin() + label_start,
                                      line.begin() + label_end});
                break;
            }
            case opcode_category::unary_register: {
                auto reg = get_register(line);
                emit(op, reg);
                break;
            }
            case opcode_category::binary_registers: {
                auto regs = get_register_pair(line);
                emit(op, regs);
                break;
            }
            default:
                // TODO: add error
                break;
            }
            break;
        }

        default:
            // TODO: Report some error
            break;
        }
    }
    emit(common::opcode::halt);
    return 0;
}

auto assembler::write_preamble() -> void {
    _out.write(common::magic_byte_string, sizeof(common::magic_byte_string));
    std::array<char, 10> version {0};
    version[0] = '!';
    version[3] = ';';
    version[6] = ';';
    version[9] = ';';
    {
        using namespace common;
        version[1] = static_cast<char>(version::major >> 8);
        version[2] = static_cast<char>((version::major << 8) >> 8);
        version[4] = static_cast<char>(version::minor >> 8);
        version[5] = static_cast<char>((version::minor << 8) >> 8);
        version[7] = static_cast<char>(version::patch >> 8);
        version[8] = static_cast<char>((version::patch << 8) >> 8);
    }
    _out.write(version.data(), version.size());

    const auto nullbyte = '\0';
    for (auto i = sizeof(common::magic_byte_string); i < 256; i++) {
        _out.write(&nullbyte, 1);
    }
}

auto assembler::emit(common::opcode op) -> void {
    LOG1(op);
    if (_has_errors) {
        return;
    }
    _pc++;
    const auto as_char = static_cast<char>(op);
    _out.write(&as_char, 1);
}

auto assembler::emit(common::opcode op, common::registers reg) -> void {
    LOG2(op, reg);
    if (_has_errors) {
        return;
    }
    _pc += 2;
    const char chars[] = {static_cast<char>(op), static_cast<char>(reg)};
    _out.write(chars, 2);
}

auto assembler::emit(common::opcode op, std::uint64_t num) -> void {
    LOG2(op, num);
    if (_has_errors) {
        return;
    }
    _pc += 9;
    const auto as_char = static_cast<char>(op);
    _out.write(&as_char, 1);
    const char bytes[] = {static_cast<char>(num >> 56),
                          static_cast<char>((num << 8) >> 56),
                          static_cast<char>((num << 16) >> 56),
                          static_cast<char>((num << 24) >> 56),
                          static_cast<char>((num << 32) >> 56),
                          static_cast<char>((num << 40) >> 56),
                          static_cast<char>((num << 48) >> 56),
                          static_cast<char>((num << 56) >> 56)};
    _out.write(bytes, 8);
}

auto assembler::emit(common::opcode op, std::string label) -> void {
    LOG2(op, label);
    if (_labels.find(label) == _labels.end()) {
        _labels[label].push_back(0);
        _labels[label].push_back(_pc);
        // Defer the rest of the work to the (opcode, uint64) overload
        emit(op, 0);
        return;
    }
    if (_labels[label][0] == 0) {
        _labels[label].push_back(_pc);
        // Defer the rest of the work to the (opcode, uint64) overload
        emit(op, 0);
        return;
    }
    // Defer the rest of the work to the (opcode, uint64) overload
    emit(op, _labels[label][0]);
}

auto assembler::emit(common::opcode op,
                     std::pair<common::registers, common::registers> regs)
    -> void {
    LOG1_NONL(op) /* << */ PAIR(regs);
    if (_has_errors) {
        return;
    }
    _pc += 3;
    const char chars[] = {static_cast<char>(op), static_cast<char>(regs.first),
                          static_cast<char>(regs.second)};
    _out.write(chars, 3);
}

auto assembler::emit(common::opcode op,
                     common::io_op subop,
                     common::registers reg) -> void {
    if (_has_errors) {
        return;
    }
    _pc += 3;
    const char chars[] = {static_cast<char>(op), static_cast<char>(subop),
                          static_cast<char>(reg)};
    _out.write(chars, sizeof(chars));
}

auto assembler::emit_remaining_labels() -> void {
    for (const auto& vecs : _labels) {
        if (vecs.second.size() > 1) {
            auto address = vecs.second[0];
            for (auto hole : vecs.second) {
                _out.seekp(hole);
                const char bytes[] = {static_cast<char>(address >> 56),
                                      static_cast<char>((address << 8) >> 56),
                                      static_cast<char>((address << 16) >> 56),
                                      static_cast<char>((address << 24) >> 56),
                                      static_cast<char>((address << 32) >> 56),
                                      static_cast<char>((address << 40) >> 56),
                                      static_cast<char>((address << 48) >> 56),
                                      static_cast<char>((address << 56) >> 56)};
                _out.write(bytes, 8);
            }
        }
    }
}

auto assembler::get_label(const std::string& line) -> std::string {
    LOG1(line);
    // TODO: reject code like .label    :
    auto label_end = line.find_first_of(':');
    return std::string {line.begin() + 1, line.begin() + label_end};
}

auto assembler::get_opcode(const std::string& line) -> common::opcode {
    LOG1(line);
    auto instruction_name =
        std::string {line.begin() + 1, line.begin() + line.find_first_of(' ')};

    using common::opcode;
    switch (hash(instruction_name.data(), instruction_name.size())) {
    case "noop"_u64:
        return opcode::noop;
    case "call"_u64:
        return opcode::call;
    case "ret"_u64:
        return opcode::ret;
    case "io"_u64:
        return opcode::io;
    case "add"_u64:
        return opcode::add;
    case "sub"_u64:
        return opcode::sub;
    case "mul"_u64:
        return opcode::mul;
    case "div"_u64:
        return opcode::div;
    case "mod"_u64:
        return opcode::mul;
    case "and"_u64:
        return opcode::and_;
    case "or"_u64:
        return opcode::or_;
    case "xor"_u64:
        return opcode::xor_;
    case "not"_u64:
        return opcode::not_;
    case "lshft"_u64:
        return opcode::lshft;
    case "rshft"_u64:
        return opcode::rshft;
    case "push"_u64:
        return opcode::push;
    case "pushc"_u64:
        return opcode::pushc;
    case "pop"_u64:
        return opcode::pop;
    case "cmp"_u64:
        return opcode::cmp;
    case "jmp"_u64:
        return opcode::jmp;
    case "jeq"_u64:
        return opcode::jeq;
    case "jneq"_u64:
        return opcode::jneq;
    case "jl"_u64:
        return opcode::jl;
    case "jleq"_u64:
        return opcode::jleq;
    case "jg"_u64:
        return opcode::jg;
    case "jgeq"_u64:
        return opcode::jgeq;
    case "halt"_u64:
        return opcode::halt;
    default:
        // TODO: show an actually informative error
        break;
    }
}

auto assembler::get_category(common::opcode op) -> opcode_category {
    LOG1(op);
    using common::opcode;
    switch (op) {
    case opcode::noop:
    case opcode::ret:
    case opcode::halt:
        return opcode_category::nullary;
    case opcode::pushc:
        return opcode_category::unary_constant;
    case opcode::not_:
    case opcode::pop:
    case opcode::push:
        return opcode_category::unary_register;
    case opcode::call:
    case opcode::jmp:
    case opcode::jeq:
    case opcode::jneq:
    case opcode::jg:
    case opcode::jgeq:
    case opcode::jl:
    case opcode::jleq:
        return opcode_category::unary_label;
    case opcode::add:
    case opcode::sub:
    case opcode::mul:
    case opcode::div:
    case opcode::mod:
    case opcode::and_:
    case opcode::or_:
    case opcode::xor_:
    case opcode::lshft:
    case opcode::rshft:
    case opcode::cmp:
        return opcode_category::binary_registers;
    case opcode::io:
        return opcode_category::binary_byte_then_register;
        // default:
        // TOOD: internal assembler error maybe? assert it's not reached?
    }
}

auto assembler::get_register(const std::string& line) -> common::registers {
    LOG1(line);
    auto reg_name_start = line.find_first_of(' ');
    while (!std::isalpha(line[reg_name_start])) {
        reg_name_start++;
    }
    auto reg_name_end = reg_name_start;
    while (!std::isalnum(line[reg_name_end])) {
        reg_name_end++;
    }
    return parse_register(
        {line.begin() + reg_name_start, line.begin() + reg_name_end});
}

auto assembler::get_register_pair(const std::string& line)
    -> std::pair<common::registers, common::registers> {
    LOG1(line);
    auto first_reg_start = line.find_first_of(' ');
    while (!std::isalpha(line[first_reg_start])) {
        first_reg_start++;
    }
    auto first_reg_end = line.find_first_of(',');
    while (!std::isalnum(line[first_reg_end])) {
        first_reg_end--;
    }
    auto second_reg_start = line.find_first_of(',');
    while (!std::isalpha(line[second_reg_start])) {
        second_reg_start++;
    }
    auto second_reg_end = second_reg_start;
    while (std::isalnum(line[second_reg_end])) {
        second_reg_end++;
    }
    return {parse_register(
                {line.begin() + first_reg_start, line.begin() + first_reg_end}),
            parse_register({line.begin() + second_reg_start,
                            line.begin() + second_reg_start})};
}

auto assembler::get_io_op(const std::string& line) -> common::io_op {
    LOG1(line);
    auto op_start = line.find_first_of(' ');
    while (!std::isalpha(line[op_start])) {
        op_start++;
    }
    auto op_end = op_start;
    while (std::isalnum(line[op_end])) {
        op_end++;
    }
    auto the_op = std::string {line.begin() + op_start, line.begin() + op_end};
    using common::io_op;
    switch (hash(the_op.data(), the_op.size())) {
    case "getc"_u64:
        return io_op::getc;
    case "putc"_u64:
        return io_op::putc;
    case "put8c"_u64:
        return io_op::put8c;
    case "putn"_u64:
        return io_op::putn;
        // default:
        // TODO: report some errors
    }
}

auto assembler::parse_register(const std::string& reg) -> common::registers {
    LOG1(reg);
    switch (hash(reg.data(), reg.size())) {
    case "sp"_u64:
        return common::registers::sp;
    case "pc"_u64:
        return common::registers::pc;
    case "ire"_u64:
        return common::registers::ire;
#define CASE_GP(n)                                                             \
    case "gp" #n##_u64:                                                        \
        return common::registers::gp##n;
#define CASE_IFA(n)                                                            \
    case "ifa" #n##_u64:                                                       \
        return common::registers::ifa##n;

        // clang-format indents these as if they were statements but they should
        // be indented like cases, force that
        // clang-format off
    CASE_GP(00)
    CASE_GP(01)
    CASE_GP(02)
    CASE_GP(03)
    CASE_GP(04)
    CASE_GP(05)
    CASE_GP(06)
    CASE_GP(07)
    CASE_GP(08)
    CASE_GP(09)
    CASE_GP(10)
    CASE_GP(11)
    CASE_GP(12)
    CASE_GP(13)
    CASE_GP(14)
    CASE_GP(15)
    CASE_GP(16)
    CASE_GP(17)
    CASE_GP(18)
    CASE_GP(19)
    CASE_GP(20)
    CASE_GP(21)
    CASE_GP(22)
    CASE_GP(23)
    CASE_GP(24)
    CASE_GP(25)
    CASE_GP(26)
    CASE_GP(27)
    CASE_GP(28)
    CASE_GP(29)
    CASE_GP(30)
    CASE_GP(31)
    CASE_GP(32)
    CASE_GP(33)
    CASE_GP(34)
    CASE_GP(35)
    CASE_GP(36)
    CASE_GP(37)
    CASE_GP(38)
    CASE_GP(39)
    CASE_GP(40)
    CASE_GP(41)
    CASE_GP(42)
    CASE_GP(43)
    CASE_GP(44)
    CASE_GP(45)
    CASE_GP(46)
    CASE_GP(47)
    CASE_GP(48)
    CASE_GP(49)
    CASE_GP(50)
    CASE_GP(51)
    CASE_GP(52)
    CASE_GP(53)
    CASE_GP(54)
    CASE_GP(55)
    CASE_GP(56)
    CASE_GP(57)
    CASE_GP(58)
    CASE_GP(59)
    CASE_GP(60)
    CASE_GP(61)
    CASE_GP(62)
    CASE_GP(63)
    CASE_IFA(00)
    CASE_IFA(01)
    CASE_IFA(02)
    CASE_IFA(03)
    CASE_IFA(04)
    CASE_IFA(05)
    CASE_IFA(06)
    CASE_IFA(07)
    CASE_IFA(08)
    CASE_IFA(09)
    CASE_IFA(10)
    CASE_IFA(11)
    CASE_IFA(12)
    CASE_IFA(13)
    CASE_IFA(14)
    CASE_IFA(15)
        // clang-format on
#undef CASE_GP
#undef CASE_IFA
    default:
        return common::registers::none;
        // TODO: report an error
    }
}

auto assembler::is_read_only(common::registers reg) noexcept -> bool {
    LOG1(reg);
    using common::registers;
    switch (reg) {
    case registers::sp:
    case registers::pc:
        return true;
    default:
        return false;
    }
}

}   // namespace reqvm
