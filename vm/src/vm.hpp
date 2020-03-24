#pragma once

#include "../../common/opcodes.hpp"
#include "registers.hpp"

#include <cstdint>
#include <vector>

namespace reqvm {

class vm final {
public:
    auto run() -> int;
private:
    auto read_preamble() -> void;
    auto cycle(common::opcode op) -> void;

    std::vector<std::uint8_t> _binary;
    registers _regs;
    bool _halted;
};

}