package pap_common

import "core:crypto"
import "core:time"

// constants
ONE_KB :: 1 * 1024
ONE_MB :: ONE_KB * 1024
ONE_GB :: ONE_MB * 1024

PI64 :: 3.14159265358979323846264338327950288419716939937510582097494459230781640628

CACHE_LINE_SIZE :: 64
MEMORY_PAGE_SIZE :: 4096

// utils
get_random_u64 :: proc() -> (u64) {
    random_eight_u8s: [8]u8
    crypto.rand_bytes(random_eight_u8s[:])
    return (cast(^u64)&random_eight_u8s[0])^
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