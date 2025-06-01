package pap_common

import "core:os"
import "core:sys/windows"
import "core:time"

// winx64_abi 
Asm_Interface_Function :: struct {
    test_name: string,
    test_func: proc "c" (rcx: u64, rdx: ^u8, r8: u64, r9: ^u8) -> u64    
}

// os_platform helpers
get_page_size :: proc() -> (int) {
    return os.get_page_size()
}

get_pid :: proc() -> (u32) {
    return windows.GetCurrentProcessId()  
}

read_page_faults :: proc() -> (u32) {
    memory_counters: PROCESS_MEMORY_COUNTERS_EX
    memory_counters.cb = size_of(memory_counters)
    GetProcessMemoryInfoW(windows.GetCurrentProcess(), cast(^PROCESS_MEMORY_COUNTERS_EX)&memory_counters, size_of(memory_counters))
    return memory_counters.PageFaultCount
}

// timer helpers
read_tsc :: proc() -> (u64) {
    return time.read_cycle_counter()     
}

get_tsc_frequency :: proc() -> (u64, bool) {
    return time.tsc_frequency()     
}

compute_seconds_from_cpu_time :: proc(cpu_time: u64, cpu_timer_freq: u64) -> (f64) {
    result: f64
    if cpu_timer_freq != 0 {
        result = cast(f64)cpu_time / cast(f64)cpu_timer_freq;
    }

    return result;
}

get_ns_from_secs :: proc(secs: f64) -> (u64) {
    return cast(u64)(secs * 1000000000)
}

sleep_for_duration :: proc(secs: f64) {
    time.accurate_sleep(cast(time.Duration)get_ns_from_secs(secs))
}
