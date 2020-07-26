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

#ifndef NDEBUG

#    include "../../common/crosscompiler_defines.hpp"

#    include <cstdint>

#    define ASSERTM(cond, msg)                                                 \
        do {                                                                   \
            if (cond) {                                                        \
                /* do nothing */                                               \
            } else {                                                           \
                reqvm::assertions::assertion_failed(                           \
                    __FILE__, __LINE__, REQVM_COMMON_FUNCTION, #cond, msg);    \
            }                                                                  \
        } while (0)

#    define ASSERT(cond) ASSERTM(cond, nullptr)

namespace reqvm {
namespace assertions {

[[noreturn]] auto assertion_failed(const char* file,
                                   std::uint64_t line,
                                   const char* function_name,
                                   const char* condition,
                                   const char* message) noexcept -> void;

}
}   // namespace reqvm

#else

#    define ASSERT(c)   // ignore macro params
#    define ASSERTM(c, m)

#endif
