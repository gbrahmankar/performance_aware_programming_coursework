#include "include/common_utils.h"
#include "include/haversine_distance_calculator.h"
#include "include/haversine_distance_generator.h"

namespace PartTwo
{
    f64 randomInRange(std::mt19937& generator, f64 minVal, f64 maxVal)
    {
        std::uniform_real_distribution<> dist(minVal, maxVal);
		return dist(generator);
    }

    f64 randomDegree(std::mt19937& generator, f64 center, f64 radius, f64 maxAllowed)
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
        std::ofstream outJson("..\\part_two_generated_files\\haversine_input.json");
        if (!outJson)
        {
            std::cerr << "failed to open the haversine_input.json file for writing !" << '\n';
            return;
        }

		std::ofstream haversineAnswers("..\\part_two_generated_files\\haversine_answers.f64");
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
            f64 haversineDistance = ReferenceHaversine(x0, y0, x1, y1, 6372.8);
            
            std::string jsonSep = (pairIndex == (pairCount - 1)) ? "\n" : ",\n";
            outJson << "    {\"x0\":" << STREAM_16BIT_PRECISION_FP(x0) <<
                ", \"y0\":" << STREAM_16BIT_PRECISION_FP(y0) <<
                ", \"x1\":" << STREAM_16BIT_PRECISION_FP(x1) <<
                ", \"y1\":" << STREAM_16BIT_PRECISION_FP(y1) << "}" <<
                jsonSep;
            
            haversineAnswers << STREAM_16BIT_PRECISION_FP(haversineDistance);

            sum += sumCoef * haversineDistance;
        }
        outJson << "]}\n";
        haversineAnswers << STREAM_16BIT_PRECISION_FP(sum);

        std::cout << "method=" << methodName << '\n';
        std::cout << "random_seed=" << seedValue << '\n';
        std::cout << "pair_count=" << pairCount << '\n';
        std::cout << "expected_sum=" << STREAM_16BIT_PRECISION_FP(sum) << '\n';
        
        outJson.close();
        haversineAnswers.close();
    }
}
