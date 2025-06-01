#+build windows
package part_three

import "core:fmt"
import "core:mem"
import "core:mem/virtual"
import "core:os"

import "../pap_common"

foreign import prefetch "../asm/chapter21_prefetch.lib"

@(default_calling_convention="c")
foreign prefetch {
	traverse_links_without_prefetch :: proc(outer: u64, src_data: ^u8, inner: u64,  /* unused */ dst_data: ^u8) -> u64 --- 
	traverse_links_with_prefetch :: proc(outer: u64, src_data: ^u8, inner: u64, /* unused */ dst_data: ^u8) -> u64 --- 
}

traverse_links_without_prefetch_asm :: traverse_links_without_prefetch 
traverse_links_with_prefetch_asm :: traverse_links_with_prefetch 

/* -----------------------------------chapter21_prefetch------------------------------------
    a) branch_predictor should not be able to predict the branch im about to take.
    b) in a branch, i will have a few math_ops based on the data that was fetched.
    c) data required to execute these ops should already be prefetched from the previous branch.
    d) a mod_3 branch switch between two execution routines.
    e) load required for the other branch would be prefetched 
------------------------------------------------------------------------------------------*/

chapter21_prefetch :: proc(cpu_freq: u64, csv_style_prints: bool) {
	using pap_common

    total_byte_count: u64 = 1 * 1024 * 1024 * 1024
    byte_padding: u64 = 256
    total_byte_count_with_padding: u64 = total_byte_count + byte_padding 

    byte_sequence_slice, _ := virtual.reserve_and_commit(cast(uint)total_byte_count_with_padding)
    defer virtual.release(&byte_sequence_slice[0], cast(uint)total_byte_count_with_padding) 

    byte_sequence: ^u8 = cast(^u8)(&byte_sequence_slice[0])

    traverse_links_without_prefetch_test: Asm_Interface_Function = {
        test_name = "traverse_links_without_prefetch",
        test_func = traverse_links_without_prefetch_asm
    }

    traverse_links_with_prefetch_test: Asm_Interface_Function = {
        test_name = "traverse_links_with_prefetch",
        test_func = traverse_links_with_prefetch_asm
    }

    test_functions: []Asm_Interface_Function = { traverse_links_without_prefetch_test, traverse_links_with_prefetch_test }

    LINKED_LIST_LENGTH :: 1024 * 1024
    TEST_SIZE_BYTES :: pap_common.CACHE_LINE_SIZE * LINKED_LIST_LENGTH

    cache_line_count: u64 = cast(u64)len(byte_sequence_slice) / pap_common.CACHE_LINE_SIZE
    jump_offset: u64 = 0

    for outer_loop_index in 0..<LINKED_LIST_LENGTH {
        next_offset: u64
        next_pointer: ^u64    

        random_number := get_random_number()

        found := false
        for search_index in 0..<cache_line_count {
            next_offset = (cast(u64)search_index + random_number) % cache_line_count   
            next_pointer = cast(^u64)(&byte_sequence_slice[next_offset * pap_common.CACHE_LINE_SIZE])

            if next_pointer^ == 0 {
                found = true
                break;    
            }
        }

        if found == false {
            fmt.panicf("unable to create a new link")
        }

        jump_data: ^u64 = cast(^u64)(&byte_sequence_slice[jump_offset * pap_common.CACHE_LINE_SIZE])
        mem.copy(jump_data, &next_pointer, size_of(&next_pointer))
        cache_line_test_data_ptr := mem.ptr_offset(jump_data, 1)
        cache_line_test_data_ptr^ = cast(u64)outer_loop_index

        jump_offset = next_offset
    }

    for func in test_functions {
        tsc0 := read_tsc()
        func.test_func(cast(u64)LINKED_LIST_LENGTH, byte_sequence, 1024, byte_sequence)
        tsc1 := read_tsc()
        time_to_traverse_links := compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq)
        bytes_per_second := cast(f64)total_byte_count / time_to_traverse_links 
        fmt.println("test_name =", func.test_name, 
                    ", elapsed =", time_to_traverse_links,
                    ", bps =", bytes_per_second)
    }
}