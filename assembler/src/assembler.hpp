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
    int run();
    int preprocess();
    int assemble();

private:
    void emit_macro();
    void emit_label();
    void emit_op();

    std::vector<char> output;
    std::string file_name;
    std::string preprocessed_file;
};

}