#+build windows
package part_three

import "core:fmt"
import "core:mem/virtual"

File_Service_Buffer_Test_Function :: struct {
    test_name: string,
    test_func: proc (file_name: string, total_file_size: u64, buffer_size: u64, buffer: ^u8)
}

MEMORY_PAGE_SIZE :: 4096

allocate_and_touch :: proc(src_file_name: string, total_src_file_size: u64, service_buffer_size: u64, src_buffer: ^u8) {
    all_bytes, _ := virtual.reserve_and_commit(cast(uint)service_buffer_size)
    defer virtual.release(&all_bytes[0], cast(uint)service_buffer_size)
    
    pages_to_touch: u64 = (service_buffer_size + MEMORY_PAGE_SIZE - 1) / MEMORY_PAGE_SIZE
    for touch_index in 0..<pages_to_touch {
    	all_bytes[MEMORY_PAGE_SIZE * touch_index] = 1
    }
}

allocate_and_touch_test : File_Service_Buffer_Test_Function = {
    test_name = "allocate_and_touch",
    test_func = allocate_and_touch 
}

file_service_buffer_tests : []File_Service_Buffer_Test_Function = { allocate_and_touch_test }
