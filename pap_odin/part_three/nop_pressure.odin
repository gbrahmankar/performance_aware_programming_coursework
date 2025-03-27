#+build windows
package part_three

foreign import nop_pressure "../asm/nop_pressure.lib"

@(default_calling_convention="c")
foreign nop_pressure {
    nop_one_three_bytes :: proc(count: u64) -> uint ---
    nop_three_one_byte :: proc(count: u64) -> uint --- 
    nop_nine_one_byte :: proc(count: u64) -> uint --- 
}

nop_one_three_bytes_asm :: nop_one_three_bytes
nop_three_one_byte_asm :: nop_three_one_byte
nop_nine_one_byte_asm :: nop_nine_one_byte