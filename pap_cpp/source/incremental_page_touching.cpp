#include "decompose_x64_pointer.h"
#include "incremental_page_touching.h"

namespace PartThree
{

int executeIncrementalPageTouchTest(int argc, char **argv)
{
    initializeOSPlatform();
    
    u64 pageSize = 4096;
    u64 pageCount = atol(argv[3]);
    u64 totalSize = pageSize * pageCount;
    
    printf("page_count, touch_count, fault_count, extra_faults, directory_index, table_index\n");
    
    for(u64 touchCount = 0; touchCount <= pageCount; ++touchCount)
    {
        u64 touchSize = pageSize * touchCount;

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

        u64 startFaultCount = readOSPageFaultCount();
        for(u64 index = 0; index < touchSize; ++index)
        {
            data[index] = (u8)index;
        }
        u64 endFaultCount = readOSPageFaultCount();
        
        u64 faultCount = endFaultCount - startFaultCount;
        
        printf("%llu, %llu, %llu, %lld\n", pageCount, touchCount, faultCount, (faultCount - touchCount));

	    VirtualAddressLayout address = decomposePointer4k(data);
		if(touchSize)
		{
			address = decomposePointer4k(data + touchSize - 1);
		}
                
		printf("%llu, %llu, %llu, %lld, %u, %u\n", pageCount, touchCount, faultCount, (faultCount - touchCount),
			   address.m_directoryIndex, address.m_pageTableIndex); 
        
#if __APPLE__
        if (munmap(data, totalSize) != 0) 
        {
            perror("munmap failed");
            return EXIT_FAILURE;
        }
#elif _WIN32
         VirtualFree(data, 0, MEM_RELEASE);
#endif

    }
    		
    return 0;
}

}
