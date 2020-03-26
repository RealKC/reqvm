#include "assembler.hpp"

#include <memory>
#include <cstdio>


int main(int argc, const char** argv) {
    if (argc < 2) {
        printf("Usage: assembler file (temp)");
    }
    auto the_assembler = std::make_unique<reqvm::assembler>(argv[1]);
    return the_assembler->run();
}