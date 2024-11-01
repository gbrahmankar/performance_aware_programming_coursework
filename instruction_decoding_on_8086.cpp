#include <fstream>
#include <iostream>
#include <sstream>
#include <stdint.h>
#include <string>
#include <unordered_map>
#include <vector>

using Mnemonic = std::bitset<6>;
enum EInstruction
{
    Mov,
    InstructionInvalid
};

struct InstructionMetadata
{
    InstructionMetadata()
    {}

    InstructionMetadata(EInstruction i, const std::string& is, uint8_t lab) :
        instruction(i),
        instructionString(is),
        lookAheadBytes(lab)
    {}

    EInstruction instruction = EInstruction::Mov;
    std::string instructionString;

    // you already read one byte for the mnemonic.
    // from the mnemonic, how many more bytes do we know, we need to look_ahead.
    uint8_t lookAheadBytes = 0;
};

std::unordered_map<uint64_t, InstructionMetadata> mnemonicToInstructionMetadata =
{
    { Mnemonic("100010").to_ulong(), InstructionMetadata(EInstruction::Mov, "mov", 1) }
};

InstructionMetadata& getInstructionMetadataFromMnemonic(const Mnemonic& m)
{
    if (mnemonicToInstructionMetadata.count(m.to_ulong()))
    {
        return mnemonicToInstructionMetadata[m.to_ulong()];
    }
    else
    {
        throw std::runtime_error("invalid mnemonic passed !");
    }
}

/* 
mov_layout : [100010[1b_d][1b_w]] [[2b_mod][3b_reg][3b_r/m]]
*/

// [1b_d]
enum EDBit : uint8_t
{
    RegSrcRMDest = 0,
    RegDestRMSrc,

    DBitInvalid
};

// [1b_w]
enum EWBit : uint8_t
{
    ByteOperation = 0,
    WordOperation,

    WBitInvalid
};

// [2b_mod]
enum EModField : uint8_t
{
    ModFieldZeroZero = 0,
    ModFieldZeroOne,
    ModFieldOneZero,
    RegisterMode,

    ModFieldInvalid
};

// [3b_reg], [3b_r/m]
enum ERegFieldsByteOperation : uint8_t
{
    al = 0,
    cl,
    dl,
    bl,
    ah,
    ch,
    dh,
    bh,

    RegFieldsByteOperationInvalid
};

enum ERegFieldsWordOperation : uint8_t
{
    ax = 0,
    cx ,
    dx,
    bx,
    sp,
    bp,
    si,
    di,

    RegFieldsWordOperationInvalid
};

int main(int argc, char* argv[])
{
    if (argc < 2) 
    {
        std::cerr << "usage : " << argv[0] << " <binary_file_to_disassemble>" << '\n';
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open()) 
    {
        std::cerr << "failed to open the file. file_name=" << argv[1] << '\n';
        return 1;
    }

    std::cout << "bits 16" << '\n';

    bool beginReadingNewInstruction = true;
    InstructionMetadata ongoingInstructionMetadata;

    char byte;
    while (file.read(&byte, sizeof(byte))) 
    {
        if (beginReadingNewInstruction)
        {
            std::bitset<8> byteBitset(byte);

            uint64_t byteInteger = (byteBitset >> 2).to_ulong();
            Mnemonic mnemonic = Mnemonic(byteInteger);
            ongoingInstructionMetadata = getInstructionMetadataFromMnemonic(mnemonic);
            std::cout << ongoingInstructionMetadata.instructionString << '\n';

            beginReadingNewInstruction = false;
        }
        else
        {
        }
    }

    return 0;    
}
