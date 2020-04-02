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
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
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
#include <stdexcept>

namespace reqvm {
namespace io {

common::io_op parse_from_byte(std::uint8_t byte);

auto getc() -> std::uint64_t;
auto putc(std::uint64_t ch) -> void;
auto put8c(std::uint64_t chars) -> void;
auto putn(std::uint64_t num) -> void;

class error : public std::runtime_error {
public:
    // Delete default constructor to force a meaningful message
    error() = delete;
    error(const char* what_arg, std::uint8_t byte)
        : runtime_error {what_arg}, _op {byte} {};
    virtual ~error() noexcept = default;

    auto the_invalid_op() const noexcept -> std::uint64_t {
        return _op;
    }

private:
    std::uint8_t _op;
};

}   // namespace io
}   // namespace reqvm
