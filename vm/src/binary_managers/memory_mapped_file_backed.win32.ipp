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
 * #pragma once directive) and is only meant to include the Windows specific
 * code of mmf_backed_binary_manager.
 *
 * It only has a .ipp extension so the Makefile ignores it. (There probably is a
 * better way, but I'm lazy. PR the better way if this bothers you that much.
 * Putting all the platform specific code in a single file is not a better way.)
 */

#if !defined(REQVM_ON_WINDOWS)
#    error "This file should only be used when compiling for MS Windows"
#endif

#if !defined(REQVM_IN_THE_MMF_CPP_FILE)
#    error "This file should only be included by memory_mapped_file_backed.cpp"
#endif

namespace reqvm {

mmf_backed_binary_manager::mmf_backed_binary_manager(
    const std::filesystem::path& path) {
    _file = ::CreateFileW(path.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING,
                          FILE_ATTRIBUTE_NORMAL, nullptr);

    {
        ::LARGE_INTEGER sz;
        ::GetFileSizeEx(_file, &sz);
        _size = static_cast<std::size_t>(sz.QuadPart);
        if (_size == 0) {
            // FIXME: We should report an error here.
        }
    }

    _mapping =
        ::CreateFileMappingA(_file, nullptr, PAGE_READONLY, 0, 0, nullptr);
    if (not _mapping) {
        // FIXME: We should report an error here.
    }

    _data = static_cast<std::uint8_t*>(
        ::MapViewOfFile(_mapping, FILE_MAP_READ, 0, 0, 0));
    if (not _data) {
        // FIXME: We should report an error here.
    }
}

mmf_backed_binary_manager::~mmf_backed_binary_manager() noexcept {
    // Technically these functions can fail, but I'm not sure how to communicate
    // that. TODO: find a way
    // Though this destructor only gets called when the VM is shutting down.
    // So is failing really an issue? Win32 API Gods help me plz.
    ::UnmapViewOfFile(_data);
    ::CloseHandle(_mapping);
    ::CloseHandle(_file);
}

}   // namespace reqvm
