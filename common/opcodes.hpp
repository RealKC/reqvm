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
