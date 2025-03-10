package pap_common

import "base:intrinsics"
import "core:os"
import "core:sys/posix" 
import "core:time"

when ODIN_OS == .Windows {

get_invariant_tsc_freq :: proc() -> (u64, bool) {
}

get_invariant_tsc :: proc() -> (u64, bool) {
}

// .MacOS
} else {

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

get_pid :: proc() -> (posix.pid_t) {
    return posix.getpid()
}

get_page_faults :: proc() -> (minor_faults: int, major_faults: int) {
    usage: os.Rusage
    os.getrusage(os.RUSAGE_SELF, &usage)
    return int(usage.ru_minflt), int(usage.ru_majflt)
}

}
