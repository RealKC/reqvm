#pragma once

#include "../../common/opcodes.hpp"

#include <cstdint>
#include <stdexcept>

namespace reqvm {
namespace io {

common::io_op parse_from_byte(std::uint8_t byte);

auto getc() -> std::uint64_t;
auto putc(std::uint64_t ch) -> void;
auto put8c(std::uint64_t chars) -> void;
auto putn(std::uint64_t num) -> void;

class error : public std::runtime_error {
public:
    // Delete default constructor to force a meaningful message
    error() = delete;
    error(const char* what_arg, std::uint8_t byte)
        : runtime_error {what_arg}, _op {byte} {};
    virtual ~error() noexcept = default;

    auto the_invalid_op() const noexcept -> std::uint64_t {
        return _op;
    }

private:
    std::uint8_t _op;
};

}   // namespace io
}   // namespace reqvm
