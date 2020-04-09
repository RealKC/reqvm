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
#include "error_reporting.hpp"
#include "logger.hpp"

#include <array>
#include <magic_enum.hpp>

#ifdef AGGRESIVE_LOGGING
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
                if (not reg.has_value()) {
                    // TODO: report error
                    break;
                }
                emit(op, reg.value());
                break;
            }
            case opcode_category::binary_registers: {
                auto regs = get_register_pair(line);
                if (not regs.has_value()) {
                    // TODO: report error
                    break;
                }
                emit(op, regs.value());
                break;
            }
            case opcode_category::binary_byte_then_register: {
                auto byte_and_reg = get_io_op_and_reg(line);
                if (not byte_and_reg.has_value()) {
                    break;
                }
                emit(op, byte_and_reg.value().first,
                     byte_and_reg.value().second);
                break;
            }
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
    _out.write(common::magic_byte_string,
               sizeof(common::magic_byte_string) - 1);
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
    for (auto i = sizeof(common::magic_byte_string) - 10; i < 256; i++) {
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
    auto op = magic_enum::enum_cast<common::opcode>(instruction_name);
    if (not op.has_value()) {
        report_to_user(level::error, instruction_name + " in line '" + line
                                         + "' is not a valid mnemonic");
    }
    return op.value();
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

auto assembler::get_register(const std::string& line)
    -> std::optional<common::registers> {
    LOG1(line);
    auto reg_name_start = line.find_first_of(' ');
    while (not std::isalpha(line[reg_name_start])) {
        reg_name_start++;
    }
    auto reg_name_end = reg_name_start;
    while (std::isalnum(line[reg_name_end])) {
        reg_name_end++;
    }
    return parse_register(
        {line.begin() + reg_name_start, line.begin() + reg_name_end});
}

auto assembler::get_register_pair(const std::string& line)
    -> std::optional<std::pair<common::registers, common::registers>> {
    LOG1(line);
    auto first_reg_start = line.find_first_of(' ');
    while (not std::isalpha(line[first_reg_start])) {
        first_reg_start++;
    }
    auto first_reg_end = line.find_first_of(',');
    while (not std::isalnum(line[first_reg_end])) {
        first_reg_end--;
    }
    auto second_reg_start = line.find_first_of(',');
    while (not std::isalpha(line[second_reg_start])) {
        second_reg_start++;
    }
    auto second_reg_end = second_reg_start;
    while (std::isalnum(line[second_reg_end])) {
        second_reg_end++;
    }
    auto r1 = parse_register(
        {line.begin() + first_reg_start, line.begin() + first_reg_end + 1});
    if (not r1.has_value()) {
        // tODO: help the user or smth
        return {};
    }
    auto r2 = parse_register(
        {line.begin() + second_reg_start, line.begin() + second_reg_end});
    if (not r2.has_value()) {
        // todo: some error maybe
        return {};
    }
    LOG1(r2.value());
    return std::pair {r1.value(), r2.value()};
}

auto assembler::get_io_op_and_reg(const std::string& line)
    -> std::optional<std::pair<common::io_op, common::registers>> {
    LOG1(line);
    auto the_io_op = get_io_op(line);
    if (not the_io_op.has_value()) {
        // Maybe report some proper error?
        return {};
    }
    auto reg_start = line.find_first_of(' ');
    // Find the start of the IO op
    while (not std::isalpha(line[reg_start])) {
        reg_start++;
    }
    reg_start += 4;
    // put8c is the only IO operation to have 5 characters currently
    // FIXME(?): more robust logic or nah?
    reg_start += (the_io_op.value() == common::io_op::put8c);
    while (not std::isalpha(line[reg_start])) {
        reg_start++;
    }
    auto reg_end = reg_start;
    while (std::isalnum(line[reg_end])) {
        reg_end++;
    }
    auto the_register =
        parse_register({line.begin() + reg_start, line.begin() + reg_end});
    if (not the_register.has_value()) {
        // Maybe some proper error handling?
        return {};
    }
    return std::pair {the_io_op.value(), the_register.value()};
}

auto assembler::get_io_op(const std::string& line)
    -> std::optional<common::io_op> {
    LOG1(line);
    auto op_start = line.find_first_of(' ');
    while (!std::isalpha(line[op_start])) {
        op_start++;
    }
    auto op_end = op_start;
    while (std::isalnum(line[op_end])) {
        op_end++;
    }
    auto op     = std::string {line.begin() + op_start, line.begin() + op_end};
    auto the_op = magic_enum::enum_cast<common::io_op>(op);
    if (not the_op.has_value()) {
        report_to_user(level::error, op + " in line '" + line
                                         + "' is not a valid IO operation.");
    }
    return the_op.value();
}

auto assembler::parse_register(const std::string& reg)
    -> std::optional<common::registers> {
    LOG1(reg);
    auto the_register = magic_enum::enum_cast<common::registers>(reg);
    if (not the_register.has_value()) {
        auto err_msg = "'" + reg + "' is not a valid register.";
        report_to_user(level::error, err_msg);
    }
    LOG1(the_register.value());
    return the_register.value();
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
