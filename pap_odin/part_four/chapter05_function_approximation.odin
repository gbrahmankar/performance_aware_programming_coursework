#+feature using-stmt

package part_four

import "core:fmt"
import "core:math"
import "core:simd"

import "../pap_common"

@(private="file")
sin_approximation :: proc(x: f64) -> f64 {
    using pap_common

	x_squared: f64 = x * x
    a: f64 = -4.0 / (PI64 * PI64);
    b: f64 = 4.0 / PI64;

    result: f64 = a*x_squared + b*x;

    return result
}

@(private="file")
sin_approximation_intrin :: proc(x: f64) -> f64 {
	using pap_common

	x_vector: #simd[1]f64 = { x }
	x_squared_vector: #simd[1]f64 = simd.mul(x_vector, x_vector)

	pi_vector: #simd[1]f64 = { PI64 }
	pi_squared_vector: #simd[1]f64 = simd.mul(pi_vector, pi_vector)

	a_vector: #simd[1]f64 = simd.div(#simd[1]f64 { -4.0 }, pi_squared_vector)
	b_vector: #simd[1]f64 = simd.div(#simd[1]f64 {  4.0 }, pi_vector)

	a_x_squared_vector: #simd[1]f64 = simd.mul(a_vector, x_squared_vector)
	b_x_vector: #simd[1]f64 = simd.mul(b_vector, x_vector)
	result_vector: #simd[1]f64 = simd.add(a_x_squared_vector, b_x_vector)

	return simd.extract(result_vector, 0)
}

chapter05_sin_approximation :: proc() {
    using pap_common

    tester: Math_Op_Tester

    fmt.printfln("Range : [%+.24f, %+.24f]", 0.0, PI64)
    for math_op_precision_tester_try_setup_next_precision_test(&tester, 0, PI64) {
		reference_value: f64 = math.sin_f64(tester.input_value)
        math_op_precision_tester_test_using_latest_precision_test(&tester,
            reference_value,
            sin_approximation(tester.input_value),
            "sin_approximation")
		math_op_precision_tester_test_using_latest_precision_test(&tester,
            reference_value,
            sin_approximation_intrin(tester.input_value),
            "sin_approximation_intrin")
    }
    math_op_precision_tester_print_precision_test_results(&tester)

    fmt.printfln("Range : [%+.24f, %+.24f]", -PI64, PI64)
	for math_op_precision_tester_try_setup_next_precision_test(&tester, -PI64, PI64) {
		reference_value: f64 = math.sin_f64(tester.input_value)
        math_op_precision_tester_test_using_latest_precision_test(&tester,
            reference_value,
            sin_approximation(tester.input_value),
            "sin_approximation")
		math_op_precision_tester_test_using_latest_precision_test(&tester,
            reference_value,
            sin_approximation_intrin(tester.input_value),
            "sin_approximation_intrin")
    }
    math_op_precision_tester_print_precision_test_results(&tester)
}