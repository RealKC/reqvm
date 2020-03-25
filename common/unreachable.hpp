#pragma once

#include <stdexcept>

namespace common {

class unreachable_code_reached : public std::runtime_error {
public:
    unreachable_code_reached(const char* what_arg) : runtime_error{what_arg} {}
    virtual ~unreachable_code_reached() noexcept = default;
};

}

#ifndef UNREACHABLE
#   ifdef NDEBUG
#       ifdef _MSC_VER
#           define UNREACHABLE(msg) __asume(0)
#       elif defined(__GNUC__)
#           define UNREACHABLE(msg) __builtin_unreachable()
#       else
#           define UNREACHABLE(msg)
#       endif
#   else
#       define UNREACHABLE(msg) throw common::unreachable_code_reached{msg}
#   endif
#endif
