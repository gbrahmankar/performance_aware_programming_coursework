#include "../common_utils.h"
#include "read_overhead_test_main.h"
#include "read_overhead_tests.h"

namespace PartThree
{

void readOverheadTestMain(int argc, char* argv[])
{
    u64 cpuFreq = Profiler::estimateCPUFrequency();

    char *fileName = argv[3];

#if _WIN32
    struct __stat64 statData;
    _stat64(fileName, &statData);
#else
    struct stat statData;
    stat(fileName, &statData);
#endif

    Buffer fileBuffer{ static_cast<u64>(statData.st_size) };
}

}
