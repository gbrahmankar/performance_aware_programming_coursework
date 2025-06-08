#pragma once

#include "include/common_utils.h"
#include "include/repetition_tester.h"

namespace PartThree
{

using FrontEndTestFunction = std::function<void(RepetitionTester& tester, Buffer::AllocationParams& params)>;


#if _WIN32
/*
in git_bash :
> nasm.exe -f win64 -g part_three/front_end_tests.asm -o fet_win64.obj

in vs_dev_cmd :
> lib.exe fet_win64.obj /out:performance_aware_programming_vsproj\fet_win64.lib
*/
extern "C" void _movAllBytesAsm(u64 count, u8 *data);
extern "C" void _nopAllBytesAsm(u64 count);
extern "C" void _cmpAllBytesAsm(u64 count);
extern "C" void _decAllBytesAsm(u64 count);
#pragma comment (lib, "fet_win64")
#else
/*
> nasm.exe -f macho64 -g part_three/front_end_tests.asm -o fet_macho64.lib
*/
extern "C" void movAllBytesAsm(u64 count, u8 *data);
extern "C" void nopAllBytesAsm(u64 count);
extern "C" void cmpAllBytesAsm(u64 count);
extern "C" void decAllBytesAsm(u64 count);
#endif

extern void movAllBytes(RepetitionTester& tester, Buffer::AllocationParams& params);
extern void nopAllBytes(RepetitionTester& tester, Buffer::AllocationParams& params);
extern void cmpAllBytes(RepetitionTester& tester, Buffer::AllocationParams& params);
extern void decAllBytes(RepetitionTester& tester, Buffer::AllocationParams& params);
extern void writeToAllBytesFrontEnd(RepetitionTester& tester, Buffer::AllocationParams& params);

}
