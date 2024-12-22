#pragma once

#include <bitset>
#include <cfloat>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <random>
#include <sstream>
#include <stdint.h>
#include <string>
#include <unordered_map>
#include <vector>

#if _WIN32
#include <intrin.h>
#include <windows.h>
#else
#include <x86intrin.h>
#include <sys/time.h>
#endif

#define STREAM_BYTE(X) "0x" << std::hex << std::setw(2) << std::setfill('0') << unsigned(X)
#define STREAM_WORD(X) "0x" << std::hex << std::setw(4) << std::setfill('0') << X
#define STREAM_16BIT_PRECISION_FP(X) std::left << std::fixed << std::setprecision(16) << X

using NibbleBitset = std::bitset<4>;
using ByteBitset = std::bitset<8>;
using WordBitset = std::bitset<16>;

using MnemonicBitset = std::bitset<8>;

constexpr std::size_t ONE_BYTE = 1;
constexpr std::size_t KILOBYTE_TO_BYTE = 1024 * ONE_BYTE;
constexpr std::size_t MEGABYTE_TO_BYTE = 1024 * KILOBYTE_TO_BYTE;
constexpr std::size_t GIGABYTE_TO_BYTE = 1024 * MEGABYTE_TO_BYTE;
constexpr std::size_t TERABYTE_TO_BYTE = 1024 * GIGABYTE_TO_BYTE;

using u8 = uint8_t;
using i8 = int8_t;
using u16 = uint16_t;
using i16 = int16_t;
using u32 = uint32_t;
using i32 = int32_t;
using u64 = uint64_t;
using i64 = int64_t;
using f32 = float;
using f64 = double;
using f128 = long double;

extern int8_t minI8;
extern int8_t maxI8;
extern uint8_t maxU8;

extern int16_t minI16;
extern int16_t maxI16;
extern uint16_t maxU16;

extern int32_t minI32;
extern int32_t maxI32;
extern uint32_t maxU32;

extern int64_t minI64;
extern int64_t maxI64;
extern uint64_t maxU64;

extern float minF32;
extern float maxF32;

extern double minF64;
extern double maxF64;

extern long double minF128;
extern long double maxF128;

namespace Profiler
{
// casey's code starts
#if _WIN32
extern u64 GetOSTimerFreq(void);
extern u64 ReadOSTimer(void);
#else
extern u64 GetOSTimerFreq(void);
extern u64 ReadOSTimer(void);
#endif

extern u64 ReadCPUTimer(void);
// casey's code ends 

extern u64 estimateCPUFrequency(void);
}
