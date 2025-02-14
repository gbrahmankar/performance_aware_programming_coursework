#pragma once

#include "../common_utils.h"
#include "repetition_tester.h"

namespace PartThree
{

using ReadOverheadTestFunction = std::function<void(RepetitionTester& tester, Buffer::AllocationParams& params)>;

extern void readViaFRead(RepetitionTester& tester, Buffer::AllocationParams& params);
extern void writeToAllBytes(RepetitionTester& tester, Buffer::AllocationParams& params);

}
