#+build windows
package part_three

import "core:fmt"
import "core:mem"
import "core:mem/virtual"
import "core:os"

import "../pap_common"

foreign import non_pow_of_two_cache_bw_tests "../asm/chapter16_non_pow_of_two.lib"

@(default_calling_convention="c")
foreign non_pow_of_two_cache_bw_tests {
	non_pow_of_two_cache_bw_test :: proc(outer: u64, data: ^u8, inner: u64) -> u64 --- 
}

non_pow_of_two_cache_bw_test_asm :: non_pow_of_two_cache_bw_test 

chapter16_non_power_of_two :: proc(cpu_freq: u64, csv_style_prints: bool) {
	using pap_common

    total_byte_count: u64 = 1 * 1024 * 1024 * 1024
    byte_padding: u64 = 256
    total_byte_count_with_padding: u64 = total_byte_count + byte_padding 

    byte_sequence_slice, _ := virtual.reserve_and_commit(cast(uint)total_byte_count_with_padding)
    defer virtual.release(&byte_sequence_slice[0], cast(uint)total_byte_count_with_padding) 

    byte_sequence: ^u8 = cast(^u8)(&byte_sequence_slice[0])

    LOAD_BLOCK_SIZE :: 256
    ILLUSORY_SERVICE_AREA :: 1 * 1024 * 1024 * 1024
    for block_load_repeat_count in 120..=250 { 
        real_service_area: u64 = LOAD_BLOCK_SIZE * cast(u64)block_load_repeat_count
        outer_loop_count: u64 = ILLUSORY_SERVICE_AREA / real_service_area
        cropped_off_illusory_area: u64 = ILLUSORY_SERVICE_AREA % real_service_area
        actual_illusory_service_area: u64 = outer_loop_count * real_service_area

        tsc0 := read_tsc()
        non_pow_of_two_cache_bw_test_asm(outer_loop_count, byte_sequence, cast(u64)block_load_repeat_count)   
        tsc1 := read_tsc()
        time_to_load_from_service_area := compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq)
        bytes_per_second := cast(f64)actual_illusory_service_area / time_to_load_from_service_area 

        fmt.println("service_area =", real_service_area, 
                    ", ts =", time_to_load_from_service_area, 
                    ", bps =", bytes_per_second,
                    ", inner_loop_count =", block_load_repeat_count, 
                    ", outer_loop_count =", outer_loop_count, 
                    ", cropped_off_total_area =", cropped_off_illusory_area)
    }
}