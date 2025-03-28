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
    number_of_iterations: u64 = 10000000000 

    all_bytes, err := virtual.reserve_and_commit(cast(uint)number_of_iterations)
    for &byte_view, i in all_bytes[:] {
        byte_view = cast(u8)(i % 5)  
    }
    data: ^u8 = cast(^u8)&all_bytes[0]

    tsc6 := read_tsc()
    try_byte_data_based_branching_asm(number_of_iterations, data)
    tsc7 := read_tsc()
    fmt.println(compute_seconds_from_cpu_time(tsc7-tsc6, cpu_freq))

    /* ------------------------nop_pressure---------------------------------
    tsc0 := read_tsc()
    nop_one_three_bytes_asm(number_of_iterations, data)
    tsc1 := read_tsc()
    fmt.println(compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

    tsc2 := read_tsc()
    nop_three_one_byte_asm(number_of_iterations, data)
    tsc3 := read_tsc()
    fmt.println(compute_seconds_from_cpu_time(tsc3-tsc2, cpu_freq))

    tsc4 := read_tsc()
    nop_nine_one_byte_asm(number_of_iterations, data)
    tsc5 := read_tsc()
    fmt.println(compute_seconds_from_cpu_time(tsc5-tsc4, cpu_freq))
    ------------------------------------------------------------------------*/
}