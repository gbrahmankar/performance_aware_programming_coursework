#include <fstream>
#include <iostream>
#include <sstream>
#include <stdint.h>
#include <string>
#include <unordered_map>
#include <vector>

using NibbleBitset = std::bitset<4>;
using ByteBitset = std::bitset<8>;
using WordBitset = std::bitset<16>;

using StandardMnemonicBitset = std::bitset<6>;

enum EInstruction
{
    /* 
    mov_layout : [100010][1b_d][1b_w]] [[2b_mod][3b_reg][3b_r/m]]
    */
    Mov,
    InstructionInvalid
};

// [1b_d]
enum EDBit : uint8_t
{
    RegFieldSrcRMFieldDest = 0,
    RegFieldDestRMFieldSrc,

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
    MemModeNoDisp = 0,
    MemModeEightBitDisp,
    MemModeSixteenBitDisp,
    RegisterMode,

    ModFieldInvalid
};

// [3b_reg], [3b_r/m]
enum ERegisterEncodingByteOperation : uint8_t
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

enum ERegisterEncodingWordOperation : uint8_t
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
    { EModField::MemModeNoDisp, "memory_mode_no_displacement_follows" },
    { EModField::MemModeEightBitDisp, "memory_mode_8_bit_displacement_follows" },
    { EModField::MemModeSixteenBitDisp, "memory_mode_16_bit_displacement_follows" },
    { EModField::RegisterMode, "register_mode" }
};

std::unordered_map<EModField, std::string> modFieldEnumToStringDispacement =
{
    { EModField::MemModeNoDisp, "" },
    { EModField::MemModeEightBitDisp, "8" },
    { EModField::MemModeSixteenBitDisp, "16" }
};

std::unordered_map<ERegisterEncodingByteOperation, std::string> byteRegisterEncodingToString =
{
    { ERegisterEncodingByteOperation::al, "al" },
    { ERegisterEncodingByteOperation::cl, "cl" },
    { ERegisterEncodingByteOperation::dl, "dl" },
    { ERegisterEncodingByteOperation::bl, "bl" },
    { ERegisterEncodingByteOperation::ah, "ah" },
    { ERegisterEncodingByteOperation::ch, "ch" },
    { ERegisterEncodingByteOperation::dh, "dh" },
    { ERegisterEncodingByteOperation::bh, "bh" }
};

std::unordered_map<ERegisterEncodingWordOperation, std::string> wordRegisterEncodingToString =
{
    { ERegisterEncodingWordOperation::ax, "ax" },
    { ERegisterEncodingWordOperation::cx, "cx" },
    { ERegisterEncodingWordOperation::dx, "dx" },
    { ERegisterEncodingWordOperation::bx, "bx" },
    { ERegisterEncodingWordOperation::sp, "sp" },
    { ERegisterEncodingWordOperation::bp, "bp" },
    { ERegisterEncodingWordOperation::si, "si" },
    { ERegisterEncodingWordOperation::di, "di" }
};

std::string& getModFieldEnumToString(EModField e)
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

std::string& getDisplacementFromModFieldEnum(EModField e)
{
    if (modFieldEnumToStringDispacement.count(e))
    {
        return modFieldEnumToStringDispacement[e];
    }
    else
    {
        throw std::runtime_error("invalid mod_field enum passed !");
    }
}

std::string& getRegisterNameFromByteOperationRegisterEncoding(ERegisterEncodingByteOperation e)
{
    if (byteRegisterEncodingToString.count(e))
    {
        return byteRegisterEncodingToString[e];
    }
    else
    {
        throw std::runtime_error("invalid byte_operation register enum passed !");
    }
}

std::string& getRegisterNameFromWordOperationRegisterEncoding(ERegisterEncodingWordOperation e)
{
    if (wordRegisterEncodingToString.count(e))
    {
        return wordRegisterEncodingToString[e];
    }
    else
    {
        throw std::runtime_error("invalid word_operation register enum passed !");
    }
}

std::string getEffectiveAddressBaseEquationString(uint8_t registerOrMemoryFieldValue, EModField mod)
{
    switch(registerOrMemoryFieldValue)
    {
        case 0 :
            return "bx + si";
        case 1 :
            return "bx + di";
        case 2 :
            return "bp + si";
        case 3 :
            return "bp + di";
        case 4 :
            return "si";
        case 5 :
            return "di";
        case 6 :
            if (mod == EModField::MemModeNoDisp)
                return "";
            else
                return "bp";
        case 7 :
            return "bx";
        default :
            throw std::runtime_error("invalid mod_field enum passed !");
    }
}

struct InstructionMetadata
{
    InstructionMetadata()
    {}

    InstructionMetadata(EInstruction i, const std::string& is) :
        instruction(i),
        instructionString(is)
    {}

    EInstruction instruction = EInstruction::Mov;
    std::string instructionString;

    EDBit dBit = EDBit::DBitInvalid;
    EWBit wBit = EWBit::WBitInvalid;
    EModField modField = EModField::ModFieldInvalid;
};

std::unordered_map<uint64_t, InstructionMetadata> mnemonicToInstructionMetadata =
{
    { StandardMnemonicBitset("100010").to_ulong(), InstructionMetadata(EInstruction::Mov, "mov") }
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

void decodeSecondInstructionByte(const ByteBitset& byteBitset, std::ifstream& file, InstructionMetadata& ongoingInstructionMetadata)
{
    // [2b_mod] : unused right now !
    EModField mod = static_cast<EModField>((byteBitset >> 6).to_ulong() & 0b00000011);
    switch(mod)
    {
        case (EModField::MemModeNoDisp) :
        {
            break;
        }
        case (EModField::MemModeEightBitDisp) :
        {
            break;
        }
        case (EModField::MemModeSixteenBitDisp) :
        {
            break;
        }
        case (EModField::RegisterMode) :
        {
            if (ongoingInstructionMetadata.wBit == EWBit::ByteOperation)
            {
                // [3b_reg]
                ERegisterEncodingByteOperation reg = static_cast<ERegisterEncodingByteOperation>((byteBitset >> 3).to_ulong() & 0b00000111);
                // [3b_r/m]
                ERegisterEncodingByteOperation rm = static_cast<ERegisterEncodingByteOperation>(byteBitset.to_ulong() & 0b00000111);

                if (ongoingInstructionMetadata.dBit == EDBit::RegFieldDestRMFieldSrc)
                {
                    std::cout << ongoingInstructionMetadata.instructionString << " " 
                        << getRegisterNameFromByteOperationRegisterEncoding(reg) << ", " 
                        << getRegisterNameFromByteOperationRegisterEncoding(rm) << '\n';
                }
                else
                {
                    std::cout << ongoingInstructionMetadata.instructionString << " " 
                        << getRegisterNameFromByteOperationRegisterEncoding(rm) << ", " 
                        << getRegisterNameFromByteOperationRegisterEncoding(reg) << '\n';
                }
            }
            else
            {
                // [3b_reg]
                ERegisterEncodingWordOperation reg = static_cast<ERegisterEncodingWordOperation>((byteBitset >> 3).to_ulong() & 0b00000111);
                // [3b_r/m]
                ERegisterEncodingWordOperation rm = static_cast<ERegisterEncodingWordOperation>(byteBitset.to_ulong() & 0b00000111);

                if (ongoingInstructionMetadata.dBit == EDBit::RegFieldDestRMFieldSrc)
                {
                    std::cout << ongoingInstructionMetadata.instructionString << " " 
                        << getRegisterNameFromWordOperationRegisterEncoding(reg) << ", " 
                        << getRegisterNameFromWordOperationRegisterEncoding(rm) << '\n';
                }
                else
                {
                    std::cout << ongoingInstructionMetadata.instructionString << " " 
                        << getRegisterNameFromWordOperationRegisterEncoding(rm) << ", " 
                        << getRegisterNameFromWordOperationRegisterEncoding(reg) << '\n';
                }
            }

            break;
        }
        default :
        {
            throw std::runtime_error("invalid mod !");
        }
    }
}

void decodeFirstInstructionByte(const ByteBitset& byteBitset, std::ifstream& file, InstructionMetadata& ongoingInstructionMetadata)
{
    uint64_t mnemonicInteger = (byteBitset >> 2).to_ulong();
    ongoingInstructionMetadata = getInstructionMetadataFromMnemonic(mnemonicInteger);
    if ((byteBitset.to_ulong() & 0b00000010) > 0)
    {
        ongoingInstructionMetadata.dBit = EDBit::RegFieldDestRMFieldSrc;
    }
    else
    {
        ongoingInstructionMetadata.dBit = EDBit::RegFieldSrcRMFieldDest;
    }

    if ((byteBitset.to_ulong() & 0b00000001) > 0)
    {
        ongoingInstructionMetadata.wBit = EWBit::WordOperation;
    }
    else
    {
        ongoingInstructionMetadata.wBit = EWBit::ByteOperation;
    }

    // needs second byte
    char byte;
    file.read(&byte, sizeof(byte));
    ByteBitset secondByteBitset(byte);
    decodeSecondInstructionByte(secondByteBitset, file, ongoingInstructionMetadata);
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
        char byte;

        file.read(&byte, sizeof(byte));
        ByteBitset firstByteBitset(byte);
        if (file.gcount() == 0)
        {
            break;
        }
        decodeFirstInstructionByte(firstByteBitset, file, ongoingInstructionMetadata); 
    }

    return 0;    
}
