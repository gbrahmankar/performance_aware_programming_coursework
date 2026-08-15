#+feature using-stmt
#+build windows

package part_three

import "core:fmt"
import "core:mem"
import "core:mem/virtual"
import "core:os"

import "../pap_common"

foreign import cache_bw_tests "../asm/chapter15_cache_bw_tests.lib"

@(default_calling_convention="c")
foreign cache_bw_tests {
	cache_size_bw_test :: proc(count: u64, data: ^u8, mask: u64) -> u64 --- 
}

cache_size_bw_test_asm :: cache_size_bw_test

chapter15_cache_bw_tests :: proc(cpu_freq: u64, csv_style_prints: bool) {
	using pap_common

	total_byte_count: u64 = 1 * 1024 * 1024 * 1024
    byte_padding: u64 = 256
    total_byte_count_with_padding: u64 = total_byte_count + byte_padding 

    byte_sequence_slice, _ := virtual.reserve_and_commit(cast(uint)total_byte_count_with_padding)
    defer virtual.release(&byte_sequence_slice[0], cast(uint)total_byte_count_with_padding) 

    byte_sequence: ^u8 = cast(^u8)(&byte_sequence_slice[0])

    start_power: u64 = 10
    end_power: u64 = 30
    for mask_power in start_power..=end_power {
        byte_service_area_mask: u64 = cast(u64)(1) << mask_power
        byte_service_area_mask -= 1
        tsc0 := read_tsc()
        cache_size_bw_test_asm(total_byte_count, byte_sequence, byte_service_area_mask)
        tsc1 := read_tsc()
        time_to_load_from_service_area := compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq)
        bytes_per_second := cast(f64)total_byte_count / time_to_load_from_service_area 
        fmt.println("service_area =", mask_power, 
                    ", ts =", time_to_load_from_service_area, 
                    ", bps =", bytes_per_second)
    }
}