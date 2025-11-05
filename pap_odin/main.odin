package pap_odin

import "core:fmt"
import "core:mem"
import "core:mem/virtual"
import "core:os"
import "core:time"

import "pap_common"
import "part_three"

csv_style_prints := false 

main :: proc() {
    using part_three
    using pap_common 

    cpu_freq, has_tsc := get_tsc_frequency()

    // chapter09_front_end_tests(cpu_freq, csv_style_prints)
    // chapter09_cpu_front_end_basics(cpu_freq, csv_style_prints) 
    // chapter10_branch_prediction(cpu_freq, csv_style_prints)  
    // chapter11_code_alignment(cpu_freq, csv_style_prints)   
    // chapter12_rat_dependency_analysis(cpu_freq, csv_style_prints)
    // chapter13_exec_ports_and_scheduler(cpu_freq, csv_style_prints) 
    // chapter14_simd(cpu_freq, csv_style_prints)  
    // chapter15_cache_bw_tests(cpu_freq, csv_style_prints)   
    // chapter16_non_power_of_two(cpu_freq, csv_style_prints)   
    // chapter18_unaligned_load_penalties(cpu_freq, csv_style_prints)   
    // chapter19_cache_set_indexing(cpu_freq, csv_style_prints)
    // chapter20_non_temporal_stores(cpu_freq, csv_style_prints)
    // chapter21_prefetch(cpu_freq, csv_style_prints) 
    // chapter23_two_times_faster_reads(cpu_freq, csv_style_prints) 

    // throwaway
    tester := repetition_tester_create(cpu_freq)
    page_size := 4 * mem.Kilobyte
    pages: u64 = 10
    alloc_bytes: uint = cast(uint)pages * cast(uint)page_size
    repitition_tester_begin_new_test_instance(&tester, cast(u64)alloc_bytes, 2)
    for repitition_tester_is_testing(&tester) {
        repetition_tester_begin_time(&tester)

        src_bytes, _ := virtual.reserve_and_commit(alloc_bytes)
        defer virtual.release(&src_bytes[0], alloc_bytes)
        for &byte_view, i in src_bytes {
            byte_view = cast(u8)i
        }

        repetition_tester_count_bytes(&tester, cast(u64)alloc_bytes)
        repetition_tester_end_time(&tester)
    }
}