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

#include "../../common/crosscompiler_defines.hpp"

#include <iostream>

#ifdef AGGRESIVE_LOGGING
#    define LOG_HEADER()                                                       \
        std::cerr << __FILE__ << ":" << __LINE__ << ": in "                    \
                  << REQVM_COMMON_FUNCTION << ":\n\t"
#    define LOG1(first)      LOG_HEADER() << #    first ": " << first << '\n'
#    define LOG1_NONL(first) LOG_HEADER() << #    first << first
#    define PAIR(pair)                                                         \
        << #pair "{ .first: " << pair.first << ", .second: " << pair.second    \
        << "}\n"
#    define LOG2(first, second)                                                \
        LOG_HEADER() << #first ": " << first << ", " #second ": " << second    \
                     << '\n'
#    define LOG_MSG(msg) LOG_HEADER() << msg
#else
#    define LOG_HEADER()
#    define LOG1(f)
#    define LOG1_NONL(f)
#    define LOG_PAIR(p)
#    define LOG2(f, s)
#    define LOG_MSG(m)
#endif
