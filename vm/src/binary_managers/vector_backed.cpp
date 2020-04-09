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

#include "vector_backed.hpp"

#include <fstream>

namespace fs = std::filesystem;

namespace reqvm {

vector_backed_binary_manager::vector_backed_binary_manager(
    const fs::path& path, std::size_t file_size) {
    _binary.resize(file_size);
    std::ifstream the_file {path, std::ios::binary};
    if (not the_file.read(reinterpret_cast<char*>(_binary.data()), file_size)) {
        // TODO: report an error
    }
}

auto vector_backed_binary_manager::operator[](std::size_t idx) noexcept
    -> std::uint8_t {
    return _binary[idx];
}

auto vector_backed_binary_manager::size() noexcept -> std::size_t {
    return _binary.size();
}

}   // namespace reqvm
