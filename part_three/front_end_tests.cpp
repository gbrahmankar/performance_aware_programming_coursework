#include "front_end_tests.h"

namespace PartThree
{

void writeToAllBytesFrontEnd(RepetitionTester& tester, Buffer::AllocationParams& params)
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

void movAllBytes(RepetitionTester& tester, Buffer::AllocationParams& params)
{
    while(tester.isTesting())
    {
        Buffer destBuffer;
        destBuffer.handleAllocation(params);

        tester.beginTime();
        movAllBytesAsm(destBuffer.m_count, destBuffer.m_data);
        tester.endTime();

        tester.countBytes(destBuffer.m_count);

        destBuffer.handleDeallocation(params);
    }
}

void nopAllBytes(RepetitionTester& tester, Buffer::AllocationParams& params)
{
    while(tester.isTesting())
    {
        Buffer destBuffer;
        destBuffer.handleAllocation(params);

        tester.beginTime();
        nopAllBytesAsm(destBuffer.m_count);
        tester.endTime();

        tester.countBytes(destBuffer.m_count);

        destBuffer.handleDeallocation(params);
    }
}

void cmpAllBytes(RepetitionTester& tester, Buffer::AllocationParams& params)
{
    while(tester.isTesting())
    {
        Buffer destBuffer;
        destBuffer.handleAllocation(params);

        tester.beginTime();
        cmpAllBytesAsm(destBuffer.m_count);
        tester.endTime();

        tester.countBytes(destBuffer.m_count);

        destBuffer.handleDeallocation(params);
    }
}

void decAllBytes(RepetitionTester& tester, Buffer::AllocationParams& params)
{
    while(tester.isTesting())
    {
        Buffer destBuffer;
        destBuffer.handleAllocation(params);

        tester.beginTime();
        decAllBytesAsm(destBuffer.m_count);
        tester.endTime();

        tester.countBytes(destBuffer.m_count);

        destBuffer.handleDeallocation(params);
    }
}

}
