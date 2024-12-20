#include "common_utils.h"

int8_t minI8 = INT8_MIN;
int8_t maxI8 = INT8_MAX;
uint8_t maxU8 = UINT8_MAX;

int16_t minI16 = INT16_MIN;
int16_t maxI16 = INT16_MAX;
uint16_t maxU16 = UINT16_MAX;

int32_t minI32 = INT32_MIN;
int32_t maxI32 = INT32_MAX;
uint32_t maxU32 = UINT32_MAX;

int64_t minI64 = INT64_MIN;
int64_t maxI64 = INT64_MAX;
uint64_t maxU64 = UINT64_MAX;

float minF32 = FLT_MIN;
float maxF32 = FLT_MAX;

double minF64 = DBL_MIN;
double maxF64 = DBL_MAX;

long double minF128 = LDBL_MIN;
long double maxF128 = LDBL_MAX;

namespace Profiler
{

#if _WIN32
u64 GetOSTimerFreq(void)
{
	LARGE_INTEGER Freq;
	QueryPerformanceFrequency(&Freq);
	return Freq.QuadPart;
}

u64 ReadOSTimer(void)
{
	LARGE_INTEGER Value;
	QueryPerformanceCounter(&Value);
	return Value.QuadPart;
}
#else
u64 GetOSTimerFreq(void)
{
	return 1000000;
}

u64 ReadOSTimer(void)
{
	struct timeval Value;
	gettimeofday(&Value, 0);

	u64 Result = GetOSTimerFreq()*(u64)Value.tv_sec + (u64)Value.tv_usec;
	return Result;
}
#endif

inline u64 ReadCPUTimer(void)
{
	return __rdtsc();
}

}
