#pragma once

#ifdef __APPLE__
extern "C" 
{
#include <libproc.h>
#include <sys/proc_info.h>
#include <mach/mach.h>
#include <mach/mach_init.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <x86intrin.h>
}

#define PROC_TASKINFO_COUNT (sizeof(struct proc_taskinfo) / sizeof(uint32_t))
#endif

#if _WIN32
#define _CRT_SECURE_NO_WARNINGS 
#include <intrin.h>
#include <windows.h>
#endif

#include <array>
#include <bitset>
#include <cfloat>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <memory>
#include <random>
#include <sstream>
#include <stdint.h>
#include <string>
#include <unordered_map>
#include <vector>

// common_defines start
#define STREAM_BYTE(X) "0x" << std::hex << std::setw(2) << std::setfill('0') << unsigned(X)
#define STREAM_WORD(X) "0x" << std::hex << std::setw(4) << std::setfill('0') << X
#define STREAM_16BIT_PRECISION_FP(X) std::left << std::fixed << std::setprecision(16) << X
#define CONCAT_IMPL(x, y) x##y
#define CONCAT(x, y) CONCAT_IMPL(x, y)
#define ARRAY_COUNT(arr) (sizeof(arr) / sizeof(arr[0]))
// common_defines end

// profiler_defines start
#ifndef PROFILER
#define PROFILER 0
#endif

#if PROFILER
#define TimeBandwidth(Label, BytesProcessed) Profiler::ProfileBlock CONCAT(block, __LINE__) (Label, __COUNTER__ + 1, BytesProcessed)
#else
#define TimeBandwidth(Label, BytesProcessed)
#define printAnchorData(...)
#endif

#define TimeBlock(Label) TimeBandwidth(Label, 0)
#define TimeFunction TimeBlock(__func__)
// profiler_defines end 

// size_conversions start
constexpr std::size_t ONE_BYTE = 1;
constexpr std::size_t KILOBYTE_TO_BYTE = 1024 * ONE_BYTE;
constexpr std::size_t MEGABYTE_TO_BYTE = 1024 * KILOBYTE_TO_BYTE;
constexpr std::size_t GIGABYTE_TO_BYTE = 1024 * MEGABYTE_TO_BYTE;
constexpr std::size_t TERABYTE_TO_BYTE = 1024 * GIGABYTE_TO_BYTE;
// size_conversions end 

// size_typedefs start
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
// size_typedefs end 

// size_limits start
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
// size_limits end 

// platform_metrics_decl. start.
namespace PlatformMetrics
{

// timer_stat helpers. start.
// casey's helpers for profiling starts 
#if _WIN32
extern u64 GetOSTimerFreq(void);
extern u64 ReadOSTimer(void);
#else
extern u64 GetOSTimerFreq(void);
extern u64 ReadOSTimer(void);
#endif

extern u64 ReadCPUTimer(void);
// casey's helpers for profiling ends 

extern u64 estimateCPUFrequency(void);
extern f64 secondsFromCPUTime(u64 cpuTime, u64 cpuTimerFreq);
// timer_stat helpers. end.

// os_metric_stat helpers. start.
#ifdef __APPLE__
struct OsMetrics
{
    bool m_initialized;
    pid_t processHandle;
};
#elif _WIN32
struct OsMetrics
{
    bool m_initialized;
    HANDLE processHandle;
};
#endif
extern OsMetrics g_globalOsMetrics;

extern void initializeOSMetrics(void);
extern u64 readOSPageFaultCount(void);
// os_metric_stat helpers. end.

}
// platform_metrics_decl. end.

// profiler_decl starts 
namespace Profiler
{

// profiler_source that you want to be included only if profiling is turned on. start.
#if PROFILER
struct ProfileAnchor
{
    u64 m_tscElapsedExclusive;
    u64 m_tscElapsedInclusive;

    u64 m_hitCount;
    u64 m_processedByteCount;

    std::string m_label;
};
extern std::array<ProfileAnchor, 1024> g_profilerAnchors;
extern u16 g_currentlyActiveAnchorIndex;

struct ProfileBlock 
{
    ProfileBlock(const std::string& label, u16 anchorIndex, u64 byteCount);
    ~ProfileBlock();

    u16 m_anchorIndex = maxU16;
    u16 m_parentAnchorIndex = maxU16;
    std::string m_label;
    
    u64 m_startTSC;
    u64 m_oldTSCElapsedInclusive;
};

extern void printTimeElapsed(u64 totalTSCElapsed, u64 cpuTimerFreq, const ProfileAnchor& anchor);
extern void printAnchorData(u64 totalCPUElapsed, u64 cpuTimerFreq);
#endif
// profiler_source that you want to be included only if profiling is turned on. end.

// profiler_source included even if we switch off profiling. start.
struct ProfilerData
{
    ProfilerData();

    u64 m_startTSC;
    u64 m_endTSC;
};
extern ProfilerData g_profilerData;

extern void beginProfile();
extern void endAndPrintProfile();
// profiler_source included even if we switch off profiling. end.

}
// profiler_decl ends 

// buffer_decl starts 
struct Buffer
{
    Buffer(u64 size = 0);
    Buffer(u64 size, u8* data, bool ownData = false);
    Buffer(const Buffer& rhs);
    ~Buffer();

    Buffer& operator=(const Buffer& rhs);

    bool isInBounds(u64 index);
    bool isEqual(const Buffer& rhs);

    bool allocateBuffer(u64 size);
    bool freeBuffer();

    bool deepCopyIntoSelf(const Buffer& rhs);

    u64 m_count = 0;

    u8* m_data = nullptr;
    bool m_isDataOwned = true;
};

#define CONSTANT_STRING(String) {sizeof(String) - 1, (u8 *)(String)}
// buffer_decl ends 
