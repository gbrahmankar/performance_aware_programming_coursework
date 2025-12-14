package part_four

import "core:fmt"
import "core:math"
import "core:simd"

import "../pap_common"

// input_domain : 0 to pi
@(private="file")
sin_approximation :: proc(x: f64) -> f64 {
    using pap_common

	x_squared: f64 = x * x
    a: f64 = -4.0 / (PI64 * PI64)
    b: f64 = 4.0 / PI64

    result: f64 = a*x_squared + b*x

    return result
}

// input_domain : -pi to pi (leverages : sin(-x) = -sin(x))
@(private="file")
sin_half_approximation :: proc(x: f64) -> f64 {
   	pos_x: f64 = math.abs(x)

    y: f64 = sin_approximation(pos_x)
    y = (x < 0) ? -y : y

    return y
}

/* input_domain : -pi to pi :
here sin(x) is approximated(fitted) over [0, pi/2].
works over rest of the input_domain by flipping and reflecting.
*/
@(private="file")
sin_quarter_approximation :: proc(x: f64) -> f64 {
    using pap_common

   	pos_x: f64 = math.abs(x)

	half_pi: f64 = PI64 / 2
    quart_x: f64 = (pos_x > half_pi) ? (PI64 - pos_x) : pos_x

    quart_x2: f64 = quart_x * quart_x
    a: f64 = -0.3357488673628103541807525733876701910953780492546723687387637750157263772845455
    b: f64 = 1.164012859946630796034863328523423717191309716948615456152205566227330270901187
    pos_result: f64 = a*quart_x2 + b*quart_x

    result: f64 = (x < 0) ? -pos_result : pos_result

    return result
}

/* input_domain : -pi to pi : leverages sin(pi/2 + x) = cos(x)*/
@(private="file")
cos_quarter_approximation :: proc(x: f64) -> f64 {
    using pap_common

    result: f64 = sin_quarter_approximation(x + PI64 / 2.0)
    return result
}

chapter06_range_reduced_math_op_approximation :: proc() {
    using pap_common

    tester: Math_Op_Tester

    fmt.printfln("Range : [%+.24f, %+.24f]", -PI64, PI64)
	for math_op_precision_tester_try_setup_next_precision_test(&tester, -PI64, PI64) {
		reference_value: f64 = math.sin_f64(tester.input_value)
        math_op_precision_tester_test_using_latest_precision_test(&tester,
            reference_value,
            sin_approximation(tester.input_value),
            "sin_approximation")
		math_op_precision_tester_test_using_latest_precision_test(&tester,
            reference_value,
            sin_half_approximation(tester.input_value),
            "sin_half_approximation")
		math_op_precision_tester_test_using_latest_precision_test(&tester,
            reference_value,
            sin_quarter_approximation(tester.input_value),
            "sin_quarter_approximation")
    }
    math_op_precision_tester_print_precision_test_results(&tester)

	fmt.printfln("Range : [%+.24f, %+.24f]", -PI64/2, PI64/2)
	for math_op_precision_tester_try_setup_next_precision_test(&tester, -PI64/2, PI64/2) {
		reference_value: f64 = math.cos_f64(tester.input_value)
        math_op_precision_tester_test_using_latest_precision_test(&tester,
            reference_value,
            cos_quarter_approximation(tester.input_value),
            "cos_quarter_approximation")
    }
    math_op_precision_tester_print_precision_test_results(&tester)
}