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

// casey's code starts
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
// casey's code ends 

u64 estimateCPUFrequency(void)
{
	u64 OSFreq = GetOSTimerFreq();

	u64 CPUStart = ReadCPUTimer();
	u64 OSStart = ReadOSTimer();
	u64 OSEnd = 0;
	u64 OSElapsed = 0;
	while(OSElapsed < OSFreq)
	{
		OSEnd = ReadOSTimer();
		OSElapsed = OSEnd - OSStart;
	}

	u64 CPUEnd = ReadCPUTimer();
	u64 CPUElapsed = CPUEnd - CPUStart;

	f64 OSSecs = (f64)OSElapsed/(f64)OSFreq;

	return static_cast<u64>((f64)CPUElapsed / OSSecs);
}

ProfilerData::ProfilerData()
	: m_startTSC{0},
	m_endTSC{0}
{
	m_anchors.resize(1024);
}

ProfilerData g_profilerData;
u16 g_currentlyActiveAnchorIndex;

ProfileBlock::ProfileBlock(const std::string& label, u16 anchorIndex)
{
	m_parentAnchorIndex = g_currentlyActiveAnchorIndex;

    m_label = label;
    m_anchorIndex = anchorIndex;

	g_currentlyActiveAnchorIndex = m_anchorIndex;

    m_startTSC = ReadCPUTimer();
}

ProfileBlock::~ProfileBlock()
{
	u64 elapsed = ReadCPUTimer() - m_startTSC;
	g_currentlyActiveAnchorIndex = m_parentAnchorIndex;

	ProfileAnchor& anchor = g_profilerData.m_anchors[m_anchorIndex];
	ProfileAnchor& parentAnchor = g_profilerData.m_anchors[m_parentAnchorIndex];

	anchor.m_tscElapsed += elapsed;
	++anchor.m_hitCount;

	parentAnchor.m_tscElapsedChildren += elapsed;

	anchor.m_label = m_label;
}

void printTimeElapsed(u64 totalTSCElapsed, const ProfileAnchor& anchor)
{
    u64 elapsed = anchor.m_tscElapsed - anchor.m_tscElapsedChildren;
    f64 percent = 100.0 * ((f64)elapsed / (f64)totalTSCElapsed);
	std::cout << anchor.m_label << "[" << anchor.m_hitCount << "]" 
		<< " : elapsed=" << elapsed 
		<< " | percent=" << std::setprecision(2) << percent;

	if (anchor.m_tscElapsedChildren > 0)
	{
		f64 percentWithChildren = 100.0 * ((f64)anchor.m_tscElapsed / (f64)totalTSCElapsed);
		std::cout << " | percentWithChildren=" << std::setprecision(2) << percentWithChildren;
	}

	std::cout << '\n';
}

void beginProfile()
{
    g_profilerData.m_startTSC = ReadCPUTimer();
}

void endAndPrintProfile()
{
    g_profilerData.m_endTSC = ReadCPUTimer();
    u64 cpuFreq = estimateCPUFrequency();
    
    u64 totalCPUElapsed = g_profilerData.m_endTSC - g_profilerData.m_startTSC;
    
    if(cpuFreq)
    {
		std::cout << "total_time=" << std::setprecision(4) << 1000.0 * (f64)totalCPUElapsed / (f64)cpuFreq << "ms" 
			<< " | cpu_freq=" << cpuFreq << '\n';;
    }
    
    for(u16 anchorIndex = 0; anchorIndex < g_profilerData.m_anchors.size(); ++anchorIndex)
    {
		ProfileAnchor& anchor = g_profilerData.m_anchors[anchorIndex];
        if(anchor.m_tscElapsed)
        {
            printTimeElapsed(totalCPUElapsed, anchor);
        }
    }
}

}
