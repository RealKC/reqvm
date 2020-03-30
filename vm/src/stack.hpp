#pragma once

#include "registers.hpp"

#include <cstdint>
#include <stdexcept>

namespace reqvm {

class stack_error : public std::runtime_error {
public:
    stack_error(const char* what_arg) : runtime_error(what_arg) {}

    virtual ~stack_error() noexcept = default;
};

class stack final {
public:
    stack() : _storage {new std::uint64_t[1024 * 1024]} {}
    ~stack() noexcept {
        delete _storage;
    }

    auto push(std::uint64_t val, registers& regs) -> void;
    auto pop(registers& regs) -> std::uint64_t;

private:
    std::uint64_t* _storage;
};

}   // namespace reqvm
