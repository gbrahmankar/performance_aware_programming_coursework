package pap_odin

import "core:fmt"
import "core:mem"
import "core:mem/virtual"
import "core:os"
import "core:time"

import "pap_common"
import "part_three"
import "part_four"

csv_style_prints := false 

main :: proc() {
    using pap_common 
    using part_three
    using part_four

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

    /*
    throwaway : rep_tester
    tester := repetition_tester_create(cpu_freq)
    page_size := 4 * mem.Kilobyte
    pages: u64 = 10
    alloc_bytes: uint = cast(uint)pages * cast(uint)page_size
    repitition_tester_begin_new_test_instance(&tester, cast(u64)alloc_bytes, 2)
    for repitition_tester_is_testing(&tester) {
        repetition_tester_begin_time(&tester)

        sleep_for_duration(2) 
        repetition_tester_count_bytes(&tester, cast(u64)alloc_bytes)

        repetition_tester_end_time(&tester)
    }
    */

    /*
    throwaway : profiler
    profiler_begin_profiling() 
    {
        scope_0: Profile_Scope
        profiler_begin_profiling_scope(&scope_0, "scope_0", 10 * mem.Gigabyte)
        defer profiler_end_profiling_scope(&scope_0)

        sleep_for_duration(5) 

        {
            scope_1: Profile_Scope
            profiler_begin_profiling_scope(&scope_1, "scope_1", 7 * mem.Gigabyte)
            defer profiler_end_profiling_scope(&scope_1)

            sleep_for_duration(3) 

            {
                scope_0: Profile_Scope
                profiler_begin_profiling_scope(&scope_0, "scope_0", 3 * mem.Gigabyte)
                defer profiler_end_profiling_scope(&scope_0)

                sleep_for_duration(2) 
            }
        } 
    }
    profiler_end_profiling()
    */

    /*
    throwaway : haversine_distance_problem_tests

    produce_haversine_distance_problem_files(500000)

    coordinate_pairs, number_of_pairs, distances_between_pairs, average_sum := process_haversine_pairs_json_file()
    defer delete(coordinate_pairs)
    defer delete(distances_between_pairs)

    tally_produced_distances_and_avg_sum_against_reference(distances_between_pairs[:number_of_pairs], average_sum)
    */

    /*
    throwaway : part_four : determining_input_ranges 

    haversine_math_ops_update_input_domains()
    haversine_math_ops_update_output_ranges()
    fmt.println(sine_approximation(1.2))
    */
}