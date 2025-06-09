#include "include/8086_decoder_simulator.h"
#include "include/part_three_main.h"
#include "include//haversine_distance_problem.h"

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "usage : " << argv[0] << "[part_one/part_two/part_three]" << '\n';
        return 1;
    }

    if (std::string(argv[1]) == "part_one")
    {
        if (argc < 3)
        {
            std::cerr << "usage : " << argv[0] << " part_one <binary_encoded_instruction_stream_file>" << '\n';
            return 1;
        }

        PartOne::executePartOne(argc, argv);
    }
    else if (std::string(argv[1]) == "part_two")
    {
        if (argc < 3)
        {
            std::cerr << "usage : " << argv[0] << " part_two [generator/parser/processor]" << '\n';
            return 1;
        }

        PartTwo::executePartTwo(argc, argv);
    }
    else if (std::string(argv[1]) == "part_three")
    {
        if (argc < 3)
        {
            std::cerr << "usage : " << argv[0] << " part_three [read_overhead_tests/incremental_page_touching/decompose_x64_pointer]" << '\n';
            return 1;
        }

        PartThree::executePartThree(argc, argv);
    } 

    return 0;
}
