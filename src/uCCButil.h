#ifndef UCCBUTIL_HPP
#define UCCBUTIL_HPP

#include <cstdint>
#include <string>

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

std::string NumToHexString(uint32 val, uint8 minLength = 4);
uint32 HexStringToNum(std::string hex);

#endif //UCCBUTIL_HPP