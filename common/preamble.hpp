#pragma once

namespace common {

// Valid reqvm binaries must start with this string,
// followed by a `\n'(ascii byte 10)
static constexpr char magic_byte_string[] = "!reqvm;VSszMEI2VSszMEVGVSszMEZDVSszMEVCVSszMEM5VSswMDIxVSs2NjQyVSszMDg4VSs2QjYyVSszMDdFVSszMDhDVSswMDIx\n";

}