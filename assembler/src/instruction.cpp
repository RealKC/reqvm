/*
 * MIT License
 *
 * Copyright (c) 2020 Mitca Dumitru
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "instruction.hpp"

#include "../../common/unreachable.hpp"
#include "error_reporting.hpp"

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <magic_enum.hpp>

namespace reqvm {

auto instruction::parse_from(std::string_view line) -> instruction {
    auto first_space_index = line.find_first_of(' ');

    auto opcode = magic_enum::enum_cast<common::opcode>(
        std::string {line.begin() + 1, line.begin() + first_space_index});
    if (not opcode.has_value()) {
        // FIXME: Better error message
        report_to_user(level::error, "The line did not have a valid mnemonic");
        return {};
    }

    switch (number_of_arguments(opcode.value())) {
    case 0:
        return {opcode.value(), {}};
    case 1: {
        auto arg_begin = line.begin() + first_space_index;
        while (std::isspace(*arg_begin)) {
            arg_begin++;
        }

        auto arg_end = [&] {
            auto arg_end_index = line.find_first_of(';');
            if (arg_end_index != std::string_view::npos) {
                while (std::isspace(line[arg_end_index])) {
                    arg_end_index--;
                }
                return line.begin() + arg_end_index;
            }
            auto arg_end_it = line.end() - 1;
            while (std::isspace(*arg_end_it)) {
                arg_end_it--;
            }
            return arg_end_it;
        }();

        std::vector<std::string> arguments;
        arguments.emplace_back(arg_begin, arg_end);

        return {opcode.value(), std::move(arguments)};
    }
    case 2: {
        auto first_arg_begin = line.begin() + first_space_index;
        while (std::isspace(*first_arg_begin)) {
            first_arg_begin++;
        }
        auto comment_begin           = line.find_first_of(';');
        auto inbetween_the_arguments = line.find_first_of(',');
        if (comment_begin < inbetween_the_arguments) {
            report_to_user(level::error, "Opcode needed two arguments");
            return {};
        }

        auto first_argument = std::string {
            first_arg_begin, line.begin() + inbetween_the_arguments - 1};

        inbetween_the_arguments++;   // skip the comma
        while (std::isspace(line[inbetween_the_arguments])) {
            inbetween_the_arguments++;
        }

        auto second_arg_end = [&] {
            auto search_for_arg_end_from = comment_begin;
            if (search_for_arg_end_from == std::string_view::npos) {
                search_for_arg_end_from = line.size() - 1;
            }
            while (std::isspace(line[search_for_arg_end_from])) {
                search_for_arg_end_from--;
            }

            return search_for_arg_end_from;
        }();

        auto second_argument =
            std::string {line.begin() + inbetween_the_arguments,
                         line.begin() + second_arg_end};
        std::vector<std::string> arguments;
        arguments.reserve(2);
        arguments.push_back(first_argument);
        arguments.push_back(second_argument);
        return {opcode.value(), std::move(arguments)};
    }
    default:
        UNREACHABLE("number_of_arguments(opcode) returned invalid value");
    }
}

auto instruction::number_of_arguments(common::opcode op) noexcept
    -> std::uint8_t {
    switch (op) {
        using common::opcode;
    case opcode::noop:
    case opcode::ret:
    case opcode::halt:
        return 0;
    case opcode::pushc:
    case opcode::not_:
    case opcode::pop:
    case opcode::push:
    case opcode::call:
    case opcode::jmp:
    case opcode::jeq:
    case opcode::jneq:
    case opcode::jg:
    case opcode::jgeq:
    case opcode::jl:
    case opcode::jleq:
        return 1;
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
    case opcode::io:
        return 2;

        // default:
        // TOOD: internal assembler error maybe? assert it's not reached?
    }
}
}   // namespace reqvm
