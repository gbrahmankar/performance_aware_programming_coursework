package pap_common

import "core:fmt"
import "core:math"

Math_Op_Test_Result :: struct {
    total_diff: f64,
    max_diff: f64,
    diff_count: u32,

    input_value_at_max_diff: f64,
    output_value_at_max_diff: f64,
    expected_value_at_max_diff: f64,

    label: string
}

Math_Op_Tester :: struct {
    result_list: [16]Math_Op_Test_Result,
    number_of_results_produced: u32,
    active_result_index_for_currently_active_test: u32,

    is_testing: bool,
    step_index: u32,

    input_value: f64
}

// math_op_precision_tester_*
math_op_precision_tester_get_avg_diff :: proc(from: ^Math_Op_Test_Result) -> f64 {
    result: f64 = (from.diff_count > 0) ? (from.total_diff / cast(f64)from.diff_count) : 0
    return result
}

math_op_precision_tester_print_decimal_bars :: proc() {
    fmt.println("            ________________                      ________________")
}

// note(gaurav) : a test is nothing more than an interpolated input value
math_op_precision_tester_try_setup_next_precision_test :: proc(tester: ^Math_Op_Tester,
    min_input_value: f64,
    max_input_value: f64,
    step_count: u32 = 100000000) -> bool {

    if tester.is_testing {
        tester.step_index += 1
    } else {
        tester.is_testing = true
        tester.step_index = 0
    }

    if tester.step_index < step_count {
        t_step: f64 = cast(f64)tester.step_index / cast(f64)(step_count - 1)
        tester.input_value = (1.0 - t_step) * min_input_value + t_step * max_input_value

        tester.active_result_index_for_currently_active_test = tester.number_of_results_produced
    } else {
        tester.number_of_results_produced = tester.active_result_index_for_currently_active_test

        if(tester.number_of_results_produced > len(tester.result_list))
        {
            fmt.printfln("tester.number_of_results_produced exceeds the result_list len=%v !",
                tester.number_of_results_produced)
            tester.number_of_results_produced = len(tester.result_list)
        }

        tester.is_testing = false
    }

    return tester.is_testing
}

// note(gaurav) : a result is testing expected against output using the input value produced
math_op_precision_tester_test_using_latest_precision_test :: proc(tester: ^Math_Op_Tester,
    expected: f64,
    output: f64,
    label: string) {

    if tester.number_of_results_produced >= len(tester.result_list) {
        return
    }

    result: ^Math_Op_Test_Result = &tester.result_list[tester.active_result_index_for_currently_active_test]

    if(tester.step_index == 0)
    {
        result.label = label
    }

    diff: f64 = math.abs(expected - output)
    result.total_diff += diff
    result.diff_count += 1

    if(diff > result.max_diff)
    {
        result.max_diff = diff
        result.input_value_at_max_diff = tester.input_value
        result.output_value_at_max_diff = output
        result.expected_value_at_max_diff = expected
    }

    tester.active_result_index_for_currently_active_test += 1
}

math_op_precision_tester_print_precision_test_results :: proc(tester: ^Math_Op_Tester) {
    for result_index in 0..<tester.number_of_results_produced {
        math_op_precision_tester_print_decimal_bars()

        result: ^Math_Op_Test_Result = &tester.result_list[result_index]
        fmt.printfln("max_diff=%+.24f (avg_diff=%+.24f) at ip_value=%+.24f [%s]",
            result.max_diff,
            math_op_precision_tester_get_avg_diff(result),
            result.input_value_at_max_diff,
            result.label)
    }
}