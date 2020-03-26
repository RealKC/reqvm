#include "assembler.hpp"

#include <memory>

int main() {
    auto the_assembler = std::make_unique<reqvm::assembler>();
    return the_assembler->run();
}