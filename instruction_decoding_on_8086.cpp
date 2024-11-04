#include <bitset>
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

using MnemonicBitset = std::bitset<7>;

enum EInstruction
{
    /* 
    mov_regmem_to_from_reg_layout : [100010][1b_d][1b_w]] [[2b_mod][3b_reg][3b_r/m]] [8b_disp_low] [8b_disp_high]
    */
    MovRegMemToFromReg,
    /* 
    mov_immediate_register_layout : [[1011][1b_w][3b_reg]] [8b_data_low] [8b_data_high]
    */
    MovImmToReg,
    /* 
    mov_memory_to_acc_layout : [[1010000][1b_w]] [8b_addr_low] [8b_addr_high]
    */
    MovMemToAcc,
    /* 
    mov_acc_to_memory_layout : [[1010001][1b_w]] [8b_addr_low] [8b_addr_high]
    */
    MovAccToMem,
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
enum ERegThreeBitEncodingByteOp : uint8_t
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

enum ERegThreeBitEncodingWordOp : uint8_t
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

std::unordered_map<ERegThreeBitEncodingByteOp, std::string> byteRegisterEncodingToString =
{
    { ERegThreeBitEncodingByteOp::al, "al" },
    { ERegThreeBitEncodingByteOp::cl, "cl" },
    { ERegThreeBitEncodingByteOp::dl, "dl" },
    { ERegThreeBitEncodingByteOp::bl, "bl" },
    { ERegThreeBitEncodingByteOp::ah, "ah" },
    { ERegThreeBitEncodingByteOp::ch, "ch" },
    { ERegThreeBitEncodingByteOp::dh, "dh" },
    { ERegThreeBitEncodingByteOp::bh, "bh" }
};

std::unordered_map<ERegThreeBitEncodingWordOp, std::string> wordRegisterEncodingToString =
{
    { ERegThreeBitEncodingWordOp::ax, "ax" },
    { ERegThreeBitEncodingWordOp::cx, "cx" },
    { ERegThreeBitEncodingWordOp::dx, "dx" },
    { ERegThreeBitEncodingWordOp::bx, "bx" },
    { ERegThreeBitEncodingWordOp::sp, "sp" },
    { ERegThreeBitEncodingWordOp::bp, "bp" },
    { ERegThreeBitEncodingWordOp::si, "si" },
    { ERegThreeBitEncodingWordOp::di, "di" }
};

std::string& getRegNameFromThreeBitEncodingByteOp(ERegThreeBitEncodingByteOp e)
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

std::string& getRegNameFromThreeBitEncodingWordOp(ERegThreeBitEncodingWordOp e)
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

    EInstruction instruction = EInstruction::InstructionInvalid;
    std::string instructionString;

    EDBit dBit = EDBit::DBitInvalid;
    EWBit wBit = EWBit::WBitInvalid;
    EModField modField = EModField::ModFieldInvalid;
};

std::unordered_map<uint64_t, InstructionMetadata> mnemonicToInstructionMetadata =
{
    { MnemonicBitset("100010").to_ulong(), InstructionMetadata(EInstruction::MovRegMemToFromReg, "mov") },
    { MnemonicBitset("1011").to_ulong(), InstructionMetadata(EInstruction::MovImmToReg, "mov") },
    { MnemonicBitset("1010000").to_ulong(), InstructionMetadata(EInstruction::MovMemToAcc, "mov") },
    { MnemonicBitset("1010001").to_ulong(), InstructionMetadata(EInstruction::MovAccToMem, "mov") }
};

InstructionMetadata& getInstructionMetadataFromMnemonic(ByteBitset bitSet)
{
    while (bitSet.to_ulong() > 0)
    {
        if (mnemonicToInstructionMetadata.count(bitSet.to_ulong()))
        {
            return mnemonicToInstructionMetadata[bitSet.to_ulong()];
        }

        bitSet >>= 1;
    }

    throw std::runtime_error("invalid mnemonic passed !");
}

void decodeSecondInstructionByte(const ByteBitset& byteBitset, std::ifstream& file, InstructionMetadata& ongoingInstructionMetadata)
{
    // [2b_mod] : unused right now !
    EModField mod = static_cast<EModField>((byteBitset >> 6).to_ulong() & 0b00000011);

    // [3b_r/m]
    uint64_t rmField = byteBitset.to_ulong() & 0b00000111;
    std::string baseEquation = getEffectiveAddressBaseEquationString(rmField , mod);

    switch(mod)
    {
        case (EModField::MemModeNoDisp) :
        {
            if (baseEquation.empty())
            {
                // needs third byte
                char byte;
                file.read(&byte, sizeof(byte));
                ByteBitset thirdByteBitset(byte);

                // needs fourth byte
                file.read(&byte, sizeof(byte));
                ByteBitset fourthByteBitset(byte);

                WordBitset netDisplacementBitset((fourthByteBitset.to_ulong() << 8) | thirdByteBitset.to_ulong()); 
                std::string directAddress = "[" + std::to_string(netDisplacementBitset.to_ulong()) + "]";

                if (ongoingInstructionMetadata.wBit == EWBit::ByteOperation)
                {
                    // [3b_reg]
                    ERegThreeBitEncodingByteOp reg = static_cast<ERegThreeBitEncodingByteOp>((byteBitset >> 3).to_ulong() & 0b00000111);

                    if (ongoingInstructionMetadata.dBit == EDBit::RegFieldDestRMFieldSrc)
                    {
                        std::cout << ongoingInstructionMetadata.instructionString << " " 
                            << getRegNameFromThreeBitEncodingByteOp(reg) << ", " 
                            << directAddress << '\n';
                    }
                    else
                    {
                        std::cout << ongoingInstructionMetadata.instructionString << " " 
                            << directAddress << ", " 
                            << getRegNameFromThreeBitEncodingByteOp(reg) << '\n';
                    }
                }
                else
                {
                    // [3b_reg]
                    ERegThreeBitEncodingWordOp reg = static_cast<ERegThreeBitEncodingWordOp>((byteBitset >> 3).to_ulong() & 0b00000111);

                    if (ongoingInstructionMetadata.dBit == EDBit::RegFieldDestRMFieldSrc)
                    {
                        std::cout << ongoingInstructionMetadata.instructionString << " " 
                            << getRegNameFromThreeBitEncodingWordOp(reg) << ", " 
                            << directAddress << '\n';
                    }
                    else
                    {
                        std::cout << ongoingInstructionMetadata.instructionString << " " 
                            << directAddress << ", " 
                            << getRegNameFromThreeBitEncodingWordOp(reg) << '\n';
                    }
                }
            }
            else
            {
                std::string effectiveAddressEquation = "[" + baseEquation + "]";

                if (ongoingInstructionMetadata.wBit == EWBit::ByteOperation)
                {
                    // [3b_reg]
                    ERegThreeBitEncodingByteOp reg = static_cast<ERegThreeBitEncodingByteOp>((byteBitset >> 3).to_ulong() & 0b00000111);

                    if (ongoingInstructionMetadata.dBit == EDBit::RegFieldDestRMFieldSrc)
                    {
                        std::cout << ongoingInstructionMetadata.instructionString << " " 
                            << getRegNameFromThreeBitEncodingByteOp(reg) << ", " 
                            << effectiveAddressEquation << '\n';
                    }
                    else
                    {
                        std::cout << ongoingInstructionMetadata.instructionString << " " 
                            << effectiveAddressEquation << ", " 
                            << getRegNameFromThreeBitEncodingByteOp(reg) << '\n';
                    }
                }
                else
                {
                    // [3b_reg]
                    ERegThreeBitEncodingWordOp reg = static_cast<ERegThreeBitEncodingWordOp>((byteBitset >> 3).to_ulong() & 0b00000111);

                    if (ongoingInstructionMetadata.dBit == EDBit::RegFieldDestRMFieldSrc)
                    {
                        std::cout << ongoingInstructionMetadata.instructionString << " " 
                            << getRegNameFromThreeBitEncodingWordOp(reg) << ", " 
                            << effectiveAddressEquation << '\n';
                    }
                    else
                    {
                        std::cout << ongoingInstructionMetadata.instructionString << " " 
                            << effectiveAddressEquation << ", " 
                            << getRegNameFromThreeBitEncodingWordOp(reg) << '\n';
                    }
                }
            }
            break;
        }
        case (EModField::MemModeEightBitDisp) :
        {
            // needs third byte
            char byte;
            file.read(&byte, sizeof(byte));
            ByteBitset thirdByteBitset(byte);

			std::string effectiveAddressEquation;
            if ((thirdByteBitset.to_ulong() & 0b10000000) > 0)
            {
				effectiveAddressEquation = "[" + baseEquation + " - " + std::to_string(-static_cast<int8_t>(thirdByteBitset.to_ulong())) + "]";
            }
            else
            {
                if (thirdByteBitset.to_ulong() != 0)
                {
                    effectiveAddressEquation = "[" + baseEquation + " + " + std::to_string(thirdByteBitset.to_ulong()) + "]";
                }
                else
                {
                    effectiveAddressEquation = "[" + baseEquation + "]";
                }
            }

            if (ongoingInstructionMetadata.wBit == EWBit::ByteOperation)
            {
                // [3b_reg]
                ERegThreeBitEncodingByteOp reg = static_cast<ERegThreeBitEncodingByteOp>((byteBitset >> 3).to_ulong() & 0b00000111);

                if (ongoingInstructionMetadata.dBit == EDBit::RegFieldDestRMFieldSrc)
                {
                    std::cout << ongoingInstructionMetadata.instructionString << " " 
                        << getRegNameFromThreeBitEncodingByteOp(reg) << ", " 
                        << effectiveAddressEquation << '\n';
                }
                else
                {
                    std::cout << ongoingInstructionMetadata.instructionString << " " 
                        << effectiveAddressEquation << ", " 
                        << getRegNameFromThreeBitEncodingByteOp(reg) << '\n';
                }
            }
            else
            {
                // [3b_reg]
                ERegThreeBitEncodingWordOp reg = static_cast<ERegThreeBitEncodingWordOp>((byteBitset >> 3).to_ulong() & 0b00000111);

                if (ongoingInstructionMetadata.dBit == EDBit::RegFieldDestRMFieldSrc)
                {
                    std::cout << ongoingInstructionMetadata.instructionString << " " 
                        << getRegNameFromThreeBitEncodingWordOp(reg) << ", " 
                        << effectiveAddressEquation << '\n';
                }
                else
                {
                    std::cout << ongoingInstructionMetadata.instructionString << " " 
                        << effectiveAddressEquation << ", " 
                        << getRegNameFromThreeBitEncodingWordOp(reg) << '\n';
                }
            }

            break;
        }
        case (EModField::MemModeSixteenBitDisp) :
        {
            // [3b_r/m]
            uint64_t rmField = byteBitset.to_ulong() & 0b00000111;
            std::string baseEquation = getEffectiveAddressBaseEquationString(rmField, mod);

            // needs third byte
            char byte;
            file.read(&byte, sizeof(byte));
            ByteBitset thirdByteBitset(byte);

            // needs fourth byte
            file.read(&byte, sizeof(byte));
            ByteBitset fourthByteBitset(byte);

            WordBitset netDisplacementBitset((fourthByteBitset.to_ulong() << 8) | thirdByteBitset.to_ulong()); 
            std::string effectiveAddressEquation;
            if ((netDisplacementBitset.to_ulong() & 0b1000000000000000) > 0)
            {
				effectiveAddressEquation = "[" + baseEquation + " - " + std::to_string(-static_cast<int16_t>(netDisplacementBitset.to_ulong())) + "]";
            }
            else
            {
                if (netDisplacementBitset.to_ulong() != 0)
                {
                    effectiveAddressEquation = "[" + baseEquation + " + " + std::to_string(netDisplacementBitset.to_ulong()) + "]";
                }
                else
                {
                    effectiveAddressEquation = "[" + baseEquation + "]";
                }
            }

            if (ongoingInstructionMetadata.wBit == EWBit::ByteOperation)
            {
                // [3b_reg]
                ERegThreeBitEncodingByteOp reg = static_cast<ERegThreeBitEncodingByteOp>((byteBitset >> 3).to_ulong() & 0b00000111);

                if (ongoingInstructionMetadata.dBit == EDBit::RegFieldDestRMFieldSrc)
                {
                    std::cout << ongoingInstructionMetadata.instructionString << " " 
                        << getRegNameFromThreeBitEncodingByteOp(reg) << ", " 
                        << effectiveAddressEquation << '\n';
                }
                else
                {
                    std::cout << ongoingInstructionMetadata.instructionString << " " 
                        << effectiveAddressEquation << ", " 
                        << getRegNameFromThreeBitEncodingByteOp(reg) << '\n';
                }
            }
            else
            {
                // [3b_reg]
                ERegThreeBitEncodingWordOp reg = static_cast<ERegThreeBitEncodingWordOp>((byteBitset >> 3).to_ulong() & 0b00000111);

                if (ongoingInstructionMetadata.dBit == EDBit::RegFieldDestRMFieldSrc)
                {
                    std::cout << ongoingInstructionMetadata.instructionString << " " 
                        << getRegNameFromThreeBitEncodingWordOp(reg) << ", " 
                        << effectiveAddressEquation << '\n';
                }
                else
                {
                    std::cout << ongoingInstructionMetadata.instructionString << " " 
                        << effectiveAddressEquation << ", " 
                        << getRegNameFromThreeBitEncodingWordOp(reg) << '\n';
                }
            }

            break;
        }
        case (EModField::RegisterMode) :
        {
            if (ongoingInstructionMetadata.wBit == EWBit::ByteOperation)
            {
                // [3b_reg]
                ERegThreeBitEncodingByteOp reg = static_cast<ERegThreeBitEncodingByteOp>((byteBitset >> 3).to_ulong() & 0b00000111);
                // [3b_r/m]
                ERegThreeBitEncodingByteOp rm = static_cast<ERegThreeBitEncodingByteOp>(byteBitset.to_ulong() & 0b00000111);

                if (ongoingInstructionMetadata.dBit == EDBit::RegFieldDestRMFieldSrc)
                {
                    std::cout << ongoingInstructionMetadata.instructionString << " " 
                        << getRegNameFromThreeBitEncodingByteOp(reg) << ", " 
                        << getRegNameFromThreeBitEncodingByteOp(rm) << '\n';
                }
                else
                {
                    std::cout << ongoingInstructionMetadata.instructionString << " " 
                        << getRegNameFromThreeBitEncodingByteOp(rm) << ", " 
                        << getRegNameFromThreeBitEncodingByteOp(reg) << '\n';
                }
            }
            else
            {
                // [3b_reg]
                ERegThreeBitEncodingWordOp reg = static_cast<ERegThreeBitEncodingWordOp>((byteBitset >> 3).to_ulong() & 0b00000111);
                // [3b_r/m]
                ERegThreeBitEncodingWordOp rm = static_cast<ERegThreeBitEncodingWordOp>(byteBitset.to_ulong() & 0b00000111);

                if (ongoingInstructionMetadata.dBit == EDBit::RegFieldDestRMFieldSrc)
                {
                    std::cout << ongoingInstructionMetadata.instructionString << " " 
                        << getRegNameFromThreeBitEncodingWordOp(reg) << ", " 
                        << getRegNameFromThreeBitEncodingWordOp(rm) << '\n';
                }
                else
                {
                    std::cout << ongoingInstructionMetadata.instructionString << " " 
                        << getRegNameFromThreeBitEncodingWordOp(rm) << ", " 
                        << getRegNameFromThreeBitEncodingWordOp(reg) << '\n';
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
    ongoingInstructionMetadata = getInstructionMetadataFromMnemonic(byteBitset);
    if (ongoingInstructionMetadata.instruction == EInstruction::MovRegMemToFromReg)
    {
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
    else if (ongoingInstructionMetadata.instruction == EInstruction::MovImmToReg)
    {
        std::string encodedRegister;
        std::string immediateValue;

        if (((byteBitset >> 3).to_ulong() & 0b00000001) > 0)
        {
            ongoingInstructionMetadata.wBit = EWBit::WordOperation;

            // needs second byte
            char byte;
            file.read(&byte, sizeof(byte));
            ByteBitset secondByteBitset(byte);

            // needs third byte
            file.read(&byte, sizeof(byte));
            ByteBitset thirdByteBitset(byte);

            WordBitset netDisplacementBitset((thirdByteBitset.to_ulong() << 8) | secondByteBitset.to_ulong());
            immediateValue = std::to_string(netDisplacementBitset.to_ulong());

            // [3b_reg]
            ERegThreeBitEncodingWordOp reg = static_cast<ERegThreeBitEncodingWordOp>(byteBitset.to_ulong() & 0b00000111);
            encodedRegister = getRegNameFromThreeBitEncodingWordOp(reg);
        }
        else
        {
            ongoingInstructionMetadata.wBit = EWBit::ByteOperation;

            // needs second byte
            char byte;
            file.read(&byte, sizeof(byte));
            ByteBitset secondByteBitset(byte);

            immediateValue = std::to_string(secondByteBitset.to_ulong()); 

            // [3b_reg]
            ERegThreeBitEncodingByteOp reg = static_cast<ERegThreeBitEncodingByteOp>(byteBitset.to_ulong() & 0b00000111);
            encodedRegister = getRegNameFromThreeBitEncodingByteOp(reg);
        }

        std::cout << ongoingInstructionMetadata.instructionString << " " 
            << encodedRegister << ", " 
            << immediateValue << '\n';
    }
    else if (ongoingInstructionMetadata.instruction == EInstruction::MovMemToAcc || 
            ongoingInstructionMetadata.instruction == EInstruction::MovAccToMem)
    {
        // needs second byte
        char byte;
        file.read(&byte, sizeof(byte));
        ByteBitset secondByteBitset(byte);

        // needs third byte
        file.read(&byte, sizeof(byte));
        ByteBitset thirdByteBitset(byte);

        WordBitset effectiveAddressBitset((thirdByteBitset.to_ulong() << 8) | secondByteBitset.to_ulong());
        std::string effectiveAddress = "[" + std::to_string(effectiveAddressBitset.to_ulong()) + "]";

        if (ongoingInstructionMetadata.instruction == EInstruction::MovMemToAcc)
        {
            std::cout << ongoingInstructionMetadata.instructionString << " " 
                << "ax" << ", " 
                << effectiveAddress << '\n';
        }
        else
        {
            std::cout << ongoingInstructionMetadata.instructionString << " " 
                << effectiveAddress << ", " 
                << "ax" << '\n';
        }
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
