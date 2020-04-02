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

#include "../../common/opcodes.hpp"
#include "../../common/registers.hpp"

#include <stdexcept>
#include <string>

namespace reqvm {

class vm_exception : public std::runtime_error {
public:
    vm_exception() : runtime_error {"A fatal exception has occured"} {}
    explicit vm_exception(const char* what_arg) : runtime_error {what_arg} {}
    explicit vm_exception(const std::string& what_arg)
        : runtime_error {what_arg} {}

    virtual ~vm_exception() noexcept = default;

    virtual auto the_opcode() const noexcept -> common::opcode      = 0;
    virtual auto the_register() const noexcept -> common::registers = 0;
};

class invalid_opcode : public vm_exception {
public:
    // Disallow default construction so the exception may provide a meaningful
    // message (that includes the opcode)
    invalid_opcode() = delete;

    explicit invalid_opcode(common::opcode op)
        : vm_exception {"The following invalid opcode was found in the "
                        "provided binary: "}
        , _op {op} {}
    explicit invalid_opcode(const char* what_arg, common::opcode op)
        : vm_exception {what_arg}, _op {op} {}
    explicit invalid_opcode(const std::string& what_arg, common::opcode op)
        : vm_exception {what_arg}, _op {op} {}

    virtual ~invalid_opcode() noexcept = default;

    auto the_opcode() const noexcept -> common::opcode override {
        return _op;
    }
    auto the_register() const noexcept -> common::registers override {
        return common::registers::none;
    }

private:
    common::opcode _op;
};

class invalid_register : public vm_exception {
public:
    // Disallow default construction to force a meaningful message (that
    // includes the register)
    invalid_register() = delete;

    invalid_register(const char* what_arg, common::registers reg)
        : vm_exception {what_arg}, _reg {reg} {}
    invalid_register(const std::string& what_arg, common::registers reg)
        : vm_exception {what_arg}, _reg {reg} {}

    virtual ~invalid_register() noexcept = default;

    auto the_opcode() const noexcept -> common::opcode override {
        return common::opcode::noop;
    }
    auto the_register() const noexcept -> common::registers override {
        return _reg;
    }

private:
    common::registers _reg;
};

class bad_argument : public vm_exception {
public:
    // Delete default constructor to force a meaningful message
    bad_argument() = delete;

    bad_argument(const char* what_arg) : vm_exception {what_arg} {};
    bad_argument(const std::string& what_arg) : vm_exception {what_arg} {}

    virtual ~bad_argument() noexcept = default;

    auto the_register() const noexcept -> common::registers override {
        return common::registers::none;
    }
    auto the_opcode() const noexcept -> common::opcode override {
        return common::opcode::noop;
    }
};

class preamble_error : public std::runtime_error {
public:
    preamble_error()
        : runtime_error {"reqvm was unable to start because the preamble of "
                         "this binary does "
                         "not follow the standard preamble format."} {}
    virtual ~preamble_error() noexcept = default;
};

}   // namespace reqvm
