#include "exceptions.hpp"
#include "vm.hpp"

namespace reqvm {

auto vm::run() -> int {
    read_preamble();
    while (_regs.pc() <= _binary.size() && !_halted) {
        cycle(static_cast<common::opcode>(_binary[_regs.pc()]));
    }
    return static_cast<int>(_regs.ire());
}

#define CHECK_LHS_REG(opcode, reg)                                \
    do {                                                                \
        if (registers::is_error_on_lhs((reg))) {                        \
            throw reqvm::invalid_register {                             \
                "Invalid lhs operand for opcode '" #opcode "': ",       \
                static_cast<common::registers>(_binary[_regs.pc() + 1]) \
            };                                                          \ 
        }                                                               \
    } while (0)

#define MAKE_8_BYTE_VAL(val)                                             \
    auto val = static_cast<std::uint64_t>(_binary[_regs.pc() + 1]) << 56 \
            | static_cast<std::uint64_t>(_binary[_regs.pc() + 2]) << 48  \
            | static_cast<std::uint64_t>(_binary[_regs.pc() + 3]) << 40  \
            | static_cast<std::uint64_t>(_binary[_regs.pc() + 4]) << 32  \
            | static_cast<std::uint64_t>(_binary[_regs.pc() + 5]) << 24  \
            | static_cast<std::uint64_t>(_binary[_regs.pc() + 6]) << 16  \
            | static_cast<std::uint64_t>(_binary[_regs.pc() + 7]) << 8   \
            | static_cast<std::uint64_t>(_binary[_regs.pc() + 8])

auto vm::cycle(common::opcode op) -> void {
    using common::opcode;
    switch (op) {
    case opcode::noop: {
        // do nothing
        break;
    }
    case opcode::call: {
        // I need to figure out the machinery for this
        break;
    }
    case opcode::ret: {
        // see above
    }
    case opcode::add: {
        auto r1 = registers::parse_from_byte(_binary[_regs.pc() + 1]);
        CHECK_LHS_REG(add, r1);
        auto r2 = registers::parse_from_byte(_binary[_regs.pc() + 2]);
        _regs[r1] += _regs[r2];
        _regs.advance_pc(3);
        break;
    }
    case opcode::sub: {
        auto r1 = registers::parse_from_byte(_binary[_regs.pc() + 1]);
        CHECK_LHS_REG(sub, r1);
        auto r2 = registers::parse_from_byte(_binary[_regs.pc() + 2]);
        _regs[r1] -= _regs[r2];
        _regs.advance_pc(3);
        break;
    }
    case opcode::mul: {
        auto r1 = registers::parse_from_byte(_binary[_regs.pc() + 1]);
        CHECK_LHS_REG(mul, r1);
        auto r2 = registers::parse_from_byte(_binary[_regs.pc() + 2]);
        _regs[r1] *= _regs[r2];
        _regs.advance_pc(3);
        break;
    }
    case opcode::div: {
        auto r1 = registers::parse_from_byte(_binary[_regs.pc() + 1]);
        CHECK_LHS_REG(div, r1);
        auto r2 = registers::parse_from_byte(_binary[_regs.pc() + 2]);
        _regs[r1] /= _regs[r2];
        _regs.advance_pc(3);
        break;
    }
    case opcode::mod: {
        auto r1 = registers::parse_from_byte(_binary[_regs.pc() + 1]);
        CHECK_LHS_REG(mod, r1);
        auto r2 = registers::parse_from_byte(_binary[_regs.pc() + 2]);
        _regs[r1] %= _regs[r2];
        _regs.advance_pc(3);
        break;
    }
    case opcode::and_: {
        auto r1 = registers::parse_from_byte(_binary[_regs.pc() + 1]);
        CHECK_LHS_REG(and, r1);
        auto r2 = registers::parse_from_byte(_binary[_regs.pc() + 2]);
        _regs[r1] &= _regs[r2];
        _regs.advance_pc(3);
        break;
    }
    case opcode::or_: {
        auto r1 = registers::parse_from_byte(_binary[_regs.pc() + 1]);
        CHECK_LHS_REG(or, r1);
        auto r2 = registers::parse_from_byte(_binary[_regs.pc() + 2]);
        _regs[r1] |= _regs[r2];
        _regs.advance_pc(3);
        break;
    }
    case opcode::xor_: {
        auto r1 = registers::parse_from_byte(_binary[_regs.pc() + 1]);
        CHECK_LHS_REG(xor, r1);
        auto r2 = registers::parse_from_byte(_binary[_regs.pc() + 2]);
        _regs[r1] ^= _regs[r2];
        _regs.advance_pc(3);
        break;
    }
    case opcode::not_: {
        auto r1 = registers::parse_from_byte(_binary[_regs.pc() + 1]);
        if (registers::is_error_on_lhs(r1)) {
            throw invalid_register {
                "Invalid operand for opcode 'not': ",
                static_cast<common::registers>(_binary[_regs.pc() + 1])
            };
        }
        _regs[r1] = ~_regs[r1];
        _regs.advance_pc(2);
        break;
    }
    case opcode::lshft: {
        auto r1 = registers::parse_from_byte(_binary[_regs.pc() + 1]);
        CHECK_LHS_REG(lshft, r1);
        auto r2 = registers::parse_from_byte(_binary[_regs.pc() + 2]);
        _regs[r1] <<= _regs[r2];
        _regs.advance_pc(3);
        break;
    }
    case opcode::rshft: {
        auto r1 = registers::parse_from_byte(_binary[_regs.pc() + 1]);
        CHECK_LHS_REG(rhsft, r1);
        auto r2 = registers::parse_from_byte(_binary[_regs.pc() + 2]);
        _regs[r1] >>= _regs[r2];
        _regs.advance_pc(3);
    }
    case opcode::push: {
        auto r1 = registers::parse_from_byte(_binary[_regs.pc() + 1]);
        // TODO: actually push to the stack
        _regs.advance_pc(2);
        break;
    }
    case opcode::pushc: {
        if (_binary.size() - _regs.pc() < 8) {
            throw bad_argument {
                "Opcode 'pushc' is the last opcode in your binary and after it "
                "there are less than 8 bytes, as such it is not possible to build"
                "its argument."
            };
        }
        MAKE_8_BYTE_VAL(val);
        // TODO: Push val to the stack once we have one
        _regs.advance_pc(9);
        break;
    }
    case opcode::pop: {
        auto r1 = registers::parse_from_byte(_binary[_regs.pc() + 1]);
        if (registers::is_error_on_lhs(r1)) {
            throw invalid_register {
                "Invalid operand for opcode 'pop': ",
                static_cast<common::registers>(_binary[_regs.pc() + 1])
            };
        }
        // TODO: actually pop the value into the target register
        _regs.advance_pc(2);
        break;
    }
    case opcode::cmp: {
        auto r1 = registers::parse_from_byte(_binary[_regs.pc() + 1]);
        auto r2 = registers::parse_from_byte(_binary[_regs.pc() + 2]);
        // TODO: actually perform the comparison
        _regs.advance_pc(2);
        break;
    }
    case opcode::jmp: {
        MAKE_8_BYTE_VAL(address);
        _regs.jump_to(address);
        break;
    }
    case opcode::jeq: {
        // TODO: check the condition for jumping
        if (0) {
            MAKE_8_BYTE_VAL(address);
            _regs.jump_to(address);
        } else {
            _regs.advance_pc(9);
        }
        break;
    }
    case opcode::jneq: {
        // TODO: check the condition for jumping
        if (0) {
            MAKE_8_BYTE_VAL(address);
            _regs.jump_to(address);
        } else {
            _regs.advance_pc(9);
        }
        break;
    }
    case opcode::jl: {
        // TODO: check the condition for jumping
        if (0) {
            MAKE_8_BYTE_VAL(address);
            _regs.jump_to(address);
        } else {
            _regs.advance_pc(9);
        }
        break;
    }
    case opcode::jleq: { 
        // TODO: check the condition for jumping
        if (0) {
            MAKE_8_BYTE_VAL(address);
            _regs.jump_to(address);
        } else {
            _regs.advance_pc(9);
        }
        break;
    }
    case opcode::jg: {
        // TODO: check the condition for jumping
        if (0) {
            MAKE_8_BYTE_VAL(address);
            _regs.jump_to(address);
        } else {
            _regs.advance_pc(9);
        }
        break;
    }
    case opcode::jgeq: {
        // TODO: check the condition for jumping
        if (0) {
            MAKE_8_BYTE_VAL(address);
            _regs.jump_to(address);
        } else {
            _regs.advance_pc(9);
        }
        break;
    }
    case opcode::halt: {
        _halted = true;
        break;
    }
    }

}

#undef CHECK_LHS_REG
#undef MAKE_8_BYTE_VAL

}