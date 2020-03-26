#pragma once

#include "../../common/opcodes.hpp"
#include "flags.hpp"
#include "registers.hpp"
#include "stack.hpp"

#include <cstdint>
#include <vector>

namespace reqvm {

class vm final {
public:
    vm() = delete;
    explicit vm(const std::string& binary);
    ~vm() noexcept = default;
    auto run() -> int;
private:
    auto read_preamble() -> void;
    auto cycle(common::opcode op) -> void;

    std::vector<std::uint8_t> _binary;
    registers _regs;
    stack _stack;
    flags _flags;
    bool _halted;
};

}
