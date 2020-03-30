#include "io.hpp"

#include <cinttypes>
#include <cstdio>


namespace reqvm {
namespace io {

common::io_op parse_from_byte(std::uint8_t byte) {
    using common::io_op;
    switch (byte) {
    case 1:
        return io_op::getc;
    case 2:
        return io_op::putc;
    case 3:
        return io_op::put8c;
    case 4:
        return io_op::putn;
    default:
        throw error {"Unknown operation passed as argument to 'io': ", byte};
    }
}

auto getc() -> std::uint64_t {
    return static_cast<std::uint64_t>(std::fgetc(stdin));
}

auto putc(std::uint64_t ch) -> void {
    std::putc(static_cast<char>(ch), stdout);
}

auto put8c(std::uint64_t string) -> void {
    char chars[9] = {0};
    chars[0]      = static_cast<char>(string >> 56);
    chars[1]      = static_cast<char>((string << 8) >> 56);
    chars[2]      = static_cast<char>((string << 16) >> 56);
    chars[3]      = static_cast<char>((string << 24) >> 56);
    chars[4]      = static_cast<char>((string << 32) >> 56);
    chars[5]      = static_cast<char>((string << 40) >> 56);
    chars[6]      = static_cast<char>((string << 48) >> 56);
    chars[7]      = static_cast<char>((string << 56) >> 56);
    std::fprintf(stdout, "%s", chars);
}

auto putn(std::uint64_t num) -> void {
    std::fprintf(stdout, "%" PRId64, num);
}

}   // namespace io
}   // namespace reqvm