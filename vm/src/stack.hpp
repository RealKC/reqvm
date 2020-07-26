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

#include "registers.hpp"

#include <cstdint>
#include <stdexcept>

namespace reqvm {

class stack_error : public std::runtime_error {
public:
    stack_error(const char* what_arg) : runtime_error(what_arg) {}

    virtual ~stack_error() noexcept = default;
};

class stack final {
public:
    stack() : _storage {new std::uint64_t[1024 * 1024]} {}
    ~stack() noexcept {
        delete[] _storage;
    }

    auto push(std::uint64_t val, registers& regs) -> void;
    auto pop(registers& regs) -> std::uint64_t;

private:
    std::uint64_t* _storage;
};

}   // namespace reqvm
