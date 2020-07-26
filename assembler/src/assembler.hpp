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
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
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

#include <cstdint>
#include <fstream>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace reqvm {

class assembler {
public:
    explicit assembler(const std::string& filename);
    ~assembler() noexcept = default;

    auto has_errors() const noexcept -> bool {
        return _has_errors;
    }

    auto run() -> int;

private:
    static auto get_label(const std::string& line) -> std::string;
    static auto get_opcode(const std::string& line) -> common::opcode;
    static auto parse_register(const std::string& name)
        -> std::optional<common::registers>;
    static auto get_register(const std::string& line)
        -> std::optional<common::registers>;
    static auto get_register_pair(const std::string& line)
        -> std::optional<std::pair<common::registers, common::registers>>;
    static auto get_io_op(const std::string& line)
        -> std::optional<common::io_op>;
    static auto get_io_op_and_reg(const std::string& line)
        -> std::optional<std::pair<common::io_op, common::registers>>;
    static auto is_read_only(common::registers reg) noexcept -> bool;

    enum class opcode_category {
        nullary,
        unary_register,
        unary_label,
        unary_constant,
        binary_registers,
        binary_byte_then_register,
    };
    static auto get_category(common::opcode op) -> opcode_category;

    auto write_preamble() -> void;
    auto emit(common::opcode op) -> void;
    auto emit(common::opcode op, std::string label) -> void;
    auto emit(common::opcode op, common::registers reg) -> void;
    auto emit(common::opcode op, std::uint64_t num) -> void;
    auto emit(common::opcode op,
              std::pair<common::registers, common::registers> regs) -> void;
    auto emit(common::opcode op, common::io_op subop, common::registers reg)
        -> void;
    auto emit_remaining_labels() -> void;

    std::ifstream _file;
    std::ofstream _out;
    std::string _output_name;
    std::unordered_map<std::string, std::vector<std::uint64_t>> _labels;
    std::uint64_t _pc {256};
    bool _has_errors {false};
};

}   // namespace reqvm
