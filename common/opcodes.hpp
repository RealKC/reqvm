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
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
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

// this enum represent all the opcodes of reqvm
enum class opcode : unsigned char {
    noop = 0,
    call = 1,
    ret  = 2,

    // Metainstruction
    io = 10,
    // Integer arithmetics
    add   = 20,
    sub   = 21,
    mul   = 22,
    div   = 23,
    mod   = 24,
    and_  = 25,
    or_   = 26,
    xor_  = 27,
    not_  = 28,
    lshft = 29,
    rshft = 30,

    // Stack operations
    push  = 35,
    pushc = 36,
    pop   = 37,

    // Branching
    cmp  = 42,
    jmp  = 43,
    jeq  = 44,
    jneq = 45,
    jl   = 46,
    jleq = 47,
    jg   = 48,
    jgeq = 49,

    halt = 255,
};

enum class io_op : unsigned char {
    getc  = 1,
    putc  = 2,
    put8c = 3,
    putn  = 4,
};

}   // namespace common
