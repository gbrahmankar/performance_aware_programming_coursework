package pap_odin

import "core:fmt"
import "core:mem/virtual"
import "core:time"

import "pap_common"
import "part_three"

main :: proc() {
    using part_three
    using pap_common 

    cpu_freq, has_tsc := get_tsc_frequency()
    number_of_iterations: u64 = 1000000000

    tsc0 := read_tsc()
    nop_one_three_bytes_asm(number_of_iterations)
    tsc1 := read_tsc()
    fmt.println(compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

    tsc2 := read_tsc()
    nop_three_one_byte_asm(number_of_iterations)
    tsc3 := read_tsc()
    fmt.println(compute_seconds_from_cpu_time(tsc3-tsc2, cpu_freq))

    tsc4 := read_tsc()
    nop_nine_one_byte_asm(number_of_iterations)
    tsc5 := read_tsc()
    fmt.println(compute_seconds_from_cpu_time(tsc5-tsc4, cpu_freq))
}