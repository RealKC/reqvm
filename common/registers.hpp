#pragma once 

#include <cstdint>

namespace common {

enum class registers : std::uint8_t{
    pc   =  0,
    sp   =  1,

    // general purpose registers
    gp00 =  64,
    gp01 =  65,
    gp02 =  66,
    gp03 =  67,
    gp04 =  68,
    gp05 =  69,
    gp06 =  70,
    gp07 =  71,
    gp08 =  72,
    gp09 =  73,
    gp10 =  74,
    gp11 =  75,
    gp12 =  76,
    gp13 =  77,
    gp14 =  78,
    gp15 =  79,
    gp16 =  80,
    gp17 =  81,
    gp18 =  82,
    gp19 =  83,
    gp20 =  84,
    gp21 =  85,
    gp22 =  86,
    gp23 =  87,
    gp24 =  88,
    gp25 =  89,
    gp26 =  90,
    gp27 =  91,
    gp28 =  92,
    gp29 =  93,
    gp30 =  94,
    gp31 =  95,
    gp32 =  96,
    gp33 =  97,
    gp34 =  98,
    gp35 =  99,
    gp36 = 100,
    gp37 = 101,
    gp38 = 102,
    gp39 = 103,
    gp40 = 104,
    gp41 = 105,
    gp42 = 106,
    gp43 = 107,
    gp44 = 108,
    gp45 = 109,
    gp46 = 110,
    gp47 = 111,
    gp48 = 112,
    gp49 = 113,
    gp50 = 114,
    gp51 = 115,
    gp52 = 116,
    gp53 = 117,
    gp54 = 118,
    gp55 = 119,
    gp56 = 120,
    gp57 = 121,
    gp58 = 122,
    gp59 = 123,
    gp60 = 124,
    gp61 = 125,
    gp62 = 126,
    gp63 = 127,

    // Registers related to functions
    ire   = 128,

    ifa00 = 132,
    ifa01 = 133,
    ifa02 = 134,
    ifa03 = 135,
    ifa04 = 136,
    ifa05 = 137,
    ifa06 = 138,
    ifa07 = 139,
    ifa08 = 140,
    ifa09 = 141,
    ifa10 = 142,
    ifa11 = 143,
    ifa12 = 144,
    ifa13 = 145,
    ifa14 = 146,
    ifa15 = 147
};

constexpr bool operator==(registers lhs, registers rhs) noexcept {
    return static_cast<unsigned char>(lhs) == static_cast<unsigned char>(rhs);
}

constexpr bool operator!=(registers lhs, registers rhs) noexcept {
    return !(lhs == rhs);
}

constexpr bool operator<(registers lhs, registers rhs) noexcept {
    return static_cast<unsigned char>(lhs) < static_cast<unsigned char>(rhs);
}

constexpr bool operator>(registers lhs, registers rhs) noexcept {
    return rhs < lhs;
}

constexpr bool operator<=(registers lhs, registers rhs) noexcept {
    return !(lhs > rhs);
}

constexpr bool operator>=(registers lhs, registers rhs) noexcept {
    return !(lhs < rhs);
}

}