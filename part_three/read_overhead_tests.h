#pragma once

#include "../common_utils.h"
#include "repetition_tester.h"

namespace PartThree
{

struct ReadParameters
{
    Buffer m_destinationBuffer;
    char const *m_fileName;
};

extern void readViaFRead(RepetitionTester& tester, const ReadParameters& params);

}
