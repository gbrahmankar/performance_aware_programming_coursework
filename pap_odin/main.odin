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
    chapter23_two_times_faster_reads(cpu_freq, csv_style_prints) 
}