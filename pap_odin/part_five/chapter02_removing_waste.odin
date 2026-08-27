#+feature using-stmt

package part_five

import "core:fmt"
import "core:math"

import "../pap_common"

Process_Haversine_Function :: struct {
    test_name: string,
    test_func: proc(coordinate_pairs : []pap_common.Coordinate_Pair) -> (f64)
}

@(private="file")
chapter02_distance_between_points_evaluation_00 :: proc(coordinate_pairs : []pap_common.Coordinate_Pair) -> (f64) {
    using pap_common

    average_sum: f64
    number_of_pair_reciprocal: f64 = 1/cast(f64)len(coordinate_pairs)
    for pair in coordinate_pairs {
        lat1: f64 = pair.y0
        lat2: f64 = pair.y1
        lon1: f64 = pair.x0
        lon2: f64 = pair.x1

        degrees_lat: f64 = radians_from_degrees(lat2 - lat1)
        degrees_lon: f64 = radians_from_degrees(lon2 - lon1)
        lat1 = radians_from_degrees(lat1)
        lat2 = radians_from_degrees(lat2)

        a: f64 = square(sine_approximation(degrees_lat/2.0)) + cosine_approximation(lat1) * cosine_approximation(lat2) * square(sine_approximation(degrees_lon/2))
        c: f64 = 2.0 * asine_approximation(math.sqrt(a))

        result: f64 = EARTH_RADIUS * c

        average_sum += result * number_of_pair_reciprocal
    }

    return average_sum
}

@(private="file")
chapter02_distance_between_points_evaluation_01 :: proc(coordinate_pairs : []pap_common.Coordinate_Pair) -> (f64) {
    using pap_common

    average_sum: f64
    number_of_pair_reciprocal: f64 = 1/cast(f64)len(coordinate_pairs)
    for pair in coordinate_pairs {
        lat1: f64 = pair.y0
        lat2: f64 = pair.y1
        lon1: f64 = pair.x0
        lon2: f64 = pair.x1

        degrees_lat: f64 = 0.01745329251994329577 * (lat2 - lat1)
        degrees_lon: f64 = 0.01745329251994329577 * (lon2 - lon1)
        lat1 = 0.01745329251994329577 * (lat1)
        lat2 = 0.01745329251994329577 * (lat2)

        a: f64 = square(sine_approximation(degrees_lat/2.0)) + sine_approximation(lat1 + PI64/2.0) * sine_approximation(lat2 + PI64/2.0) * square(sine_approximation(degrees_lon/2))
        c: f64 = 2.0 * asine_approximation(math.sqrt(a))

        result: f64 = EARTH_RADIUS * c

        average_sum += result * number_of_pair_reciprocal
    }

    return average_sum
}

@(private="file")
chapter02_distance_between_points_evaluation_02 :: proc(coordinate_pairs : []pap_common.Coordinate_Pair) -> (f64) {
    using pap_common

    average_sum: f64
    number_of_pair_reciprocal: f64 = 1/cast(f64)len(coordinate_pairs)
    for pair in coordinate_pairs {
        lat1: f64 = pair.y0
        lat2: f64 = pair.y1
        lon1: f64 = pair.x0
        lon2: f64 = pair.x1

        degrees_lat: f64 = 0.01745329251994329577 * (lat2 - lat1) / 2
        degrees_lon: f64 = 0.01745329251994329577 * (lon2 - lon1) / 2
        lat1 = 0.01745329251994329577 * (lat1)
        lat2 = 0.01745329251994329577 * (lat2)

        a: f64 = square(sine_approximation(degrees_lat)) + sine_approximation(lat1 + PI64/2.0) * sine_approximation(lat2 + PI64/2.0) * square(sine_approximation(degrees_lon))
        c: f64 = 2.0 * asine_approximation(math.sqrt(a))

        result: f64 = EARTH_RADIUS * c

        average_sum += result * number_of_pair_reciprocal
    }

    return average_sum
}

@(private="file")
chapter02_distance_between_points_tester :: proc(coordinate_pairs : []pap_common.Coordinate_Pair, reference_average_sum: f64) {
    using pap_common

    test_functions: []Process_Haversine_Function = {
        { "chapter02_distance_between_points_evaluation_00", chapter02_distance_between_points_evaluation_00 },
        { "chapter02_distance_between_points_evaluation_02", chapter02_distance_between_points_evaluation_02 },
    }

    cpu_freq, _ := get_tsc_frequency()

    for func in test_functions {
        _ = fmt.printfln("\n\n\n------ start repeat_testing : %s ------", func.test_name)

        tester := repetition_tester_create(cpu_freq)
        repitition_tester_begin_new_test_instance(&tester, size_of(Coordinate_Pair) * cast(u64)len(coordinate_pairs), 2)
        for repitition_tester_is_testing(&tester) {
            repetition_tester_begin_time(&tester)

            average_sum := func.test_func(coordinate_pairs)
	        fmt.assertf(approx_equal(average_sum, reference_average_sum),
                "average_sum is incorrect. reference_average_sum=%v != average_sum=%v | delta=%v",
                reference_average_sum, average_sum, reference_average_sum - average_sum)
            repetition_tester_count_bytes(&tester, size_of(Coordinate_Pair) * cast(u64)len(coordinate_pairs))

            repetition_tester_end_time(&tester)
        }

        _ = fmt.printfln("------ end repeat_testing : %s ------", func.test_name)
    }
}

chapter02_removing_waste :: proc() {
    using pap_common

    multiline_str := `this is chapter02 of part_05.
in this exercise, i will progressively try and remove waste from the loop
that does the haversine distance computation between all the point pairs.`

    fmt.println(multiline_str)

    coordinate_pairs, number_of_pairs, distances_between_pairs, reference_average_sum := process_haversine_pairs_json_file()
    defer delete(coordinate_pairs)
    defer delete(distances_between_pairs)

    chapter02_distance_between_points_tester(coordinate_pairs[: number_of_pairs], reference_average_sum)
}