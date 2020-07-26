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

#include "memory_mapped_file_backed.hpp"

#define REQVM_IN_THE_MMF_CPP_FILE
#if defined(REQVM_ON_WINDOWS)
#    include "memory_mapped_file_backed.win32.ipp"
#elif defined(REQVM_ON_POSIX)
#    include "memory_mapped_file_backed.posix.ipp"
#endif
#undef REQVM_IN_THE_MMF_CPP_FILE

/*
 * README:
 *
 * This file is only meant to contain the platform agnostic code of
 * mmf_backed_binary_manager.
 *
 * All platform specific code should reside in the appropriate .ipp files.
 */

namespace reqvm {

auto mmf_backed_binary_manager::operator[](std::size_t idx) noexcept
    -> std::uint8_t {
    return _data[idx];
}

auto mmf_backed_binary_manager::size() noexcept -> std::size_t {
    return _size;
}

}   // namespace reqvm
