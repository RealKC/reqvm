#include "error_reporting.hpp"

#include <cstdio>

namespace reqvm {

auto report_to_user(level lvl, const std::string& msg) noexcept -> void {
    switch (lvl) {
    case level::info:
        std::printf("INFO: ");
        break;
    case level::warning:
        std::printf("WARNING: ");
        break;
    case level::error:
        std::printf("ERROR: ");
        break;
    case level::internal_assembler_error:
        std::printf("INTERNAL ASSEMBLER ERROR: ");
        break;
    }
    printf("%s\n", msg.c_str());
}

}   // namespace reqvm
