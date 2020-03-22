#pragma once

#include <array>
#include <cstdint>

#include "../../common/registers.hpp"

namespace reqvm {

class registers final {
public:
    struct tag {
        enum class kind : std::uint8_t {
            pc = 0,
            sp,
            ire,
            gp,
            ifa,
        } kind;
        std::uint8_t idx;
    };

    static tag parse_from_byte(std::uint8_t byte) noexcept;
    static bool is_error_on_lhs(tag reg) noexcept;
    std::uint64_t& operator[](tag reg) noexcept;
    std::array<std::uint64_t, 64>& general_purpose() noexcept {
        return _general_purpose;
    }
    std::array<std::uint64_t, 16>& integer_function_args() noexcept {
        return _integer_functions_args;
    }
    const std::uint64_t& pc() noexcept {
        return _program_counter;
    }
    const std::uint64_t& sp() noexcept {
        return _stack_pointer;
    }
private:
    std::array<std::uint64_t, 64> _general_purpose;
    std::array<std::uint64_t, 16> _integer_functions_args;
    std::uint64_t _program_counter;
    std::uint64_t _stack_pointer;
    std::uint64_t _integer_return;
};

}