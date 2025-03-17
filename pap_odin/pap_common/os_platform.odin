package pap_common

import "base:intrinsics"
import "core:os"
import "core:sys/windows"
import "core:time"

get_invariant_tsc_freq :: proc() -> (u64, bool) {
    return time.tsc_frequency()
}

get_invariant_tsc :: proc() -> (i64) {
    return intrinsics.read_cycle_counter()
}

seconds_from_cpu_time :: proc(cpu_time: u64, cpu_timer_freq: u64) -> (f64) {
    result: f64
    if cpu_timer_freq != 0 {
        result = cast(f64)cpu_time / cast(f64)cpu_timer_freq;
    }

    return result;
}

get_page_size :: proc() -> (int) {
    return os.get_page_size()
}

get_pid :: proc() -> (u32) {
    return windows.GetCurrentProcessId()  
}