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