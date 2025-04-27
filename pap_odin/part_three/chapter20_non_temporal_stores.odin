#+build windows
package part_three

foreign import non_temporal_stores "../asm/chapter20_non_temporal_stores.lib"

@(default_calling_convention="c")
foreign non_temporal_stores {
	non_temporal_stores_test :: proc(outer: u64, src_data: ^u8, inner: u64, dst_data: ^u8) -> u64 --- 
}

non_temporal_stores_test_asm :: non_temporal_stores_test 

test_stores :: proc() {
    for source_buffer_size := BASE_SRC_BUFFER_READ_SIZE ; source_buffer_size <= SRC_BUFFER_SIZE; source_buffer_size *= 2 {
        inner_loop_count: u64 = cast(u64)source_buffer_size / BASE_SRC_BUFFER_READ_SIZE 
        outer_loop_count: u64 = DST_BUFFER_SIZE / (BASE_SRC_BUFFER_READ_SIZE * inner_loop_count)

        tsc0 := read_tsc()
        non_temporal_stores_test_asm(outer_loop_count, src_data, inner_loop_count, dst_data)
        tsc1 := read_tsc()
        time_elapsed := compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq)

        fmt.println("src_size =", source_buffer_size, ", elapsed = ", time_elapsed)
    }
}