#+build windows
package part_three

foreign import cache_set_indexing "../asm/chapter19_cache_set_indexing.lib"

@(default_calling_convention="c")
foreign cache_set_indexing {
	cache_set_index_test :: proc(outer: u64, data: ^u8, inner: u64) -> u64 --- 
}

cache_set_index_test_asm :: cache_set_index_test 