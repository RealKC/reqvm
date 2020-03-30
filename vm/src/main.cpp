#include "../../common/unreachable.hpp"
#include "exceptions.hpp"
#include "io.hpp"
#include "vm.hpp"

#include <cstddef>
#include <cstdio>
#include <memory>

static constexpr auto panic = R"(
 __      ____  __   _____            _      
 \ \    / /  \/  | |  __ \          (_)     
  \ \  / /| \  / | | |__) |_ _ _ __  _  ___ 
   \ \/ / | |\/| | |  ___/ _` | '_ \| |/ __|
    \  /  | |  | | | |  | (_| | | | | | (__ 
     \/   |_|  |_| |_|   \__,_|_| |_|_|\___|
                                            

)";

auto main(int argc, char** argv) -> int try {
    if (argc < 2) {
        printf("usage: vm binary.reqvm");
        return EXIT_SUCCESS;
    }
    auto the_vm = reqvm::vm {argv[1]};
    return the_vm.run();
} catch (const reqvm::invalid_opcode& e) {
    std::puts(panic);
    std::puts("reqvm has encountered an error during the execution of your "
              "program.\n");
    std::printf("e.what(): %s %#x (%u)\n", e.what(),
                static_cast<unsigned int>(e.the_opcode()),
                static_cast<unsigned int>(e.the_opcode()));
    return EXIT_FAILURE;
} catch (const reqvm::invalid_register& e) {
    std::puts(panic);
    std::puts("reqvm has encountered an error during the execution of your "
              "program.\n");
    std::printf("e.what(): %s %#x (%u)\n", e.what(),
                static_cast<unsigned int>(e.the_register()),
                static_cast<unsigned int>(e.the_register()));
    return EXIT_FAILURE;
} catch (const reqvm::bad_argument& e) {
    std::puts(panic);
    std::puts("reqvm has encountered an error during the execution of your "
              "program.\n");
    std::printf("e.what(): %s\n", e.what());
    return EXIT_FAILURE;
} catch (const reqvm::vm_exception& e) {
    std::puts(panic);
    std::puts("reqvm has encountered an unhandled reqvm::vm_exception during "
              "the execution of your program. Please file an issue at "
              "https://github.com/RealKC/reqvm/issues as this is not meant to "
              "happen.\n");
    std::printf("e.what(): %s\n", e.what());
    return EXIT_FAILURE;
} catch (const reqvm::stack_error& e) {
    std::puts(panic);
    std::puts("reqvm has detected an illegal manipulation of the stack\n");
    std::printf("e.what(): %s\n", e.what());
    return EXIT_FAILURE;
} catch (const reqvm::preamble_error& e) {
    std::puts(panic);
    std::puts(
        "reqvm has an ecountered an issue with the format of your binary.\n");
    std::printf("e.what(): %s\n", e.what());
    return EXIT_FAILURE;
} catch (const reqvm::io::error& e) {
    std::puts(panic);
    std::puts("reqvm has encountered an issue during the execution of your "
              "binary.\n");
    std::printf("e.what(): %s %#x (%u)\n", e.what(),
                static_cast<unsigned int>(e.the_invalid_op()),
                static_cast<unsigned int>(e.the_invalid_op()));
    return EXIT_FAILURE;
#ifndef NDEBUG
} catch (const common::unreachable_code_reached& e) {
    std::puts(panic);
    std::puts(
        "reqvm has detected a critical issue in its code. Please file an issue"
        " at https://github.com/RealKC/reqvm/issues .\n");
    std::printf("e.what(): %s\n", e.what());
    return EXIT_FAILURE;
#endif
} catch (const std::exception& e) {
    std::puts(panic);
    std::puts("reqvm has encountered an error during the execution of your "
              "program.\n");
    std::printf("e.what(): %s\n", e.what());
    return EXIT_FAILURE;
}
