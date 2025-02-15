#include "backwards_page_touching.h"
#include "decompose_x64_pointer.h"
#include "front_end_test_main.h"
#include "incremental_page_touching.h"
#include "part_three_main.h"
#include "read_overhead_test_main.h"

namespace PartThree
{

void executePartThree(int argc, char* argv[])
{
    if (std::string(argv[2]) == "read_overhead_tests")
    {
        if (argc < 4)
        {
            std::cerr << "usage : " << argv[0] << " part_three read_overhead [file_to_read]" << '\n';
            return;
        }

        PartThree::readOverheadTestMain(argc, argv);
    }
    else if (std::string(argv[2]) == "incremental_page_touching")
    {
        if (argc < 4)
        {
            std::cerr << "usage : " << argv[0] << " part_three incremental_page_touching [page_count]" << '\n';
            return;
        }

        PartThree::executeIncrementalPageTouchTest(argc, argv);
    }
    else if (std::string(argv[2]) == "decompose_x64_pointer")
    {
        if (argc < 4)
        {
            std::cerr << "usage : " << argv[0] << " part_three decompose_x64_pointer [place_holder]" << '\n';
            return;
        }
    }
    else if (std::string(argv[2]) == "backwards_page_touching")
    {
        if (argc < 3)
        {
            std::cerr << "usage : " << argv[0] << " part_three backwards_page_touching" << '\n';
            return;
        }

        PartThree::executeBackwardsPageTouchTest(argc, argv);
    }
    else if (std::string(argv[2]) == "front_end_tests")
    {
        if (argc < 4)
        {
            std::cerr << "usage : " << argv[0] << " part_three front_end_tests [file_name]" << '\n';
            return;
        }

        PartThree::frontEndTestMain(argc, argv);
    }
}

}
