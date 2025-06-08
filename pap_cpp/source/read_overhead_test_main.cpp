#include "include/read_overhead_test_main.h"
#include "include/read_overhead_tests.h"

namespace PartThree
{

struct TestFunctionReadOverhead
{
    char const *name;
    ReadOverheadTestFunction func;
};

std::vector<TestFunctionReadOverhead> testFunctions =
{
    {"read_via_fread", readViaFRead}
};

void readOverheadTestMain(int argc, char* argv[])
{
    initializeOSPlatform();
    u64 cpuTimerFreq = estimateCPUFrequency();

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

    Buffer::AllocationParams params;
    params.m_destinationBuffer = &fileBuffer;
    params.m_fileName = fileName;

    for(u32 testFuncIndex = 0; testFuncIndex < testFunctions.size(); ++testFuncIndex)
    {
        TestFunctionReadOverhead& testFunc = testFunctions[testFuncIndex];

        for(u32 allocType = 0; allocType < Buffer::AllocTypeCount; ++allocType)
        {
            params.m_allocType = (Buffer::AllocationType)allocType;

            RepetitionTester repetitionTester;
            printf("\n------repeat_testing_%s%s%s_starts------\n",
                   Buffer::describeAllocationType((Buffer::AllocationType)allocType),
                   params.m_allocType ? "+" : "",
                   testFunc.name);
            repetitionTester.newTestWave(params.m_destinationBuffer->m_count, cpuTimerFreq);
            testFunc.func(repetitionTester, params);
            printf("------repeat_testing_%s%s%s_ends-------\n",
                   Buffer::describeAllocationType((Buffer::AllocationType)allocType),
                   params.m_allocType ? "+" : "",
                   testFunc.name);
        }
    }
}

}
