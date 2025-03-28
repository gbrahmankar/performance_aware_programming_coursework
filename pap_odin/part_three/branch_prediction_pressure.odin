#+build windows
package part_three

foreign import bp_pressure "../asm/branch_prediction_pressure.lib"

@(default_calling_convention="c")
foreign bp_pressure {
    try_byte_data_based_branching :: proc(count: u64, data: ^u8) -> u64 ---
}

try_byte_data_based_branching_asm :: try_byte_data_based_branching