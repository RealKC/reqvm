#include "assembler.hpp"

#include <cstdio>
#include <memory>

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: assembler file (temp)");
    }
    auto the_assembler = std::make_unique<reqvm::assembler>(argv[1]);
    return the_assembler->run();
}