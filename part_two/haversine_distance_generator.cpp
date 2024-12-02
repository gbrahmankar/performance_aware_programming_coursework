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

        u64 seedValue = static_cast<u64>(std::stoul(std::string(argv[4])));

        std::mt19937 gen(seedValue);
        std::uniform_real_distribution<> dist(1.0, 10.0); // range

        // random number
		f64 randomFloat = dist(gen);

        outJson.close();
    }
}
