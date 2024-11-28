#include "part_one/8086_decoder_simulator.h"

void executePartOne(int argc, char* argv[])
{
    using namespace PartOne;

    std::ifstream file(argv[2], std::ios::binary);
    if (!file.is_open()) 
    {
        std::cerr << "failed to open the file. file_name=" << argv[2] << '\n';
        return;
    }

    uint16_t totalProgramCost = 0;
    InstructionMetadata ongoingInstructionMetadata;
    std::cout << "bits 16" << '\n';
    while (true) 
    {
        ByteBitset firstByteBitset;
        if (!getBitsetFromInstructionByteStream(file, ongoingInstructionMetadata, firstByteBitset, true))
        {
            break;
        }

        decodeFirstInstructionByte(firstByteBitset, file, ongoingInstructionMetadata); 

        if (argc >= 3 && std::string(argv[3]) == "simulate")
        {
            simulateInstruction(ongoingInstructionMetadata);
            totalProgramCost += ongoingInstructionMetadata.totalInstructionCost;

            std::cout << getDisassStream(ongoingInstructionMetadata, false).str() 
                << registers.getAllRegisterFileStream(true).str() 
                << registers.getFlagsStream().str() << ongoingInstructionMetadata.getInstructionCostStream().str() 
                << "running_cost=" << totalProgramCost << '\n';
        }
        else
        {
            std::cout << getDisassStream(ongoingInstructionMetadata, true).str();
        }
    }

    std::cout << "Final registers:" << '\n';
    std::cout << registers.getAllRegisterFileStream(false).str() << '\n';

    std::cout << "TotalProgramCost=" << totalProgramCost << '\n';
}

void executePartTwo(int argc, char* argv[])
{
}

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
        executePartOne(argc, argv);
    }
    else if (std::string(argv[1]) == "part_two")
    {
        executePartTwo(argc, argv);
    }

    return 0;
}
