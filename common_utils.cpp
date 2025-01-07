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

// platform_metrics_impl. start.
namespace PlatformMetrics
{

// timer_stat helpers. start.
// casey's helpers for profiling starts
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
// casey's helpers for profiling ends 

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

f64 secondsFromCPUTime(u64 cpuTime, u64 cpuTimerFreq)
{
    f64 result = 0.0;
    if(cpuTimerFreq)
    {
        result = (cpuTime / (f64)cpuTimerFreq);
    }

    return result;
}
// timer_stat helpers. end.

// os_metric_stat helpers. start.
OsMetrics g_globalOsMetrics;

#ifdef __APPLE__
u64 readOSPageFaultCount(void)
{
    if(!g_globalOsMetrics.m_initialized)
    {
        std::cerr << "error : failed to read_page_faults since process_handle not init" << '\n';
        return 1;
    }

    struct proc_taskinfo taskInfo;
    mach_msg_type_number_t infoCount = PROC_TASKINFO_COUNT;

    kern_return_t kr = proc_pidinfo(g_globalOsMetrics.processHandle, PROC_PIDTASKINFO, 0, &taskInfo, infoCount);

    if (kr != KERN_SUCCESS) 
    {
        std::cerr << "error : failed to get task_info for pid=" << g_globalOsMetrics.processHandle << '\n';
        return 1;
    }
/*
    std::cout << "task_info : cow=" << taskInfo.pti_cow_faults 
              << " | page_ins=" << static_cast<u64>(taskInfo.pti_pageins)
              << " | sys_calls_unix=" << static_cast<u64>(taskInfo.pti_syscalls_unix) 
              << " | sys_calls_mach=" << static_cast<u64>(taskInfo.pti_syscalls_mach) 
              << '\n'; 
*/

    return taskInfo.pti_faults;
}

void initializeOSMetrics(void)
{
    if(!g_globalOsMetrics.m_initialized)
    {
        g_globalOsMetrics.m_initialized = true;
        g_globalOsMetrics.processHandle = getpid();
    }
}
#elif _WIN32
// casey's helpers for os_metrics. start. 
u64 readOSPageFaultCount(void)
{
    if(!g_globalOsMetrics.m_initialized)
    {
        std::cerr << "error : failed to read_page_faults since process_handle not init" << '\n';
        return 1;
    }

    PROCESS_MEMORY_COUNTERS_EX MemoryCounters = {};
    MemoryCounters.cb = sizeof(MemoryCounters);
    GetProcessMemoryInfo(GlobalMetrics.ProcessHandle, (PROCESS_MEMORY_COUNTERS *)&MemoryCounters, sizeof(MemoryCounters));
    
    u64 Result = MemoryCounters.PageFaultCount;
    return Result;
}

void initializeOSMetrics(void)
{
    if(!g_globalOsMetrics.m_initialized)
    {
        g_globalOsMetrics.m_initialized = true;
        g_globalOsMetrics.processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, GetCurrentProcessId());
    }
}
// casey's helpers for os_metrics. end. 
#endif
// os_metric_stat helpers. end.

}
// platform_metrics_impl. end.

// profiler_impl starts 
namespace Profiler
{

// profiler_source that you want to be included only if profiling is turned on. start.
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

    m_startTSC = PlatformMetrics::ReadCPUTimer();
}

ProfileBlock::~ProfileBlock()
{
	u64 elapsed = PlatformMetrics::ReadCPUTimer() - m_startTSC;
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
// profiler_source that you want to be included only if profiling is turned on. end.

// profiler_source included even if we switch off profiling. start.
ProfilerData::ProfilerData()
	: m_startTSC{0},
	m_endTSC{0}
{
}
ProfilerData g_profilerData;

void beginProfile()
{
    g_profilerData.m_startTSC = PlatformMetrics::ReadCPUTimer();
}

void endAndPrintProfile()
{
    g_profilerData.m_endTSC = PlatformMetrics::ReadCPUTimer();
    u64 cpuFreq = PlatformMetrics::estimateCPUFrequency();
    
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
// profiler_source included even if we switch off profiling. end.

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

Buffer::Buffer(u64 size, u8* data, bool ownData)
{
    m_isDataOwned = ownData;
    m_data = data;

    m_count = size;
}

Buffer::Buffer(const Buffer& rhs)
{
    m_isDataOwned = false;
    m_data = rhs.m_data;

    m_count = rhs.m_count;
}

Buffer::~Buffer()
{
    freeBuffer();
}

Buffer& Buffer::operator=(const Buffer& rhs)
{
    if (this == &rhs)
    {
        return *this; 
    }

    m_isDataOwned = false;
    m_data = rhs.m_data;

    m_count = rhs.m_count;

    return *this;        
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
    if (size == 0)
    {
        return false;
    }

    if(freeBuffer())
    {
        m_data = (u8*)malloc(size);
        if (m_data != nullptr)
        {
            m_count = size;    
            m_isDataOwned = true;
            return true;
        }
        else
        {
            std::cerr << "failed to allocate block_size=" << size;
            return false;
        }
    }

    return false;
}

bool Buffer::freeBuffer()
{
    if (m_data == nullptr)
    {
        return true;
    }

    if(m_isDataOwned)
    {
        free(m_data);
        m_data = nullptr;
        m_count = 0;
        return true;
    }

    return false;
}

bool Buffer::deepCopyIntoSelf(const Buffer& rhs)
{
    if (m_count < rhs.m_count)
    {
        return false;    
    }

    memcpy(m_data, rhs.m_data, rhs.m_count);  
    return true;
}
// buffer_impl ends 
