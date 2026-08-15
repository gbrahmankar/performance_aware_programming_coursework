#+feature using-stmt
#+build windows

package part_three

import "core:fmt"
import "core:mem"
import "core:mem/virtual"
import "core:os"

import "../pap_common"

foreign import non_temporal_stores "../asm/chapter20_non_temporal_stores.lib"

@(default_calling_convention="c")
foreign non_temporal_stores {
	raw_movs_nt :: proc(outer: u64, src_data: ^u8, inner: u64, dst_data: ^u8) -> u64 --- 
	streaming_bytes :: proc(outer: u64, src_data: ^u8, inner: u64, dst_data: ^u8) -> u64 --- 
}

raw_movs_nt_asm :: raw_movs_nt
streaming_bytes_asm :: streaming_bytes 

raw_movs_test: pap_common.Asm_Interface_Function = {
    test_name = "raw_movs",
    test_func = raw_movs_nt_asm 
}

streaming_bytes_test: pap_common.Asm_Interface_Function = {
    test_name = "streaming_bytes",
    test_func = streaming_bytes_asm 
}

test_functions: []pap_common.Asm_Interface_Function = { raw_movs_test, streaming_bytes_test }

/* -----------------------------chapter20_non_temporal_stores-------------------------------
a) this instruction doesn't care about the standard memory consistency rules. 
b) a very weak write, which will evict the line from every level and then write it to the memory. 
------------------------------------------------------------------------------------------*/

chapter20_non_temporal_stores :: proc(cpu_freq: u64, csv_style_prints: bool) {
	using pap_common 

	BASE_SRC_BUFFER_READ_SIZE :: 256
	SRC_BUFFER_SIZE  :: 1 * 1024 * 1024

	src_bytes, _ := virtual.reserve_and_commit(cast(uint)SRC_BUFFER_SIZE)
    defer virtual.release(&src_bytes[0], cast(uint)SRC_BUFFER_SIZE) 
	for &byte_view, i in src_bytes {
	    byte_view = cast(u8)i
	}
	src_data: ^u8 = cast(^u8)&src_bytes[0] 

	DST_BUFFER_SIZE :: 1 * 1024 * 1024 * 1024
	dst_bytes, _ := virtual.reserve_and_commit(cast(uint)DST_BUFFER_SIZE)
    defer virtual.release(&dst_bytes[0], cast(uint)DST_BUFFER_SIZE) 
	for &byte_view, i in dst_bytes {
	    byte_view = cast(u8)0
	}
	dst_data: ^u8 = cast(^u8)&dst_bytes[0] 

	for source_buffer_size := BASE_SRC_BUFFER_READ_SIZE ; source_buffer_size <= SRC_BUFFER_SIZE; source_buffer_size *= 2 {
	    inner_loop_count: u64 = cast(u64)source_buffer_size / BASE_SRC_BUFFER_READ_SIZE 
	    outer_loop_count: u64 = DST_BUFFER_SIZE / (BASE_SRC_BUFFER_READ_SIZE * inner_loop_count)

	    if csv_style_prints == false {
	    	fmt.println("----------------------", "src_size =", source_buffer_size, "----------------------")
		}

	    for func in test_functions {
	        for &byte_view, i in dst_bytes {
	            byte_view = cast(u8)0
	        }
	        dst_data: ^u8 = cast(^u8)&dst_bytes[0]

	        tsc0 := read_tsc()
	        func.test_func(outer_loop_count, src_data, inner_loop_count, dst_data)
	        tsc1 := read_tsc()
	        time_elapsed := compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq)

		    if csv_style_prints == true {
	        	fmt.println(func.test_name, time_elapsed)
	    	}
	    	else {
	        	fmt.println("test_name =", func.test_name, ", elapsed =", time_elapsed)
	    	}
	    }
	}
}