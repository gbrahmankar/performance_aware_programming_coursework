#include "../common_utils.h"
#include "test.h"

namespace Test
{

void executeTest(int argc, char* argv[])
{
// os_timer starts
{
    u64 OSFreq = PlatformMetrics::GetOSTimerFreq();
    std::cout << "os_freq=" << OSFreq << '\n';	

	u64 OSStart = PlatformMetrics::ReadOSTimer();
	u64 OSEnd = 0;
	u64 OSElapsed = 0;
	while(OSElapsed < OSFreq)
	{
		OSEnd = PlatformMetrics::ReadOSTimer();
		OSElapsed = OSEnd - OSStart;
	}

    std::cout << "os_start="   << STREAM_16BIT_PRECISION_FP(OSStart) << " | " <<
                 "os_end="     << STREAM_16BIT_PRECISION_FP(OSEnd) << " | " <<
                 "os_elapsed=" << STREAM_16BIT_PRECISION_FP(OSElapsed) << '\n';

    std::cout << "os_secs=" << STREAM_16BIT_PRECISION_FP((f64)OSElapsed/(f64)OSFreq) << '\n';
}
// os_timer ends 

// cpu_timer starts
{
    u64 OSFreq = PlatformMetrics::GetOSTimerFreq();
    std::cout << "os_freq=" << OSFreq << '\n';	

	u64 CPUStart = PlatformMetrics::ReadCPUTimer();
	u64 OSStart = PlatformMetrics::ReadOSTimer();
	u64 OSEnd = 0;
	u64 OSElapsed = 0;
	while(OSElapsed < OSFreq)
	{
		OSEnd = PlatformMetrics::ReadOSTimer();
		OSElapsed = OSEnd - OSStart;
	}

	u64 CPUEnd = PlatformMetrics::ReadCPUTimer();
	u64 CPUElapsed = CPUEnd - CPUStart;

    std::cout << "os_start="   << STREAM_16BIT_PRECISION_FP(OSStart) << " | " <<
                 "os_end="     << STREAM_16BIT_PRECISION_FP(OSEnd) << " | " <<
                 "os_elapsed=" << STREAM_16BIT_PRECISION_FP(OSElapsed) << '\n';

    std::cout << "os_secs=" << STREAM_16BIT_PRECISION_FP((f64)OSElapsed/(f64)OSFreq) << '\n';
	f64 OSSecs = (f64)OSElapsed/(f64)OSFreq;

    std::cout << "cpu_start="   << STREAM_16BIT_PRECISION_FP(CPUStart) << " | " <<
                 "cpu_end="     << STREAM_16BIT_PRECISION_FP(CPUEnd) << " | " <<
                 "cpu_elapsed=" << STREAM_16BIT_PRECISION_FP(CPUElapsed) << '\n';

	u64 CPUFreq = static_cast<u64>((f64)CPUElapsed / OSSecs);
    std::cout << "cpu_freq=" << CPUFreq << '\n';
	std::cout << "estd_cpu_freq=" << PlatformMetrics::estimateCPUFrequency() << '\n';
}
// cpu_timer ends 

// custom_buffer starts
Buffer buffer = CONSTANT_STRING("gaurav");
std::cout << "is_buffer_equal=" << ((buffer.isEqual(CONSTANT_STRING("gaurav"))) ? "true" : "false") << 'n';
// custom_buffer ends

// read_page_faults starts 
{
    PlatformMetrics::initializeOSMetrics();

    u64 freq = PlatformMetrics::estimateCPUFrequency();

    u64 pageFaultsBeforeMalloc = PlatformMetrics::readOSPageFaultCount();

    Buffer destBuffer;
    destBuffer.allocateBuffer(1024*1024);

    u64 pageFaultsAfterMalloc = PlatformMetrics::readOSPageFaultCount();

    u64 time0 = PlatformMetrics::ReadCPUTimer();
    for(u64 index = 0; index < 1024*1024; ++index)
    {
        destBuffer.m_data[index] = (u8)index;
    }
    u64 time1 = PlatformMetrics::ReadCPUTimer();

    u64 pageFaultsAfterFirstWrite = PlatformMetrics::readOSPageFaultCount();

    std::cout << "time_elaspsed=" << (f64)(time1 - time0)/(f64)freq << '\n';

    time0 = PlatformMetrics::ReadCPUTimer();
    for(u64 index = 0; index < 1024*1024; ++index)
    {
        destBuffer.m_data[index] = (u8)index;
    }
    time1 = PlatformMetrics::ReadCPUTimer();

    u64 pageFaultsAfterSecondWrite = PlatformMetrics::readOSPageFaultCount();

    std::cout << "time_elaspsed=" << (f64)(time1 - time0)/(f64)freq << '\n';
    std::cout << "page_faults=" << pageFaultsAfterMalloc << " | " << pageFaultsAfterFirstWrite << " | " << pageFaultsAfterSecondWrite << '\n';
    std::cout << "page_faults=" << pageFaultsAfterFirstWrite - pageFaultsAfterMalloc << " | " << 
        pageFaultsAfterSecondWrite - pageFaultsAfterFirstWrite << '\n';
}
// read_page_faults ends 
}

}
