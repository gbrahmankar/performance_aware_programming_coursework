#+build windows
package part_three

foreign import prefetch "../asm/chapter21_prefetch.lib"

@(default_calling_convention="c")
foreign prefetch {
	traverse_links_without_prefetch :: proc(outer: u64, src_data: ^u8, inner: u64,  /* unused */ dst_data: ^u8) -> u64 --- 
	traverse_links_with_prefetch :: proc(outer: u64, src_data: ^u8, inner: u64, /* unused */ dst_data: ^u8) -> u64 --- 
}

traverse_links_without_prefetch_asm :: traverse_links_without_prefetch 
traverse_links_with_prefetch_asm :: traverse_links_with_prefetch 