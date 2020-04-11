/*
 *  MIT License
 *
 *  Copyright (c) 2020 Mitca Dumitru
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "../../common/unreachable.hpp"
#include "binary_managers/exceptions.hpp"
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

using std::printf;
using std::puts;
auto main(int argc, char** argv) -> int try {
    if (argc < 2) {
        printf("usage: vm binary.reqvm");
        return EXIT_SUCCESS;
    }
    auto the_vm = reqvm::vm {argv[1]};
    return the_vm.run();
} catch (const reqvm::invalid_opcode& e) {
    puts(panic);
    puts("reqvm has encountered an error during the execution of your "
         "program.\n");
    printf("e.what(): %s %#x (%u)\n", e.what(),
           static_cast<unsigned int>(e.the_opcode()),
           static_cast<unsigned int>(e.the_opcode()));
    return EXIT_FAILURE;
} catch (const reqvm::invalid_register& e) {
    puts(panic);
    puts("reqvm has encountered an error during the execution of your "
         "program.\n");
    printf("e.what(): %s %#x (%u)\n", e.what(),
           static_cast<unsigned int>(e.the_register()),
           static_cast<unsigned int>(e.the_register()));
    return EXIT_FAILURE;
} catch (const reqvm::bad_argument& e) {
    puts(panic);
    puts("reqvm has encountered an error during the execution of your "
         "program.\n");
    printf("e.what(): %s\n", e.what());
    return EXIT_FAILURE;
} catch (const reqvm::vm_exception& e) {
    puts(panic);
    puts("reqvm has encountered an unhandled reqvm::vm_exception during "
         "the execution of your program. Please file an issue at "
         "https://github.com/RealKC/reqvm/issues as this is not meant to "
         "happen.\n");
    printf("e.what(): %s\n", e.what());
    return EXIT_FAILURE;
} catch (const reqvm::stack_error& e) {
    puts(panic);
    puts("reqvm has detected an illegal manipulation of the stack\n");
    printf("e.what(): %s\n", e.what());
    return EXIT_FAILURE;
} catch (const reqvm::preamble_error& e) {
    puts(panic);
    puts("reqvm has an ecountered an issue with the format of your binary.\n");
    printf("e.what(): %s\n", e.what());
    return EXIT_FAILURE;
} catch (const reqvm::io::error& e) {
    puts(panic);
    puts("reqvm has encountered an issue during the execution of your "
         "binary.\n");
    printf("e.what(): %s %#x (%u)\n", e.what(),
           static_cast<unsigned int>(e.the_invalid_op()),
           static_cast<unsigned int>(e.the_invalid_op()));
    return EXIT_FAILURE;
} catch (const reqvm::mmap_error& e) {
    puts(panic);
    puts("reqvm has encountered an issue trying to open your binary.\n");
    printf("e.what(): %s\n", e.what());
    return EXIT_FAILURE;
#ifndef NDEBUG
} catch (const common::unreachable_code_reached& e) {
    puts(panic);
    puts("reqvm has detected a critical issue in its code. Please file an issue"
         " at https://github.com/RealKC/reqvm/issues .\n");
    printf("e.what(): %s\n", e.what());
    return EXIT_FAILURE;
#endif
} catch (const std::exception& e) {
    puts(panic);
    puts("reqvm has encountered an error during the execution of your "
         "program.\n");
    printf("e.what(): %s\n", e.what());
    return EXIT_FAILURE;
}
