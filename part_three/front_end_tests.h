#pragma once

#include "../common_utils.h"
#include "repetition_tester.h"

namespace PartThree
{

using FrontEndTestFunction = std::function<void(RepetitionTester& tester, Buffer::AllocationParams& params)>;

extern "C" void movAllBytesAsm(u64 count, u8 *data);
extern "C" void nopAllBytesAsm(u64 count);
extern "C" void cmpAllBytesAsm(u64 count);
extern "C" void decAllBytesAsm(u64 count);
#if _WIN32
#pragma comment (lib, "front_end_test_assembly")
#endif

extern void movAllBytes(RepetitionTester& tester, Buffer::AllocationParams& params);
extern void nopAllBytes(RepetitionTester& tester, Buffer::AllocationParams& params);
extern void cmpAllBytes(RepetitionTester& tester, Buffer::AllocationParams& params);
extern void decAllBytes(RepetitionTester& tester, Buffer::AllocationParams& params);
extern void writeToAllBytesFrontEnd(RepetitionTester& tester, Buffer::AllocationParams& params);

}
