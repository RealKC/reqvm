#include "registers.hpp"
#include "exceptions.hpp"
namespace reqvm {

auto registers::parse_from_byte(std::uint8_t byte) noexcept -> registers::tag {
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
#pragma GCC diagnostic pop

    throw invalid_register {
        "A byte that does not name a register was supplied as operand to an opcode",
        static_cast<common::registers>(byte)
    };
}

auto registers::operator[](registers::tag tag) noexcept -> std::uint64_t& {
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
        throw 0; // for now we just throw
    }
}

auto registers::is_error_on_lhs(registers::tag reg) noexcept -> bool {
    switch(reg.kind) {
    case registers::tag::kind::pc:
    case registers::tag::kind::sp:
        return true;
    default:
        return false;
    }
}

}