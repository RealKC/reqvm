#include <cstddef>
#include <cstdio>

#include "exceptions.hpp"
#include "vm.hpp"

static constexpr auto panic = R"(
 __      ____  __   _____            _      
 \ \    / /  \/  | |  __ \          (_)     
  \ \  / /| \  / | | |__) |_ _ _ __  _  ___ 
   \ \/ / | |\/| | |  ___/ _` | '_ \| |/ __|
    \  /  | |  | | | |  | (_| | | | | | (__ 
     \/   |_|  |_| |_|   \__,_|_| |_|_|\___|
                                            
   
)";

int main() try {
    reqvm::vm the_vm;
    auto ret = the_vm.run();
    return ret;
} catch (const reqvm::invalid_opcode& e) {
    std::puts(panic);
    std::puts("reqvm has encountered an error during the execution of your program.\n");
    std::printf(
        "e.what(): %s %#x (%u)\n",
        e.what(),
        static_cast<unsigned int>(e.the_opcode()),
        static_cast<unsigned int>(e.the_opcode()));
    return EXIT_FAILURE;
} catch (const reqvm::invalid_register& e) {
    std::puts(panic);
    std::puts("reqvm has encountered an error during the execution of your program.\n");
    std::printf(
        "e.what(): %s %#x (%u)\n", 
        e.what(),
        static_cast<unsigned int>(e.the_register()),
        static_cast<unsigned int>(e.the_register()));
    return EXIT_FAILURE;
} catch (const reqvm::bad_argument& e) {
    std::puts(panic);
    std::puts("reqvm has encountered an error during the execution of your program.\n");
    std::printf("e.what(): %s\n", e.what());
    return EXIT_FAILURE;
} catch (const reqvm::vm_exception& e) {
    std::puts(panic);
    std::puts(
        "reqvm has encountered an unhandled reqvm::vm_exception during "
        "the execution of your program. Please file an issue at "
        "https://github.com/RealKC/reqvm/issues as this is not meant to happen.\n");
    std::printf("e.what(): %s\n", e.what());
    return EXIT_FAILURE;
} catch (const std::exception& e) {
    std::puts(panic);
    std::puts("reqvm has encountered an error during the execution of your program.\n");
    std::printf("e.what(): %s\n", e.what());
    return EXIT_FAILURE;
}
