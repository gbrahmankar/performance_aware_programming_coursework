#+feature using-stmt
#+build windows

package part_three

import "core:fmt"
import "core:mem"
import "core:mem/virtual"
import "core:os"

import "../pap_common"

foreign import chapter12_rat_dependency_analysis "../asm/chapter12_rat_dependency_analysis.lib"

@(default_calling_convention="c")
foreign chapter12_rat_dependency_analysis {
	rat_add :: proc(count: u64) -> u64 ---
	rat_mov_add :: proc(count: u64) -> u64 ---
}

rat_add_asm :: rat_add 
rat_mov_add_asm :: rat_mov_add 

chapter12_rat_dependency_analysis :: proc(cpu_freq: u64, csv_style_prints: bool) {
    using pap_common 

    total_byte_count: u64 = pap_common.ONE_GB 
    
    tsc0 := read_tsc()
    rat_add_asm(total_byte_count) 
    tsc1 := read_tsc()
    fmt.println("rat_add = ", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

    tsc0 = read_tsc()
    rat_mov_add_asm(total_byte_count) 
    tsc1 = read_tsc()
    fmt.println("rat_mov_add = ", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))
}