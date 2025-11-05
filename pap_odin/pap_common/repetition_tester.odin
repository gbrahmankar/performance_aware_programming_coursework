package pap_common

import "core:fmt"
import "core:mem"

Test_Phase :: enum {
    Test_Phase_Uninitialized,
    Test_Phase_Testing,
    Test_Phase_Completed,
    Test_Phase_Error,

    Test_Phase_Invalid
}

Repetition_Metric :: enum {
    Repetition_Metric_CPU_Timer,
    Repetition_Metric_MemPageFaults,
    Repetition_Metric_ByteCount,
    Repetition_Metric_Test_Count,

    Repetition_Metric_Count
}

Repetition_Metrics_Init_Type :: enum {
    Repetition_Metrics_Init_Type_Min,
    Repetition_Metrics_Init_Type_Max,

    Repetition_Metrics_Init_Type_Count
}

Repetition_Metrics :: struct {
	metrics: [Repetition_Metric.Repetition_Metric_Count]u64,
	observed_in_test_number: u64 	
}

repetition_tester_metrics_init :: proc(metrics: ^Repetition_Metrics, default_init_type: Repetition_Metrics_Init_Type) {
	metrics.observed_in_test_number = 0
	for i in cast(Repetition_Metric)0..<Repetition_Metric.Repetition_Metric_Count {
		if default_init_type == .Repetition_Metrics_Init_Type_Min {
			metrics.metrics[i] = 0
		} else {
			metrics.metrics[i] = max(u64) - 1
		}
	}
}

Repetition_Test_Result :: struct {
	min_metrics: Repetition_Metrics,
	max_metrics: Repetition_Metrics,
	total_accumulated_metrics: Repetition_Metrics
}

Repetition_Tester :: struct {
	test_phase: Test_Phase,

	cpu_timer_frequency: u64,
	target_processed_byte_count: u64,
	try_for_time: u64,	
	test_instance_started_at: u64,	

	open_block_count: u64,
	closed_block_count: u64,

	current_wave_metrics: Repetition_Metrics,
	test_result: Repetition_Test_Result
}

repetition_tester_create :: proc(cpu_freq: u64) -> (Repetition_Tester) {
	tester: Repetition_Tester = {
		test_phase = Test_Phase.Test_Phase_Uninitialized,
		cpu_timer_frequency = cpu_freq,
		target_processed_byte_count = 0,
		try_for_time = 0,
		open_block_count = 0,
		closed_block_count = 0
	}

	repetition_tester_metrics_init(&tester.current_wave_metrics, .Repetition_Metrics_Init_Type_Min)

	test_result: ^Repetition_Test_Result = &tester.test_result
	repetition_tester_metrics_init(&test_result.min_metrics, .Repetition_Metrics_Init_Type_Max)
	repetition_tester_metrics_init(&test_result.max_metrics, .Repetition_Metrics_Init_Type_Min)
	repetition_tester_metrics_init(&test_result.total_accumulated_metrics, .Repetition_Metrics_Init_Type_Min)

	return tester
}

repitition_tester_begin_new_test_instance :: proc(tester: ^Repetition_Tester, target_byte_count: u64, seconds_to_try: u64) {
	if tester.test_phase == .Test_Phase_Uninitialized {
		tester.test_phase = .Test_Phase_Testing
		tester.target_processed_byte_count = target_byte_count
		repetition_tester_metrics_init(&tester.current_wave_metrics, .Repetition_Metrics_Init_Type_Min)
	} else if tester.test_phase == .Test_Phase_Completed {
		tester.test_phase = .Test_Phase_Testing

		if tester.target_processed_byte_count != target_byte_count {
			fmt.printfln("target_byte_count changed from=%v to=%v", tester.target_processed_byte_count, target_byte_count)
		}
	}

    tester.try_for_time = seconds_to_try * tester.cpu_timer_frequency
    tester.test_instance_started_at = read_tsc();
}

repitition_tester_is_testing :: proc(tester: ^Repetition_Tester) -> (bool) {
	if tester.test_phase != .Test_Phase_Testing {
		return false
	}	

	if tester.open_block_count == 0 {
		return true	
	}	

	current_wave_metrics: ^Repetition_Metrics = &tester.current_wave_metrics
	assert(tester.open_block_count == tester.closed_block_count)
	assert(current_wave_metrics.metrics[Repetition_Metric.Repetition_Metric_ByteCount] == tester.target_processed_byte_count)

	test_result: ^Repetition_Test_Result = &tester.test_result
    test_result.total_accumulated_metrics.metrics[Repetition_Metric.Repetition_Metric_Test_Count] += 1;
	for i in cast(Repetition_Metric)0..<Repetition_Metric.Repetition_Metric_Count {
    	test_result.total_accumulated_metrics.metrics[i] += current_wave_metrics.metrics[i];
	}

	if test_result.max_metrics.metrics[Repetition_Metric.Repetition_Metric_CPU_Timer] < current_wave_metrics.metrics[Repetition_Metric.Repetition_Metric_CPU_Timer] {
		test_result.max_metrics = current_wave_metrics^
		test_result.max_metrics.observed_in_test_number = test_result.total_accumulated_metrics.metrics[Repetition_Metric.Repetition_Metric_Test_Count] 

		fmt.printfln("\n---worst_results_yet_start---")
  	  	repetition_tester_print_repetition_metric(tester, "current_max_metrics", &test_result.max_metrics);
		fmt.printfln("---worst_results_yet_end---")
	}

    current_time: u64 = read_tsc();
	if test_result.min_metrics.metrics[Repetition_Metric.Repetition_Metric_CPU_Timer] > current_wave_metrics.metrics[Repetition_Metric.Repetition_Metric_CPU_Timer] {
		test_result.min_metrics = current_wave_metrics^
		test_result.min_metrics.observed_in_test_number = test_result.total_accumulated_metrics.metrics[Repetition_Metric.Repetition_Metric_Test_Count] 

		fmt.printfln("\n---best_results_yet_start---")
  	  	repetition_tester_print_repetition_metric(tester, "current_min_metrics", &test_result.min_metrics);
		fmt.printfln("---best_results_yet_end---")

		tester.test_instance_started_at = current_time
	}

    tester.open_block_count = 0;
    tester.closed_block_count = 0;
	repetition_tester_metrics_init(current_wave_metrics, .Repetition_Metrics_Init_Type_Min)

	if current_time - tester.test_instance_started_at > tester.try_for_time {
		tester.test_phase = .Test_Phase_Completed	

		repetition_tester_print_test_result(tester)
	}

	return true
}

repetition_tester_begin_time :: proc(tester: ^Repetition_Tester) {
	tester.open_block_count += 1	

	tester.current_wave_metrics.metrics[Repetition_Metric.Repetition_Metric_CPU_Timer] -= read_tsc()
	tester.current_wave_metrics.metrics[Repetition_Metric.Repetition_Metric_MemPageFaults] -= cast(u64)read_page_faults() 
}

repetition_tester_end_time :: proc(tester: ^Repetition_Tester) {
	tester.closed_block_count += 1	

	tester.current_wave_metrics.metrics[Repetition_Metric.Repetition_Metric_CPU_Timer] += read_tsc()
	tester.current_wave_metrics.metrics[Repetition_Metric.Repetition_Metric_MemPageFaults] += cast(u64)read_page_faults() 
}

repetition_tester_count_bytes :: proc(tester: ^Repetition_Tester, bytes_to_accumulate: u64) {
	tester.current_wave_metrics.metrics[Repetition_Metric.Repetition_Metric_ByteCount] += bytes_to_accumulate
}

repetition_tester_print_repetition_metric :: proc(tester: ^Repetition_Tester, label: string, metric: ^Repetition_Metrics) {
	test_count: u64 = metric.metrics[Repetition_Metric.Repetition_Metric_Test_Count]
	divisor: f64 = test_count > 0 ? cast(f64)test_count : 1

	normalized_metrics: [Repetition_Metric.Repetition_Metric_Count]f64
	for i in cast(Repetition_Metric)0..<Repetition_Metric.Repetition_Metric_Count {
		normalized_metrics[i] = cast(f64)metric.metrics[i] / divisor
	}

	seconds: f64 = compute_seconds_from_cpu_time(cast(u64)normalized_metrics[Repetition_Metric.Repetition_Metric_CPU_Timer], tester.cpu_timer_frequency)

	fmt.printfln("label=%v | observed_in_test_number=%v | time_taken=%.4vms | processed_megabytes=%.4v | page_faults=%.2v", 
		label, 
		metric.observed_in_test_number,
		seconds * 1000, 
		normalized_metrics[Repetition_Metric.Repetition_Metric_ByteCount] / mem.Megabyte,
		normalized_metrics[Repetition_Metric.Repetition_Metric_MemPageFaults])

	if normalized_metrics[Repetition_Metric.Repetition_Metric_ByteCount] > 0 {
		gigabyte_per_sec: f64 = normalized_metrics[Repetition_Metric.Repetition_Metric_ByteCount] / (mem.Gigabyte * seconds)
		fmt.printfln("bandwidth=%.3vgb/s", gigabyte_per_sec)
	}

	fmt.printfln("kilo_bytes_per_page_fault=%.3v", 
		normalized_metrics[Repetition_Metric.Repetition_Metric_ByteCount] / (cast(f64)normalized_metrics[Repetition_Metric.Repetition_Metric_MemPageFaults] * 1024))
}

repetition_tester_print_test_result :: proc(tester: ^Repetition_Tester) {
	fmt.printfln("\n\n---test_results_start---")

	test_result: ^Repetition_Test_Result = &tester.test_result

	fmt.printfln("\n---best_results_start---")
	repetition_tester_print_repetition_metric(tester, "min_time", &test_result.min_metrics);
	fmt.printfln("---best_results_end---")

	fmt.printfln("\n---worst_results_start---")
    repetition_tester_print_repetition_metric(tester, "max_time", &test_result.max_metrics);
	fmt.printfln("---worst_results_start---")

    if test_result.total_accumulated_metrics.metrics[Repetition_Metric.Repetition_Metric_Test_Count] > 0 {
		fmt.printfln("\n---average_results_start---")
        repetition_tester_print_repetition_metric(tester, "average", &test_result.total_accumulated_metrics);
		fmt.printfln("---average_results_end---")
    }

	fmt.printfln("\n---test_results_end---")
}