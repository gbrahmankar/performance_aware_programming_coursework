#+build windows
package part_three

foreign import exec_ports "../asm/exec_ports.lib"

@(default_calling_convention="c")
foreign exec_ports {
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
