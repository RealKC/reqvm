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
    assembler() = default;
    assembler(std::string file_name) : file_name{file_name} {}
    ~assembler() noexcept = default;

    // Runs every stage of the assembler over the file
    auto run()        -> int;
    //auto preprocess() -> int;
    auto assemble()   -> int;

private:
    auto emit_label() -> void;
    auto emit_op(
        common::opcode op,
        common::registers r1 = common::registers::none,
        common::registers r2 = common::registers::none) -> void;
    auto emit_pushc(const std::string& line, std::size_t start) -> void;
    auto emit_jump(const std::string& line, std::size_t label_start) -> void;
    auto emit_call(const std::string& line, std::size_t label_start) -> void;
    auto emit_labels() -> void;

    static auto get_opcode(const std::string& line, std::size_t len) 
        -> common::opcode;
    static auto get_register(const std::string& line, std::size_t start, bool is_lhs) 
        -> common::registers;

    std::vector<std::uint8_t> output;
    std::unordered_map<std::string, std::uint64_t> labels;
    std::string file_name;
    std::string preprocessed_file;
};

}