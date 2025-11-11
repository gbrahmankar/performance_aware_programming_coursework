package pap_common

import "core:fmt"
import "core:mem"

MAX_PROFILE_ANCHORS :: 1024

Profile_Anchor :: struct {
	tsc_elapsed_exclusive: u64,
	tsc_elapsed_inclusive: u64,

	hit_count: u64,
	processed_byte_count: u64,

    label: string
}

Profile_Scope :: struct {
	anchor_index: u64,
	parent_anchor_index: u64,

	label: string,

	start_tsc: u64,
	old_tsc_elapsed_inclusive: u64	
}

Global_Profiler_Data :: struct {
	profiler_start_tsc: u64,
	profiler_end_tsc: u64,	

	profile_anchors: [MAX_PROFILE_ANCHORS]Profile_Anchor,
	profile_scope_label_to_anchor_index_lookup: map[string]u64,
	currently_active_profile_anchor_index: u64,
	next_free_profile_anchor_index: u64
}

@(private)
global_profiler_data: Global_Profiler_Data 

profiler_begin_profiling :: proc() {	
	global_profiler_data.currently_active_profile_anchor_index = max(u64)
	global_profiler_data.next_free_profile_anchor_index = 0
	global_profiler_data.profile_scope_label_to_anchor_index_lookup = make(map[string]u64)

	global_profiler_data.profiler_start_tsc = read_tsc()
}

profiler_end_profiling :: proc() {	
	global_profiler_data.profiler_end_tsc = read_tsc()

	total_cpu_elapsed: u64 = global_profiler_data.profiler_end_tsc - global_profiler_data.profiler_start_tsc 
	
	if tsc_freq, _ := get_tsc_frequency(); tsc_freq > 0 {
		fmt.printfln("total_seconds_elapsed=%.3vms", compute_seconds_from_cpu_time(total_cpu_elapsed, tsc_freq) * 1000)

		for anchor_index in 0..<global_profiler_data.next_free_profile_anchor_index {
			anchor: ^Profile_Anchor = &global_profiler_data.profile_anchors[anchor_index]
			fmt.printfln("anchor_label=%v | time_elapsed_exclusive=%.3vms | time_elapsed_inclusive=%.3vms", 
				anchor.label, 
				compute_seconds_from_cpu_time(anchor.tsc_elapsed_exclusive, tsc_freq) * 1000,
				compute_seconds_from_cpu_time(anchor.tsc_elapsed_inclusive, tsc_freq) * 1000)
		}
	}
}

profiler_begin_profiling_scope :: proc(scope: ^Profile_Scope, scope_label: string, scope_bytes_to_process: u64) {
	scope.label = scope_label
	scope.parent_anchor_index = global_profiler_data.currently_active_profile_anchor_index 
	scope_anchor_index, existed := global_profiler_data.profile_scope_label_to_anchor_index_lookup[scope_label]
	if existed {
		scope.anchor_index = scope_anchor_index
	} else {
		scope.anchor_index = global_profiler_data.next_free_profile_anchor_index 
		global_profiler_data.next_free_profile_anchor_index += 1
		global_profiler_data.profile_scope_label_to_anchor_index_lookup[scope_label] = scope.anchor_index
	}
	global_profiler_data.currently_active_profile_anchor_index = scope.anchor_index	

	scope_anchor: ^Profile_Anchor = &global_profiler_data.profile_anchors[scope.anchor_index]
	scope_anchor.label = scope_label
	scope_anchor.hit_count += 1
	scope_anchor.processed_byte_count += scope_bytes_to_process

	scope.start_tsc = read_tsc()
}

profiler_end_profiling_scope :: proc(scope: ^Profile_Scope) {
	end_tsc := read_tsc()
	cpu_time_elapsed_in_scope: u64 = end_tsc - scope.start_tsc

	global_profiler_data.currently_active_profile_anchor_index = scope.parent_anchor_index	

	scope_anchor: ^Profile_Anchor = &global_profiler_data.profile_anchors[scope.anchor_index]
	scope_anchor.tsc_elapsed_exclusive += cpu_time_elapsed_in_scope
	scope_anchor.tsc_elapsed_inclusive += cpu_time_elapsed_in_scope

	if scope.parent_anchor_index == max(u64) {
		return
	}
	parent_scope_anchor: ^Profile_Anchor = &global_profiler_data.profile_anchors[scope.parent_anchor_index]
	parent_scope_anchor.tsc_elapsed_exclusive -= cpu_time_elapsed_in_scope
}