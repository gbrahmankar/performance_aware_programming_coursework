#+feature using-stmt

package part_five

import "../pap_common"

chapter01_our_very_own_haversine :: proc() {
    using pap_common

    coordinate_pairs, number_of_pairs, distances_between_pairs, average_sum := process_haversine_pairs_json_file()
    defer delete(coordinate_pairs)
    defer delete(distances_between_pairs)

    tally_produced_distances_and_avg_sum_against_reference(distances_between_pairs[:number_of_pairs], average_sum)
}