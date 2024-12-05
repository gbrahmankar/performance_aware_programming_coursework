#include "haversine_distance_generator.h"
#include "haversine_distance_parser.h"
#include "haversine_distance_problem.h"

namespace PartTwo
{

void executePartTwo(int argc, char* argv[])
{
    if (std::string(argv[2]) == "generator")
    {
        if (argc < 6)
        {
            std::cerr << "usage : " << argv[0] << " part_two [generator] [uniform/cluster] [random_seed] [number_of_coordinate_points_to_generate]" << '\n';
            return;
        }

        PartTwo::generateHaversineInputFiles(argc, argv);
    } 
    if (std::string(argv[2]) == "parser")
    {
        if (argc < 3)
        {
            std::cerr << "usage : " << argv[0] << " part_two [parser]" << '\n';
            return;
        }

        PartTwo::parseHaversineInput(argc, argv);
    }
}

}
