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

namespace common {

// Valid reqvm binaries must start with this string,
// followed by a `\n'(ascii byte 10)
static constexpr char magic_byte_string[] =
    "!reqvm;"
    "VSszMEI2VSszMEVGVSszMEZDVSszMEVCVSszMEM5VSswMDIxVSs2NjQyVSszMDg4VSs2QjYyVS"
    "szMDdFVSszMDhDVSswMDIx\n";

namespace version {

static constexpr std::uint16_t major = 0;
static constexpr std::uint16_t minor = 1;
static constexpr std::uint16_t patch = 5;

}   // namespace version

}   // namespace common
