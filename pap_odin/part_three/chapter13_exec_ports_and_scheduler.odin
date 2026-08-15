#+feature using-stmt
#+build windows

package part_three

import "core:fmt"
import "core:mem"
import "core:mem/virtual"
import "core:os"

import "../pap_common"

foreign import chapter13_exec_ports_and_scheduler "../asm/chapter13_exec_ports_and_scheduler.lib"
	
@(default_calling_convention="c")
foreign chapter13_exec_ports_and_scheduler {
	one_load_per_loop :: proc(count: u64, data: ^u8) -> u64 --- 
	two_loads_per_loop :: proc(count: u64, data: ^u8) -> u64 --- 
	three_loads_per_loop :: proc(count: u64, data: ^u8) -> u64 --- 
	four_loads_per_loop :: proc(count: u64, data: ^u8) -> u64 ---
	five_loads_per_loop :: proc(count: u64, data: ^u8) -> u64 ---
	one_store_per_loop :: proc(count: u64, data: ^u8) -> u64 --- 
	two_stores_per_loop :: proc(count: u64, data: ^u8) -> u64 --- 
	three_stores_per_loop :: proc(count: u64, data: ^u8) -> u64 --- 
	four_stores_per_loop :: proc(count: u64, data: ^u8) -> u64 ---
	five_stores_per_loop :: proc(count: u64, data: ^u8) -> u64 ---
}

one_load_per_loop_asm :: one_load_per_loop 
two_loads_per_loop_asm :: two_loads_per_loop 
three_loads_per_loop_asm :: three_loads_per_loop 
four_loads_per_loop_asm :: four_loads_per_loop 
five_loads_per_loop_asm :: five_loads_per_loop 
one_store_per_loop_asm :: one_store_per_loop 
two_stores_per_loop_asm :: two_stores_per_loop 
three_stores_per_loop_asm :: three_stores_per_loop 
four_stores_per_loop_asm :: four_stores_per_loop 
five_stores_per_loop_asm :: five_stores_per_loop 

chapter13_exec_ports_and_scheduler :: proc(cpu_freq: u64, csv_style_prints: bool) {
    using pap_common 

    total_byte_count: u64 = pap_common.ONE_GB 
    byte_padding: u64 = 256
    total_byte_count_with_padding: u64 = total_byte_count + byte_padding 

    byte_sequence_slice, _ := virtual.reserve_and_commit(cast(uint)total_byte_count_with_padding)
    defer virtual.release(&byte_sequence_slice[0], cast(uint)total_byte_count_with_padding) 

    byte_sequence := cast(^u8)&byte_sequence_slice[0]

    tsc0 := read_tsc()
    one_load_per_loop_asm(total_byte_count, byte_sequence) 
    tsc1 := read_tsc()
    fmt.println("one_load_per_loop = ", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

    tsc0 = read_tsc()
    two_loads_per_loop_asm(total_byte_count, byte_sequence) 
    tsc1 = read_tsc()
    fmt.println("two_loads_per_loop = ", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

    tsc0 = read_tsc()
    three_loads_per_loop_asm(total_byte_count, byte_sequence) 
    tsc1 = read_tsc()
    fmt.println("three_loads_per_loop = ", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

    tsc0 = read_tsc()
    four_loads_per_loop_asm(total_byte_count, byte_sequence) 
    tsc1 = read_tsc()
    fmt.println("four_loads_per_loop = ", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

    tsc0 = read_tsc()
    five_loads_per_loop_asm(total_byte_count, byte_sequence) 
    tsc1 = read_tsc()
    fmt.println("five_loads_per_loop = ", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

    tsc0 = read_tsc()
    one_store_per_loop_asm(total_byte_count, byte_sequence) 
    tsc1 = read_tsc()
    fmt.println("one_store_per_loop = ", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

    tsc0 = read_tsc()
    two_stores_per_loop_asm(total_byte_count, byte_sequence) 
    tsc1 = read_tsc()
    fmt.println("two_stores_per_loop = ", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

    tsc0 = read_tsc()
    three_stores_per_loop_asm(total_byte_count, byte_sequence) 
    tsc1 = read_tsc()
    fmt.println("three_stores_per_loop = ", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

    tsc0 = read_tsc()
    four_stores_per_loop_asm(total_byte_count, byte_sequence) 
    tsc1 = read_tsc()
    fmt.println("four_stores_per_loop = ", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

    tsc0 = read_tsc()
    five_stores_per_loop_asm(total_byte_count, byte_sequence) 
    tsc1 = read_tsc()
    fmt.println("five_stores_per_loop = ", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))
}