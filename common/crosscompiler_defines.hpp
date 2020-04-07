#pragma once

#if defined(__GNUC__)
#    define REQVM_COMMON_FUNCTION __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
#    define REQVM_COMMON_FUNCTION __FUNCSIG__
#else
#    define REQVM_COMMON_FUNCTION __func__
#endif
