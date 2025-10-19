#+build windows
package part_three

import "core:fmt"
import "core:mem"
import "core:mem/virtual"
import "core:os"

import "../pap_common"

foreign import chapter09_front_end_tests "../asm/chapter09_front_end_tests.lib"

@(default_calling_convention="c")
foreign chapter09_front_end_tests {
	mov_all_bytes :: proc(count: u64, data: ^u8) -> u64 ---
	nop_all_bytes :: proc(count: u64, data: ^u8) -> u64 --- 
	cmp_all_bytes :: proc(count: u64, data: ^u8) -> u64 --- 
	dec_all_bytes :: proc(count: u64, data: ^u8) -> u64 --- 
}

mov_all_bytes_asm :: mov_all_bytes
nop_all_bytes_asm :: nop_all_bytes
cmp_all_bytes_asm :: cmp_all_bytes
dec_all_bytes_asm :: dec_all_bytes

chapter09_front_end_tests :: proc(cpu_freq: u64, csv_style_prints: bool) {
	using pap_common 

	total_byte_count: u64 = pap_common.ONE_GB 
	byte_padding: u64 = 256
	total_byte_count_with_padding: u64 = total_byte_count + byte_padding 

	byte_sequence_slice, _ := virtual.reserve_and_commit(cast(uint)total_byte_count_with_padding)
	defer virtual.release(&byte_sequence_slice[0], cast(uint)total_byte_count_with_padding) 
	for &byte_view, i in byte_sequence_slice {
	byte_view = cast(u8)i
	}
	byte_sequence: ^u8 = cast(^u8)&byte_sequence_slice[0]

	tsc0 := read_tsc()
	mov_all_bytes_asm(total_byte_count, byte_sequence)
	tsc1 := read_tsc()
	fmt.println("mov_all_bytes_asm =", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

	tsc0 = read_tsc()
	nop_all_bytes_asm(total_byte_count, byte_sequence)
	tsc1 = read_tsc()
	fmt.println("nop_all_bytes_asm =", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

	tsc0 = read_tsc()
	cmp_all_bytes_asm(total_byte_count, byte_sequence)
	tsc1 = read_tsc()
	fmt.println("cmp_all_bytes_asm =", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

	tsc0 = read_tsc()
	dec_all_bytes_asm(total_byte_count, byte_sequence)
	tsc1 = read_tsc()
	fmt.println("dec_all_bytes_asm =", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))
} 
