#include "incremental_page_touching.h"

namespace PartThree
{

int executeIncrementalPageTouchTest(int argc, char **argv)
{
    PlatformMetrics::initializeOSMetrics();
    
    u64 pageSize = 4096;
    u64 pageCount = atol(argv[3]);
    u64 totalSize = pageSize * pageCount;
    
    printf("page_count, touch_count, fault_count, extra_faults\n");
    
    for(u64 touchCount = 0; touchCount <= pageCount; ++touchCount)
    {
        u64 touchSize = pageSize * touchCount;

        u8 * data = (u8 *)mmap(NULL, totalSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (data == MAP_FAILED)
        {
            perror("mmap failed");
            return EXIT_FAILURE;
        }

        u64 startFaultCount = PlatformMetrics::readOSPageFaultCount();
        for(u64 index = 0; index < touchSize; ++index)
        {
            data[index] = (u8)index;
        }
        u64 endFaultCount = PlatformMetrics::readOSPageFaultCount();
        
        u64 faultCount = endFaultCount - startFaultCount;
        
        printf("%llu, %llu, %llu, %lld\n", pageCount, touchCount, faultCount, (faultCount - touchCount));
        
        if (munmap(data, totalSize) != 0) 
        {
            perror("munmap failed");
            return EXIT_FAILURE;
        }
    }
    		
    return 0;
}

}
