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
#include "binary_manager.hpp"
#include "flags.hpp"
#include "registers.hpp"
#include "stack.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace reqvm {

class vm final {
public:
    vm() = delete;
    explicit vm(const std::string& binary);
    ~vm() noexcept = default;

    auto run() -> int;

private:
    auto read_preamble() -> void;
    auto cycle(common::opcode op) -> void;

    std::unique_ptr<binary_manager> _binary;
    registers _regs;
    stack _stack;
    flags _flags;
    bool _halted {false};
};

}   // namespace reqvm
