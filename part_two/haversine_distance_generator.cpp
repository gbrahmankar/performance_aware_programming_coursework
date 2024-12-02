#include "../common_utils.h"
#include "haversine_distance_calculator.h"
#include "haversine_distance_generator.h"

namespace PartTwo
{
    void generateHaversineInputFiles(int argc, char* argv[])
    {
        std::ofstream outJson("haversine_input.json");
        if (!outJson)
        {
            std::cerr << "failed to open the json file for writing !" << '\n';
            return;
        }

		std::ofstream haversineAnswers("haversine_answers.f64");
        if (!haversineAnswers)
        {
            std::cerr << "failed to open the haversine answers file for writing !" << '\n';
            return;
        }

        u64 seedValue = static_cast<u64>(std::stoul(std::string(argv[4])));
        std::mt19937 generator(seedValue);
        std::uniform_real_distribution<> dist(1.0, 10.0); // range
		f64 randomFloat = dist(generator);

        f64 xMax = 180; // x ranges from -xMax to +xMax
        f64 yMax = 90; // y ranges from -yMax to +yMax
        
        // center/extents for choosing the 64 random_sized sectors on earth's surface
        f64 xCenter = 0;
        f64 YCenter = 0;
        f64 XRadius = xMax;
        f64 YRadius = yMax;

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

        {
			

			{
				
				FILE *HaverAnswers = Open(PairCount, "haveranswer", "f64");
				if(FlexJSON && HaverAnswers)
				{
					fprintf(FlexJSON, "{\"pairs\":[\n");
					f64 Sum = 0;
					f64 SumCoef = 1.0 / (f64)PairCount;
					for(u64 PairIndex = 0; PairIndex < PairCount; ++PairIndex)
					{
						if(ClusterCountLeft-- == 0)
						{
							ClusterCountLeft = ClusterCountMax;
							XCenter = RandomInRange(&Series, -MaxAllowedX, MaxAllowedX);
							YCenter = RandomInRange(&Series, -MaxAllowedY, MaxAllowedY);
							XRadius = RandomInRange(&Series, 0, MaxAllowedX);
							YRadius = RandomInRange(&Series, 0, MaxAllowedY);
						}
						
						f64 X0 = RandomDegree(&Series, XCenter, XRadius, MaxAllowedX);
						f64 Y0 = RandomDegree(&Series, YCenter, YRadius, MaxAllowedY);
						f64 X1 = RandomDegree(&Series, XCenter, XRadius, MaxAllowedX);
						f64 Y1 = RandomDegree(&Series, YCenter, YRadius, MaxAllowedY);
						
						f64 EarthRadius = 6372.8;
						f64 HaversineDistance = ReferenceHaversine(X0, Y0, X1, Y1, EarthRadius);
						
						Sum += SumCoef*HaversineDistance;
						
						char const *JSONSep = (PairIndex == (PairCount - 1)) ? "\n" : ",\n";
						fprintf(FlexJSON, "    {\"x0\":%.16f, \"y0\":%.16f, \"x1\":%.16f, \"y1\":%.16f}%s", X0, Y0, X1, Y1, JSONSep);
						
						fwrite(&HaversineDistance, sizeof(HaversineDistance), 1, HaverAnswers);
					}
					fprintf(FlexJSON, "]}\n");
					fwrite(&Sum, sizeof(Sum), 1, HaverAnswers);
			
					fprintf(stdout, "Method: %s\n", MethodName);
					fprintf(stdout, "Random seed: %llu\n", SeedValue);
					fprintf(stdout, "Pair count: %llu\n", PairCount);
					fprintf(stdout, "Expected sum: %.16f\n", Sum);
				}
				
				if(FlexJSON) fclose(FlexJSON);
				if(HaverAnswers) fclose(HaverAnswers);
			}
			else
			{
				fprintf(stderr, "To avoid accidentally generating massive files, number of pairs must be less than %llu.\n", MaxPairCount);
			}
        }

        outJson.close();
    }
}
