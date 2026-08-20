package pap_common

import "base:intrinsics"
import "core:math"
import "core:simd"

EARTH_RADIUS :: 6372.8

square :: proc(number: f64) -> (f64) {  
    return number * number
}

radians_from_degrees :: proc(degrees: f64) -> (f64) {
    return 0.01745329251994329577 * degrees;
}

// haversine math operations
sine_approximation :: proc(orig_x: f64) -> f64 {
    pos_x: f64 = math.abs(orig_x)
    x: f64 = (pos_x > PI64/2) ? (PI64 - pos_x) : pos_x

    x_squared: f64 = x * x

    result: f64 = 0.0000000000000027216757170572905341802418158794654755574192982192638368132975301705300807952880859375;
    result = intrinsics.fused_mul_add(result, x_squared,
        -0.000000000000764304326459753109422067633142294847017721159954106724399025551974773406982421875);
    result = intrinsics.fused_mul_add(result, x_squared,
        0.0000000001605894189662054249701294800524160703847353914852646994404494762420654296875);
    result = intrinsics.fused_mul_add(result, x_squared,
        -0.0000000250521069215027131438789363172825463976778337382711470127105712890625);
    result = intrinsics.fused_mul_add(result, x_squared,
        0.0000027557319211743496625388431808811873224840383045375347137451171875);
    result = intrinsics.fused_mul_add(result, x_squared,
        -0.0001984126984121336516146694162188168775173835456371307373046875);
    result = intrinsics.fused_mul_add(result, x_squared,
        0.00833333333333320831759483127143539604730904102325439453125);
    result = intrinsics.fused_mul_add(result, x_squared,
        -0.1666666666666666574148081281236954964697360992431640625);
    result = intrinsics.fused_mul_add(result, x_squared,
        1.000000000000000000000000);
    result *= x

    result = (orig_x < 0) ? -result : result

    return result
}

cosine_approximation :: proc(x: f64) -> f64 {
    result: f64 = sine_approximation(x + PI64/2.0)
    return result
}

// note(gaurav) : sqrt(...) is free because cpu has a dedicated instruction for it. no need to approximate.
sqrt_approximation :: proc(input_value: f64) -> f64 {
    return simd.sqrt(input_value)
}

asine_approximation :: proc(orig_x: f64) -> f64 {
    needs_transform: bool = (orig_x > 0.7071067811865475244)
    x: f64 = needs_transform ? sqrt_approximation(1.0 - orig_x*orig_x) : orig_x

    x_squared: f64 = x * x

    r: f64 = 0.937051490234062756101707;
    r = simd.fma(r, x_squared, -3.490443583255612214344410);
    r = simd.fma(r, x_squared, 6.186819757295904054217317);
    r = simd.fma(r, x_squared, -6.756794510834697398138360);
    r = simd.fma(r, x_squared, 5.081534831018740483443707);
    r = simd.fma(r, x_squared, -2.753168488249635892373135);
    r = simd.fma(r, x_squared, 1.123860218902060115198083);
    r = simd.fma(r, x_squared, -0.335174736401442241451321);
    r = simd.fma(r, x_squared, 0.088305893045829114695877);
    r = simd.fma(r, x_squared, -0.004413816332621807794756);
    r = simd.fma(r, x_squared, 0.013447193221907542334814);
    r = simd.fma(r, x_squared, 0.013776824789416922470431);
    r = simd.fma(r, x_squared, 0.017366257890261350277372);
    r = simd.fma(r, x_squared, 0.022371484508100256821672);
    r = simd.fma(r, x_squared, 0.030381966628286034143303);
    r = simd.fma(r, x_squared, 0.044642856704867270312143);
    r = simd.fma(r, x_squared, 0.075000000004415118270984);
    r = simd.fma(r, x_squared, 0.166666666666650920003434);
    r = simd.fma(r, x_squared, 1.000000000000000000000000);
    r *= x

    result: f64 = needs_transform ? (1.57079632679489661923 - r) : r
    return result;
}

reference_haversine :: proc(x0: f64, y0: f64, x1: f64, y1: f64) -> (f64) {
    lat1: f64 = y0
    lat2: f64 = y1
    lon1: f64 = x0
    lon2: f64 = x1

    degrees_lat: f64 = radians_from_degrees(lat2 - lat1)
    degrees_lon: f64 = radians_from_degrees(lon2 - lon1)
    lat1 = radians_from_degrees(lat1)
    lat2 = radians_from_degrees(lat2)

    a: f64 = square(math.sin(degrees_lat/2.0)) + math.cos(lat1) * math.cos(lat2) * square(math.sin(degrees_lon/2))
    c: f64 = 2.0 * math.asin_f64(math.sqrt(a))
    result: f64 = EARTH_RADIUS * c

    return result
}

// using our own approximations -- tested in part_five_chapter01
our_own_haversine :: proc(x0: f64, y0: f64, x1: f64, y1: f64) -> (f64) {
    lat1: f64 = y0
    lat2: f64 = y1
    lon1: f64 = x0
    lon2: f64 = x1

    degrees_lat: f64 = radians_from_degrees(lat2 - lat1)
    degrees_lon: f64 = radians_from_degrees(lon2 - lon1)
    lat1 = radians_from_degrees(lat1)
    lat2 = radians_from_degrees(lat2)

    a: f64 = square(sine_approximation(degrees_lat/2.0)) + cosine_approximation(lat1) * cosine_approximation(lat2) * square(sine_approximation(degrees_lon/2))
    c: f64 = 2.0 * asine_approximation(math.sqrt(a))
    result: f64 = EARTH_RADIUS * c

    return result
}