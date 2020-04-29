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

#include "exceptions.hpp"

#include <cerrno>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace reqvm {

mmf_backed_binary_manager::mmf_backed_binary_manager(const fs::path& path) {
    _file_descriptor = ::open(path.c_str(), O_RDONLY);
    if (_file_descriptor == -1) {
        throw mmap_error {errno, mmap_error::kind::file};
    }
    {
        // We ask the kernel for the file size so we're 100% in sync with what
        // the kernel believes the file size is.
        struct stat st;
        if (::fstat(_file_descriptor, &st) != 0) {
            throw mmap_error {errno, mmap_error::kind::file_size};
        }
        _size = static_cast<std::size_t>(st.st_size);
    }
    _data = static_cast<std::uint8_t*>(
        ::mmap(nullptr, _size, PROT_READ, MAP_SHARED, _file_descriptor, 0));
    if (_data == static_cast<std::uint8_t*>(MAP_FAILED)) {
        throw mmap_error {errno, mmap_error::kind::mapping};
    }
}

mmf_backed_binary_manager::~mmf_backed_binary_manager() {
    // These functions can fail, how should we handle that?
    ::munmap(_data, _size);
    ::close(_file_descriptor);
}

mmap_error::mmap_error(error_code_t ec, kind k) noexcept : _ec {ec}, _k {k} {}

mmap_error::~mmap_error() noexcept = default;

auto mmap_error::what() const noexcept -> const char* {
#define CASE(the_error_code, message)                                          \
    case the_error_code:                                                       \
        return PREFIX message " (" #the_error_code ")";
    // We check the error values in their respective domains as that's the only
    // way to make sense of them.
    switch (_k) {
    case kind::file: {
#define PREFIX "An error occured while trying to open the binary:"
        switch (_ec) {
            // clang-format off
        // Note: this doesn't check all error codes from open
        //       I only picked those that seemed relevant for my usecase
        //       I also have written error messages corresponding to the
        //       kind of errors we should get in our usecase
        CASE(EACCES, "Access to the file not allowed, or search permission "
                     "denied for one of the directories in the path.")
        CASE(ELOOP, "Too many symbolic links were encountered while "
                    "trying to resolve the path of the binary.")
        CASE(ENAMETOOLONG, "The path was too long.")
        CASE(ENFILE, "The system-wide limit on the total number of open "
                     "files was reached.")
        CASE(ENOENT, "The file does not exist or a directory in your path "
                     "doesn't exist or is a dangling symbolic link.")
        CASE(ENOMEM, "There wasn't enough kernel memory.")
        CASE(ENOTDIR, "A directory component in your path is not actually "
                      "a directory.")
        CASE(EOVERFLOW, "The file was too large to be opened.")
            // clang-format on

        default:
            return "An unknown error occured while trying to open the file.";
        }
#undef PREFIX
    }
    case kind::file_size: {
#define PREFIX                                                                 \
    "An error occured while trying to obtain the size of the binary: "
        switch (_ec) {
            // clang-format off
        // Note: See previous note, same things apply
        //       Except errors that can happen at the ::open level are not 
        //       checked for here
        CASE(ENOMEM, "There wasn't enough kernel memory.")
        CASE(EOVERFLOW, "The file size/inode number/number of bytes cannot be"
                        "expressed in the type off_t/ino_t/blkcnt_t.")
            // clang-format on
        default:
            return "An unknown error occured while trying to obtain the size "
                   "of the binary.";
        }
#undef PREFIX
    }
    case kind::mapping: {
#define PREFIX "An error occured while trying to memory map the binary: "
        switch (_ec) {
            // clang-format off
        CASE(EACCES, "The file descriptor referring to your file is nonregular")
        CASE(ENFILE, "The system-wide limit on the total number of open "
                     "files was reached.")
        CASE(ENOMEM, "No memory is available") // Can this happen in our situation ???
        CASE(ENODEV, "The filesystem on which your binary resides does not"
                     " support memory mapping.")
        CASE(EINVAL, "Either your file was zero-sized (this indicates a bug in "
                     "reqvm as this code path is not supposed to be reached "
                     "in that case. Please report a bug at"
                     "https://github.com/RealKC/reqvm) or your file was too"
                     "large to be memory mapped.")
            // clang-format on

        default:
            return "An unknown error occured while trying to memory map the "
                   "binary.";
        }
#undef PREFIX
    }
    }
#undef CASE
}

}   // namespace reqvm
