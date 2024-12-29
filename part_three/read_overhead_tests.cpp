#include "read_overhead_tests.h"

namespace PartThree
{

void ReadViaFRead(RepetitionTester& tester, const ReadParameters& params)
{
    while(tester.isTesting())
    {
        FILE *file = fopen(params.m_fileName, "rb");
        if(!file)
        {
            tester.reportError("fopen failed");
        }

        Buffer destBuffer = params.m_destinationBuffer;
        
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
        
        fclose(file);
    }
}

}
