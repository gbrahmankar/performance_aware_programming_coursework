#include "haversine_distance_problem.h"

namespace PartTwo
{

// ----- casey's code starts -----------
static float Square(float A)
{
    float Result = (A*A);
    return Result;
}

static float RadiansFromDegrees(float Degrees)
{
    float Result = 0.01745329251994329577 * Degrees;
    return Result;
}

// NOTE(casey): EarthRadius is generally expected to be 6372.8
static float ReferenceHaversine(float X0, float Y0, float X1, float Y1, float EarthRadius)
{
    /* NOTE(casey): This is not meant to be a "good" way to calculate the Haversine distance.
       Instead, it attempts to follow, as closely as possible, the formula used in the real-world
       question on which these homework exercises are loosely based.
    */
    
    float lat1 = Y0;
    float lat2 = Y1;
    float lon1 = X0;
    float lon2 = X1;
    
    float dLat = RadiansFromDegrees(lat2 - lat1);
    float dLon = RadiansFromDegrees(lon2 - lon1);
    lat1 = RadiansFromDegrees(lat1);
    lat2 = RadiansFromDegrees(lat2);
    
    float a = Square(sin(dLat/2.0)) + cos(lat1)*cos(lat2)*Square(sin(dLon/2));
    float c = 2.0*asin(sqrt(a));
    
    float Result = EarthRadius * c;
    
    return Result;
}
// ----- casey's code ends -----------

void executePartTwo(int argc, char* argv[])
{
    uint64_t ClusterCountLeft = maxU64;
    f64 maxAllowedX = 180;
    f64 maxAllowedY = 90;

    f64 xCenter = 0;
    f64 yCenter = 0;
    f64 xRadius = maxAllowedX;
    f64 yRadius = maxAllowedY;
}

}
