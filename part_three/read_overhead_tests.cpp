#include "read_overhead_tests.h"

namespace PartThree
{

char const *describeAllocationType(AllocationType allocType)
{
    char const *result;
    switch(allocType)
    {
        case AllocTypeNone : 
            {
                result = "";
                break;
            } 
        case AllocTypeMalloc : 
            {
                result = "malloc";
                break;
            } 
        default : 
            {
                result = "UNKNOWN";
                break;
            } 
    }

    return result;
}

void handleAllocation(const ReadParameters& params, Buffer& buffer)
{
    switch(params.m_allocType)
    {
        case AllocTypeNone :
        {
        } break;
        
        case AllocTypeMalloc :
        {
            buffer.allocateBuffer(params.m_destinationBuffer.m_count);
        } break;
        
        default :
        {
            std::cerr << "error : unrecognized alloc_type\n";
        } break;
    }
}

void handleDeallocation(const ReadParameters& params, Buffer& buffer)
{
    switch(params.m_allocType)
    {
        case AllocTypeNone:
        {
        } break;
        
        case AllocTypeMalloc:
        {
            buffer.~Buffer();
        } break;
        
        default:
        {
            std::cerr << "error : unrecognized alloc_type\n";
        } break;
    }
}

void readViaFRead(RepetitionTester& tester, ReadParameters& params)
{
    Buffer destBuffer;
    destBuffer.transferOwnershipIntoSelf(params.m_destinationBuffer);

    while(tester.isTesting())
    {
        FILE *file = fopen(params.m_fileName, "rb");
        if(!file)
        {
            tester.reportError("fopen failed");
        }

        handleAllocation(params, destBuffer);
        
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

        handleDeallocation(params, destBuffer);
        
        fclose(file);
    }
}

}
