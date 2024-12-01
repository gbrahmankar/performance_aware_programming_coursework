#include "part_one/8086_decoder_simulator.h"
#include "part_two/haversine_distance_problem.h"

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "usage : " << argv[0] << "<what_part_to_execute. eg : part_one/part_two>" << '\n';
        return 1;
    }

    if (std::string(argv[1]) == "part_one")
    {
        if (argc < 3)
        {
            std::cerr << "usage : " << argv[0] << " part_one <binary_encoded_instruction_stream file>" << '\n';
            return 1;
        }
        PartOne::executePartOne(argc, argv);
    }
    else if (std::string(argv[1]) == "part_two")
    {
        if (argc < 4)
        {
            std::cerr << "usage : " << argv[0] << "[uniform/cluster] [random_seed] [number_of_coordinate_points_to_generate]" << '\n';
            return 1;
        }
        PartTwo::executePartTwo(argc, argv);
    }

    return 0;
}
