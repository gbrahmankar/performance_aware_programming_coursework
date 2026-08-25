#+feature using-stmt

package part_five

import "core:fmt"
import "core:math"
import "core:mem"

import "../pap_common"

Process_Haversine_Function :: struct {
    test_name: string,
    test_func: proc(coordinate_pairs : []pap_common.Coordinate_Pair)
}

@(private="file")
chapter02_distance_between_points_evaluation_00 :: proc(coordinate_pairs : []pap_common.Coordinate_Pair) {
    using pap_common

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
        result += 0.0
    }
}

@(private="file")
chapter02_distance_between_points_evaluation_01 :: proc(coordinate_pairs : []pap_common.Coordinate_Pair) {
    using pap_common

    for pair in coordinate_pairs {
        lat1: f64 = pair.y0
        lat2: f64 = pair.y1
        lon1: f64 = pair.x0
        lon2: f64 = pair.x1
    }
}

@(private="file")
chapter02_distance_between_points_tester :: proc(coordinate_pairs : []pap_common.Coordinate_Pair) {
    using pap_common

    test_functions: []Process_Haversine_Function = {
        { "chapter02_distance_between_points_evaluation_00", chapter02_distance_between_points_evaluation_00 },
        { "chapter02_distance_between_points_evaluation_01", chapter02_distance_between_points_evaluation_01 },
    }

    cpu_freq, _ := get_tsc_frequency()

    for func in test_functions {
        _ = fmt.printf("\n\n\n------ start repeat_testing : %s ------\n", func.test_name)

        tester := repetition_tester_create(cpu_freq)
        repitition_tester_begin_new_test_instance(&tester, size_of(Coordinate_Pair) * cast(u64)len(coordinate_pairs), 2)
        for repitition_tester_is_testing(&tester) {
            repetition_tester_begin_time(&tester)

            func.test_func(coordinate_pairs)
            repetition_tester_count_bytes(&tester, size_of(Coordinate_Pair) * cast(u64)len(coordinate_pairs))

            repetition_tester_end_time(&tester)
        }

        _ = fmt.printf("------ end repeat_testing : %s ------\n", func.test_name)
    }
}

chapter02_removing_waste :: proc() {
    using pap_common

    coordinate_pairs, number_of_pairs, distances_between_pairs, reference_average_sum := process_haversine_pairs_json_file()
    defer delete(coordinate_pairs)
    defer delete(distances_between_pairs)

    chapter02_distance_between_points_tester(coordinate_pairs)
}