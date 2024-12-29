#include "part_three_main.h"
#include "read_overhead_test_main.h"

namespace PartThree
{

void executePartThree(int argc, char* argv[])
{
    if (std::string(argv[2]) == "read_overhead")
    {
        if (argc < 4)
        {
            std::cerr << "usage : " << argv[0] << " part_two read_overhead [file_to_read]" << '\n';
            return;
        }

        PartThree::readOverheadTestMain(argc, argv);
    }
}

}
