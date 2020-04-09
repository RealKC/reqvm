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

#include "assembler.hpp"

#include <cstdio>
#include <memory>

auto print_self_info() noexcept -> void;
auto print_thirdparty_licenses() noexcept -> void;

auto main(int argc, char** argv) -> int {
    print_self_info();

    if (argc < 2) {
        printf("Usage: assembler file (temp)");
    }

    // FIXME: actually parse command line arguments and only print this when
    //        an argument asking for it is passed.
    if (argc == 3) {
        print_thirdparty_licenses();
    }

    auto the_assembler = reqvm::assembler {argv[1]};
    return the_assembler.run();
}

auto print_self_info() noexcept -> void {
    constexpr char info[] = R"(
The reqvm assembler.
Copyright (c) Mitca Dumitru 2020-present. (MIT license)

Pass --help or -h for help (not implemented yet).
Pass --licenses to see third party licenses. (Not implemented yet, shows for 3 arguments).
)";

    std::puts(info);
}

auto print_thirdparty_licenses() noexcept -> void {
    const char* libs[] = {
        "magic enum\nCopyright (c) 2019 - 2020 Daniil Goncharov\nLicense: "
        "MIT.\n",
    };
    const char* licenses[] = {
        R"(MIT License

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.)",
    };
    for (auto& lib : libs) {
        printf("%s\n", lib);
    }
    for (auto& license : licenses) {
        printf("%s\n\n", license);
    }
}
