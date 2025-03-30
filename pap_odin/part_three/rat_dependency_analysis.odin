#+build windows
package part_three

foreign import rat_dependency "../asm/rat_dependency_analysis.lib"

@(default_calling_convention="c")
foreign rat_dependency {
	rat_add :: proc(count: u64) -> u64 ---
	rat_mov_add :: proc(count: u64) -> u64 ---
}

rat_add_asm :: rat_add 
rat_mov_add_asm :: rat_mov_add 
