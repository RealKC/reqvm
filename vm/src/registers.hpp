/*
* MIT License
* 
* Copyright (c) 2020 Mitca Dumitru
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#pragma once

#include "../../common/registers.hpp"

#include <array>
#include <cstdint>

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

    registers() noexcept;
    ~registers() noexcept = default;

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

}   // namespace reqvm
