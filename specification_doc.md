# reqvm specification

reqvm is a register based bytecode VM.

Floating point won't be supported for a while.

There will be a stack, but an exact design for it has not been decided yet.

## Binaries

Binaries start with a 256 byte preamble. I've no idea if I'll ever fill this up, but whatever.

### Preamble format

Binaries will start with the following magic byte string `!reqvm;ZA WARUDO! TOKI WO TOMARE!\n`.  
Then a string of bytes following this format: `!<major>;<minor>;<patch>;<enabled features>;`. `major`, `minor`, and `patch` are 2-byte numbers. `enabled features` is a string of bytes, every byte represents an individual optional feature, and the presence of the byte enables it.  
This is padded with `noop`s until 256 bytes have been reached.

After that follows the program itself.

## Registers

All registers will be 64bit. List of registers:

* 64 general purpose registers: `gp0..63`
* 16 integer function argument registers: `ifa0..15`
* program counter(read only): `pc`
* a register dedicated to the return value of the last function: `ire`
* the stack pointer: `sp`(read only)

## Internal VM flags

* CF("comparison flag") - stores the result of a comparison between two registers. Possible values(these will be reffered to as `cf::value`): `eq`(equal), `less`(less than), `leq`(less or equal than), `gr`(greater than), `geq`(greater or equal than).

## Instruction table

| opcode(byte) | mnemonic(?) | instruction | what it does |
|:------:|----------|-------------|------------|
|   `00`   | `noop` | `noop` | is a noop |
|   `01`   | `call` | `call func_name` | calls the function `func_name` |
|   `02`   | `ret` | `ret` | returns from a function, performing necessary cleanup |
|   `20`   | `add` | `add r1, r2` | adds `r1` and `r2`, stores result in `r1`|
|   `21`   | `sub` | `sub r1, r2`| subtract `r2` from `r1`, stores result in `r1`|
|   `22`   | `mul` | `mul r1, r2` | multiplies `r1` by `r2`, stores result in `r1`|
|   `23`   | `div` | `div r1, r2` | divides `r1` by `r2`, stores quotient in `r1`|
|   `24`   | `mod` | `mod r1, r2` | divides `r1` by `r2`, stores remainder in `r1`|
|   `25`   | `and` | `and r1, r2` | bitwise ANDs `r1` and `r2`, stores result in `r1`|
|   `26`   | `or`  | `or r1, r2`  | bitwise ORs `r1` and `r2`, stores result in `r1`|
|   `27`   | `xor` | `xor r1, r2` | bitwise XORs `r1` and `r2`, stores result in `r1`|
|   `28`   | `not` | `not r1`     | bitwise NOTs `r1`, stores result in `r1`|
|   `29`   | `lshft` | `lshft r1, r2` | Performs `r1 <<= r2` |
|   `30`   | `rshft` | `rshft r1, r2` | Performs `r1 >>= r2` |
|   `35`   | `push` | `push reg`  | pushes `reg` onto the stack, increases the `sp` by 1|
|   `36`   | `pushc` | `pushc constant` | pushes the `constant` on the stack, stored in the binary as 8 bytes|
|   `37`   | `pop`   | `pop reg` | pops the top value from the stack into `reg`|
|   `42`   | `cmp` | `cmp r1, r2` | compares `r1` and `r2`, stores result in CF |
|   `43`   | `jmp` | `jmp label` | jumps to `label` |
|   `44`   | `jeq` | `jeq label` | if `CF == cf::eq`, jumps to `label` |
|   `45`   | `jneq` | `jneq label` | if `CF != cf::eq` jumps to `label` |
|   `46`   | `jl` | `jl label` | if `CF == cf::less`, jumps to `label` |
|   `47`   | `jleq` | `jleq label` | if `CF == cf::leq`, jumps to `label` |
|   `48`   | `jg` | `jg label` | if `CF == cf::gr`, jumps to `label` |
|   `49`   | `jgeq` | `jgeq label` | if `CF == cf::geq`, jumps to `label`
|   `255`  | `halt` | `halt` | stops program execution, and the VM, returning the value in `ire` to the OS |
