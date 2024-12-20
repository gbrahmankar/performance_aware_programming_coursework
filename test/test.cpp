#include "../common_utils.h"
#include "test.h"

namespace Test
{

void executeTest(int argc, char* argv[])
{
// os_timer starts
{
    u64 OSFreq = Profiler::GetOSTimerFreq();
    std::cout << "os_freq=" << OSFreq << '\n';	

	u64 OSStart = Profiler::ReadOSTimer();
	u64 OSEnd = 0;
	u64 OSElapsed = 0;
	while(OSElapsed < OSFreq)
	{
		OSEnd = Profiler::ReadOSTimer();
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
    u64 OSFreq = Profiler::GetOSTimerFreq();
    std::cout << "os_freq=" << OSFreq << '\n';	

	u64 CPUStart = Profiler::ReadCPUTimer();
	u64 OSStart = Profiler::ReadOSTimer();
	u64 OSEnd = 0;
	u64 OSElapsed = 0;
	while(OSElapsed < OSFreq)
	{
		OSEnd = Profiler::ReadOSTimer();
		OSElapsed = OSEnd - OSStart;
	}

	u64 CPUEnd = Profiler::ReadCPUTimer();
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
}
// cpu_timer ends 
}

}
