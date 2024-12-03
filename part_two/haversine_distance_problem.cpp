#include "haversine_distance_generator.h"
#include "haversine_distance_parser.h"
#include "haversine_distance_problem.h"

namespace PartTwo
{

void executePartTwo(int argc, char* argv[])
{
    if (std::string(argv[2]) == "generator")
    {
        PartTwo::generateHaversineInputFiles(argc, argv);
    } 
    if (std::string(argv[2]) == "parser")
    {
        PartTwo::parseHaversineInput(argc, argv);
    }
}

}
