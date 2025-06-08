#pragma once

#include "include/common_utils.h"
#include "include/repetition_tester.h"

namespace PartThree
{

extern void readViaFRead(RepetitionTester& tester, Buffer::AllocationParams& params);
extern void writeToAllBytesReadOverhead(RepetitionTester& tester, Buffer::AllocationParams& params);

}
