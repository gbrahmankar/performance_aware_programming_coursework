#include "part_one/8086_decoder_simulator.h"
#include "part_three/part_three_main.h"
#include "part_two/haversine_distance_problem.h"
#include "test/test.h"

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "usage : " << argv[0] << "[part_one/part_two/test]" << '\n';
        return 1;
    }

    if (std::string(argv[1]) == "part_one")
    {
        if (argc < 3)
        {
            std::cerr << "usage : " << argv[0] << " part_one [binary_encoded_instruction_stream_file]" << '\n';
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
            std::cerr << "usage : " << argv[0] << " part_three [read_overhead/incremental_page_touching]" << '\n';
            return 1;
        }

        PartThree::executePartThree(argc, argv);
    }
    if (std::string(argv[1]) == "test")
    {
        Test::executeTest(argc, argv);
    }

    return 0;
}
