#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <unordered_map>

namespace reqvm {

class assembler {
public:
    assembler() = default;
    assembler(std::string file_name) : file_name{file_name} {}
    ~assembler() noexcept = default;

    // Runs every stage of the assembler over the file
    auto run()        -> int;
    auto preprocess() -> int;
    auto assemble()   -> int;

private:
    auto emit_macro() -> void;
    auto emit_label() -> void;
    auto emit_op()    -> void;

    std::vector<char> output;
    std::string file_name;
    std::string preprocessed_file;
};

}