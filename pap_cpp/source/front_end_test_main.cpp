#include "front_end_test_main.h"
#include "front_end_tests.h"

namespace PartThree
{

struct TestFunctionFrontEnd
{
    char const *name;
    FrontEndTestFunction func;
};

std::vector<TestFunctionFrontEnd> testFunctionsFrontEnd =
{
    {"write_to_all_bytes", writeToAllBytesFrontEnd},
    // {"mov_all_bytes", movAllBytes},
    // {"nop_all_bytes", nopAllBytes},
    // {"cmp_all_bytes", cmpAllBytes},
    // {"dec_all_bytes", decAllBytes}
};

void frontEndTestMain(int argc, char* argv[])
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

    for(u32 testFuncIndex = 0; testFuncIndex < testFunctionsFrontEnd.size(); ++testFuncIndex)
    {
        TestFunctionFrontEnd& testFunc = testFunctionsFrontEnd[testFuncIndex];

		RepetitionTester repetitionTester;
		printf("\n------%s_starts------\n", testFunc.name);
		repetitionTester.newTestWave(params.m_destinationBuffer->m_count, cpuTimerFreq);
		testFunc.func(repetitionTester, params);
		printf("------%s_ends-------\n", testFunc.name);
    }
}

}
