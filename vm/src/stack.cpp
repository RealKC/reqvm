#include "stack.hpp"

namespace reqvm {

auto stack::push(std::uint64_t val, registers& regs) -> void {
    if (regs.sp() + 1 > (1024 * 1024)) {
        throw 0; // stack overflow
    }
    _storage[regs.sp()++] = val;
}

auto stack::pop(registers& regs) -> std::uint64_t {
        if (regs.sp() == 0) {
            throw 0; // stack underflow ?
        }
        regs.sp()--;
        return _storage[regs.sp() + 1];
    }

}