package pap_common

import "core:math"

EARTH_RADIUS :: 6372.8

square :: proc(number: f64) -> (f64) {  
    return number * number
}

radians_from_degrees :: proc(degrees: f64) -> (f64) {
    return 0.01745329251994329577 * degrees;
}

@(private="file")
reference_haversine_using_mathlib :: proc(x0: f64, y0: f64, x1: f64, y1: f64) -> (f64) {
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
reference_haversine :: proc(x0: f64, y0: f64, x1: f64, y1: f64) -> (f64) {
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