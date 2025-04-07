#+build windows
package part_three

foreign import cache_bw_tests "../asm/chapter15_cache_bw_tests.lib"

@(default_calling_convention="c")
foreign cache_bw_tests {
	cache_size_bw_test :: proc(count: u64, data: ^u8, mask: u64) -> u64 --- 
}

cache_size_bw_test_asm :: cache_size_bw_test