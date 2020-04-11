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

#if defined(_WIN32)
#    define REQVM_ON_WINDOWS 1
#elif defined(__linux__) || defined(__APPLE__) || defined(__unix__)            \
    || defined(_POSIX_VERSION)
// The check above should probably catch anything that qualifies as POSIX
// Except, maybe iOS or Android, but I don't see why you'd compile this project
// for those platforms.
#    define REQVM_ON_POSIX 1
#else
#    error Unknown platform. Consider submitting a patch upstream to add your \
    platform. As this is a for fun project I am willing to accept even weird \
    and/or unpopular platforms as long as their addition is not too big of a \
    maintenance burden.
#endif
