/* 
 *  MIT License
 *  
 *  Copyright (c) 2020 Mitca Dumitru
 *  
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *  
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *  
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

#include "registers.hpp"

#include "../../common/unreachable.hpp"
#include "exceptions.hpp"

namespace reqvm {

registers::registers() noexcept
    : _program_counter {0}, _stack_pointer {0}, _integer_return {0} {
    for (auto& elem : _general_purpose) {
        elem = 0;
    }
    for (auto& elem : _integer_functions_args) {
        elem = 0;
    }
}

auto registers::parse_from_byte(std::uint8_t byte) -> registers::tag {
    const auto reg = static_cast<common::registers>(byte);

    if (reg == common::registers::sp) {
        return {registers::tag::kind::sp, 0};
    }
    if (reg == common::registers::pc) {
        return {registers::tag::kind::pc, 0};
    }
    if (reg == common::registers::ire) {
        return {registers::tag::kind::ire, 0};
    }

// Let me tell you something fun, the standard says that arithmetic on types
// smaller than `int` must have its operands cast to int, so we ignore a
// narrowing warning here, because we know our operands won't overflow a uint8_t
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"
    if (common::registers::gp00 <= reg && reg <= common::registers::gp63) {
        return {registers::tag::kind::gp,
                byte - static_cast<std::uint8_t>(common::registers::gp00)};
    }
    if (common::registers::ifa00 <= reg && reg <= common::registers::ifa15) {
        return {registers::tag::kind::ifa,
                byte - static_cast<std::uint8_t>(common::registers::ifa00)};
    }
#pragma GCC diagnostic pop

    throw invalid_register {"A byte that does not name a register was supplied "
                            "as operand to an opcode",
                            static_cast<common::registers>(byte)};
}

auto registers::operator[](registers::tag tag) -> std::uint64_t& {
    switch (tag.kind) {
    case registers::tag::kind::pc:
        return _program_counter;
    case registers::tag::kind::sp:
        return _stack_pointer;
    case registers::tag::kind::ire:
        return _integer_return;
    case registers::tag::kind::gp:
        return _general_purpose[tag.idx];
    case registers::tag::kind::ifa:
        return _integer_functions_args[tag.idx];
    default:
        UNREACHABLE(
            "register::operator[]: switch was not actually exhaustive.");
    }
}

auto registers::is_error_on_lhs(registers::tag reg) noexcept -> bool {
    switch (reg.kind) {
    case registers::tag::kind::pc:
    case registers::tag::kind::sp:
        return true;
    default:
        return false;
    }
}

}   // namespace reqvm
