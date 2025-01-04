#include "../common_utils.h"
#include "read_overhead_test_main.h"
#include "read_overhead_tests.h"
#include "repetition_tester.h"

namespace PartThree
{

void readOverheadTestMain(int argc, char* argv[])
{
    u64 cpuTimerFreq = Profiler::estimateCPUFrequency();

    char *fileName = argv[3];

#if _WIN32
    struct __stat64 statData;
    _stat64(fileName, &statData);
#else
    struct stat statData;
    stat(fileName, &statData);
#endif

    Buffer fileBuffer{ static_cast<u64>(statData.st_size) };
    if (fileBuffer.m_count == 0)
    {
        std::cerr << "test alloc failed" << '\n';
        return;
    }

    ReadParameters params;
    params.m_destinationBuffer = fileBuffer;
    params.m_fileName = fileName;

    for(u32 allocType = 1; allocType < 2; ++allocType)
    {
        params.m_allocType = (AllocationType)allocType;

        RepetitionTester repetitionTester;
        printf("\n------repeat_testing_%s%s%s_starts------\n",
               describeAllocationType((AllocationType)allocType),
               params.m_allocType ? "+" : "",
               "fread");
        repetitionTester.newTestWave(params.m_destinationBuffer.m_count, cpuTimerFreq);
        readViaFRead(repetitionTester, params);
        std::cout << "------repeat_testing_fread_ends------" << '\n';
    }
}

}
