# reqvm specification

reqvm is a register based bytecode VM.

Floating point won't be supported for a while.

There will be a stack, but an exact design for it has not been decided yet.

## Registers

All registers will be 64bit. List of registers:

* 64 general purpose registers: `gp0..63`
* 16 integer function argument registers: `ifa0..15`
* program counter(read only): `pc`
* a register dedicated to the return value of the last function: `ire`(read only)

## Instruction table

| opcode(byte) | mnemonic(?) | instruction |
|:------:|----------|-------------|
|   `01`   | `call` | `call func_name` |
|   `02`   | `ret` | `ret` |
|   `20`   | `add` | `add r1, r2` |
|   `21`   | `sub` | `sub r1, r`|
|   `22`   | `mul` | `mul r1, r2` |
|   `23`   | `div` | `div r1, r2` |
|   `24`   | `mod` | `mod r1, r2` |
|   `25`   | `and` | `and r1, r2` |
|   `26`   | `or`  | `or r1, r2`  |
|   `27`   | `xor` | `xor r1, r2` |
|   `28`   | `not` | `not r1`     |
|   `35`   | `push` | `push reg`  |
|   `36`   | `pushc` | `push constant` |
|   `37`   | `pop`   | `pop reg` |
|   `42`   | `cmp` | `cmp r1, r2` |
|   `43`   | `jmp` | `jmp label` |
|   `44`   | `jeq` | `jeq label` |
|   `45`   | `jneq` | `jneq label` |
|   `46`   | `jl` | `jl label` |
|   `47`   | `jleq` | `jleq label` |
|   `48`   | `jg` | `jg label` |
|   `49`   | `jgeq` | `jgeq label` |
