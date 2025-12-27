package part_four

import "base:intrinsics"
import "core:fmt"
import "core:math"
import "core:simd"

import "../pap_common"

@(private="file")
sine_approximation :: proc(orig_x: f64) -> f64 {
    using pap_common

    pos_x: f64 = math.abs(orig_x)
    x: f64 = (pos_x > PI64/2) ? (PI64 - pos_x) : pos_x

    x_squared: f64 = x * x

    result: f64 = 0.0000000000000027216757170572905341802418158794654755574192982192638368132975301705300807952880859375;
    result = intrinsics.fused_mul_add(result, x_squared,
        -0.000000000000764304326459753109422067633142294847017721159954106724399025551974773406982421875);
    result = intrinsics.fused_mul_add(result, x_squared,
        0.0000000001605894189662054249701294800524160703847353914852646994404494762420654296875);
    result = intrinsics.fused_mul_add(result, x_squared,
        -0.0000000250521069215027131438789363172825463976778337382711470127105712890625);
    result = intrinsics.fused_mul_add(result, x_squared,
        0.0000027557319211743496625388431808811873224840383045375347137451171875);
    result = intrinsics.fused_mul_add(result, x_squared,
        -0.0001984126984121336516146694162188168775173835456371307373046875);
    result = intrinsics.fused_mul_add(result, x_squared,
        0.00833333333333320831759483127143539604730904102325439453125);
    result = intrinsics.fused_mul_add(result, x_squared,
        -0.1666666666666666574148081281236954964697360992431640625);
    result = intrinsics.fused_mul_add(result, x_squared,
        1.000000000000000000000000);
    result *= x

    result = (orig_x < 0) ? -result : result

    return result
}

@(private="file")
cosine_approximation :: proc(x: f64) -> f64 {
    result: f64 = sine_approximation(x + pap_common.PI64/2.0)
    return result
}

@(private="file")
sqrt_sd :: proc(input_value: f64) -> f64 {
	return simd.sqrt(input_value)
}

@(private="file")
asine_approximation :: proc(orig_x: f64) -> f64 {
    needs_transform: bool = (orig_x > 0.7071067811865475244)
    x: f64 = needs_transform ? sqrt_sd(1.0 - orig_x*orig_x) : orig_x

    x_squared: f64 = x * x

    r: f64 = 0.937051490234062756101707;
    r = simd.fma(r, x_squared, -3.490443583255612214344410);
    r = simd.fma(r, x_squared, 6.186819757295904054217317);
    r = simd.fma(r, x_squared, -6.756794510834697398138360);
    r = simd.fma(r, x_squared, 5.081534831018740483443707);
    r = simd.fma(r, x_squared, -2.753168488249635892373135);
    r = simd.fma(r, x_squared, 1.123860218902060115198083);
    r = simd.fma(r, x_squared, -0.335174736401442241451321);
    r = simd.fma(r, x_squared, 0.088305893045829114695877);
    r = simd.fma(r, x_squared, -0.004413816332621807794756);
    r = simd.fma(r, x_squared, 0.013447193221907542334814);
    r = simd.fma(r, x_squared, 0.013776824789416922470431);
    r = simd.fma(r, x_squared, 0.017366257890261350277372);
    r = simd.fma(r, x_squared, 0.022371484508100256821672);
    r = simd.fma(r, x_squared, 0.030381966628286034143303);
    r = simd.fma(r, x_squared, 0.044642856704867270312143);
    r = simd.fma(r, x_squared, 0.075000000004415118270984);
    r = simd.fma(r, x_squared, 0.166666666666650920003434);
    r = simd.fma(r, x_squared, 1.000000000000000000000000);
    r *= x

    result: f64 = needs_transform ? (1.57079632679489661923 - r) : r
    return result;
}

chapter12_all_math_op_replacement_approximations :: proc() {
    using pap_common

    tester: Math_Op_Tester

    fmt.printfln("sin_input_domain : [%+.24f, %+.24f]", -PI64, PI64)
    for math_op_precision_tester_try_setup_next_precision_test(&tester, -PI64, PI64) {
        math_op_precision_tester_test_using_latest_precision_test(&tester,
            math.sin_f64(tester.input_value),
            sine_approximation(tester.input_value),
            fmt.tprintf("sine_approximation"))
    }
    math_op_precision_tester_print_precision_test_results(&tester)

    fmt.printfln("cos_input_domain : [%+.24f, %+.24f]", -PI64/2, PI64/2)
    for math_op_precision_tester_try_setup_next_precision_test(&tester, -PI64/2, PI64/2) {
        math_op_precision_tester_test_using_latest_precision_test(&tester,
            math.cos_f64(tester.input_value),
            cosine_approximation(tester.input_value),
            fmt.tprintf("cosine_approximation"))
    }
    math_op_precision_tester_print_precision_test_results(&tester)

    fmt.printfln("asin_input_domain : [%+.24f, %+.24f]", 0.0, 1.0)
    for math_op_precision_tester_try_setup_next_precision_test(&tester, 0.0, 1.0) {
        math_op_precision_tester_test_using_latest_precision_test(&tester,
            math.asin_f64(tester.input_value),
            asine_approximation(tester.input_value),
            fmt.tprintf("asine_approximation"))
    }
    math_op_precision_tester_print_precision_test_results(&tester)

	fmt.printfln("sqrt_input_domain : [%+.24f, %+.24f]", 0.0, 1.0)
    for math_op_precision_tester_try_setup_next_precision_test(&tester, 0.0, 1.0) {
        math_op_precision_tester_test_using_latest_precision_test(&tester,
            math.sqrt_f64(tester.input_value),
            sqrt_sd(tester.input_value),
            fmt.tprintf("sqrt_sd"))
    }
    math_op_precision_tester_print_precision_test_results(&tester)
}