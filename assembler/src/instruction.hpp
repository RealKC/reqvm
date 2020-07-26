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

#pragma once

#include "../../common/opcodes.hpp"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace reqvm {

class instruction {
public:
    static auto parse_from(std::string_view line) -> instruction;

    ~instruction() noexcept = default;

    auto append_argument(const std::string& arg) -> void {
        _arguments.push_back(arg);
    }

    auto is_invalid() const noexcept { return _invalid; }

private:
    instruction() noexcept : _opcode {common::opcode::noop}, _invalid {true} {}
    instruction(common::opcode op, std::vector<std::string>&& args) noexcept
        : _opcode {op}, _arguments {std::move(args)} {}

    static auto number_of_arguments(common::opcode op) noexcept -> std::uint8_t;

    common::opcode _opcode;
    bool _invalid {false};
    std::vector<std::string> _arguments;
};

}   // namespace reqvm
