#pragma once

#include "../common_utils.h"
#include "repetition_tester.h"

namespace PartThree
{

enum AllocationType
{
    AllocTypeNone,
    AllocTypeMalloc,

    AllocTypeCount,
};

struct ReadParameters
{
    AllocationType m_allocType;
    Buffer m_destinationBuffer;
    char const *m_fileName;
};

using ReadOverheadTestFunction = std::function<void(RepetitionTester& tester, ReadParameters& params)>;

extern char const* describeAllocationType(AllocationType allocType);
extern void handleAllocation(const ReadParameters& params, Buffer& buffer);
extern void handleDeallocation(const ReadParameters& params, Buffer& buffer);

extern void readViaFRead(RepetitionTester& tester, ReadParameters& params);
extern void writeToAllBytes(RepetitionTester& tester, ReadParameters& params);

}
