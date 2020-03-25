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

    static auto parse_from_byte(std::uint8_t byte) -> tag;
    static auto is_error_on_lhs(tag reg) noexcept -> bool;

    auto operator[](tag reg) -> std::uint64_t&;

    auto general_purpose() noexcept -> std::array<std::uint64_t, 64>& {
        return _general_purpose;
    }
    auto integer_function_args() noexcept -> std::array<std::uint64_t, 16>& {
        return _integer_functions_args;
    }

    auto ire() noexcept -> std::uint64_t& {
        return _integer_return;
    }

    auto pc() noexcept -> const std::uint64_t& {
        return _program_counter;
    }
    auto advance_pc(std::uint64_t n) noexcept -> void {
        _program_counter += n;
    }
    auto jump_to(std::uint64_t address) noexcept -> void {
        _program_counter = address;
    }

    auto sp() noexcept -> std::uint64_t& {
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