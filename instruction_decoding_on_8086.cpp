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
    /* 
    mov_layout : [100010[1b_d][1b_w]] [[2b_mod][3b_reg][3b_r/m]]
    */
    Mov,
    InstructionInvalid
};

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

std::unordered_map<EModField, std::string> modFieldEnumToString =
{
    { EModField::ModFieldZeroZero, "mod00" },
    { EModField::ModFieldZeroOne, "mod01" },
    { EModField::ModFieldOneZero, "mod10" },
    { EModField::RegisterMode, "register_mode" }
};

std::unordered_map<ERegFieldsByteOperation, std::string> byteRegEnumToString =
{
    { ERegFieldsByteOperation::al, "al" },
    { ERegFieldsByteOperation::cl, "cl" },
    { ERegFieldsByteOperation::dl, "dl" },
    { ERegFieldsByteOperation::bl, "bl" },
    { ERegFieldsByteOperation::ah, "ah" },
    { ERegFieldsByteOperation::ch, "ch" },
    { ERegFieldsByteOperation::dh, "dh" },
    { ERegFieldsByteOperation::bh, "bh" }
};

std::unordered_map<ERegFieldsWordOperation, std::string> wordRegEnumToString =
{
    { ERegFieldsWordOperation::ax, "ax" },
    { ERegFieldsWordOperation::cx, "cx" },
    { ERegFieldsWordOperation::dx, "dx" },
    { ERegFieldsWordOperation::bx, "bx" },
    { ERegFieldsWordOperation::sp, "sp" },
    { ERegFieldsWordOperation::bp, "bp" },
    { ERegFieldsWordOperation::si, "si" },
    { ERegFieldsWordOperation::di, "di" }
};

std::string& getStringFromModFieldEnum(EModField e)
{
    if (modFieldEnumToString.count(e))
    {
        return modFieldEnumToString[e];
    }
    else
    {
        throw std::runtime_error("invalid mod_field enum passed !");
    }
}

std::string& getStringFromByteRegEnum(ERegFieldsByteOperation e)
{
    if (byteRegEnumToString.count(e))
    {
        return byteRegEnumToString[e];
    }
    else
    {
        throw std::runtime_error("invalid byte_operation register enum passed !");
    }
}

std::string& getStringFromWordRegEnum(ERegFieldsWordOperation e)
{
    if (wordRegEnumToString.count(e))
    {
        return wordRegEnumToString[e];
    }
    else
    {
        throw std::runtime_error("invalid word_operation register enum passed !");
    }
}

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

    EDBit dBit = EDBit::DBitInvalid;
    EWBit wBit = EWBit::WBitInvalid;
    EModField modField = EModField::ModFieldInvalid;

    // you already read one byte for the mnemonic.
    // from the mnemonic, how many more bytes do we know, we need to look_ahead.
    uint8_t lookAheadBytes = 0;
};

std::unordered_map<uint64_t, InstructionMetadata> mnemonicToInstructionMetadata =
{
    { Mnemonic("100010").to_ulong(), InstructionMetadata(EInstruction::Mov, "mov", 1) }
};

InstructionMetadata& getInstructionMetadataFromMnemonic(uint64_t m)
{
    if (mnemonicToInstructionMetadata.count(m))
    {
        return mnemonicToInstructionMetadata[m];
    }
    else
    {
        throw std::runtime_error("invalid mnemonic passed !");
    }
}

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

    while (true) 
    {
        if (beginReadingNewInstruction)
        {
            char byte;
            file.read(&byte, sizeof(byte));
            std::bitset<8> byteBitset(byte);
            if (file.gcount() == 0)
            {
                break;
            }
            
            uint64_t byteInteger = (byteBitset >> 2).to_ulong();
            ongoingInstructionMetadata = getInstructionMetadataFromMnemonic(byteInteger);
            if ((byteBitset.to_ulong() & 0b00000010) > 0)
            {
                ongoingInstructionMetadata.dBit = EDBit::RegDestRMSrc;
            }
            else
            {
                ongoingInstructionMetadata.dBit = EDBit::RegSrcRMDest;
            }

            if ((byteBitset.to_ulong() & 0b00000001) > 0)
            {
                ongoingInstructionMetadata.wBit = EWBit::WordOperation;
            }
            else
            {
                ongoingInstructionMetadata.wBit = EWBit::ByteOperation;
            }

            beginReadingNewInstruction = false;
        }
        else
        {
            for (uint8_t bytesToRead = ongoingInstructionMetadata.lookAheadBytes; bytesToRead > 0; --bytesToRead)
            {
                char byte;
                file.read(&byte, sizeof(byte));
                std::bitset<8> byteBitset(byte);
                
                // [2b_mod] : unused right now !
                EModField mod = static_cast<EModField>((byteBitset >> 6).to_ulong() & 0b00000011);
                if (ongoingInstructionMetadata.wBit == EWBit::ByteOperation)
                {
                    // [3b_reg]
                    ERegFieldsByteOperation reg = static_cast<ERegFieldsByteOperation>((byteBitset >> 3).to_ulong() & 0b00000111);
                    // [3b_r/m]
                    ERegFieldsByteOperation rm = static_cast<ERegFieldsByteOperation>(byteBitset.to_ulong() & 0b00000111);

                    if (ongoingInstructionMetadata.dBit == EDBit::RegDestRMSrc)
                    {
                        std::cout << ongoingInstructionMetadata.instructionString << " " 
                            << getStringFromByteRegEnum(reg) << ", " 
                            << getStringFromByteRegEnum(rm) << '\n';
                    }
                    else
                    {
                        std::cout << ongoingInstructionMetadata.instructionString << " " 
                            << getStringFromByteRegEnum(rm) << ", " 
                            << getStringFromByteRegEnum(reg) << '\n';
                    }
                }
                else
                {
                    // [3b_reg]
                    ERegFieldsWordOperation reg = static_cast<ERegFieldsWordOperation>((byteBitset >> 3).to_ulong() & 0b00000111);
                    // [3b_r/m]
                    ERegFieldsWordOperation rm = static_cast<ERegFieldsWordOperation>(byteBitset.to_ulong() & 0b00000111);

                    if (ongoingInstructionMetadata.dBit == EDBit::RegDestRMSrc)
                    {
                        std::cout << ongoingInstructionMetadata.instructionString << " " 
                            << getStringFromWordRegEnum(reg) << ", " 
                            << getStringFromWordRegEnum(rm) << '\n';
                    }
                    else
                    {
                        std::cout << ongoingInstructionMetadata.instructionString << " " 
                            << getStringFromWordRegEnum(rm) << ", " 
                            << getStringFromWordRegEnum(reg) << '\n';
                    }
                }
            }

            beginReadingNewInstruction = true;
        }
    }

    return 0;    
}
