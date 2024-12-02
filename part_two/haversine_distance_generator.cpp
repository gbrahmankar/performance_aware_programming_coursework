#include "../common_utils.h"
#include "haversine_distance_calculator.h"
#include "haversine_distance_generator.h"

namespace PartTwo
{
    f64 randomInRange(const std::mt19937& generator, f64 minVal, f64 maxVal)
    {
        std::uniform_real_distribution<> dist(minVal, maxVal);
		return dist(generator);
    }

    f64 randomDegree(const std::mt19937& generator, f64 center, f64 radius, f64 maxAllowed)
    {
        f64 minVal = center - radius;
        if(minVal < -maxAllowed)
        {
            minVal = -maxAllowed;
        }

        f64 maxVal = center + radius;
        if(maxVal > maxAllowed)
        {
            maxVal = maxAllowed;
        }

        return randomInRange(generator, minVal, maxVal);
    }

    void generateHaversineInputFiles(int argc, char* argv[])
    {
        std::ofstream outJson("haversine_input.json");
        if (!outJson)
        {
            std::cerr << "failed to open the haversine_input.json file for writing !" << '\n';
            return;
        }

		std::ofstream haversineAnswers("haversine_answers.f64");
        if (!haversineAnswers)
        {
            std::cerr << "failed to open the haversine_answers.f64 file for writing !" << '\n';
            return;
        }

        u64 seedValue = static_cast<u64>(std::stoul(std::string(argv[4])));
        std::mt19937 generator(seedValue);

        f64 xMax = 180; // x ranges from -xMax to +xMax
        f64 yMax = 90; // y ranges from -yMax to +yMax
        
        // center/extents for choosing the 64 random_sized sectors on earth's surface
        f64 xCenter = 0;
        f64 yCenter = 0;
        f64 xRadius = xMax;
        f64 yRadius = yMax;

		// cluster values
		u64 clusterCountLeft = maxU64;
		u64 maxPairCount = (1ULL << 34);
		u64 pairCount = static_cast<u64>(std::stoul(std::string(argv[5])));
		if (pairCount > maxPairCount)
		{
			std::cerr << "pairCount > maxPairCount\n";
			return;
		}
		u64 clusterCountMax = 1 + (pairCount / 64);

		std::string methodName = "uniform";
		if(std::string(argv[3]) == "cluster")
		{
			methodName = "cluster";
			clusterCountLeft = 0;
		}
		else if(std::string(argv[3]) == "uniform")
		{
			methodName = "uniform";
		}

        f64 sum = 0;
        f64 sumCoef = 1.0 / (f64)pairCount;

        outJson << "{\"pairs\":[\n";

        for(u64 pairIndex = 0; pairIndex < pairCount; ++pairIndex)
        {
            if(clusterCountLeft-- == 0)
            {
                clusterCountLeft = clusterCountMax;
                xCenter = randomInRange(generator, -xMax, xMax);
                yCenter = randomInRange(generator, -yMax, yMax);
                xRadius = randomInRange(generator, 0, xMax);
                yRadius = randomInRange(generator, 0, yMax);
            }
            
            f64 x0 = randomDegree(generator, xCenter, xRadius, xMax);
            f64 y0 = randomDegree(generator, yCenter, yRadius, yMax);
            f64 x1 = randomDegree(generator, xCenter, xRadius, xMax);
            f64 y1 = randomDegree(generator, yCenter, yRadius, yMax);
            
            f64 earthRadius = 6372.8;
            f64 haversineDistance = ReferenceHaversine(x0, y0, x1, y1, earthRadius);
            
            sum += sumCoef * haversineDistance;
            
            std::string jsonSep = (pairIndex == (pairCount - 1)) ? "\n" : ",\n";
            fprintf(FlexJSON, "    {\"x0\":%.16f, \"y0\":%.16f, \"x1\":%.16f, \"y1\":%.16f}%s", X0, Y0, X1, Y1, JSONSep);
            
            fwrite(&HaversineDistance, sizeof(HaversineDistance), 1, HaverAnswers);
        }
        fprintf(FlexJSON, "]}\n");
        fwrite(&Sum, sizeof(Sum), 1, HaverAnswers);

        fprintf(stdout, "Method: %s\n", MethodName);
        fprintf(stdout, "Random seed: %llu\n", SeedValue);
        fprintf(stdout, "Pair count: %llu\n", PairCount);
        fprintf(stdout, "Expected sum: %.16f\n", Sum);
        
        outJson.close();
        haversineAnswers.close();
    }
}
