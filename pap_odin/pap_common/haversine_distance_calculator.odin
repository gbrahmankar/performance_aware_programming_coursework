package pap_common

import "core:math"

EARTH_RADIUS :: 6372.8

square :: proc(number: f32) -> (f32) {	
    return number * number
}

radians_from_degrees :: proc(degrees: f32) -> (f32) {
    return 0.01745329251994329577 * degrees;
}

reference_haversine :: proc(x0: f32, y0: f32, x1: f32, y1: f32) -> (f32) {
    lat1: f32 = y0
    lat2: f32 = y1
    lon1: f32 = x0
    lon2: f32 = x1

    degrees_lat: f32 = radians_from_degrees(lat2 - lat1)
    degrees_lon: f32 = radians_from_degrees(lon2 - lon1)
    lat1 = radians_from_degrees(lat1)
    lat2 = radians_from_degrees(lat2)

    a: f32 = square(math.sin(degrees_lat/2.0)) + math.cos(lat1) * math.cos(lat2) * square(math.sin(degrees_lon/2))
    c: f32 = 2.0 * math.asin_f32(math.sqrt(a))
    result: f32 = EARTH_RADIUS * c

    return result
}