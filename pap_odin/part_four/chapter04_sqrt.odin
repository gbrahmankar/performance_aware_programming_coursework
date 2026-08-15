#+feature using-stmt

package part_four

import "core:fmt"
import "core:math"
import "core:simd"

import "../pap_common"

sqrt_sd :: proc(input_value: f64) -> f64 {
	return simd.sqrt(input_value)
}

sqrt_ss :: proc(input_value: f32) -> f32 {
	return simd.sqrt(input_value)
}

run_square_root_precision_tests :: proc() {
    using pap_common

    tester: Math_Op_Tester
    for math_op_precision_tester_try_setup_next_precision_test(&tester, 0, 1) {
    	reference_output: f64 = math.sqrt_f64(tester.input_value);
        math_op_precision_tester_test_using_latest_precision_test(&tester,
            reference_output,
            sqrt_sd(tester.input_value),
            "sqrt_sd")

        math_op_precision_tester_test_using_latest_precision_test(&tester,
            reference_output,
            cast(f64)sqrt_ss(cast(f32)tester.input_value),
            "sqrt_ss")
    }

    math_op_precision_tester_print_precision_test_results(&tester)
}