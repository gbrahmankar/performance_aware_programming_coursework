#include "read_overhead_tests.h"

namespace PartThree
{

void readViaFRead(RepetitionTester& tester, Buffer::AllocationParams& params)
{
    while(tester.isTesting())
    {
        FILE *file = fopen(params.m_fileName, "rb");
        if(!file)
        {
            tester.reportError("fopen failed");
        }

        Buffer destBuffer;
        destBuffer.handleAllocation(params);
        
        tester.beginTime();
        size_t result = fread(destBuffer.m_data, destBuffer.m_count, 1, file);
        tester.endTime();
        
        if(result == 1)
        {
            tester.countBytes(destBuffer.m_count);
        }
        else
        {
            tester.reportError("fread failed");
        }

        destBuffer.handleDeallocation(params);
        
        fclose(file);
    }
}

void writeToAllBytesReadOverhead(RepetitionTester& tester, Buffer::AllocationParams& params)
{
    while(tester.isTesting())
    {
        Buffer destBuffer;
        destBuffer.handleAllocation(params);

        tester.beginTime();
        for(u64 index = 0; index < destBuffer.m_count; ++index)
        {
            destBuffer.m_data[index] = (u8)index;
        }
        tester.endTime();

        tester.countBytes(destBuffer.m_count);

        destBuffer.handleDeallocation(params);
    }
}

}
