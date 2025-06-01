#+build windows
package part_three

import "core:fmt"
import "core:mem"
import "core:mem/virtual"
import "core:os"

import "../pap_common"

foreign import unaligned_load_penalties "../asm/chapter18_unaligned_load_penalties.lib"

@(default_calling_convention="c")
foreign unaligned_load_penalties {
	load_penalty_test :: proc(outer: u64, data: ^u8, inner: u64) -> u64 --- 
}

load_penalty_test_asm :: load_penalty_test 

/* --------------------------chapter18_unaligned_load_penalties-----------------------------
    modern x64_cache_structure -> my lenovo_loq is a 8_way set_associative, 32kb cache =>
    [---64_bytes---] : this is a cache_line (this is a standard for modern x64).
    [---8*[cache_line]---] : this is a cache_set. this is where that "8_way" comes in. 
    => 8 * 64 = there are 512_bytes in a cache_set.
    32kb/number_of_cache_sets = 512.
    => number_of_cache_sets = 64 in the L1 of my pc.

    if we unalign our memory_requests, we potentially incur 2 type of penalties :
    a) more cache_lines have to move through the memory_subsystem to satisfy the request.
    b) the loading/combining units have to do more work when the cache_lines
       arrive in the core. they have to combine potentially multiple cache_lines to 
       fill up the register involved in the memory_request.
------------------------------------------------------------------------------------------*/

chapter18_unaligned_load_penalties :: proc(cpu_freq: u64, csv_style_prints: bool) {
    using pap_common

    total_byte_count: u64 = pap_common.ONE_GB 
    byte_padding: u64 = 256
    total_byte_count_with_padding: u64 = total_byte_count + byte_padding 

    byte_sequence_slice, _ := virtual.reserve_and_commit(cast(uint)total_byte_count_with_padding)
    defer virtual.release(&byte_sequence_slice[0], cast(uint)total_byte_count_with_padding) 

    LOAD_BLOCK_SIZE :: 256
    ILLUSORY_SERVICE_AREA :: 1 * 1024 * 1024 * 1024
    for block_load_repeat_count in 120..=250 {
        real_service_area: u64 = LOAD_BLOCK_SIZE * cast(u64)block_load_repeat_count
        outer_loop_count: u64 = ILLUSORY_SERVICE_AREA / real_service_area
        cropped_off_illusory_area: u64 = ILLUSORY_SERVICE_AREA % real_service_area
        actual_illusory_service_area: u64 = outer_loop_count * real_service_area

        fmt.println("service_area =", real_service_area, 
            ", inner_loop_count =", block_load_repeat_count, 
            ", outer_loop_count =", outer_loop_count, 
            ", cropped_off_total_area =", cropped_off_illusory_area, ":")

        for unaligned_offset in 0..=10 {
            data: ^u8 = cast(^u8)&byte_sequence_slice[unaligned_offset]
            tsc0 := read_tsc()
            load_penalty_test_asm(outer_loop_count, data, cast(u64)block_load_repeat_count)   
            tsc1 := read_tsc()
            time_to_load_from_service_area := compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq)
            bytes_per_second := cast(f64)actual_illusory_service_area / time_to_load_from_service_area 

            fmt.println("unaligned_offset =", unaligned_offset, 
                        "ts =", time_to_load_from_service_area, 
                        ", bps =", bytes_per_second)
        }
    }
}