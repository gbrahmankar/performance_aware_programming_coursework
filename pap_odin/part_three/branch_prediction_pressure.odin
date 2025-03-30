#+build windows
package part_three

foreign import bp_pressure "../asm/branch_prediction_pressure.lib"

@(default_calling_convention="c")
foreign bp_pressure {
    try_byte_data_based_branching :: proc(count: u64, data: ^u8) -> u64 ---
}

try_byte_data_based_branching_asm :: try_byte_data_based_branching

// -------------------------------------------------------------------------------

import "core:crypto"
import "core:math/rand"

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