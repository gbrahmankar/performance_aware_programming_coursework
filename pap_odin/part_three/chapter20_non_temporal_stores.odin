#+build windows
package part_three

foreign import non_temporal_stores "../asm/chapter20_non_temporal_stores.lib"

@(default_calling_convention="c")
foreign non_temporal_stores {
	raw_movs_nt :: proc(outer: u64, src_data: ^u8, inner: u64, dst_data: ^u8) -> u64 --- 
	streaming_bytes :: proc(outer: u64, src_data: ^u8, inner: u64, dst_data: ^u8) -> u64 --- 
}

raw_movs_nt_asm :: raw_movs_nt
streaming_bytes_asm :: streaming_bytes 