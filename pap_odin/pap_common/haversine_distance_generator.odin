package pap_common

import "core:fmt"
import "core:math/rand"
import "core:os"
import "core:path/filepath"

X_MAX :: 180 // x ranges from -X_MAX to +X_MAX
Y_MAX :: 90 // y ranges from -Y_MAX to +Y_MAX

Coordinate_Pair :: struct {
	x0: f32,
	y0: f32,
	x1: f32,
	y1: f32	
}

produce_haversine_distance_problem_files :: proc() {
  	haversine_input_file_relative_path := "haversine_json_files\\haversine_input.json";
  	haversine_reference_sum_file_relative_path := "haversine_json_files\\haversine_reference_sum.json";

	hav_input_fd, hav_input_err := os.open(haversine_input_file_relative_path, os.O_CREATE | os.O_RDWR)
	if hav_input_err != nil {
		return	
	}
	defer os.close(hav_input_fd)

	hav_ref_sum_fd, hav_ref_sum_err := os.open(haversine_reference_sum_file_relative_path, os.O_CREATE | os.O_RDWR)
	if hav_ref_sum_err != nil {
		return	
	}
	defer os.close(hav_ref_sum_fd)

/*
    x_center: f32 = 0;
    y_center: f32 = 0;
    x_radius: f32 = X_MAX;
    y_radius: f32 = Y_MAX;
*/
}