#pragma once

#include <stdexcept>
#include <string>

#include "../../common/opcodes.hpp"
#include "../../common/registers.hpp"

namespace reqvm {

class vm_exception : public std::runtime_error {
public:
    vm_exception() : runtime_error{"A fatal exception has occured"} {}
    explicit vm_exception(const char* what_arg) : runtime_error{what_arg} {}
    explicit vm_exception(const std::string& what_arg) : runtime_error{what_arg} {}

    virtual ~vm_exception() noexcept = default;

    virtual common::opcode the_invalid_opcode() const noexcept = 0;
    virtual common::registers the_invalid_register() const noexcept = 0;
};

class invalid_opcode : public vm_exception {
public:
    // Disallow default construction so the exception may provide a meaningful
    // message (that includes the opcode)
    invalid_opcode() = delete;

    explicit invalid_opcode(common::opcode op) 
        : vm_exception{"The following invalid opcode was found in the provided binary: "}
        , _op{op} {}
    explicit invalid_opcode(const char* what_arg, common::opcode op)
        : vm_exception{what_arg}, _op{op} {}
    explicit invalid_opcode(const std::string& what_arg, common::opcode op)
        : vm_exception{what_arg}, _op{op} {}
    
    virtual ~invalid_opcode() noexcept = default;

    virtual common::opcode the_invalid_opcode() const noexcept override {
        return _op;
    }
    virtual common::registers the_invalid_register() const noexcept override {
        return common::registers::none;
    }
private:
    common::opcode _op;
};

class invalid_register : public vm_exception {
public:
    // Disallow default construction to force a meaningful message (that includes
    // the register)
    invalid_register() = delete;

    invalid_register(common::registers reg)
        : vm_exception{"The following invalid register was found in the provided binary: "}
        , _reg{reg} {}
    invalid_register(const char* what_arg, common::registers reg)
        : vm_exception{what_arg}, _reg{reg} {}
    invalid_register(const std::string& what_arg, common::registers reg)
        : vm_exception{what_arg}, _reg{reg} {}
    
    virtual ~invalid_register() noexcept = default;

    virtual common::opcode the_invalid_opcode() const noexcept override { 
        return common::opcode::noop;
    }
    virtual common::registers the_invalid_register() const noexcept override {
        return _reg;
    }
private:
    common::registers _reg;
};

}
