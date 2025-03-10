package pap_odin

import "core:fmt"

import "pap_common"

main :: proc() {
    using pap_common

    tsc_freq, is_invariant_tsc_present := get_invariant_tsc_freq()
    if is_invariant_tsc_present == true {
        fmt.printf("cpu_freq=%d\n", tsc_freq)
    }

    fmt.printf("cpu_time=%d\n", get_invariant_tsc())
    fmt.printf("secs=%f\n", seconds_from_cpu_time(cast(u64)get_invariant_tsc(), tsc_freq))
    fmt.printf("page_size=%d\n", get_page_size())
    fmt.printf("pid=%d\n", get_pid())
}
