#include "part_one/8086_decoder_simulator.h"

int main(int argc, char* argv[])
{
    using namespace PartOne;

    if (argc < 2) 
    {
        std::cerr << "usage : " << argv[0] << " <binary_file_to_disassemble>" << '\n';
        return 1;
    }

    std::ifstream file(argv[1], std::ios::binary);
    if (!file.is_open()) 
    {
        std::cerr << "failed to open the file. file_name=" << argv[1] << '\n';
        return 1;
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

        if (argc >= 3 && std::string(argv[2]) == "simulate")
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

    return 0;    
}
