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

#include "io.hpp"

#include <cinttypes>
#include <cstdio>

namespace reqvm {
namespace io {

common::io_op parse_from_byte(std::uint8_t byte) {
    using common::io_op;
    switch (byte) {
    case 1:
        return io_op::getc;
    case 2:
        return io_op::putc;
    case 3:
        return io_op::put8c;
    case 4:
        return io_op::putn;
    default:
        throw error {"Unknown operation passed as argument to 'io': ", byte};
    }
}

auto getc() -> std::uint64_t {
    return static_cast<std::uint64_t>(std::fgetc(stdin));
}

auto putc(std::uint64_t ch) -> void {
    std::putc(static_cast<char>(ch), stdout);
}

auto put8c(std::uint64_t string) -> void {
    char chars[9] = {0};
    chars[0]      = static_cast<char>(string >> 56);
    chars[1]      = static_cast<char>((string << 8) >> 56);
    chars[2]      = static_cast<char>((string << 16) >> 56);
    chars[3]      = static_cast<char>((string << 24) >> 56);
    chars[4]      = static_cast<char>((string << 32) >> 56);
    chars[5]      = static_cast<char>((string << 40) >> 56);
    chars[6]      = static_cast<char>((string << 48) >> 56);
    chars[7]      = static_cast<char>((string << 56) >> 56);
    for (auto ch : chars) {
        std::putc(ch, stdout);
    }
}

auto putn(std::uint64_t num) -> void {
    std::fprintf(stdout, "%" PRId64, num);
}

}   // namespace io
}   // namespace reqvm