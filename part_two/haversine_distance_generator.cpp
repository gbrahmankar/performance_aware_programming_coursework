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

        outJson.close();
    }
}
