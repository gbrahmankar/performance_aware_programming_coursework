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

    total_byte_count: u64 = 1 * 1024 * 1024 * 1024 
    all_bytes, err := virtual.reserve_and_commit(cast(uint)total_byte_count)
    for &byte_view, i in all_bytes {
        byte_view = cast(u8)i
    }
    data: ^u8 = cast(^u8)&all_bytes[0]

    /* --------------------------chapter16_non_pow_of_two_cache_bw_tests------------------------
    ------------------------------------------------------------------------------------------*/

    LOAD_BLOCK_SIZE :: 256
    ILLUSORY_SERVICE_AREA :: 1 * 1024 * 1024 * 1024
    for block_load_repeat_count in 120..=250 { 
        real_service_area: u64 = LOAD_BLOCK_SIZE * cast(u64)block_load_repeat_count
        outer_loop_count: u64 = ILLUSORY_SERVICE_AREA / real_service_area
        actual_illusory_service_area: u64 = outer_loop_count * real_service_area

        tsc0 := read_tsc()
        non_pow_of_two_cache_bw_test_asm(outer_loop_count, data, cast(u64)block_load_repeat_count)   
        tsc1 := read_tsc()
        time_to_load_from_service_area := compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq)
        bytes_per_second := cast(f64)actual_illusory_service_area / time_to_load_from_service_area 
        fmt.println("service_area =", real_service_area, 
                    ", ts =", time_to_load_from_service_area, 
                    ", inner_loop_count =", block_load_repeat_count, 
                    ", outer_loop_count =", outer_loop_count, 
                    ", bps =", bytes_per_second)
    }

    /* --------------------------chapter15_cache_and_bw_tests-----------------------------------
    start_power: u64 = 10
    end_power: u64 = 30
    for mask_power in start_power..=end_power {
        byte_service_area_mask: u64 = cast(u64)(1) << mask_power
        byte_service_area_mask -= 1
        tsc0 := read_tsc()
        cache_size_bw_test_asm(total_byte_count, data, byte_service_area_mask)
        tsc1 := read_tsc()
        time_to_load_from_service_area := compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq)
        bytes_per_second := cast(f64)total_byte_count / time_to_load_from_service_area 
        fmt.println("service_area =", mask_power, 
                    ", ts =", time_to_load_from_service_area, 
                    ", bps =", bytes_per_second)
    }
    ------------------------------------------------------------------------------------------*/

    /* --------------------------chapter14_simd-----------------------------------
    tsc0 := read_tsc()
    load_four_bytes_three_times_per_loop_asm(total_byte_count, data) 
    tsc1 := read_tsc()
    fmt.println("load_four_bytes_three_times_per_loop = ", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

    tsc0 = read_tsc()
    load_eight_bytes_three_times_per_loop_asm(total_byte_count, data) 
    tsc1 = read_tsc()
    fmt.println("load_eight_bytes_three_times_per_loop = ", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

    tsc0 = read_tsc()
    load_sixteen_bytes_three_times_per_loop_asm(total_byte_count, data) 
    tsc1 = read_tsc()
    fmt.println("load_sixteen_bytes_three_times_per_loop = ", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

    tsc0 = read_tsc()
    load_thirty_two_bytes_three_times_per_loop_asm(total_byte_count, data) 
    tsc1 = read_tsc(    )
    fmt.println("load_thirty_two_bytes_three_times_per_loop = ", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))
    ----------------------------------------------------------------------------*/

    /* --------------------------exec_ports-----------------------------------
    tsc0 := read_tsc()
    one_load_per_loop_asm(total_byte_count, data) 
    tsc1 := read_tsc()
    fmt.println("one_load_per_loop = ", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

    tsc0 = read_tsc()
    two_loads_per_loop_asm(total_byte_count, data) 
    tsc1 = read_tsc()
    fmt.println("two_loads_per_loop = ", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

    tsc0 = read_tsc()
    three_loads_per_loop_asm(total_byte_count, data) 
    tsc1 = read_tsc()
    fmt.println("three_loads_per_loop = ", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

    tsc0 = read_tsc()
    four_loads_per_loop_asm(total_byte_count, data) 
    tsc1 = read_tsc()
    fmt.println("four_loads_per_loop = ", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

    tsc0 = read_tsc()
    five_loads_per_loop_asm(total_byte_count, data) 
    tsc1 = read_tsc()
    fmt.println("five_loads_per_loop = ", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

    tsc0 = read_tsc()
    one_store_per_loop_asm(total_byte_count, data) 
    tsc1 = read_tsc()
    fmt.println("one_store_per_loop = ", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

    tsc0 = read_tsc()
    two_stores_per_loop_asm(total_byte_count, data) 
    tsc1 = read_tsc()
    fmt.println("two_stores_per_loop = ", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

    tsc0 = read_tsc()
    three_stores_per_loop_asm(total_byte_count, data) 
    tsc1 = read_tsc()
    fmt.println("three_stores_per_loop = ", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

    tsc0 = read_tsc()
    four_stores_per_loop_asm(total_byte_count, data) 
    tsc1 = read_tsc()
    fmt.println("four_stores_per_loop = ", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

    tsc0 = read_tsc()
    five_stores_per_loop_asm(total_byte_count, data) 
    tsc1 = read_tsc()
    fmt.println("five_stores_per_loop = ", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))
    ------------------------------------------------------------------------*/

    /* ------------------------rat_dependency_analysis------------------------
    tsc0 := read_tsc()
    rat_add_asm(total_byte_count) 
    tsc1 := read_tsc()
    fmt.println("rat_add = ", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

    tsc0 = read_tsc()
    rat_mov_add_asm(total_byte_count) 
    tsc1 = read_tsc()
    fmt.println("rat_mov_add = ", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))
    ------------------------------------------------------------------------*/

    /* --------------------------code_alignment_tests-------------------------
    tsc0 := read_tsc()
    aligned_at_64_bytes_asm(total_byte_count, data) 
    tsc1 := read_tsc()
    fmt.println("aligned_at_64_bytes_asm = ", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

    data = cast(^u8)&all_bytes[0]
    tsc0 = read_tsc()
    offset_by_1_nop_asm(total_byte_count, data) 
    tsc1 = read_tsc()
    fmt.println("offset_by_1_nop_asm = ", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

    data = cast(^u8)&all_bytes[0]
    tsc0 = read_tsc()
    offset_by_15_nops_asm(total_byte_count, data) 
    tsc1 = read_tsc()
    fmt.println("offset_by_15_nops_asm = ", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

    data = cast(^u8)&all_bytes[0]
    tsc0 = read_tsc()
    offset_by_31_nops_asm(total_byte_count, data) 
    tsc1 = read_tsc()
    fmt.println("offset_by_31_nops_asm = ", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

    data = cast(^u8)&all_bytes[0]
    tsc0 = read_tsc()
    offset_by_63_nops_asm(total_byte_count, data) 
    tsc1 = read_tsc()
    fmt.println("offset_by_63_nops_asm = ", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))
    ------------------------------------------------------------------------*/

    /* --------------------------bp_pressure----------------------------------
    lenovo_loq_results :
    always_taken = 0.49508963746081219
    never_taken = 0.26138778113452477
    every_2 =  0.6604545476145216
    every_3 =  0.4305665305168544
    every_4 =  0.68952228243110858
    os_random =  3.1684721444964636
    crt_random =  3.1744982892599047
    --------------------------------------------------------------------------

    fill_with_branch_pattern(.Always_Taken, all_bytes)
    data = cast(^u8)&all_bytes[0]
    tsc0 := read_tsc()
    try_byte_data_based_branching_asm(total_byte_count, data)
    tsc1 := read_tsc()
    fmt.println("always_taken =", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

    fill_with_branch_pattern(.Never_Taken, all_bytes)
    data = cast(^u8)&all_bytes[0]
    tsc0 = read_tsc()
    try_byte_data_based_branching_asm(total_byte_count, data)
    tsc1 = read_tsc()
    fmt.println("never_taken =", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

    fill_with_branch_pattern(.Every_2, all_bytes)
    data = cast(^u8)&all_bytes[0]
    tsc0 = read_tsc()
    try_byte_data_based_branching_asm(total_byte_count, data)
    tsc1 = read_tsc()
    fmt.println("every_2 = ", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

    fill_with_branch_pattern(.Every_3, all_bytes)
    data = cast(^u8)&all_bytes[0]
    tsc0 = read_tsc()
    try_byte_data_based_branching_asm(total_byte_count, data)
    tsc1 = read_tsc()
    fmt.println("every_3 = ", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

    fill_with_branch_pattern(.Every_4, all_bytes)
    data = cast(^u8)&all_bytes[0]
    tsc0 = read_tsc()
    try_byte_data_based_branching_asm(total_byte_count, data)
    tsc1 = read_tsc()
    fmt.println("every_4 = ", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

    fill_with_branch_pattern(.Os_Random, all_bytes)
    data = cast(^u8)&all_bytes[0]
    tsc0 = read_tsc()
    try_byte_data_based_branching_asm(total_byte_count, data)
    tsc1 = read_tsc()
    fmt.println("os_random = ", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

    fill_with_branch_pattern(.Crt_Random, all_bytes)
    data = cast(^u8)&all_bytes[0]
    tsc0 = read_tsc()
    try_byte_data_based_branching_asm(total_byte_count, data)
    tsc1 = read_tsc()
    fmt.println("crt_random = ", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))
    ------------------------------------------------------------------------*/
 
    /* --------------------------nop_pressure---------------------------------
    tsc0 := read_tsc()
    nop_one_three_bytes_asm(total_byte_count, data)
    tsc1 := read_tsc()
    fmt.println(compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

    tsc2 := read_tsc()
    nop_three_one_byte_asm(total_byte_count, data)
    tsc3 := read_tsc()
    fmt.println(compute_seconds_from_cpu_time(tsc3-tsc2, cpu_freq))

    tsc4 := read_tsc()
    nop_nine_one_byte_asm(total_byte_count, data)
    tsc5 := read_tsc()
    fmt.println(compute_seconds_from_cpu_time(tsc5-tsc4, cpu_freq))
    ------------------------------------------------------------------------*/
}