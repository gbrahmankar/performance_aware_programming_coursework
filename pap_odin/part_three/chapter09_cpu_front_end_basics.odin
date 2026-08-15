#+feature using-stmt
#+build windows

package part_three

import "core:fmt"
import "core:mem"
import "core:mem/virtual"
import "core:os"

import "../pap_common"

foreign import chapter09_cpu_front_end_basics "../asm/chapter09_cpu_front_end_basics.lib"

@(default_calling_convention="c")
foreign chapter09_cpu_front_end_basics {
    nop_one_three_bytes :: proc(count: u64, data: ^u8) -> u64 ---
    nop_three_one_byte :: proc(count: u64, data: ^u8) -> u64 --- 
    nop_nine_one_byte :: proc(count: u64, data: ^u8) -> u64 --- 
}

nop_one_three_bytes_asm :: nop_one_three_bytes
nop_three_one_byte_asm :: nop_three_one_byte
nop_nine_one_byte_asm :: nop_nine_one_byte

chapter09_cpu_front_end_basics :: proc(cpu_freq: u64, csv_style_prints: bool) {
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
    nop_one_three_bytes_asm(total_byte_count, byte_sequence)
    tsc1 := read_tsc()
    fmt.println("nop_one_three_bytes =", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

    tsc2 := read_tsc()
    nop_three_one_byte_asm(total_byte_count, byte_sequence)
    tsc3 := read_tsc()
    fmt.println("nop_three_one_byte =", compute_seconds_from_cpu_time(tsc3-tsc2, cpu_freq))

    tsc4 := read_tsc()
    nop_nine_one_byte_asm(total_byte_count, byte_sequence)
    tsc5 := read_tsc()
    fmt.println("nop_nine_one_byte =", compute_seconds_from_cpu_time(tsc5-tsc4, cpu_freq))
}
