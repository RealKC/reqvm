#pragma once

#include "../../common/opcodes.hpp"
#include "registers.hpp"

#include <cstdint>
#include <vector>

namespace reqvm {

class vm final {
public:
    int run();
private:
    void read_preamble();
    void cycle(common::opcode op);

    std::vector<std::uint8_t> _binary;
    registers _regs;
    bool _halted;
};

}