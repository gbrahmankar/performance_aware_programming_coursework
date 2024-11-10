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
    /*
    mov_immediate_rm : [[1100011][1b_w]] [[2b_mod]000[3b_rm]] [8b_disp_low] [8b_disp_high] [8b_data_low] [8b_data_high]
    */
    MovImmToRM,
    /*
    add_reg_mem_with_reg_to_either : [000000][1b_d][1b_w]] [[2b_mod][3b_reg][3b_r/m]] [8b_disp_low] [8b_disp_high]
    */
    AddRegMemWithRegToEither,
    /*
    add_imm_to_acc : [0000010][1b_w]] [8b_data_low] [8b_data_high]
    */
    AddImmToAcc,
    /*
    add_imm_to_rm : [100000][1b_s][1b_w]] [[2b_mod][000][3b_r/m]] [8b_disp_low] [8b_disp_high] [8b_data_low] [8b_data_high]
    sub_imm_from_rm : [100000][1b_s][1b_w]] [[2b_mod][101][3b_r/m]] [8b_disp_low] [8b_disp_high] [8b_data_low] [8b_data_high]
    cmp_imm_from_rm : [100000][1b_s][1b_w]] [[2b_mod][111][3b_r/m]] [8b_disp_low] [8b_disp_high] [8b_data_low] [8b_data_high]
    */
    AddSubCmpImmToRM,
    /*
    sub_reg_mem_with_reg_to_either : [001010][1b_d][1b_w]] [[2b_mod][3b_reg][3b_r/m]] [8b_disp_low] [8b_disp_high]
    */
    SubRegMemWithRegToEither,
    /*
    sub_imm_from_acc : [0010110][1b_w]] [8b_data_low] [8b_data_high]
    */
    SubImmFromAcc,
    /*
    cmp_reg_mem_with_reg : [001010][1b_d][1b_w]] [[2b_mod][3b_reg][3b_r/m]] [8b_disp_low] [8b_disp_high]
    */
    CmpRegMemAndReg,
    /*
    cmp_imm_with_acc : [0010110][1b_w]] [8b_data_low] [8b_data_high]
    */
    CmpImmWithAcc,
    /*
    jump_if_not_zero : [01110101] [8b_ip_increments]
    */
    JmpIfNotZero,

    InstructionInvalid
};

enum EInstructionFormat
{
    RegReg,
    MemReg,

    RegImm,
    MemImm,

    Imm,

    InstructionFormatInvalid
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

// [1b_s]
enum ESBit : uint8_t
{
    SixteenBitImmValue = 0,
    EightBitImmValue,

    SBitInvalid
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

enum EBitsDisplacement : uint8_t
{
    NoDisp = 0,
    EightBitDisp,
    SixteenBitDisp,

    DispInvalid
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
    EInstructionFormat instructionFormat = EInstructionFormat::InstructionFormatInvalid;
    std::string instructionString = "";

    EDBit dBit = EDBit::DBitInvalid;
    EWBit wBit = EWBit::WBitInvalid;
    ESBit sBit = ESBit::SBitInvalid;

    EModField modField = EModField::ModFieldInvalid;
    uint8_t regField = UINT8_MAX;
    uint8_t rmField = UINT8_MAX;

    std::string effectiveAddress;

    // if the instruction has two registers, 0_src, 1_dst. else, based on instruction_format
    std::vector<uint8_t> registers;
    std::vector<std::string> stringifiedRegisters;

    std::string immediateValue;
};

std::unordered_map<uint64_t, InstructionMetadata> mnemonicToInstructionMetadata =
{
    { MnemonicBitset("100010").to_ulong(), InstructionMetadata(EInstruction::MovRegMemToFromReg, "mov") },
    { MnemonicBitset("1011").to_ulong(), InstructionMetadata(EInstruction::MovImmToReg, "mov") },
    { MnemonicBitset("1100011").to_ulong(), InstructionMetadata(EInstruction::MovImmToRM, "mov") },
    { MnemonicBitset("1010000").to_ulong(), InstructionMetadata(EInstruction::MovMemToAcc, "mov") },
    { MnemonicBitset("1010001").to_ulong(), InstructionMetadata(EInstruction::MovAccToMem, "mov") },

    { MnemonicBitset("000000").to_ulong(), InstructionMetadata(EInstruction::AddRegMemWithRegToEither, "add") },
    { MnemonicBitset("100000").to_ulong(), InstructionMetadata(EInstruction::AddSubCmpImmToRM, "add") },
    { MnemonicBitset("0000010").to_ulong(), InstructionMetadata(EInstruction::AddImmToAcc, "add") },

    { MnemonicBitset("001010").to_ulong(), InstructionMetadata(EInstruction::SubRegMemWithRegToEither, "sub") },
    { MnemonicBitset("0010110").to_ulong(), InstructionMetadata(EInstruction::SubImmFromAcc, "sub") },

    { MnemonicBitset("001110").to_ulong(), InstructionMetadata(EInstruction::CmpRegMemAndReg, "cmp") },
    { MnemonicBitset("0011110").to_ulong(), InstructionMetadata(EInstruction::CmpImmWithAcc, "cmp") },

    { MnemonicBitset("01110101").to_ulong(), InstructionMetadata(EInstruction::JmpIfNotZero, "jnz") }
};

void printInstruction(InstructionMetadata& metadata)
{
    if (metadata.wBit == EWBit::WordOperation)
	{
        for (uint8_t reg : metadata.registers)
        {
            metadata.stringifiedRegisters.push_back(getRegNameFromThreeBitEncodingWordOp(static_cast<ERegThreeBitEncodingWordOp>(reg)));
        }
	}
	else
	{
        for (uint8_t reg : metadata.registers)
        {
            metadata.stringifiedRegisters.push_back(getRegNameFromThreeBitEncodingByteOp(static_cast<ERegThreeBitEncodingByteOp>(reg)));
        }
	}

    switch (metadata.instructionFormat)
    {
    case (EInstructionFormat::Imm):
    {
        std::cout << metadata.instructionString << " " << metadata.immediateValue << '\n';
        break;
    }
    case (EInstructionFormat::MemImm):
    {
		if (metadata.wBit == EWBit::WordOperation)
		{
            std::cout << metadata.instructionString << " " << metadata.effectiveAddress << ", word " << metadata.immediateValue << '\n';
		}
		else
		{
            std::cout << metadata.instructionString << " " << metadata.effectiveAddress << ", byte " << metadata.immediateValue << '\n';
		}
        break;
    }
    case (EInstructionFormat::MemReg):
    {
        if (metadata.dBit == EDBit::RegFieldDestRMFieldSrc)
        {
            std::cout << metadata.instructionString << " " << metadata.stringifiedRegisters[0] << ", " << metadata.effectiveAddress << '\n';
        }
        else
        {
            std::cout << metadata.instructionString << " " << metadata.effectiveAddress << ", " << metadata.stringifiedRegisters[0] << '\n';
        }

        break;
    }
    case (EInstructionFormat::RegImm):
    {
        if (metadata.dBit == EDBit::RegFieldDestRMFieldSrc)
        {
            std::cout << metadata.instructionString << " " << metadata.stringifiedRegisters[0] << ", " << metadata.immediateValue << '\n';
        }
        else
        {
            std::cout << metadata.instructionString << " " << metadata.stringifiedRegisters[1] << ", " << metadata.immediateValue << '\n';
        }

        break;
    }
    case (EInstructionFormat::RegReg):
    {
        if (metadata.dBit == EDBit::RegFieldDestRMFieldSrc)
        {
            std::cout << metadata.instructionString << " " << metadata.stringifiedRegisters[0] << ", " << metadata.stringifiedRegisters[1] << '\n';
        }
        else
        {
            std::cout << metadata.instructionString << " " << metadata.stringifiedRegisters[1] << ", " << metadata.stringifiedRegisters[0] << '\n';
        }

        break;
    }
    default :
        throw std::runtime_error("invalid instruction format !");
    }
}

void simulateInstruction(const InstructionMetadata& metadata)
{
}

void constructImmediateValueFromOperationWidth(std::ifstream& file, InstructionMetadata& ongoingInstructionMetadata, EWBit w)
{
    if (w == EWBit::WordOperation)
	{
		char byte;
		file.read(&byte, sizeof(byte));
		ByteBitset firstByteBitset(byte);

		file.read(&byte, sizeof(byte));
		ByteBitset secondByteBitset(byte);

		WordBitset immediateValueBitset((secondByteBitset.to_ulong() << 8) | firstByteBitset.to_ulong());
		ongoingInstructionMetadata.immediateValue = std::to_string(immediateValueBitset.to_ulong());
	}
	else
	{
		char byte;
		file.read(&byte, sizeof(byte));
		ByteBitset firstByteBitset(byte);

		ongoingInstructionMetadata.immediateValue = std::to_string(firstByteBitset.to_ulong()); 
	}
}

void constructEffectiveAddressFromMode(std::ifstream& file, InstructionMetadata& metadata, EBitsDisplacement disp)
{
    if (disp == EBitsDisplacement::DispInvalid)
    {
        return;
    }

    std::string baseEquation = getEffectiveAddressBaseEquationString(metadata.rmField , metadata.modField);
    if (baseEquation.empty())
    {
        metadata.effectiveAddress = "[";
    }
    else
    {
        metadata.effectiveAddress = "[" + baseEquation;
    }

    char byte;
    if (disp == EBitsDisplacement::EightBitDisp)
    {
		file.read(&byte, sizeof(byte));
		ByteBitset firstByteBitset(byte);

        ByteBitset effectiveAddressBitset(firstByteBitset);
        if (effectiveAddressBitset.to_ulong() != 0)
        {
            if (baseEquation.empty()) // direct_address case
            {
                metadata.effectiveAddress += std::to_string(effectiveAddressBitset.to_ulong()) + "]";
            }
            else
            {
                if ((effectiveAddressBitset.to_ulong() & 0b10000000) > 0)
                {
                    metadata.effectiveAddress += " - " + std::to_string(-static_cast<int8_t>(effectiveAddressBitset.to_ulong())) + "]";
                }
                else
                {
                    metadata.effectiveAddress += " + " + std::to_string(effectiveAddressBitset.to_ulong()) + "]";
                }
            }
        }
        else
        {
		    metadata.effectiveAddress += "]";
        }
    }
    else if (disp == EBitsDisplacement::SixteenBitDisp)
    {
		file.read(&byte, sizeof(byte));
		ByteBitset firstByteBitset(byte);

        file.read(&byte, sizeof(byte));
		ByteBitset secondByteBitset(byte);

		WordBitset effectiveAddressBitset((secondByteBitset.to_ulong() << 8) | firstByteBitset.to_ulong()); 
        if (effectiveAddressBitset.to_ulong() != 0)
        {
            if (baseEquation.empty()) // direct_address case
            {
                metadata.effectiveAddress += std::to_string(effectiveAddressBitset.to_ulong()) + "]";
            }
            else
            {
                if ((effectiveAddressBitset.to_ulong() & 0x8000) > 0)
                {
                    metadata.effectiveAddress += " - " + std::to_string(-static_cast<int16_t>(effectiveAddressBitset.to_ulong())) + "]";
                }
                else
                {
                    metadata.effectiveAddress += " + " + std::to_string(effectiveAddressBitset.to_ulong()) + "]";
                }
            }
        }
        else
        {
		    metadata.effectiveAddress += "]";
        }
    }
    else
    {
		metadata.effectiveAddress += "]";
    }
}

InstructionMetadata& getInstructionMetadataFromMnemonic(ByteBitset bitSet)
{
    while (bitSet.to_ulong() >= 0)
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
	// [2b_mod]
	EModField mod = static_cast<EModField>((byteBitset >> 6).to_ulong() & 0b00000011);

	// [3b_r/m]
	ongoingInstructionMetadata.rmField = byteBitset.to_ulong() & 0b00000111;

	// [3b_reg]
	ongoingInstructionMetadata.regField = (byteBitset >> 3).to_ulong() & 0b00000111;

	ongoingInstructionMetadata.registers.push_back(ongoingInstructionMetadata.regField);
	ongoingInstructionMetadata.registers.push_back(ongoingInstructionMetadata.rmField);

	switch (mod)
	{
	case (EModField::MemModeNoDisp) :
	{
        ongoingInstructionMetadata.modField = EModField::MemModeNoDisp;

		if (ongoingInstructionMetadata.rmField != 6) // 0b110 -> direct_address
		{
			constructEffectiveAddressFromMode(file, ongoingInstructionMetadata, EBitsDisplacement::NoDisp);
		}
		else
		{
			constructEffectiveAddressFromMode(file, ongoingInstructionMetadata, EBitsDisplacement::SixteenBitDisp);
		}

		break;
	}
	case (EModField::MemModeEightBitDisp) :
	{
        ongoingInstructionMetadata.modField = EModField::MemModeEightBitDisp;

		constructEffectiveAddressFromMode(file, ongoingInstructionMetadata, EBitsDisplacement::EightBitDisp);
		break;
	}
	case (EModField::MemModeSixteenBitDisp) :
	{
        ongoingInstructionMetadata.modField = EModField::MemModeSixteenBitDisp;

		constructEffectiveAddressFromMode(file, ongoingInstructionMetadata, EBitsDisplacement::SixteenBitDisp);
		break;
	}
	case (EModField::RegisterMode) :
	{
        ongoingInstructionMetadata.modField = EModField::RegisterMode;

        break;
	}
	default :
		return;
	}
}

void decodeFirstInstructionByte(const ByteBitset& byteBitset, std::ifstream& file, InstructionMetadata& ongoingInstructionMetadata)
{
    ongoingInstructionMetadata = getInstructionMetadataFromMnemonic(byteBitset);

    switch (ongoingInstructionMetadata.instruction)
    {
    case (EInstruction::JmpIfNotZero) :
    {
        ongoingInstructionMetadata.instructionFormat = EInstructionFormat::Imm;
		constructImmediateValueFromOperationWidth(file, ongoingInstructionMetadata, EWBit::ByteOperation);
        break;
    }
	case (EInstruction::MovRegMemToFromReg) :
	case (EInstruction::AddRegMemWithRegToEither) :
	case (EInstruction::SubRegMemWithRegToEither) :
	case (EInstruction::CmpRegMemAndReg) :
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

        if (ongoingInstructionMetadata.modField == EModField::RegisterMode)
        {
            ongoingInstructionMetadata.instructionFormat = EInstructionFormat::RegReg;
        }
        else
        {
            ongoingInstructionMetadata.instructionFormat = EInstructionFormat::MemReg;
        }

		break;
	}
	case (EInstruction::MovImmToReg) :
	{
		ongoingInstructionMetadata.instructionFormat = EInstructionFormat::RegImm;

		// d_bit is implied. since, we HAVE to mov the immediate value into the register.
		ongoingInstructionMetadata.dBit = EDBit::RegFieldDestRMFieldSrc;

		// [3b_reg]
		uint8_t reg = byteBitset.to_ulong() & 0b00000111;
		ongoingInstructionMetadata.registers.push_back(reg);;

		constructImmediateValueFromOperationWidth(file, ongoingInstructionMetadata, static_cast<EWBit>((byteBitset >> 3).to_ulong() & 0b00000001));

		break;
	}
	case (EInstruction::MovMemToAcc) :
	case (EInstruction::MovAccToMem) :
	{
		ongoingInstructionMetadata.instructionFormat = EInstructionFormat::MemReg;

		// d_bit bit is implied since mov_mem_to_acc and mov_acc_to_mem are two separate mnemonics !
		if (ongoingInstructionMetadata.instruction == EInstruction::MovMemToAcc)
		{
			ongoingInstructionMetadata.dBit = EDBit::RegFieldDestRMFieldSrc;
		}
		else
		{
			ongoingInstructionMetadata.dBit = EDBit::RegFieldSrcRMFieldDest;
		}

		// w_bit is implied since it HAS to be ax !
		ongoingInstructionMetadata.wBit = EWBit::WordOperation;

		ongoingInstructionMetadata.rmField = 6; // 0b110 -> direct_address is implied
		ongoingInstructionMetadata.modField = EModField::MemModeNoDisp; // 0b110 -> direct_address is implied

		ongoingInstructionMetadata.registers.push_back(static_cast<uint8_t>(ERegThreeBitEncodingWordOp::ax));

		constructEffectiveAddressFromMode(file, ongoingInstructionMetadata, EBitsDisplacement::SixteenBitDisp);

		break;
	}
    case (EInstruction::MovImmToRM) :
	{
		ongoingInstructionMetadata.instructionFormat = EInstructionFormat::MemImm;

		ongoingInstructionMetadata.dBit = EDBit::RegFieldSrcRMFieldDest;

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

		constructImmediateValueFromOperationWidth(file, ongoingInstructionMetadata, ongoingInstructionMetadata.wBit);

		break;
	}
    case (EInstruction::AddImmToAcc) :
    case (EInstruction::SubImmFromAcc) :
    case (EInstruction::CmpImmWithAcc) :
	{
		ongoingInstructionMetadata.instructionFormat = EInstructionFormat::RegImm;

		// d_bit bit is implied since mov_mem_to_acc and mov_acc_to_mem are two separate mnemonics !
		ongoingInstructionMetadata.dBit = EDBit::RegFieldDestRMFieldSrc;

        if ((byteBitset.to_ulong() & 0b00000001) > 0)
		{
			ongoingInstructionMetadata.wBit = EWBit::WordOperation;
		    ongoingInstructionMetadata.registers.push_back(static_cast<uint8_t>(ERegThreeBitEncodingWordOp::ax));
		}
		else
		{
			ongoingInstructionMetadata.wBit = EWBit::ByteOperation;
		    ongoingInstructionMetadata.registers.push_back(static_cast<uint8_t>(ERegThreeBitEncodingByteOp::al));
		}

		constructImmediateValueFromOperationWidth(file, ongoingInstructionMetadata, ongoingInstructionMetadata.wBit);

		break;
	}
	case (EInstruction::AddSubCmpImmToRM) :
    {
        // needs second byte
		char byte;
		file.read(&byte, sizeof(byte));
		ByteBitset secondByteBitset(byte);
		decodeSecondInstructionByte(secondByteBitset, file, ongoingInstructionMetadata);

        switch(ongoingInstructionMetadata.regField)
        {
        case (0) :
        {
            ongoingInstructionMetadata.instructionString = "add";
            break;
        }
        case (5) :
        {
            ongoingInstructionMetadata.instructionString = "sub";
            break;
        }
        case (7) :
        {
            ongoingInstructionMetadata.instructionString = "cmp";
            break;
        }
        default:
            break;
        }

        if (ongoingInstructionMetadata.modField != EModField::RegisterMode)
        {
            ongoingInstructionMetadata.instructionFormat = EInstructionFormat::MemImm;
        }
        else
        {
		    ongoingInstructionMetadata.instructionFormat = EInstructionFormat::RegImm;
        }

        if ((byteBitset.to_ulong() & 0b00000001) > 0)
		{
			ongoingInstructionMetadata.wBit = EWBit::WordOperation;

            if (((byteBitset >> 1).to_ulong() & 0b00000001) > 0)
            {
                ongoingInstructionMetadata.sBit = ESBit::EightBitImmValue;
                constructImmediateValueFromOperationWidth(file, ongoingInstructionMetadata, EWBit::ByteOperation);
            }
            else
            {
                ongoingInstructionMetadata.sBit = ESBit::SixteenBitImmValue;
                constructImmediateValueFromOperationWidth(file, ongoingInstructionMetadata, EWBit::WordOperation);
            }
		}
		else
		{
			ongoingInstructionMetadata.wBit = EWBit::ByteOperation;

			ongoingInstructionMetadata.sBit = ESBit::EightBitImmValue;
            constructImmediateValueFromOperationWidth(file, ongoingInstructionMetadata, EWBit::ByteOperation);
		}

        break;
    }	
	default :
		return;
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

    InstructionMetadata ongoingInstructionMetadata;
    std::cout << "bits 16" << '\n';
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

        printInstruction(ongoingInstructionMetadata);
        simulateInstruction(ongoingInstructionMetadata);
    }

    return 0;    
}
