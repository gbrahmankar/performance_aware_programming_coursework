#include "common_utils.h"

// size_limits_defn starts 
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
// size_limits_defn ends 

// profiler_impl starts 
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

#if PROFILER

std::array<ProfileAnchor, 1024> g_profilerAnchors;
u16 g_currentlyActiveAnchorIndex;

ProfileBlock::ProfileBlock(const std::string& label, u16 anchorIndex, u64 byteCount)
{
	m_parentAnchorIndex = g_currentlyActiveAnchorIndex;

    m_label = label;
    m_anchorIndex = anchorIndex;

	g_currentlyActiveAnchorIndex = m_anchorIndex;

	ProfileAnchor& anchor = g_profilerAnchors[m_anchorIndex];
	m_oldTSCElapsedInclusive = anchor.m_tscElapsedInclusive;
    anchor.m_processedByteCount += byteCount;

    m_startTSC = ReadCPUTimer();
}

ProfileBlock::~ProfileBlock()
{
	u64 elapsed = ReadCPUTimer() - m_startTSC;
	g_currentlyActiveAnchorIndex = m_parentAnchorIndex;

	ProfileAnchor& anchor = g_profilerAnchors[m_anchorIndex];
	ProfileAnchor& parentAnchor = g_profilerAnchors[m_parentAnchorIndex];

	parentAnchor.m_tscElapsedExclusive -= elapsed;
	anchor.m_tscElapsedExclusive += elapsed;
	anchor.m_tscElapsedInclusive = m_oldTSCElapsedInclusive + elapsed;

	++anchor.m_hitCount;
	anchor.m_label = m_label;
}

void printTimeElapsed(u64 totalTSCElapsed, u64 cpuTimerFreq, const ProfileAnchor& anchor)
{
    f64 percent = 100.0 * ((f64)anchor.m_tscElapsedExclusive / (f64)totalTSCElapsed);
	std::cout << anchor.m_label << "[" << anchor.m_hitCount << "]" 
		<< " : exclusive_elapsed=" << anchor.m_tscElapsedExclusive 
		<< " | exclusive_percent=" << std::setprecision(2) << percent;

	if (anchor.m_tscElapsedInclusive != anchor.m_tscElapsedExclusive)
	{
		f64 percentWithChildren = 100.0 * ((f64)anchor.m_tscElapsedInclusive / (f64)totalTSCElapsed);
		std::cout << " | inclusive_percent=" << std::setprecision(2) << percentWithChildren;
	}

    if (anchor.m_processedByteCount > 0)
    {
        f64 seconds = (f64)anchor.m_tscElapsedInclusive / (f64)cpuTimerFreq;
        f64 bytesPerSecs = (f64)anchor.m_processedByteCount / seconds;
        f64 megaBytes = (f64)anchor.m_processedByteCount / MEGABYTE_TO_BYTE;
        f64 gigaBytesPerSecs = bytesPerSecs / GIGABYTE_TO_BYTE;
        
        std::cout << " | processed " << std::setprecision(3) << megaBytes << "mbs at " << gigaBytesPerSecs << "gb/s";
    }

	std::cout << '\n';
}

void printAnchorData(u64 totalCPUElapsed, u64 cpuTimerFreq)
{
	for(u16 anchorIndex = 0; anchorIndex < g_profilerAnchors.size(); ++anchorIndex)
    {
		ProfileAnchor& anchor = g_profilerAnchors[anchorIndex];
        if(anchor.m_tscElapsedInclusive > 0)
        {
            printTimeElapsed(totalCPUElapsed, cpuTimerFreq, anchor);
        }
    }
}

#endif

ProfilerData::ProfilerData()
	: m_startTSC{0},
	m_endTSC{0}
{
}
ProfilerData g_profilerData;

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
		std::cout << "-----------------------------------profiling_info_starts--------------------------------------\n";
		std::cout << "total_time=" << std::setprecision(4) << 1000.0 * (f64)totalCPUElapsed / (f64)cpuFreq << "ms" 
			<< " | cpu_timer_freq=" << cpuFreq << "\n\n";
    } 

	printAnchorData(totalCPUElapsed, cpuFreq);
	std::cout << "-----------------------------------profiling_info_ends----------------------------------------\n";
}

}
// profiler_impl ends 

// buffer_impl starts
Buffer::Buffer(u64 size)
{
    if (size > 0)
    {
        allocateBuffer(size); 
    }
}

Buffer::~Buffer()
{
    freeBuffer();
}

bool Buffer::isInBounds(u64 index)
{
    if (index < m_count)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Buffer::isEqual(const Buffer& rhs)
{
    if (rhs.m_count != m_count)
    {
        return false;
    }

    for (u64 index = 0; index < m_count; ++index)
    {
        if (m_data[index] != rhs.m_data[index])
        {
            return false;
        }
    }

    return true;
}

bool Buffer::allocateBuffer(u64 size)
{
    if (size > 0)
    {
        m_data = (u8*)malloc(size);
        if (m_data != nullptr)
        {
            m_count = size;    
            return true;
        }
        else
        {
            std::cerr << "failed to allocate block_size=" << size;
            return false;
        }
    }

    return true;
}

void Buffer::freeBuffer()
{
    free(m_data);

    m_data = nullptr;
    m_count = 0;
}
// buffer_impl ends 
