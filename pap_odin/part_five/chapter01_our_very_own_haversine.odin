#+feature using-stmt

package part_five

import "core:fmt"

import "../pap_common"

chapter01_our_very_own_haversine :: proc() {
    using pap_common

    tester : Math_Op_Tester

    coordinate_pairs, number_of_pairs, distances_between_pairs, average_sum := process_haversine_pairs_json_file()
    defer delete(coordinate_pairs)
    defer delete(distances_between_pairs)

    fmt.printfln("precision_testing our_own_haversine against the reference_haversine")
    for math_op_precision_tester_try_setup_next_precision_test(&tester, 0.0, cast(f64)number_of_pairs - 1, number_of_pairs) {
        pair := coordinate_pairs[tester.step_index]
        math_op_precision_tester_test_using_latest_precision_test(&tester,
            reference_haversine(pair.x0, pair.y0, pair.x1, pair.y1),
            our_own__haversine(pair.x0, pair.y0, pair.x1, pair.y1),
            fmt.tprintf("our_own_haversine"))
    }
    math_op_precision_tester_print_precision_test_results(&tester)
}