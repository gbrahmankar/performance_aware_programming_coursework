#include "include/decompose_x64_pointer.h"

namespace PartThree
{

void printDecomposedVirtualAddress(const VirtualAddressLayout& address)
{
    printf("|%3u|%3u|%3u|%3u|%10u|",
           address.m_pml4Index, address.m_directoryPtrIndex,
           address.m_directoryIndex, address.m_pageTableIndex,
           address.m_offset);
}

void printAsLine(char const *label, const VirtualAddressLayout& address)
{
    printf("%s", label);
    printDecomposedVirtualAddress(address);
    printf("\n");
}

VirtualAddressLayout decomposePointer4k(void *ptr)
{
    VirtualAddressLayout result = {};
    
    u64 address = (u64)ptr;
    result.m_pml4Index = ((address >> 39) & 0x1ff);
    result.m_directoryPtrIndex = ((address >> 30) & 0x1ff);
    result.m_directoryIndex = ((address >> 21) & 0x1ff);
    result.m_pageTableIndex = ((address >> 12) & 0x1ff);
    result.m_offset = ((address >> 0) & 0xfff);
    
    return result;
}

VirtualAddressLayout decomposePointer2MB(void *ptr)
{
    VirtualAddressLayout result = {};
    
    u64 address = (u64)ptr;
    result.m_pml4Index = ((address >> 39) & 0x1ff);
    result.m_directoryPtrIndex = ((address >> 30) & 0x1ff);
    result.m_directoryIndex = ((address >> 21) & 0x1ff);
    result.m_offset = ((address >> 0) & 0x1fffff);
    
    return result;
}

VirtualAddressLayout decomposePointer1GB(void *ptr)
{
    VirtualAddressLayout result = {};
    
    u64 address = (u64)ptr;
    result.m_pml4Index = ((address >> 39) & 0x1ff);
    result.m_directoryPtrIndex = ((address >> 30) & 0x1ff);
    result.m_offset = ((address >> 0) & 0x3fffffff);
    
    return result;
}

}
