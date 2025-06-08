#include "include/backwards_page_touching.h"
#include "include/decompose_x64_pointer.h"

namespace PartThree
{

int executeBackwardsPageTouchTest(int argc, char **argv)
{
    initializeOSPlatform();
    
    u64 pageSize = 4096;
    u64 pageCount = 16384;
    u64 totalSize = pageSize * pageCount;

#if __APPLE__
    u8 * data = (u8 *)mmap(NULL, totalSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (data == MAP_FAILED)
    {
        perror("mmap failed");
        return EXIT_FAILURE;
    }
#elif _WIN32
	u8 *data = (u8 *)VirtualAlloc(0, totalSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
#endif

    printAsLine("buffer_base=", decomposePointer4k((void*)data));

    u64 startFaultCount = readOSPageFaultCount();

    u64 priorOverFaultCount = 0;
    u64 priorPageIndex = 0;

    for (u64 pageIndex = 0; pageIndex < pageCount; ++pageIndex)
    {
        data[totalSize - 1 - pageSize*pageIndex] = (u8)pageIndex;
        u64 endFaultCount = readOSPageFaultCount();

        u64 overFaultCount = (endFaultCount - startFaultCount) - pageIndex;
        if (overFaultCount > priorOverFaultCount)
        {
            printf("page %llu: %llu extra faults (%llu pages since last increase)\n",
                   pageIndex, overFaultCount, (pageIndex - priorPageIndex));
            if(pageIndex > 0)
            {
                printAsLine("     previous_pointer=", decomposePointer4k(data + totalSize - 1 - pageSize*(pageIndex - 1)));
            }
            printAsLine("         this_pointer=", decomposePointer4k(data + totalSize - 1 - pageSize*pageIndex));

            priorOverFaultCount = overFaultCount;
            priorPageIndex = pageIndex;
        }
    }

#if __APPLE__
        if (munmap(data, totalSize) != 0) 
        {
            perror("munmap failed");
            return EXIT_FAILURE;
        }
#elif _WIN32
         VirtualFree(data, 0, MEM_RELEASE);
#endif
    
    return 0;
}

}
