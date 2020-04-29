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

#include "../utility.hpp"
#include "exceptions.hpp"

#include <cstdio>
#include <cstring>
#include <new>

#if defined(REQVM_ON_WINDOWS)
// Any Windows API Gods able to help me trim down this include?
// Alternatively I could figure out exactly which headers I need and only
// include those, but that seems tedious.
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h>
#endif

namespace reqvm {

mmf_backed_binary_manager::mmf_backed_binary_manager(const fs::path& path) {
    _file = ::CreateFileW(path.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING,
                          FILE_ATTRIBUTE_NORMAL, nullptr);
    if (_file == INVALID_HANDLE_VALUE) {
        throw mmap_error {::GetLastError(), mmap_error::kind::file};
    }

    {
        // We ask the kernel for the file size so we're 100% sure we're in sync
        // with what the kernel believes.
        ::LARGE_INTEGER sz;
        if (::GetFileSizeEx(_file, &sz)) {
            throw mmap_error {::GetLastError(), mmap_error::kind::file_size};
        }
        _size = static_cast<std::size_t>(sz.QuadPart);
    }

    _mapping =
        ::CreateFileMappingA(_file, nullptr, PAGE_READONLY, 0, 0, nullptr);
    if (not _mapping) {
        throw mmap_error {::GetLastError(), mmap_error::kind::mapping};
    }

    _data = static_cast<std::uint8_t*>(
        ::MapViewOfFile(_mapping, FILE_MAP_READ, 0, 0, 0));
    if (not _data) {
        throw mmap_error {::GetLastError(), mmap_error::kind::mapping};
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

mmap_error::mmap_error(error_code_t ec, kind k) noexcept {
    class deferred_local_free final {
        REQVM_MAKE_NONCOPYABLE(deferred_local_free)
        REQVM_MAKE_NONMOVABLE(deferred_local_free)

        deferred_local_free(void* p) noexcept : _p {p} {}
        ~deferred_local_free() noexcept {
            ::LocalFree(_p);
        }

    private:
        void* _p;
    };

    char* err_msg {nullptr};
    auto succeeded = ::FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM
            | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, ec, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&err_msg,
        0, nullptr);

    if (not succeeded) {
        const char* fmt_string =
            "An error occured while retrieving extended format information. "
            "The error code returned by the system API: %d.";
        int ec_as_int = static_cast<int>(ec);
        auto size     = std::snprintf(nullptr, 0, fmt_string, ec_as_int);
        if (size < 0) {
            // Probably memory exhaustion? kinda weird given this should only be
            // called on start up. Oh well, leave _message untouched.
            // We'll check for this in ::what()
            return;
        }
        _message = new (std::nothrow) char[size + 1];
        if (not _message) {
            return;
        }
        if (std::sprintf(_message, fmt_string, ec_as_int) < 0) {
            _message = nullptr;
            return;
        }
        return;
    }

    deferred_local_free dlf {err_msg};

    const char* fmt_string {nullptr};
    switch (k) {
    case kind::file:
        fmt_string = "An error occured while opening the binary: %s";
        break;
    case kind::file_size:
        fmt_string =
            "An error occured while calculating the size of your binary: %s";
        break;
    case kind::mapping:
        fmt_string = "An error occured while memory mapping the binary: %s";
        break;
    }

    auto buf_size = std::snprintf(nullptr, 0, fmt_string, _message);
    if (buf_size < 0) {
        return;
    }
    _message = new (std::nothrow) char[buf_size + 1];
    if (not _message) {
        return;
    }
    if (std::sprintf(_message, fmt_string, err_msg) < 0) {
        _message = nullptr;   // ensure _message is null
    }
    return;
}

mmap_error::~mmap_error() noexcept {
    delete _message;
}

auto mmap_error::what() const noexcept -> const char* {
    return _message ? _message
                    : "There was an error allocating a buffer to store the "
                      "error message.";
}

}   // namespace reqvm
