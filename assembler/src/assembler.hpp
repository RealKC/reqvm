#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <unordered_map>

#include "../../common/opcodes.hpp"
#include "../../common/registers.hpp"

namespace reqvm {

class assembler {
public:
    assembler() = delete;
    assembler(std::string file_name) : _file_name{file_name} {}
    ~assembler() noexcept = default;

    // Runs every stage of the assembler over the file
    auto run()        -> int;
    //auto preprocess() -> int;
    auto assemble()   -> int;

private:
    auto process_instruction(const std::string& line) -> void;
    auto process_label(const std::string& line) -> void;
    
    auto emit_op(
        common::opcode op,
        common::registers r1 = common::registers::none,
        common::registers r2 = common::registers::none) -> void;
    auto emit_pushc(const std::string& line) -> void;
    auto emit_jump(const std::string& line, std::size_t label_start) -> void;
    auto emit_labels() -> void;

    static auto get_opcode(const std::string& line, std::size_t len) 
        -> common::opcode;
    static auto get_register(const std::string& line, std::size_t start, bool is_lhs) 
        -> common::registers;

    std::vector<std::uint8_t> _output;
    // Note that the first element of the vector is the address of the label
    // itself
    std::unordered_map<std::string, std::vector<std::uint64_t>> _labels;
    std::uint64_t _idx {0};
    std::string _file_name;
    std::string preprocessed_file;
    bool _has_errors {false};
};

}