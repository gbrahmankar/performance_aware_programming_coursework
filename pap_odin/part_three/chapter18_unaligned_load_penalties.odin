#+build windows
package part_three

foreign import unaligned_load_penalties "../asm/chapter18_unaligned_load_penalties.lib"

@(default_calling_convention="c")
foreign unaligned_load_penalties {
	load_penalty_test :: proc(outer: u64, data: ^u8, inner: u64) -> u64 --- 
}

load_penalty_test_asm :: load_penalty_test 