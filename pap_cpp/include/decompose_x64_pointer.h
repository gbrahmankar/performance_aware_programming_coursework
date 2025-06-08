#pragma once

#include "include/common_utils.h"

namespace PartThree
{

struct VirtualAddressLayout
{
    u16 m_pml4Index; // level_0
    u16 m_directoryPtrIndex; // level_1
    u16 m_directoryIndex; // level_2
    u16 m_pageTableIndex; // level_3
    u32 m_offset; // offset in the final page. this will contain the physical address.
};

extern void printDecomposedVirtualAddress(const VirtualAddressLayout& address);
extern void printAsLine(char const *label, const VirtualAddressLayout& address);

extern VirtualAddressLayout decomposePointer4k(void *ptr);
extern VirtualAddressLayout decomposePointer2MB(void *ptr);
extern VirtualAddressLayout decomposePointer1GB(void *ptr);

}
