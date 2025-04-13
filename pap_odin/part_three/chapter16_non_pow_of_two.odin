#+build windows
package part_three

foreign import non_pow_of_two_cache_bw_tests "../asm/chapter16_non_pow_of_two.lib"

@(default_calling_convention="c")
foreign non_pow_of_two_cache_bw_tests {
	non_pow_of_two_cache_bw_test :: proc(outer: u64, data: ^u8, inner: u64) -> u64 --- 
}

non_pow_of_two_cache_bw_test_asm :: non_pow_of_two_cache_bw_test 