#pragma once

#include "../../common/opcodes.hpp"
#include "../../common/registers.hpp"

#include <cstdint>
#include <fstream>
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
    static auto parse_register(const std::string& name) -> common::registers;
    static auto get_register(const std::string& line) -> common::registers;
    static auto get_register_pair(const std::string& line)
        -> std::pair<common::registers, common::registers>;
    static auto get_io_op(const std::string& line) -> common::io_op;
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
