package part_four

import "core:fmt"
import "core:math"

import "../pap_common"

Interval :: struct {
	min: f64,
	max: f64
}

Math_Op_Info :: struct {
	input_domain: Interval,
	output_range: Interval
}

Haversine_Math_Ops_Info :: struct {
	sin: Math_Op_Info,
	cos: Math_Op_Info,
	asin: Math_Op_Info,

	sqrt: Math_Op_Info,

	initialized_yet: bool
}
global_haversine_math_ops_info: Haversine_Math_Ops_Info

// haversine_math_ops_*
haversine_math_ops_update_interval :: proc(interval: ^Interval, value: f64) {
	if value < interval.min {
		interval.min = value
	}

	if value > interval.max {
		interval.max = value
	}
}

haversine_math_ops_initialize_info :: proc() {	
	initial_interval: Interval = {
		min = max(f64),
		max = min(f64)
	}

	global_haversine_math_ops_info.sin.input_domain = initial_interval
	global_haversine_math_ops_info.sin.output_range = initial_interval

	global_haversine_math_ops_info.cos.input_domain = initial_interval
	global_haversine_math_ops_info.cos.output_range = initial_interval

	global_haversine_math_ops_info.asin.input_domain = initial_interval
	global_haversine_math_ops_info.asin.output_range = initial_interval

	global_haversine_math_ops_info.sqrt.input_domain = initial_interval
	global_haversine_math_ops_info.sqrt.output_range = initial_interval

	global_haversine_math_ops_info.initialized_yet = true
}

haversine_math_ops_update_sin_input_domain :: proc(value: f64) -> (f64) {
	fmt.assertf(global_haversine_math_ops_info.initialized_yet == true, 
		"global_haversine_math_ops_info not initialized yet !")

	haversine_math_ops_update_interval(&global_haversine_math_ops_info.sin.input_domain, value)
	return math.sin(value)
}

haversine_math_ops_update_cos_input_domain :: proc(value: f64) -> (f64) {
	fmt.assertf(global_haversine_math_ops_info.initialized_yet == true, 
		"global_haversine_math_ops_info not initialized yet !")

	haversine_math_ops_update_interval(&global_haversine_math_ops_info.cos.input_domain, value)
	return math.cos(value)
}

haversine_math_ops_update_asin_input_domain :: proc(value: f64) -> (f64) {
	fmt.assertf(global_haversine_math_ops_info.initialized_yet == true, 
		"global_haversine_math_ops_info not initialized yet !")

	haversine_math_ops_update_interval(&global_haversine_math_ops_info.asin.input_domain, value)
	return math.asin_f64(value)
}

haversine_math_ops_update_sqrt_input_domain :: proc(value: f64) -> (f64) {
	fmt.assertf(global_haversine_math_ops_info.initialized_yet == true, 
		"global_haversine_math_ops_info not initialized yet !")

	haversine_math_ops_update_interval(&global_haversine_math_ops_info.sqrt.input_domain, value)
	return math.sqrt(value)
}

haversine_math_ops_calculate_distance_and_update_math_ops_input_domain :: proc(x0: f64, y0: f64, x1: f64, y1: f64) -> (f64) {
	using pap_common

    lat1: f64 = y0
    lat2: f64 = y1
    lon1: f64 = x0
    lon2: f64 = x1

    degrees_lat: f64 = radians_from_degrees(lat2 - lat1)
    degrees_lon: f64 = radians_from_degrees(lon2 - lon1)
    lat1 = radians_from_degrees(lat1)
    lat2 = radians_from_degrees(lat2)

    a: f64 = square(haversine_math_ops_update_sin_input_domain(degrees_lat/2.0)) + haversine_math_ops_update_cos_input_domain(lat1) * haversine_math_ops_update_cos_input_domain(lat2) * square(haversine_math_ops_update_sin_input_domain(degrees_lon/2))
    c: f64 = 2.0 * haversine_math_ops_update_asin_input_domain(haversine_math_ops_update_sqrt_input_domain(a))
    result: f64 = EARTH_RADIUS * c

    return result
}

haversine_math_ops_print_input_domains :: proc() {
	fmt.assertf(global_haversine_math_ops_info.initialized_yet == true, 
		"global_haversine_math_ops_info not initialized yet !")

	fmt.printfln("sin : min=%.8f, max=%.8f", 
		global_haversine_math_ops_info.sin.input_domain.min, 
		global_haversine_math_ops_info.sin.input_domain.max)

	fmt.printfln("cos : min=%.8f, max=%.8f", 
		global_haversine_math_ops_info.cos.input_domain.min, 
		global_haversine_math_ops_info.cos.input_domain.max)

	fmt.printfln("asin : min=%.8f, max=%.8f", 
		global_haversine_math_ops_info.asin.input_domain.min, 
		global_haversine_math_ops_info.asin.input_domain.max)

	fmt.printfln("sqrt : min=%.8f, max=%.8f", 
		global_haversine_math_ops_info.sqrt.input_domain.min, 
		global_haversine_math_ops_info.sqrt.input_domain.max)
}

haversine_math_ops_update_input_domains :: proc() {
	using pap_common

    coordinate_pairs, number_of_pairs, distances_between_pairs, average_sum := process_haversine_pairs_json_file()
    defer delete(coordinate_pairs)
    defer delete(distances_between_pairs)

	haversine_math_ops_initialize_info()

    for pair_index in 0..<number_of_pairs {
    	pair: ^Coordinate_Pair = &coordinate_pairs[pair_index]	
    	ref_pair_distance: f64 = reference_haversine(pair.x0, pair.y0, pair.x1, pair.y1)
		update_input_domain_pair_distance: f64 = haversine_math_ops_calculate_distance_and_update_math_ops_input_domain(pair.x0, pair.y0, pair.x1, pair.y1)

		fmt.assertf(update_input_domain_pair_distance == ref_pair_distance, 
			"o/p of update_interval_distance_calculator and ref_calculator do not match !")
    }

    haversine_math_ops_print_input_domains()
}