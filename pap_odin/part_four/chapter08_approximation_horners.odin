#+feature using-stmt

package part_four

import "core:fmt"
import "core:math"

import "../pap_common"

@(private="file")
factorial :: proc(x: u64) -> u64 {
	input: u64 = x
	result: u64 = input
    for input > 1
    {
        input -= 1
        result *= input
    }

    return result
}

@(private="file")
taylor_sine_coefficient :: proc(power: u64) -> f64 {
    sign: f64 = ((((power - 1) / 2) % 2) == 1) ? -1.0 : 1.0
    result: f64 = (sign / cast(f64)factorial(power))

    return result
}

@(private="file")
taylor_sine_approximation :: proc(max_power: u64, x: f64) -> f64 {
    result: f64 = 0

    x_squared: f64 = x * x
    x_power: f64 = x
    for power: u64 = 1; power <= max_power; power += 2 {
        result += x_power * taylor_sine_coefficient(power)
        x_power *= x_squared
    }

    return result
}

@(private="file")
taylor_horners_sine_approximation :: proc(max_power: u64, x: f64) -> f64 {
	result: f64 = 0

    x_squared: f64 = x * x
    for inv_power: u64 = 1; inv_power <= max_power; inv_power += 2 {
        power: u64 = max_power - (inv_power - 1)
        result = result * x_squared + taylor_sine_coefficient(power)
    }
    result *= x

    return result
}

chapter08_approximation_horners :: proc() {
    using pap_common

    tester: Math_Op_Tester

	fmt.printfln("Range : [%+.24f, %+.24f]", 0.0, PI64/2)
    for power: u64 = 1; power <= 31; power += 2 {
		for math_op_precision_tester_try_setup_next_precision_test(&tester, 0.0, PI64/2, 1000) {
			reference_value: f64 = math.sin_f64(tester.input_value)
	        math_op_precision_tester_test_using_latest_precision_test(&tester,
	            reference_value,
	            taylor_sine_approximation(power, tester.input_value),
	            "taylor_sine_approximation")
			math_op_precision_tester_test_using_latest_precision_test(&tester,
	            reference_value,
	            taylor_horners_sine_approximation(power, tester.input_value),
	            "taylor_horners_sine_approximation")
	    }
	}
    math_op_precision_tester_print_precision_test_results(&tester)
}