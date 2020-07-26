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

#include "utility.hpp"

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>

namespace fs = std::filesystem;

namespace reqvm {

/*
 * A binary_manager is an interface modeling the operations that the VM needs to
 * do on a binary.
 *
 * As a polymoprhic type it is not movable or copyable to prevent slicing
 * issues.
 */
class binary_manager {
    REQVM_MAKE_NONCOPYABLE(binary_manager)
    REQVM_MAKE_NONMOVABLE(binary_manager)
public:
    binary_manager() noexcept = default;

    virtual ~binary_manager() noexcept = default;

    virtual auto operator[](std::size_t idx) noexcept -> std::uint8_t = 0;

    virtual auto size() noexcept -> std::size_t = 0;
};

auto load_from(const fs::path&) -> std::unique_ptr<binary_manager>;

}   // namespace reqvm
