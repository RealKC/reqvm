#include "registers.hpp"

namespace reqvm {

registers::tag registers::parse_from_byte(std::uint8_t byte) noexcept {
    const common::registers reg = static_cast<common::registers>(byte);

    if (reg == common::registers::sp) {
        return {registers::tag::kind::sp, 0};
    } 
    if (reg == common::registers::pc) {
        return {registers::tag::kind::pc, 0};
    }
    if (reg == common::registers::ire) {
        return {registers::tag::kind::ire, 0};
    }
    if (common::registers::gp00 <= reg && reg <= common::registers::gp63) {
        return {
            registers::tag::kind::gp,
            byte - static_cast<std::uint8_t>(common::registers::gp00)
        };
    }
    if (common::registers::ifa00 <= reg && reg <= common::registers::ifa15) {
        return {
            registers::tag::kind::ifa,
            byte - static_cast<std::uint8_t>(common::registers::ifa00)
        };
    }

    // We shouldn't reach this
    // TODO: make a macro to tell that to the compiler
}

std::uint64_t& registers::operator[](registers::tag tag) noexcept {
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
        // Should not be reached
        // TODO: make a macro to tell that to the compiler
    }
}

bool registers::is_error_on_lhs(registers::tag reg) noexcept {
    switch(reg.kind) {
    case registers::tag::kind::pc:
    case registers::tag::kind::sp:
        return true;
    default:
        return false;
    }
}

}