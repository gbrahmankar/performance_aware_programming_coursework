#+build windows

package pap_common 

import "core:sys/windows"

foreign import psapi "system:Psapi.lib"

@(default_calling_convention="system")
foreign psapi {
    GetProcessMemoryInfo :: proc(hProcess: windows.HANDLE, ppsmemCounters: PPROCESS_MEMORY_COUNTERS, cb: windows.DWORD) -> windows.BOOL ---
}

GetProcessMemoryInfoW :: GetProcessMemoryInfo

PROCESS_MEMORY_COUNTERS :: struct {
    cb:                         windows.DWORD,
    PageFaultCount:             windows.DWORD,
    PeakWorkingSetSize:         windows.SIZE_T,
    WorkingSetSize:             windows.SIZE_T,
    QuotaPeakPagedPoolUsage:    windows.SIZE_T,
    QuotaPagedPoolUsage:        windows.SIZE_T,
    QuotaPeakNonPagedPoolUsage: windows.SIZE_T,
    QuotaNonPagedPoolUsage:     windows.SIZE_T,
    PagefileUsage:              windows.SIZE_T,
    PeakPagefileUsage:          windows.SIZE_T,
}

PROCESS_MEMORY_COUNTERS_EX :: struct {
    using counter: PROCESS_MEMORY_COUNTERS,
    PrivateUsage:  windows.SIZE_T,
}

PROCESS_MEMORY_COUNTERS_EX2 :: struct {
    using counter_ex:      PROCESS_MEMORY_COUNTERS_EX,
    PrivateWorkingSetSize: windows.SIZE_T ,
    SharedCommitUsage:     windows.ULONG64,
}

PPROCESS_MEMORY_COUNTERS :: ^PROCESS_MEMORY_COUNTERS

// winx64_abi 
Asm_Interface_Function :: struct {
    test_name: string,
    test_func: proc "c" (rcx: u64, rdx: ^u8, r8: u64, r9: ^u8) -> u64    
}

// general
get_pid :: proc() -> (u32) {
    return windows.GetCurrentProcessId()  
}

read_page_faults :: proc() -> (u32) {
    memory_counters: PROCESS_MEMORY_COUNTERS_EX
    memory_counters.cb = size_of(memory_counters)
    GetProcessMemoryInfoW(windows.GetCurrentProcess(), cast(^PROCESS_MEMORY_COUNTERS_EX)&memory_counters, size_of(memory_counters))
    return memory_counters.PageFaultCount
}