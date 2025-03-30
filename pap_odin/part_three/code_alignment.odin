#+build windows
package part_three

foreign import ca_tests "../asm/code_alignment.lib"

@(default_calling_convention="c")
foreign ca_tests {
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
