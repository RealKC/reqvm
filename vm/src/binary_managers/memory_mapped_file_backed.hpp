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
#include "../detect_platform.hpp"
#include "../utility.hpp"

#include <cstdint>
#include <filesystem>

namespace reqvm {

class mmf_backed_binary_manager final : public binary_manager {
    REQVM_MAKE_NONCOPYABLE(mmf_backed_binary_manager)
    REQVM_MAKE_NONMOVABLE(mmf_backed_binary_manager)
public:
    mmf_backed_binary_manager() = delete;
    explicit mmf_backed_binary_manager(const std::filesystem::path&);

    virtual ~mmf_backed_binary_manager() noexcept;

    auto operator[](std::size_t idx) noexcept -> std::uint8_t override;

    auto size() noexcept -> std::size_t override;

private:
#if defined(REQVM_ON_WINDOWS)
    // We use this instead of ::HANDLE to avoid including <windows.h> here
    // "But what if Microsoft decides to change that typedef?"
    // Well blimey, that'd be quiet the ABI breaking change, and Microsoft is
    // known for their backwards compat, so it won't be happening dawg.
    using handle_t = void*;
    handle_t _file;
    handle_t _mapping;
#elif defined(REQVM_ON_POSIX)
    int _file_descriptor;
#endif
    /* NOTE:
     *       * On MS Windows this is a view into the mapping
     *       * On POSIX platforms this is both the view and the handle to the
     *       mapping
     */
    std::uint8_t* _data;

    // We cache the size as it is requested every cycle by the VM
    std::size_t _size;
};

}   // namespace reqvm
