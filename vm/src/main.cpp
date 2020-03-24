#include <cstddef>

#include "exceptions.hpp"

int main() try {

    return EXIT_SUCCESS;
} catch (const reqvm::invalid_opcode& e) {
    
    return EXIT_FAILURE;
} catch (const reqvm::invalid_register& e) {

    return EXIT_FAILURE;
} catch (const reqvm::vm_exception& e) {

    return EXIT_FAILURE;
} catch (const std::exception& e) {

    return EXIT_FAILURE;
}
