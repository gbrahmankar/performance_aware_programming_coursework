package pap_odin

import "core:fmt"
import "core:time"

import "pap_common"

main :: proc() {
    using pap_common 

    cpu_freq, has_tsc := get_tsc_frequency()

    tsc0 := read_tsc()
    sleep_for_duration(3.14)
    tsc1 := read_tsc()

    fmt.println(compute_seconds_from_cpu_time(tsc1 - tsc0, cpu_freq))
}