#pragma once
#include "../common_utils.h"
#include "repetition_tester.h"

namespace PartThree
{

using FrontEndTestFunction = std::function<void(RepetitionTester& tester, Buffer::AllocationParams& params)>;

void frontEndTestMain(int argc, char* argv[]);

}
