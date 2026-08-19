#+feature using-stmt

package part_five

import "core:fmt"

import "../pap_common"

chapter01_our_very_own_haversine :: proc() {
    using pap_common

    tester : Math_Op_Tester

    coordinate_pairs, number_of_pairs, distances_between_pairs, reference_average_sum := process_haversine_pairs_json_file()
    defer delete(coordinate_pairs)
    defer delete(distances_between_pairs)

    fmt.printfln("precision_testing our_own_haversine against the reference_haversine")
    for math_op_precision_tester_try_setup_next_precision_test(&tester, 0.0, cast(f64)number_of_pairs - 1, number_of_pairs) {
        pair := coordinate_pairs[tester.step_index]
        math_op_precision_tester_test_using_latest_precision_test(&tester,
            reference_haversine(pair.x0, pair.y0, pair.x1, pair.y1),
            our_own_haversine(pair.x0, pair.y0, pair.x1, pair.y1),
            fmt.tprintf("our_own_haversine"))
    }
    math_op_precision_tester_print_precision_test_results(&tester)

    our_own_haversine_average_sum : f64 = 0.0
    for pair in coordinate_pairs {
        our_own_haversine_average_sum += our_own_haversine(pair.x0, pair.y0, pair.x1, pair.y1)/cast(f64)number_of_pairs
    }

    fmt.printfln("reference_average_sum=%v", reference_average_sum)
    fmt.printfln("our_own_haversine_average_sum=%v, ref_avg_sum - our_own_haversine_avg_sum=%+.24f",
        our_own_haversine_average_sum,
        reference_average_sum - our_own_haversine_average_sum)
}