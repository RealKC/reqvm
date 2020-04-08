#include "error_reporting.hpp"

#include <iostream>

namespace reqvm {

auto report_to_user(level lvl, const std::string& msg) noexcept -> void {
    switch (lvl) {
    case level::info:
        std::cout << "INFO: ";
        break;
    case level::warning:
        std::cout << "WARNING: ";
        break;
    case level::error:
        std::cout << "ERROR: ";
        break;
    case level::internal_assembler_error:
        std::cout << "INTERNAL ASSEMBLER ERROR: ";
        break;
    }
    std::cout << msg;
}

}   // namespace reqvm
