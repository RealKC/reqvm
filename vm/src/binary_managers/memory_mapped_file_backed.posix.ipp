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

/*
 * README:
 *
 * Please note that this is not a classical header file (and as such lacks a
 * #pragma once directive) and is only meant to contained the POSIX specific
 * code of mmf_backed_binary_manager.
 *
 * It only has a .ipp extension so the Makefile ignores it. (There probably is a
 * better way, but I'm lazy. PR the better way if this bothers you that much.
 * Putting all the platform specific code in a single file is not a better way.)
 */

#if !defined(REQVM_ON_POSIX)
#    error "This file should only be used when compiling for POSIX OS'es"
#endif

#if !defined(REQVM_IN_THE_MMF_CPP_FILE)
#    error "This file should only be included by memory_mapped_file_backed.cpp"
#endif

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace reqvm {

mmf_backed_binary_manager::mmf_backed_binary_manager(
    const std::filesystem::path& path) {
    _file_descriptor = ::open(path.c_str(), O_RDONLY);
    // FIXME: open can fail, so we should report an error if it does.
    {
        struct stat st;
        if (fstat(_file_descriptor, &st) != 0) {
            // FIXME: report an error.
        }
        _size = static_cast<std::size_t>(st.st_size);
    }
    _data = static_cast<std::uint8_t*>(
        ::mmap(nullptr, _size, PROT_READ, MAP_SHARED, _file_descriptor, 0));
    if (_data == static_cast<std::uint8_t*>(MAP_FAILED)) {
        // FIXME: report an error.
    }
}

mmf_backed_binary_manager::~mmf_backed_binary_manager() {
    // These functions can fail, how should we handle that?
    ::munmap(_data, _size);
    ::close(_file_descriptor);
}

}   // namespace reqvm
