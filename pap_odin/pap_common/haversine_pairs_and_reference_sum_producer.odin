package pap_common

import "core:fmt"
import "core:math/rand"
import "core:os"
import "core:strings"

X_MAX :: 180 // x ranges from -X_MAX to +X_MAX
Y_MAX :: 90 // y ranges from -Y_MAX to +Y_MAX
MAX_PAIR_PRODUCE_COUNT :: 1 << 34

Coordinate_Pair :: struct {
	x0: f64,
	y0: f64,
	x1: f64,
	y1: f64	
}

random_degree :: proc(center, radius, max_allowed: f64) -> (f64) {
    min_val: f64 = center - radius;
    if min_val < -max_allowed {
        min_val = -max_allowed;
    }

    max_val: f64 = center + radius;
    if max_val > max_allowed {
        max_val = max_allowed
    }

    return rand.float64_range(min_val, max_val)
}

produce_haversine_distance_problem_files :: proc(pairs_to_produce: u64) {
	fmt.assertf(pairs_to_produce <= MAX_PAIR_PRODUCE_COUNT, 
		"pairs_to_produce=%v > MAX_PAIR_PRODUCE_COUNT=%v", 
		pairs_to_produce, MAX_PAIR_PRODUCE_COUNT)

  	haversine_input_file_relative_path := "haversine_json_files\\haversine_input.json";
  	haversine_reference_sum_file_relative_path := "haversine_json_files\\haversine_reference_sum.json";

	hav_input_fd, hav_input_err := os.open(haversine_input_file_relative_path, os.O_CREATE | os.O_RDWR | os.O_TRUNC)
	if hav_input_err != nil {
		return	
	}
	defer os.close(hav_input_fd)

	hav_ref_sum_fd, hav_ref_sum_err := os.open(haversine_reference_sum_file_relative_path, os.O_CREATE | os.O_RDWR | os.O_TRUNC)
	if hav_ref_sum_err != nil {
		return	
	}
	defer os.close(hav_ref_sum_fd)

    // center/extents for choosing the 64 random_sized sectors on earth's surface
    x_center: f64 = 0
    y_center: f64 = 0
    x_radius: f64 = X_MAX
    y_radius: f64 = Y_MAX

	max_pairs_to_produce_per_cluster: u64 = 1 + pairs_to_produce/64
	remaining_pairs_to_produce_for_current_cluster: u64 = 0
	current_cluster_index: i64 = -1
	running_average_sum: f64	
	sum_coeff: f64 = 1 / cast(f64)pairs_to_produce 

	os.write_string(hav_input_fd, "{ \"pairs\" : [\n")
	for pair_index in 0..<pairs_to_produce {
		if remaining_pairs_to_produce_for_current_cluster == 0 {
			remaining_pairs_to_produce_for_current_cluster = max_pairs_to_produce_per_cluster

			x_center = rand.float64_range(-X_MAX, X_MAX)
            y_center = rand.float64_range(-Y_MAX, Y_MAX)
            x_radius = rand.float64_range(0, X_MAX)
            y_radius = rand.float64_range(0, Y_MAX)

			/*
			current_cluster_index += 1
			fmt.printfln("----------------%v-------------------", current_cluster_index)
			*/

			/*
			fmt.printfln("new_cluster_chosen_index=%v", current_cluster_index)
			*/
		}
		remaining_pairs_to_produce_for_current_cluster -= 1

		x0: f64 = random_degree(x_center, x_radius, X_MAX);
        y0: f64 = random_degree(y_center, y_radius, Y_MAX);
        x1: f64 = random_degree(x_center, x_radius, X_MAX);
        y1: f64 = random_degree(y_center, y_radius, Y_MAX);

       	coordinate_pair_string: strings.Builder = strings.builder_make_none()
		json_seperator: string = (pair_index == (pairs_to_produce - 1)) ? "\n" : ",\n";
        fmt.sbprintf(&coordinate_pair_string, 
        	"	{{ \"x0\" : %v, \"y0\" : %v, \"x1\" : %v, \"y1\" : %v }}%v", 
        	x0, y0, x1, y1, json_seperator)
        os.write_string(hav_input_fd, strings.to_string(coordinate_pair_string))
        strings.builder_reset(&coordinate_pair_string)

        pair_distance: f64 = reference_haversine(x0, y0, x1, y1)
		pair_distance_string: strings.Builder = strings.builder_make_none()
        fmt.sbprintf(&pair_distance_string, "%v\n", pair_distance)
        os.write_string(hav_ref_sum_fd, strings.to_string(pair_distance_string))
        strings.builder_reset(&pair_distance_string)

        running_average_sum += sum_coeff * pair_distance

		/*
		fmt.printfln("x0=%v, y0=%v, x1=%v, y1=%v",
			x0, y0, x1, y1)
		*/

		/*
		fmt.printfln("producing_pair_index=%v for current_cluster_index=%v | remaining_pairs_to_produce_for_current_cluster=%v", 
			pair_index, 
			current_cluster_index,
			remaining_pairs_to_produce_for_current_cluster)
		*/
	}
	os.write_string(hav_input_fd, "] }\n")

	average_sum_string: strings.Builder = strings.builder_make_none()
    fmt.sbprintf(&average_sum_string, "%v", running_average_sum)
    os.write_string(hav_ref_sum_fd, strings.to_string(average_sum_string))
    strings.builder_reset(&average_sum_string)
}