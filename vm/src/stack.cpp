#include "stack.hpp"

namespace reqvm {

auto stack::push(std::uint64_t val, registers& regs) -> void {
    if (regs.sp() + 1 > (1024 * 1024)) {
        throw stack_error {
            "Stack overflow: the binary has tried writing past the end of the "
            "stack."
        };
    }
    _storage[regs.sp()++] = val;
}

auto stack::pop(registers& regs) -> std::uint64_t {
    if (regs.sp() == 0) {
        throw stack_error {
            "Stack underflow: the binary has tried reading before the start "
            "of the stack."
        };
    }
    regs.sp()--;
    return _storage[regs.sp() + 1];
}

}
