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

#include "../binary_manager.hpp"
#include "../utility.hpp"

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <memory>

namespace reqvm {

class ifstream_backed_binary_manager final : public binary_manager {
    REQVM_MAKE_NONCOPYABLE(ifstream_backed_binary_manager)
    REQVM_MAKE_NONMOVABLE(ifstream_backed_binary_manager)
public:
    ifstream_backed_binary_manager() = delete;
    ifstream_backed_binary_manager(const std::filesystem::path&) noexcept;

    virtual ~ifstream_backed_binary_manager() noexcept = default;

    auto operator[](std::size_t idx) noexcept -> std::uint8_t override;

    auto size() noexcept -> std::size_t override;

private:
    std::ifstream _binary;
    std::size_t _size {0};
};

}   // namespace reqvm
