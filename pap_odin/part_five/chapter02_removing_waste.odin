#+feature using-stmt

package part_five

import "core:fmt"
import "core:math"

import "../pap_common"

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

chapter02_removing_waste :: proc() {
    using pap_common

    coordinate_pairs, number_of_pairs, distances_between_pairs, reference_average_sum := process_haversine_pairs_json_file()
    defer delete(coordinate_pairs)
    defer delete(distances_between_pairs)

    chapter02_distance_between_points_evaluation_00(coordinate_pairs)
}