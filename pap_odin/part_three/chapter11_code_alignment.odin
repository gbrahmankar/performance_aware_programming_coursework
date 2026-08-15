#+feature using-stmt
#+build windows

package part_three

import "core:fmt"
import "core:mem"
import "core:mem/virtual"
import "core:os"

import "../pap_common"

foreign import chapter11_code_alignment "../asm/chapter11_code_alignment.lib"

@(default_calling_convention="c")
foreign chapter11_code_alignment {
    aligned_at_64_bytes :: proc(count: u64, data: ^u8) -> u64 ---
    offset_by_1_nop :: proc(count: u64, data: ^u8) -> u64 ---
    offset_by_15_nops :: proc(count: u64, data: ^u8) -> u64 ---
    offset_by_31_nops :: proc(count: u64, data: ^u8) -> u64 ---
    offset_by_63_nops :: proc(count: u64, data: ^u8) -> u64 ---
}

aligned_at_64_bytes_asm :: aligned_at_64_bytes
offset_by_1_nop_asm :: offset_by_1_nop
offset_by_15_nops_asm :: offset_by_15_nops
offset_by_31_nops_asm :: offset_by_31_nops
offset_by_63_nops_asm :: offset_by_63_nops

chapter11_code_alignment :: proc(cpu_freq: u64, csv_style_prints: bool) {
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
    aligned_at_64_bytes_asm(total_byte_count, byte_sequence) 
    tsc1 := read_tsc()
    fmt.println("aligned_at_64_bytes_asm = ", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

    tsc0 = read_tsc()
    offset_by_1_nop_asm(total_byte_count, byte_sequence) 
    tsc1 = read_tsc()
    fmt.println("offset_by_1_nop_asm = ", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

    tsc0 = read_tsc()
    offset_by_15_nops_asm(total_byte_count, byte_sequence) 
    tsc1 = read_tsc()
    fmt.println("offset_by_15_nops_asm = ", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

    tsc0 = read_tsc()
    offset_by_31_nops_asm(total_byte_count, byte_sequence) 
    tsc1 = read_tsc()
    fmt.println("offset_by_31_nops_asm = ", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

    tsc0 = read_tsc()
    offset_by_63_nops_asm(total_byte_count, byte_sequence) 
    tsc1 = read_tsc()
    fmt.println("offset_by_63_nops_asm = ", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))
}
