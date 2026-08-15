#+feature using-stmt

package part_four

import "base:intrinsics"
import "core:fmt"
import "core:math"

import "../pap_common"

sin_radians_c_taylor: [16]f64 = {
    1.0,
    -0.1666666666666666666666666666666666666666666666666666666666666666666666666666667, // 1 / 3!
    0.008333333333333333333333333333333333333333333333333333333333333333333333333333333, // 1 / 5!
    -0.0001984126984126984126984126984126984126984126984126984126984126984126984126984127, // 1 / 7!

    2.755731922398589065255731922398589065255731922398589065255731922398589065255732e-6, // 1 / 9!
    -2.505210838544171877505210838544171877505210838544171877505210838544171877505211e-8, // 1 / 11!
    1.605904383682161459939237717015494793272571050348828126605904383682161459939238e-10, // 1 / 13!
    -7.647163731819816475901131985788070444155100239756324412409068493724578380663037e-13, // 1 / 15!

    2.811457254345520763198945583010320016233492735204531033973922240339918522302587e-15, // 1 / 17!
    -8.220635246624329716955981236872280749220738991826114134426673217368182813750255e-18, // 1 / 19!
    1.957294106339126123084757437350543035528747379006217651053969813659091146131013e-20, // 1 / 21!
    -3.868170170630684037716911931522812323179342646257347136470296074425081316464453e-23, // 1 / 23!

    6.446950284384473396194853219204687205298904410428911894117160124041802194107421e-26, // 1 / 25!
    -9.183689863795546148425716836473913397861687194343179336349230945928493153999175e-29, // 1 / 27!
    1.130996288644771693155876457693831699244050147086598440437097407134050881034381e-31, // 1 / 29!
    -1.216125041553517949629974685692292149724785104394191871437739147455968689284281e-34, // 1 / 31!
}

sin_radians_c_mftwp: [][]f64 = {
    // NOTE(casey): This minimax coefficient table was donated by Demetri Spanos

    {},
    {},

    {
	    9.961946980917455673916221e-01,
	   -1.482842712474619009760283e-01
	},

    {
        9.999281892109478476930401e-01,
       -1.648521135619455820378336e-01,
        7.560984839243006772977282e-03
    },

    {
        9.999993963052213389772869e-01,
       -1.666430606399511334029606e-01,
        8.337626007874941940825769e-03,
       -1.519754102400600552202617e-04
    },

    {
        9.999999994468865363130221e-01,
       -1.666665930227216735197680e-01,
        8.333916605540694618945070e-03,
       -1.543209912260127413625255e-04,
        4.398587920412271209512914e-06
    },

    {
        9.999999999997531743192112e-01,
       -1.666666642178561214922684e-01,
        8.333333842090844497771819e-03,
       -1.984124836043382031475849e-04,
        2.755723392860484413223696e-06,
       -3.719701166298862392037200e-08
    },

    {
        9.999999999999989897774152e-01,
       -1.666666665927825590408594e-01,
        8.333333333091738015747147e-03,
       -1.984126461212154195351156e-04,
        2.755731370707006767891547e-06,
       -2.505202978629679858903840e-08,
        1.351705687492395486949265e-10
    },

    {
        9.999999999999999444888487e-01,
       -1.666666666666621707658768e-01,
        8.333333333333208317594831e-03,
       -1.984126984121336516146694e-04,
        2.755731921174349662538843e-06,
       -2.505210692150271314387894e-08,
        1.605894189662054249701295e-10,
       -7.643043264597531094220676e-13
    },

	 {
   		1.000000000000000000000000e+00,
   		-1.666666666666666574148081e-01,
   		8.333333333333208317594831e-03,
   		-1.984126984121336516146694e-04,
   		2.755731921174349662538843e-06,
   		-2.505210692150271314387894e-08,
  		1.605894189662054249701295e-10,
  		-7.643043264597531094220676e-13,
	 	2.721675717057290534180242e-15,
   	},

    {
        1.000000000000000000000000e+00,
       -1.666666666666666574148081e-01,
        8.333333333333337553583926e-03,
       -1.984126984127273771445944e-04,
        2.755731923535281052393230e-06,
       -2.505210986084194333309167e-08,
        1.605923109573672146037452e-10,
       -7.649682007120227136824152e-13,
        2.744028517580152354848846e-15,
       -1.640127949858836731468045e-17
    },

    {
	     1.000000000000000000000000e+00,
	    -1.666666666666666574148081e-01,
	     8.333333333333336685121128e-03,
	    -1.984126984127065946990430e-04,
	     2.755731922417465801211373e-06,
	    -2.505210663024163233383057e-08,
	     1.605889428829893122392864e-10,
	    -7.623808477733182205245004e-13,
	     2.646244594596232722915146e-15,
	     4.589142760101694201378354e-17,
	    -8.617108010622280282273155e-18
	}
}

@(private="file")
odd_power_polynomial_approximation :: proc(coefficients: []f64, x: f64) -> f64 {
    x_squared: f64 = x * x

    coeff_index: int = len(coefficients) - 1
    result: f64 = coefficients[coeff_index]
    for coeff_index > 0 {
		coeff_index -= 1
        result = intrinsics.fused_mul_add(result, x_squared, coefficients[coeff_index])
    }
    result *= x

    return result
}

@(private="file")
sine_approximation_mftwp :: proc(x: f64) -> f64 {
    x_squared: f64 = x * x

    result: f64 = 2.721675717057290534180242e-15;
    result = intrinsics.fused_mul_add(result, x_squared, -7.643043264597531094220676e-13);
    result = intrinsics.fused_mul_add(result, x_squared, 1.605894189662054249701295e-10);
    result = intrinsics.fused_mul_add(result, x_squared, -2.505210692150271314387894e-08);
    result = intrinsics.fused_mul_add(result, x_squared, 2.755731921174349662538843e-06);
    result = intrinsics.fused_mul_add(result, x_squared, -1.984126984121336516146694e-04);
    result = intrinsics.fused_mul_add(result, x_squared, 8.333333333333208317594831e-03);
    result = intrinsics.fused_mul_add(result, x_squared, -1.666666666666666574148081e-01);
    result = intrinsics.fused_mul_add(result, x_squared, 1.000000000000000000000000e+00);
    result *= x;

    return result;
}

chapter10_dimitri_spanos_coefficient_arrays :: proc() {
    using pap_common

    /*
    for coeff_count in 2..<16 {
		for math_op_precision_tester_try_setup_next_precision_test(&tester, 0.0, PI64/2) {
			/*
            if coeff_count < len(sin_radians_c_taylor) {
		        math_op_precision_tester_test_using_latest_precision_test(&tester,
		            reference_value,
		            odd_power_polynomial_approximation(sin_radians_c_taylor[:coeff_count], tester.input_value),
		            fmt.tprintf("taylor_sine_approximation[%d]", coeff_count))
	    	}

			if coeff_count < len(sin_radians_c_mftwp) {
		        math_op_precision_tester_test_using_latest_precision_test(&tester,
		            reference_value,
		            odd_power_polynomial_approximation(sin_radians_c_mftwp[coeff_count][:], tester.input_value),
		            fmt.tprintf("min_max_sine_approximation[%d]", coeff_count))
	    	}
            */

	    	if coeff_count == 9 {
                reference_value: f64 = math.sin_f64(tester.input_value)
				math_op_precision_tester_test_using_latest_precision_test(&tester,
		            reference_value,
		            sine_approximation_mftwp(tester.input_value),
		            fmt.tprintf("min_max_sine_approximation_nine_coefficients[%d]", coeff_count))
	    	}
	    }
	}
    */

    tester: Math_Op_Tester

    fmt.printfln("input_domain : [%+.24f, %+.24f]", 0.0, PI64/2)
    for math_op_precision_tester_try_setup_next_precision_test(&tester, 0.0, PI64/2) {
        reference_value: f64 = math.sin_f64(tester.input_value)

        math_op_precision_tester_test_using_latest_precision_test(&tester,
            reference_value,
            odd_power_polynomial_approximation(sin_radians_c_taylor[:9], tester.input_value),
            fmt.tprintf("taylor_sine_approximation_nine_coefficients[%d]", 9))

        math_op_precision_tester_test_using_latest_precision_test(&tester,
            reference_value,
            sine_approximation_mftwp(tester.input_value),
            fmt.tprintf("min_max_sine_approximation_nine_coefficients[%d]", 9))
    }
    math_op_precision_tester_print_precision_test_results(&tester)
}