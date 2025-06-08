#pragma once
#include "include/common_utils.h"
#include "include/repetition_tester.h"

namespace PartThree
{

using FrontEndTestFunction = std::function<void(RepetitionTester& tester, Buffer::AllocationParams& params)>;

void frontEndTestMain(int argc, char* argv[]);

}
