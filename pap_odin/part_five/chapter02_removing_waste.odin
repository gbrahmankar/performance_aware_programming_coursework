#+feature using-stmt

package part_five

import "core:fmt"

import "../pap_common"

chapter02_removing_waste :: proc() {
    using pap_common

    coordinate_pairs, number_of_pairs, distances_between_pairs, reference_average_sum := process_haversine_pairs_json_file()
    defer delete(coordinate_pairs)
    defer delete(distances_between_pairs)

    for pair in coordinate_pairs {
        work := our_own_haversine(pair.x0, pair.y0, pair.x1, pair.y1)
        work += 0.0
    }
}