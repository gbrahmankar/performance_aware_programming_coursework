#+feature using-stmt
#+build windows

package part_three

import "core:fmt"
import "core:mem"
import "core:mem/virtual"
import "core:os"

import "../pap_common"

foreign import cache_set_indexing "../asm/chapter19_cache_set_indexing.lib"

@(default_calling_convention="c")
foreign cache_set_indexing {
	cache_set_index_test :: proc(outer: u64, data: ^u8, inner: u64, stride: u64) -> u64 --- 
}

cache_set_index_test_asm :: cache_set_index_test 

/* --------------------------chapter19_cache_sets_and_indexing-----------------------------
cache_line_size = 2^6 = 64b
8_way_set_associative ==> one cache_set will have 8 cache_lines ==> 3_tag_bits 
my_lenovo_loq_l1_size = 32kb

number_of_cache_lines = 32kb/64b = 512
number_of_cache_sets = 512/8 = 64 ==> 6 index_bits to refer to each cache_set

memory_request = msb_[[---xremaining_bitsx---][---3_tag_bits---][---6_index_bits---][---x6_bitsx---]]_lsb

lowest to highest in significance :
[---x6_bitsx---] : each byte in a cache_line. we would be indexing at a level above this ie. cache_set.
[---6_index_bits---] : 6 bits to index a cache_set in a cache.
[---3_tag_bits---] : 3 bits for tagging a cache_line within a cache_set.
[---xremaining_bitsx---] : reserved
------------------------------------------------------------------------------------------*/

chapter19_cache_set_indexing :: proc(cpu_freq: u64, csv_style_prints: bool) {
	using pap_common 

    NUMBER_OF_CACHE_LINES_TO_LOAD :: 256
    REPEAT_COUNT :: 256
    TOTAL_BYTES :: pap_common.CACHE_LINE_SIZE * NUMBER_OF_CACHE_LINES_TO_LOAD * REPEAT_COUNT

    BASE_STRIDE :: pap_common.CACHE_LINE_SIZE

    byte_sequence_slice, _ := virtual.reserve_and_commit(cast(uint)TOTAL_BYTES)
    defer virtual.release(&byte_sequence_slice[0], cast(uint)TOTAL_BYTES) 

    for &byte_view, i in byte_sequence_slice {
        byte_view = cast(u8)i
    }
    byte_sequence: ^u8 = cast(^u8)&byte_sequence_slice[0]

    for stride_multiplier in 0..=128 {
        stride := BASE_STRIDE * stride_multiplier

        tsc0 := read_tsc()
        cache_set_index_test_asm(REPEAT_COUNT, byte_sequence, cast(u64)NUMBER_OF_CACHE_LINES_TO_LOAD, cast(u64)stride)
        tsc1 := read_tsc()
        time_elapsed := compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq)

        if csv_style_prints == true { 
	        fmt.println(stride, time_elapsed)
    	}
    	else {
	        fmt.println("stride =", stride, ", time_taken =", time_elapsed)
    	}
    }
}
