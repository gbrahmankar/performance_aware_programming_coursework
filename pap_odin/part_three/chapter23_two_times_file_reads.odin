#+feature using-stmt
#+build windows

package part_three

import "core:fmt"
import "core:mem"
import "core:mem/virtual"
import "core:os"

import "../pap_common"

/* -----------------------------chapter23_2x_faster_file_reads-------------------------------
a)  just once, think about the idea of allocating for the entire file_size ...
    --  what are we paying for ? :
        --  we are paying a massive price for page management.
        --  since our buffer is huge, we are totally missinig the cache. 

b)  can we do a bit better ? casey proves yes ! how ? we go hunting again ! :
    --  allocate and touch every page to see what bandwidths we get for various buffer sizes. 
        --  insights we might get ? : 
            -- for smaller buffer sizes, we may stay within our cache. 
            -- for smaller buffer sizes, we will pay less for page faults.

c)  now, we also want to service an entire file through this buffer that our program can access :   
    --  writing to a smaller buffer ==> servicing through the caches for the most part. 
    --  we want to read the 1gb file and write to our smaller buffer.

d)  any cons to the approach we are taking ? :
    --  we are making continuous api calls to initiate a read->write.

e)  so, what we are essentially trying to do is, we are trying to find a sweet spot between :
    --  minimizing the page fault and cache miss penalties by limiting the size of the buffer
        mapped in our program and ....
    --  paying for making continuous api calls to service this huge file through our relatively tiny buffer.
------------------------------------------------------------------------------------------*/

File_Service_Buffer_Test_Function :: struct {
    test_name: string,
    test_func: proc (file_name: string, total_file_size: u64, buffer_size: u64, scratch_buffer: ^u8)
}

allocate_and_touch :: proc(src_file_name: string, total_src_file_size: u64, service_buffer_size: u64, scratch_buffer: ^u8) {
    using pap_common
    
    service_buffer_bytes, _ := virtual.reserve_and_commit(cast(uint)service_buffer_size)
    defer virtual.release(&service_buffer_bytes[0], cast(uint)service_buffer_size)
    
    pages_to_touch: u64 = (service_buffer_size + MEMORY_PAGE_SIZE - 1) / MEMORY_PAGE_SIZE
    for touch_index in 0..<pages_to_touch {
    	data_ptr: ^u8 = &service_buffer_bytes[MEMORY_PAGE_SIZE * touch_index]
    	data_ptr^ = cast(u8)touch_index
    }
}

allocate_and_copy :: proc(src_file_name: string, total_src_file_size: u64, service_buffer_size: u64, scratch_buffer: ^u8) {
    using pap_common

    service_buffer_bytes, _ := virtual.reserve_and_commit(cast(uint)service_buffer_size)
    first_service_buffer_byte: ^u8 = &service_buffer_bytes[0]
    defer virtual.release(first_service_buffer_byte, cast(uint)service_buffer_size) 

    next_read_from_scratch_buffer_ptr := scratch_buffer
    src_size_remaining: u64 = total_src_file_size
    for src_size_remaining > 0 {
		read_size: u64 = service_buffer_size;
        if read_size > src_size_remaining {
            read_size = src_size_remaining;
        }

    	mem.copy(first_service_buffer_byte, next_read_from_scratch_buffer_ptr, cast(int)read_size)

    	next_read_from_scratch_buffer_ptr = mem.ptr_offset(next_read_from_scratch_buffer_ptr, read_size)
    	src_size_remaining -= read_size 
    }
}

open_allocate_and_read :: proc(src_file_name: string, total_src_file_size: u64, service_buffer_size: u64, scratch_buffer: ^u8) {
    using pap_common

    service_buffer_bytes, _ := virtual.reserve_and_commit(cast(uint)service_buffer_size)
    first_service_buffer_byte: ^u8 = &service_buffer_bytes[0]
    defer virtual.release(first_service_buffer_byte, cast(uint)service_buffer_size) 

   	fd, err := os.open(src_file_name, os.O_RDWR)
   	if err == nil {
   		next_read_from_file_offset: i64 = 0
	    src_size_remaining: u64 = total_src_file_size
	    for src_size_remaining > 0 {
			read_size: u64 = service_buffer_size
	        if read_size > src_size_remaining {
	            read_size = src_size_remaining
	        }

	   		bytes_read, _ := os.read_at(fd, service_buffer_bytes, next_read_from_file_offset)	
	   		if cast(u64)bytes_read != read_size {
		   		fmt.panicf("error reading file :", src_file_name)
	   		}

	    	next_read_from_file_offset += cast(i64)read_size 
	    	src_size_remaining -= read_size 
	    }
	}
   	else {
   		fmt.panicf("error opening file :", src_file_name)
   	}
}

allocate_and_touch_test : File_Service_Buffer_Test_Function = {
    test_name = "allocate_and_touch",
    test_func = allocate_and_touch 
}

allocate_and_copy_test : File_Service_Buffer_Test_Function = {
    test_name = "allocate_and_copy",
    test_func = allocate_and_copy
}

open_allocate_and_read_test : File_Service_Buffer_Test_Function = {
    test_name = "open_allocate_and_read",
    test_func = open_allocate_and_read
}

file_service_buffer_tests : []File_Service_Buffer_Test_Function = { allocate_and_touch_test }

chapter23_two_times_faster_reads :: proc(cpu_freq: u64, csv_style_prints: bool)
{
    using pap_common

    BASE_BUFFER_SIZE :: 256 * 1024
    scratch_bytes, _ := virtual.reserve_and_commit(cast(uint)ONE_GB)
    for &byte_view, i in scratch_bytes {
        byte_view = cast(u8)i
    }
    first_scratch_byte: ^u8 = &scratch_bytes[0] 
    defer virtual.release(first_scratch_byte, cast(uint)ONE_GB)
    
    src_file_name := "haversine_json_files\\test.json";

    fd, err := os.open(src_file_name, os.O_RDWR)
    if err == nil {
        os.write(fd, scratch_bytes)
    }
    else {
        fmt.panicf("error opening file :", src_file_name)
    }
    os.close(fd)

    for source_buffer_size := BASE_BUFFER_SIZE; source_buffer_size <= ONE_GB; source_buffer_size *= 2 {
        if csv_style_prints == false {
            fmt.println("----------------------", "src_size =", source_buffer_size, "----------------------")
        }

        for func in file_service_buffer_tests {
            min_time: f64 = max(f64)
            for tries in 0..<1 {
                tsc0 := read_tsc()
                func.test_func(src_file_name, ONE_GB, cast(u64)source_buffer_size, first_scratch_byte)
                tsc1 := read_tsc()
                time_elapsed := compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq)
                if time_elapsed < min_time {
                    min_time = time_elapsed    
                } 
            }

            if csv_style_prints {
                fmt.println(cast(f64)source_buffer_size, ",", cast(f64)source_buffer_size/min_time)
            }
            else {
                fmt.println("test_name =", func.test_name, 
                    ", buffer_size =", cast(f64)source_buffer_size/ONE_KB,
                    ", elapsed =", min_time, 
                    ", gb/s =", cast(f64)source_buffer_size/min_time)
            }
        }
    }
}