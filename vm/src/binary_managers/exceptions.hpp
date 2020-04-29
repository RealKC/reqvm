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

#include "../detect_platform.hpp"

#include <stdexcept>

namespace reqvm {

/*
 * Represents an error encountered in the process of memory mapping a file
 *
 * Definitions for the member functions of this class are present in
 * memory_mapped_file_backed.{win32,posix}.ipp in a platform appropriate manner
 */
class mmap_error : public std::exception {
public:
#if defined(REQVM_ON_WINDOWS)
    // ::DWORD is defined as unsigned long, and as Microsoft is known for their
    // backwards compatibility, that won't change.
    // This way we can avoid including windows.h here.
    using error_code_t = unsigned long;
#elif defined(REQVM_ON_POSIX)
    using error_code_t = int;
#endif
    enum class kind {
        file = 1,
        mapping,
        file_size,
    };

    mmap_error(error_code_t, kind) noexcept;

    virtual ~mmap_error() noexcept;

    auto what() const noexcept -> const char* override;

private:
// On MS Windows we store the message associated with the error code
// On POSIX Platforms we store the error code and its kind and retrieve a
//    message associated with it in ::what()
#if defined(REQVM_ON_WINDOWS)
    char* _message {nullptr};
#elif defined(REQVM_ON_POSIX)
    error_code_t _ec {0};
    kind _k {0};
#endif
};

}   // namespace reqvm
