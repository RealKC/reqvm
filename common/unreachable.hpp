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

#include <sstream>
#include <stdexcept>
#include <string>

namespace common {

class unreachable_code_reached : public std::runtime_error {
public:
    unreachable_code_reached(const char* what_arg) : runtime_error {what_arg} {}
    unreachable_code_reached(const std::string& what_arg)
        : runtime_error {what_arg} {}
    virtual ~unreachable_code_reached() noexcept = default;
};

}   // namespace common

#ifndef UNREACHABLE
    // REQVM_FUNCTION needs to live wherever UNREACHABLE is called so
    // intentionally don't undefine it, however we won't define it if
    // unreachable isn't used
#    if defined(__GNUC__)
#        define REQVM_COMMON_FUNCTION __PRETTY_FUNCTION__
#    elif defined(_MSC_VER)
#        define REQVM_COMMON_FUNCTION __FUNCSIG__
#    else
#        define REQVM_COMMON_FUNCTION __func__
#    endif
#    ifdef NDEBUG
#        if defined(_MSC_VER)
// See https://docs.microsoft.com/en-us/cpp/intrinsics/assume?view=vs-2019
// "The __assume(0) statement is a special case. Use __assume(0) to indicate a
// code path that cannot be reached."
#            define UNREACHABLE(msg) __assume(0)
#        elif defined(__GNUC__)
#            define UNREACHABLE(msg) __builtin_unreachable()
#        else
#            define UNREACHABLE(msg)
#        endif
#    else
#        define UNREACHABLE(msg)                                               \
            throw common::unreachable_code_reached {                           \
                [](const auto& the_msg) -> std::string {                       \
                    std::ostringstream str;                                    \
                    str << __FILE__ << ":" << __LINE__ << ":"                  \
                        << REQVM_COMMON_FUNCTION << ": " << the_msg;           \
                    return str.str();                                          \
                }(msg)                                                         \
            }
#    endif
#endif
