#pragma once

#include <cstdint>

namespace reqvm {

struct flags {
    enum class cf {
        eq   = 0b0000'0000,
        less = 0b0000'0001,
        gr   = 0b0000'0010,
    };

    std::uint64_t cmp_flag : 2;
    std::uint64_t reserved : 62;
};

}   // namespace reqvm
