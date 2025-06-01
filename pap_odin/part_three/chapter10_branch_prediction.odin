#+build windows
package part_three

import "core:crypto"
import "core:fmt"
import "core:math/rand"
import "core:mem"
import "core:mem/virtual"
import "core:os"

import "../pap_common"

foreign import chapter10_branch_prediction "../asm/chapter10_branch_prediction.lib"

@(default_calling_convention="c")
foreign chapter10_branch_prediction {
    try_byte_data_based_branching :: proc(count: u64, data: ^u8) -> u64 ---
}

try_byte_data_based_branching_asm :: try_byte_data_based_branching

Branch_Pattern :: enum {
    Invalid,
    Never_Taken,
    Always_Taken,
    Every_2,
    Every_3,
    Every_4,
    Os_Random,
    Crt_Random,    
}

fill_with_branch_pattern :: proc(pattern: Branch_Pattern, data: []byte) {
    if pattern == .Os_Random {
        _ = rand.read(data)
    } else if pattern == .Crt_Random {
        crypto.rand_bytes(data)
    } else {
        for &byte_view, i in data {
            #partial switch pattern {
                case .Never_Taken :
                    byte_view = cast(u8)0 
                case .Always_Taken :
                    byte_view = cast(u8)1
                case .Every_2 :
                    byte_view = cast(u8)(i % 2)
                case .Every_3 :
                    byte_view = cast(u8)(i % 3)
                case .Every_4 :
                    byte_view = cast(u8)(i % 4)
                case .Crt_Random :
                    byte_view = cast(u8)1
            }
        }
    }
}

/* --------------------------bp_pressure----------------------------------
    lenovo_loq_results :
    always_taken = 0.49508963746081219
    never_taken = 0.26138778113452477
    every_2 =  0.6604545476145216
    every_3 =  0.4305665305168544
    every_4 =  0.68952228243110858
    os_random =  3.1684721444964636
    crt_random =  3.1744982892599047
------------------------------------------------------------------------*/

chapter10_branch_prediction :: proc(cpu_freq: u64, csv_style_prints: bool) {
    using pap_common 
 
    total_byte_count: u64 = pap_common.ONE_GB 
    byte_padding: u64 = 256
    total_byte_count_with_padding: u64 = total_byte_count + byte_padding 

    byte_sequence_slice, _ := virtual.reserve_and_commit(cast(uint)total_byte_count_with_padding)
    defer virtual.release(&byte_sequence_slice[0], cast(uint)total_byte_count_with_padding) 

    byte_sequence := cast(^u8)&byte_sequence_slice[0]

    fill_with_branch_pattern(.Always_Taken, byte_sequence_slice)
    tsc0 := read_tsc()
    try_byte_data_based_branching_asm(total_byte_count, byte_sequence)
    tsc1 := read_tsc()
    fmt.println("always_taken =", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

    fill_with_branch_pattern(.Never_Taken, byte_sequence_slice)
    tsc0 = read_tsc()
    try_byte_data_based_branching_asm(total_byte_count, byte_sequence)
    tsc1 = read_tsc()
    fmt.println("never_taken =", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

    fill_with_branch_pattern(.Every_2, byte_sequence_slice)
    tsc0 = read_tsc()
    try_byte_data_based_branching_asm(total_byte_count, byte_sequence)
    tsc1 = read_tsc()
    fmt.println("every_2 = ", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

    fill_with_branch_pattern(.Every_3, byte_sequence_slice)
    tsc0 = read_tsc()
    try_byte_data_based_branching_asm(total_byte_count, byte_sequence)
    tsc1 = read_tsc()
    fmt.println("every_3 = ", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

    fill_with_branch_pattern(.Every_4, byte_sequence_slice)
    tsc0 = read_tsc()
    try_byte_data_based_branching_asm(total_byte_count, byte_sequence)
    tsc1 = read_tsc()
    fmt.println("every_4 = ", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

    fill_with_branch_pattern(.Os_Random, byte_sequence_slice)
    tsc0 = read_tsc()
    try_byte_data_based_branching_asm(total_byte_count, byte_sequence)
    tsc1 = read_tsc()
    fmt.println("os_random = ", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))

    fill_with_branch_pattern(.Crt_Random, byte_sequence_slice)
    tsc0 = read_tsc()
    try_byte_data_based_branching_asm(total_byte_count, byte_sequence)
    tsc1 = read_tsc()
    fmt.println("crt_random = ", compute_seconds_from_cpu_time(tsc1-tsc0, cpu_freq))
}
