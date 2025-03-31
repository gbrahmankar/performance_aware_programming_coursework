#+build windows
package part_three

foreign import c14_simd "../asm/chapter14_simd.lib"

@(default_calling_convention="c")
foreign c14_simd {
	load_four_bytes_three_times_per_loop :: proc(count: u64, data: ^u8) -> u64 --- 
	load_eight_bytes_three_times_per_loop :: proc(count: u64, data: ^u8) -> u64 --- 
	load_sixteen_bytes_three_times_per_loop :: proc(count: u64, data: ^u8) -> u64 --- 
	load_thirty_two_bytes_three_times_per_loop :: proc(count: u64, data: ^u8) -> u64 ---
}

load_four_bytes_three_times_per_loop_asm :: load_four_bytes_three_times_per_loop
load_eight_bytes_three_times_per_loop_asm :: load_eight_bytes_three_times_per_loop
load_sixteen_bytes_three_times_per_loop_asm :: load_sixteen_bytes_three_times_per_loop
load_thirty_two_bytes_three_times_per_loop_asm :: load_thirty_two_bytes_three_times_per_loop
