#include "8086_decoder_simulator.h"

namespace PartOne
{

Memory memory;
RegisterFile registers{0x0000};

std::unordered_map<uint64_t, InstructionMetadata> mnemonicToInstructionMetadata =
{
    { MnemonicBitset("100010").to_ulong(), InstructionMetadata(EInstruction::MovRegMemToFromReg, "mov", 6) },
    { MnemonicBitset("1011").to_ulong(), InstructionMetadata(EInstruction::MovImmToReg, "mov", 4) },
    { MnemonicBitset("1100011").to_ulong(), InstructionMetadata(EInstruction::MovImmToRM, "mov", 7) },
    { MnemonicBitset("1010000").to_ulong(), InstructionMetadata(EInstruction::MovMemToAcc, "mov", 7) },
    { MnemonicBitset("1010001").to_ulong(), InstructionMetadata(EInstruction::MovAccToMem, "mov", 7) },
    { MnemonicBitset("10001110").to_ulong(), InstructionMetadata(EInstruction::MovRegMemToSegReg, "mov", 8) },
    { MnemonicBitset("10001100").to_ulong(), InstructionMetadata(EInstruction::MovSegRegToRegMem, "mov", 8) },

    { MnemonicBitset("000000").to_ulong(), InstructionMetadata(EInstruction::AddRegMemWithRegToEither, "add", 6) },
    { MnemonicBitset("100000").to_ulong(), InstructionMetadata(EInstruction::AddSubCmpImmToRM, "add", 6) },
    { MnemonicBitset("0000010").to_ulong(), InstructionMetadata(EInstruction::AddImmToAcc, "add", 7) },

    { MnemonicBitset("001010").to_ulong(), InstructionMetadata(EInstruction::SubRegMemWithRegToEither, "sub", 6) },
    { MnemonicBitset("0010110").to_ulong(), InstructionMetadata(EInstruction::SubImmFromAcc, "sub", 7) },

    { MnemonicBitset("001110").to_ulong(), InstructionMetadata(EInstruction::CmpRegMemAndReg, "cmp", 6) },
    { MnemonicBitset("0011110").to_ulong(), InstructionMetadata(EInstruction::CmpImmWithAcc, "cmp", 7) },

    { MnemonicBitset("01110101").to_ulong(), InstructionMetadata(EInstruction::JmpIfNotZero, "jnz", 8) }
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

std::unordered_map<ESegmentRegisterEncoding, std::string> segmentRegisterEncodingToString =
{
    { ESegmentRegisterEncoding::es, "es" },
    { ESegmentRegisterEncoding::cs, "cs" },
    { ESegmentRegisterEncoding::ss, "ss" },
    { ESegmentRegisterEncoding::ds, "ds" }
};

std::unordered_map<std::string, std::unordered_map<EInstructionOperandLayout, uint16_t>> instructionBaseCostMap = 
{
{ 
	"mov",
	{
        { EInstructionOperandLayout::AccumulatorToMemory, 10 },
        { EInstructionOperandLayout::MemoryToAccumulator, 10 },

        { EInstructionOperandLayout::MemoryToRegister, 8 },
        { EInstructionOperandLayout::RegisterToMemory, 9 },

        { EInstructionOperandLayout::RegisterToRegister, 2 },
        { EInstructionOperandLayout::ImmediateToRegister, 4 },
        { EInstructionOperandLayout::ImmediateToMemory, 10 }
	}
},
{
    "add",
    {
        { EInstructionOperandLayout::RegisterToRegister, 3 },

        { EInstructionOperandLayout::MemoryToRegister, 9 },
        { EInstructionOperandLayout::RegisterToMemory, 16 },

        { EInstructionOperandLayout::ImmediateToRegister, 4 },
        { EInstructionOperandLayout::ImmediateToMemory, 17 },

        { EInstructionOperandLayout::ImmediateToAccumulator, 4 }
	}
},
{
    "sub",
    {
        { EInstructionOperandLayout::RegisterToRegister, 3 },

        { EInstructionOperandLayout::MemoryToRegister, 9 },
        { EInstructionOperandLayout::RegisterToMemory, 16 },

        { EInstructionOperandLayout::ImmediateToAccumulator, 4 },
        { EInstructionOperandLayout::ImmediateToRegister, 4 },

        { EInstructionOperandLayout::ImmediateToMemory, 17 }
	}
},
{
    "cmp",
    {
        { EInstructionOperandLayout::RegisterToRegister, 3 },

        { EInstructionOperandLayout::MemoryToRegister, 9 },
        { EInstructionOperandLayout::RegisterToMemory, 9 },

        { EInstructionOperandLayout::ImmediateToRegister, 4 },
        { EInstructionOperandLayout::ImmediateToMemory, 10 },

        { EInstructionOperandLayout::ImmediateToAccumulator, 4 }
	}
},
{
    "jne",
    {
        { EInstructionOperandLayout::JmpTaken, 16 },
        { EInstructionOperandLayout::JmpNotTaken, 4 }
	}
}
};

std::unordered_map<ERegThreeBitEncodingByteOp, EGeneralPurposeRegisterAccess> generalPurposeRegisterAccess =
{
    { ERegThreeBitEncodingByteOp::al, EGeneralPurposeRegisterAccess::LowByteAccess },
    { ERegThreeBitEncodingByteOp::cl, EGeneralPurposeRegisterAccess::LowByteAccess },
    { ERegThreeBitEncodingByteOp::dl, EGeneralPurposeRegisterAccess::LowByteAccess },
    { ERegThreeBitEncodingByteOp::bl, EGeneralPurposeRegisterAccess::LowByteAccess },
    { ERegThreeBitEncodingByteOp::ah, EGeneralPurposeRegisterAccess::HighByteAccess },
    { ERegThreeBitEncodingByteOp::ch, EGeneralPurposeRegisterAccess::HighByteAccess },
    { ERegThreeBitEncodingByteOp::dh, EGeneralPurposeRegisterAccess::HighByteAccess },
    { ERegThreeBitEncodingByteOp::bh, EGeneralPurposeRegisterAccess::HighByteAccess }
};

EGeneralPurposeRegisterAccess getGeneralPurposeRegisterAccess(ERegThreeBitEncodingByteOp e)
{
    if (generalPurposeRegisterAccess.count(e))
    {
        return generalPurposeRegisterAccess[e];
    }
    else
    {
        throw std::runtime_error("invalid general_purpose register access !");
    }
}

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
        throw std::runtime_error("invalid byte_operation register enum passed !");
    }
}

std::string& getSegmentRegisterEncoding(ESegmentRegisterEncoding e)
{
    if (segmentRegisterEncodingToString.count(e))
    {
        return segmentRegisterEncodingToString[e];
    }
    else
    {
        throw std::runtime_error("invalid segment_register enum passed !");
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

uint16_t getEffectiveAddressBaseValue(uint8_t registerOrMemoryFieldValue, EModField mod)
{
    switch(registerOrMemoryFieldValue)
    {
	case 0 :
		return registers.get(ERegThreeBitEncodingWordOp::bx) + registers.get(ERegThreeBitEncodingWordOp::si);
	case 1 :
		return registers.get(ERegThreeBitEncodingWordOp::bx) + registers.get(ERegThreeBitEncodingWordOp::di);
	case 2 :
		return registers.get(ERegThreeBitEncodingWordOp::bp) + registers.get(ERegThreeBitEncodingWordOp::si);
	case 3 :
		return registers.get(ERegThreeBitEncodingWordOp::bp) + registers.get(ERegThreeBitEncodingWordOp::di);
	case 4 :
		return registers.get(ERegThreeBitEncodingWordOp::si);
	case 5 :
        return registers.get(ERegThreeBitEncodingWordOp::di);
	case 6 :
		if (mod == EModField::MemModeNoDisp)
			return 0;
		else
            return registers.get(ERegThreeBitEncodingWordOp::bp);
	case 7 :
        return registers.get(ERegThreeBitEncodingWordOp::bx);
	default :
		throw std::runtime_error("invalid mod_field enum passed !");
    }
}

uint16_t getEACostWithoutDisplacement(uint8_t registerOrMemoryFieldValue, EModField mod)
{
    if (mod == EModField::RegisterMode)
    {
        return 0;
    }

    switch(registerOrMemoryFieldValue)
    {
	case 0 :
        return 7;
	case 1 :
        return 8;
	case 2 :
        return 8;
	case 3 :
        return 7;
	case 4 :
        return 5;
	case 5 :
        return 5;
	case 6 :
		if (mod == EModField::MemModeNoDisp)
			return 6;
		else
            return 5;
	case 7 :
        return 5;
	default :
		return 0;
    }
}

void setNetInstructionCost(InstructionMetadata& metadata)
{
    metadata.effectiveAddressCost = getEACostWithoutDisplacement(metadata.rmField, metadata.modField);
    if (instructionBaseCostMap.count(metadata.instructionString))
    {
        metadata.totalInstructionCost = instructionBaseCostMap[metadata.instructionString][metadata.instructionOperandLayout] + metadata.effectiveAddressCost;
    }
    else
    {
        metadata.totalInstructionCost = metadata.effectiveAddressCost;
    }

    if (metadata.hasDisplacement)
    {
        metadata.totalInstructionCost += 4;
    }
}

std::stringstream getDisassStream(InstructionMetadata& metadata, bool finishWithNewline)
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

    std::stringstream s;

    std::string finishWithChar = "\n";
    if (!finishWithNewline)
    {
        finishWithChar = " ; ";
    }

    switch (metadata.instructionFormat)
    { case (EInstructionFormat::Imm):
    {
        s << metadata.instructionString << " " << STREAM_WORD(std::stoul(metadata.immediateValue)) << finishWithChar;

        break;
    }
    case (EInstructionFormat::MemImm):
    {
		if (metadata.wBit == EWBit::WordOperation)
		{
            s << metadata.instructionString << " " << metadata.effectiveAddress << ", word " << STREAM_WORD(std::stoul(metadata.immediateValue)) << finishWithChar;
		}
		else
		{
            s << metadata.instructionString << " " << metadata.effectiveAddress << ", byte " << STREAM_BYTE(std::stoul(metadata.immediateValue)) << finishWithChar;
		}

        break;
    }
    case (EInstructionFormat::MemReg):
    {
        if (metadata.dBit == EDBit::RegFieldDestRMFieldSrc)
        {
            s << metadata.instructionString << " " << metadata.stringifiedRegisters[0] << ", " << metadata.effectiveAddress << finishWithChar;
        }
        else
        {
            s << metadata.instructionString << " " << metadata.effectiveAddress << ", " << metadata.stringifiedRegisters[0] << finishWithChar;
        }

        break;
    }
    case (EInstructionFormat::RegImm):
    {
        if (metadata.dBit == EDBit::RegFieldDestRMFieldSrc)
        {
            s << metadata.instructionString << " " << metadata.stringifiedRegisters[0] << ", " << STREAM_WORD(std::stoul(metadata.immediateValue)) << finishWithChar;
        }
        else
        {
            s << metadata.instructionString << " " << metadata.stringifiedRegisters[1] << ", " << STREAM_WORD(std::stoul(metadata.immediateValue)) << finishWithChar;
        }

        break;
    }
    case (EInstructionFormat::RegReg):
    {
        if (metadata.dBit == EDBit::RegFieldDestRMFieldSrc)
        {
            s << metadata.instructionString << " " << metadata.stringifiedRegisters[0] << ", " << metadata.stringifiedRegisters[1] << finishWithChar;
        }
        else
        {
            s << metadata.instructionString << " " << metadata.stringifiedRegisters[1] << ", " << metadata.stringifiedRegisters[0] << finishWithChar;
        }

        break;
    }
    case (EInstructionFormat::SegRegReg):
    {
        if (metadata.dBit == EDBit::RegFieldDestRMFieldSrc)
        {
            s << metadata.instructionString << " " << getSegmentRegisterEncoding(static_cast<ESegmentRegisterEncoding>(metadata.regField)) << ", " << metadata.stringifiedRegisters[1] << finishWithChar;
        }
        else
        {
            s << metadata.instructionString << " " << metadata.stringifiedRegisters[1] << ", " << getSegmentRegisterEncoding(static_cast<ESegmentRegisterEncoding>(metadata.regField)) << finishWithChar;
        }

        break;
    }
    default :
        throw std::runtime_error("invalid instruction format !");
    }

    return s;
}

void simulateInstruction(InstructionMetadata& metadata)
{
   switch (metadata.instruction)
   {
   case (EInstruction::MovImmToReg) :
   {
       if (metadata.wBit == EWBit::WordOperation)
       {
           registers.set(static_cast<ERegThreeBitEncodingWordOp>(metadata.registers[0]), static_cast<uint16_t>(std::stoul(metadata.immediateValue)));
       }
       else
       {
           registers.set(static_cast<ERegThreeBitEncodingByteOp>(metadata.registers[0]), static_cast<uint8_t>(std::stoul(metadata.immediateValue)));
       }

       metadata.instructionOperandLayout = EInstructionOperandLayout::ImmediateToRegister;
       setNetInstructionCost(metadata);
       break;
   }
   case (EInstruction::MovImmToRM) :
   {
       if (metadata.instructionFormat == EInstructionFormat::MemImm)
       {
           uint16_t effectiveAddressValue = metadata.effectiveAddressValue;

           if (metadata.dBit == EDBit::RegFieldSrcRMFieldDest)
           {
               if (metadata.wBit == EWBit::WordOperation)
               {
                   Memory::Slot& lowSlotRef = memory[effectiveAddressValue];
                   Memory::Slot& highSlotRef = memory[effectiveAddressValue + 1];

                   uint16_t value = static_cast<uint16_t>(std::stoul(metadata.immediateValue));
                   lowSlotRef.value = static_cast<uint8_t>(value & 0x00ff);
                   highSlotRef.value = static_cast<uint8_t>((value >> 8) & 0x00ff);
               }
               else
               {
                   Memory::Slot& slotRef = memory[effectiveAddressValue];

                   uint8_t value = static_cast<uint8_t>(std::stoul(metadata.immediateValue));
                   slotRef.value = value;
               }

               metadata.instructionOperandLayout = EInstructionOperandLayout::ImmediateToMemory;
			   setNetInstructionCost(metadata);
           }
       }
   }
   case (EInstruction::MovRegMemToFromReg) :
   {
       if (metadata.instructionFormat == EInstructionFormat::RegReg)
       {
           if (metadata.dBit == EDBit::RegFieldDestRMFieldSrc)
           {
               if (metadata.wBit == EWBit::WordOperation)
               {
                   uint16_t value = registers.get(static_cast<ERegThreeBitEncodingWordOp>(metadata.registers[1]));
                   registers.set(static_cast<ERegThreeBitEncodingWordOp>(metadata.registers[0]), value);
               }
               else
               {
                   uint8_t value = registers.get(static_cast<ERegThreeBitEncodingByteOp>(metadata.registers[1]));
                   registers.set(static_cast<ERegThreeBitEncodingByteOp>(metadata.registers[0]), value);
               }
           }
           else
           {
               if (metadata.wBit == EWBit::WordOperation)
               {
                   uint16_t value = registers.get(static_cast<ERegThreeBitEncodingWordOp>(metadata.registers[0]));
                   registers.set(static_cast<ERegThreeBitEncodingWordOp>(metadata.registers[1]), value);
               }
               else
               {
                   uint8_t value = registers.get(static_cast<ERegThreeBitEncodingByteOp>(metadata.registers[0]));
                   registers.set(static_cast<ERegThreeBitEncodingByteOp>(metadata.registers[1]), value);
               }
           }

           metadata.instructionOperandLayout = EInstructionOperandLayout::RegisterToRegister;
		   setNetInstructionCost(metadata);
       }
       else if (metadata.instructionFormat == EInstructionFormat::MemReg)
       {
           uint16_t effectiveAddressValue = metadata.effectiveAddressValue;

           if (metadata.dBit == EDBit::RegFieldDestRMFieldSrc)
           {
               if (metadata.wBit == EWBit::WordOperation)
               {
                   Memory::Slot lowByte = memory[effectiveAddressValue];
                   Memory::Slot highByte = memory[effectiveAddressValue + 1];
                   uint16_t value = ((uint16_t)highByte.value << 8) | lowByte.value;

                   registers.set(static_cast<ERegThreeBitEncodingWordOp>(metadata.registers[0]), value);
               }
               else
               {
                   Memory::Slot slot = memory[effectiveAddressValue];

                   registers.set(static_cast<ERegThreeBitEncodingByteOp>(metadata.registers[0]), slot.value);
               }

               metadata.instructionOperandLayout = EInstructionOperandLayout::MemoryToRegister;
		       setNetInstructionCost(metadata);
           }
           else
           {
               if (metadata.wBit == EWBit::WordOperation)
               {
                   Memory::Slot& lowSlotRef = memory[effectiveAddressValue];
                   Memory::Slot& highSlotRef = memory[effectiveAddressValue + 1];

                   uint16_t value = registers.get(static_cast<ERegThreeBitEncodingWordOp>(metadata.registers[0]));
                   lowSlotRef.value = static_cast<uint8_t>(value & 0x00ff);
                   highSlotRef.value = static_cast<uint8_t>((value >> 8) & 0x00ff);
               }
               else
               {
                   Memory::Slot& slotRef = memory[effectiveAddressValue];

                   uint8_t value = registers.get(static_cast<ERegThreeBitEncodingByteOp>(metadata.registers[0]));
                   slotRef.value = value;
               }

               metadata.instructionOperandLayout = EInstructionOperandLayout::RegisterToMemory;
		       setNetInstructionCost(metadata);
           }
       }

       break;
   }
   case (EInstruction::MovSegRegToRegMem) :
   case (EInstruction::MovRegMemToSegReg) :
   {
       if (metadata.instructionFormat == EInstructionFormat::SegRegReg)
       {
           if (metadata.dBit == EDBit::RegFieldDestRMFieldSrc) // writing to seg_register(regField)
           {
               uint16_t value = registers.get(static_cast<ERegThreeBitEncodingWordOp>(metadata.rmField));
               registers.set(static_cast<ESegmentRegisterEncoding>(metadata.regField), value);
           }
           else // writing from seg_register(regField)
           {
               uint16_t value = registers.get(static_cast<ESegmentRegisterEncoding>(metadata.regField));
               registers.set(static_cast<ERegThreeBitEncodingWordOp>(metadata.rmField), value);
           }
       }

       break;
   }
   case (EInstruction::AddRegMemWithRegToEither) :
   case (EInstruction::SubRegMemWithRegToEither) :
   case (EInstruction::CmpRegMemAndReg) :
   {
       if (metadata.instructionFormat == EInstructionFormat::RegReg)
       {
           if (metadata.dBit == EDBit::RegFieldDestRMFieldSrc)
           {
               if (metadata.wBit == EWBit::WordOperation)
               {
                   ERegThreeBitEncodingWordOp leftHandOperandRegisterEncoding = static_cast<ERegThreeBitEncodingWordOp>(metadata.registers[0]);
                   uint16_t leftHandOperand  = registers.get(leftHandOperandRegisterEncoding);
                   uint16_t rightHandOperand = registers.get(static_cast<ERegThreeBitEncodingWordOp>(metadata.registers[1]));
                   uint16_t result = 0;
				   if (metadata.instructionString == "add")
				   {
					   result = leftHandOperand + rightHandOperand;
                       registers.setFlags(result); 
                       registers.set(leftHandOperandRegisterEncoding, result);
				   }
				   else if (metadata.instructionString == "sub")
				   {
                       result = leftHandOperand - rightHandOperand;
                       registers.setFlags(result); 
                       registers.set(leftHandOperandRegisterEncoding, result);
				   }
				   else if (metadata.instructionString == "cmp")
				   {
					   result = leftHandOperand - rightHandOperand;
                       registers.setFlags(result); 
                   }

               }
               else
               {
				   ERegThreeBitEncodingByteOp leftHandOperandRegisterEncoding = static_cast<ERegThreeBitEncodingByteOp>(metadata.registers[0]);
				   uint8_t leftHandOperand  = registers.get(leftHandOperandRegisterEncoding);
                   uint8_t rightHandOperand = registers.get(static_cast<ERegThreeBitEncodingByteOp>(metadata.registers[1]));
                   uint8_t result = 0;
				   if (metadata.instructionString == "add")
				   {
					   result = leftHandOperand + rightHandOperand;
                       registers.setFlags(result); 
                       registers.set(leftHandOperandRegisterEncoding, result);
				   }
				   else if (metadata.instructionString == "sub")
				   {
					   result = leftHandOperand - rightHandOperand;
                       registers.setFlags(result); 
                       registers.set(leftHandOperandRegisterEncoding, result);
				   }
				   else if (metadata.instructionString == "cmp")
				   {
					   result = leftHandOperand - rightHandOperand;
                       registers.setFlags(result); 
				   }

               }
           }
           else
           {
               if (metadata.wBit == EWBit::WordOperation)
               {
                   ERegThreeBitEncodingWordOp leftHandOperandRegisterEncoding = static_cast<ERegThreeBitEncodingWordOp>(metadata.registers[1]);
                   uint16_t leftHandOperand  = registers.get(leftHandOperandRegisterEncoding);
                   uint16_t rightHandOperand = registers.get(static_cast<ERegThreeBitEncodingWordOp>(metadata.registers[0]));
                   uint16_t result = 0;
				   if (metadata.instructionString == "add")
				   {
					   result = leftHandOperand + rightHandOperand;
                       registers.setFlags(result); 
                       registers.set(leftHandOperandRegisterEncoding, result);
				   }
				   else if (metadata.instructionString == "sub")
				   {
					   result = leftHandOperand - rightHandOperand;
                       registers.setFlags(result); 
                       registers.set(leftHandOperandRegisterEncoding, result);
				   }
				   else if (metadata.instructionString == "cmp")
				   {
					   result = leftHandOperand - rightHandOperand;
                       registers.setFlags(result); 
				   }

               }
               else
               {
                   ERegThreeBitEncodingByteOp leftHandOperandRegisterEncoding = static_cast<ERegThreeBitEncodingByteOp>(metadata.registers[1]);
				   uint8_t leftHandOperand  = registers.get(leftHandOperandRegisterEncoding);
                   uint8_t rightHandOperand = registers.get(static_cast<ERegThreeBitEncodingByteOp>(metadata.registers[0]));
                   uint8_t result = 0;
				   if (metadata.instructionString == "add")
				   {
					   result = leftHandOperand + rightHandOperand;
                       registers.setFlags(result); 
                       registers.set(leftHandOperandRegisterEncoding, result);
				   }
				   else if (metadata.instructionString == "sub")
				   {
					   result = leftHandOperand - rightHandOperand;
                       registers.setFlags(result); 
                       registers.set(leftHandOperandRegisterEncoding, result);
				   }
				   else if (metadata.instructionString == "cmp")
				   {
					   result = leftHandOperand - rightHandOperand;
                       registers.setFlags(result); 
				   }

               }
           }

           metadata.instructionOperandLayout = EInstructionOperandLayout::RegisterToRegister;
		   setNetInstructionCost(metadata);
       }

       break;
   }
   case (EInstruction::AddSubCmpImmToRM) :
   {
       if (metadata.instructionFormat == EInstructionFormat::RegImm)
       {
		   if (metadata.wBit == EWBit::WordOperation)
		   {
               ERegThreeBitEncodingWordOp leftHandOperandRegisterEncoding = static_cast<ERegThreeBitEncodingWordOp>(metadata.registers[1]);
			   int16_t leftHandOperand  = registers.get(leftHandOperandRegisterEncoding);
			   int16_t result = 0;
               if (metadata.sBit == ESBit::EightBitImmValue)
               {
                   int8_t rightHandOperand = static_cast<int8_t>(std::stoul(metadata.immediateValue));
                   if (metadata.instructionString == "add")
				   {
					   result = leftHandOperand + rightHandOperand;
                       registers.setFlags((uint16_t)result); 
                       registers.set(leftHandOperandRegisterEncoding, result);
				   }
				   else if (metadata.instructionString == "sub")
				   {
					   result = leftHandOperand - rightHandOperand;
                       registers.setFlags((uint16_t)result); 
                       registers.set(leftHandOperandRegisterEncoding, result);
				   }
				   else if (metadata.instructionString == "cmp")
				   {
					   result = leftHandOperand - rightHandOperand;
                       registers.setFlags((uint16_t)result); 
				   }
               }
               else
               {
                   int16_t rightHandOperand = static_cast<int16_t>(std::stoul(metadata.immediateValue));
                   if (metadata.instructionString == "add")
				   {
					   result = leftHandOperand + rightHandOperand;
                       registers.setFlags((uint16_t)result); 
                       registers.set(leftHandOperandRegisterEncoding, result);
				   }
				   else if (metadata.instructionString == "sub")
				   {
					   result = leftHandOperand - rightHandOperand;
                       registers.setFlags((uint16_t)result); 
                       registers.set(leftHandOperandRegisterEncoding, result);
				   }
				   else if (metadata.instructionString == "cmp")
				   {
					   result = leftHandOperand - rightHandOperand;
                       registers.setFlags((uint16_t)result); 
				   }
               }
		   }
		   else
		   {
			   ERegThreeBitEncodingByteOp leftHandOperandRegisterEncoding = static_cast<ERegThreeBitEncodingByteOp>(metadata.registers[1]);
			   uint8_t leftHandOperand  = registers.get(leftHandOperandRegisterEncoding);
			   uint8_t rightHandOperand = std::stoul(metadata.immediateValue);
               uint8_t result = 0;
               if (metadata.instructionString == "add")
               {
                   result = leftHandOperand + rightHandOperand;
                   registers.setFlags(result); 
                   registers.set(leftHandOperandRegisterEncoding, result);
               }
               else if (metadata.instructionString == "sub")
               {
                   result = leftHandOperand - rightHandOperand;
                   registers.setFlags(result); 
                   registers.set(leftHandOperandRegisterEncoding, result);
               }
               else if (metadata.instructionString == "cmp")
               {
                   result = leftHandOperand - rightHandOperand;
                   registers.setFlags(result); 
               }
		   }

           metadata.instructionOperandLayout = EInstructionOperandLayout::ImmediateToRegister;
		   setNetInstructionCost(metadata);
       }

       break;
   }
   case (EInstruction::JmpIfNotZero) :
   {
       if (!registers.getFlag(EFlag::Zero))
       {
           registers.modifyIp(static_cast<int8_t>(std::stoi(metadata.immediateValue)));

           metadata.instructionOperandLayout = EInstructionOperandLayout::JmpTaken;
           setNetInstructionCost(metadata);
       }
       else
       {
           metadata.instructionOperandLayout = EInstructionOperandLayout::JmpNotTaken;
           setNetInstructionCost(metadata);
       }

       break;
   }
   default:
       return;
   }
}

bool getBitsetFromInstructionByteStream(std::ifstream& file, InstructionMetadata& metadata, ByteBitset& byteBitset, bool isFirstByteOfInstruction)
{
    file.seekg(registers.getIp());
    if (file.fail()) 
    {
        return false;
    }

    char byte;
    file.read(&byte, sizeof(byte));
    if (file.fail()) 
    {
        return false;
    }

    if (file.gcount() == 0)
    {
        return false;
    }
    registers.modifyIp(1, isFirstByteOfInstruction);

    byteBitset = ByteBitset(byte);
    return true;
}

void constructImmediateValueFromOperationWidth(std::ifstream& file, InstructionMetadata& ongoingInstructionMetadata, EWBit w)
{
    if (w == EWBit::WordOperation)
	{
		ByteBitset firstByteBitset;
        getBitsetFromInstructionByteStream(file, ongoingInstructionMetadata, firstByteBitset);

		ByteBitset secondByteBitset;
        getBitsetFromInstructionByteStream(file, ongoingInstructionMetadata, secondByteBitset);

		WordBitset immediateValueBitset((secondByteBitset.to_ulong() << 8) | firstByteBitset.to_ulong());
		ongoingInstructionMetadata.immediateValue = std::to_string(immediateValueBitset.to_ulong());
	}
	else
	{
		ByteBitset firstByteBitset;
        getBitsetFromInstructionByteStream(file, ongoingInstructionMetadata, firstByteBitset);

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
    uint16_t baseValue = getEffectiveAddressBaseValue(metadata.rmField , metadata.modField);

    if (baseEquation.empty())
    {
        metadata.effectiveAddress = "[";
        metadata.effectiveAddressValue = 0;
    }
    else
    {
        metadata.effectiveAddress = "[" + baseEquation;
        metadata.effectiveAddressValue = baseValue;
    }

    char byte;
    if (disp == EBitsDisplacement::EightBitDisp)
    {
		ByteBitset firstByteBitset;
        getBitsetFromInstructionByteStream(file, metadata, firstByteBitset);

        ByteBitset effectiveAddressBitset(firstByteBitset);
        if (effectiveAddressBitset.to_ulong() != 0)
        {
            if (baseEquation.empty()) // direct_address case
            {
                metadata.effectiveAddress += std::to_string(effectiveAddressBitset.to_ulong()) + "]";
                metadata.effectiveAddressValue += effectiveAddressBitset.to_ulong();
            }
            else
            {
                if ((effectiveAddressBitset.to_ulong() & 0b10000000) > 0)
                {
                    metadata.effectiveAddress += " - " + std::to_string(-static_cast<int8_t>(effectiveAddressBitset.to_ulong())) + "]";
                    metadata.effectiveAddressValue += static_cast<int8_t>(effectiveAddressBitset.to_ulong());
                }
                else
                {
                    metadata.effectiveAddress += " + " + std::to_string(effectiveAddressBitset.to_ulong()) + "]";
                    metadata.effectiveAddressValue += effectiveAddressBitset.to_ulong();
                }

                metadata.hasDisplacement = true;
            }
        }
        else
        {
		    metadata.effectiveAddress += "]";
        }
    }
    else if (disp == EBitsDisplacement::SixteenBitDisp)
    {
		ByteBitset firstByteBitset;
        getBitsetFromInstructionByteStream(file, metadata, firstByteBitset);

		ByteBitset secondByteBitset;
        getBitsetFromInstructionByteStream(file, metadata, secondByteBitset);

		WordBitset effectiveAddressBitset((secondByteBitset.to_ulong() << 8) | firstByteBitset.to_ulong()); 
        if (effectiveAddressBitset.to_ulong() != 0)
        {
            if (baseEquation.empty()) // direct_address case
            {
                metadata.effectiveAddress += std::to_string(effectiveAddressBitset.to_ulong()) + "]";
                metadata.effectiveAddressValue += effectiveAddressBitset.to_ulong();
            }
            else
            {
                if ((effectiveAddressBitset.to_ulong() & 0x8000) > 0)
                {
                    metadata.effectiveAddress += " - " + std::to_string(-static_cast<int16_t>(effectiveAddressBitset.to_ulong())) + "]";
                    metadata.effectiveAddressValue += static_cast<int16_t>(effectiveAddressBitset.to_ulong());
                }
                else
                {
                    metadata.effectiveAddress += " + " + std::to_string(effectiveAddressBitset.to_ulong()) + "]";
                    metadata.effectiveAddressValue += effectiveAddressBitset.to_ulong();
                }

                metadata.hasDisplacement = true;
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
    int8_t testMnemonicLength = 8;
    while (bitSet.to_ulong() >= 0)
    {
        if (mnemonicToInstructionMetadata.count(bitSet.to_ulong()) &&
            testMnemonicLength == mnemonicToInstructionMetadata[bitSet.to_ulong()].mnemonicSize)
        {
            return mnemonicToInstructionMetadata[bitSet.to_ulong()];
        }

        bitSet >>= 1;
        testMnemonicLength -= 1;
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
		ByteBitset secondByteBitset;
        getBitsetFromInstructionByteStream(file, ongoingInstructionMetadata, secondByteBitset);

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

        ongoingInstructionMetadata.wBit = static_cast<EWBit>((byteBitset >> 3).to_ulong() & 0b00000001);
		constructImmediateValueFromOperationWidth(file, ongoingInstructionMetadata, ongoingInstructionMetadata.wBit);

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
		ByteBitset secondByteBitset;
        getBitsetFromInstructionByteStream(file, ongoingInstructionMetadata, secondByteBitset);

		decodeSecondInstructionByte(secondByteBitset, file, ongoingInstructionMetadata);

		constructImmediateValueFromOperationWidth(file, ongoingInstructionMetadata, ongoingInstructionMetadata.wBit);

		break;
	}
    case (EInstruction::MovSegRegToRegMem) :
    case (EInstruction::MovRegMemToSegReg) :
	{
		ongoingInstructionMetadata.instructionFormat = EInstructionFormat::SegMemReg;

        // d_bit is implied
        if (ongoingInstructionMetadata.instruction == EInstruction::MovRegMemToSegReg)
        {
            ongoingInstructionMetadata.dBit = EDBit::RegFieldDestRMFieldSrc;
        }
        else
        {
		    ongoingInstructionMetadata.dBit = EDBit::RegFieldSrcRMFieldDest;
        }

        // w_bit is implied
        ongoingInstructionMetadata.wBit = EWBit::WordOperation;

		// needs second byte
		ByteBitset secondByteBitset;
        getBitsetFromInstructionByteStream(file, ongoingInstructionMetadata, secondByteBitset);

		decodeSecondInstructionByte(secondByteBitset, file, ongoingInstructionMetadata);

        if (ongoingInstructionMetadata.modField == EModField::RegisterMode)
        {
		    ongoingInstructionMetadata.instructionFormat = EInstructionFormat::SegRegReg;
        }

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
		ByteBitset secondByteBitset;
        getBitsetFromInstructionByteStream(file, ongoingInstructionMetadata, secondByteBitset);

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

RegisterFile::RegisterFile(uint16_t initialValue)
{
    file.resize(ERegThreeBitEncodingWordOp::RegFieldsWordOperationCount);
    segFile.resize(ESegmentRegisterEncoding::SegmentRegisterCount);

    for (RegisterEntry& r : file)
    {
        r.prevValue = initialValue;
        r.value = initialValue;
        r.isDirty = false;
    }

    for (RegisterEntry& r : segFile)
    {
        r.prevValue = initialValue;
        r.value = initialValue;
        r.isDirty = false;
    }

    ip.beforeFetchingInstructionBytes = std::streampos(0);
    ip.beforeStartingInstructionExecution = std::streampos(0);

    setFlag(EFlag::Carry, false);
    setFlag(EFlag::Signed, false);
    setFlag(EFlag::Zero, false);
}

uint8_t RegisterFile::get(ERegThreeBitEncodingByteOp r)
{
    uint8_t fileIndex = (uint8_t)r % 4;

    if (getGeneralPurposeRegisterAccess(r) == EGeneralPurposeRegisterAccess::LowByteAccess)
    {
        return static_cast<uint8_t>(file[fileIndex].value & 0x00ff);
    }
    else
    {
        return static_cast<uint8_t>((file[fileIndex].value >> 8) & 0x00ff);
    }
}

uint16_t RegisterFile::get(ERegThreeBitEncodingWordOp r)
{
    return file[(uint8_t)r].value;
}

uint16_t RegisterFile::get(ESegmentRegisterEncoding r)
{
    return segFile[(uint8_t)r].value;
}

void RegisterFile::set(ERegThreeBitEncodingByteOp r, uint8_t value)
{
    uint8_t fileIndex = (uint8_t)r % 4;

    file[fileIndex].prevValue = file[fileIndex].value;
    if (getGeneralPurposeRegisterAccess(r) == EGeneralPurposeRegisterAccess::HighByteAccess) // ah, bh, ch, dh
    {
        file[fileIndex].value &= 0x00ff;
        file[fileIndex].value |= (WordBitset(value).to_ulong() << 8);
    }
    else // al, bl, cl, dl
    {
        file[fileIndex].value &= 0xff00;
        file[fileIndex].value |= value;
    }

    file[fileIndex].isDirty = true;
}

void RegisterFile::set(ERegThreeBitEncodingWordOp r, uint16_t value)
{
    file[(uint8_t)r].prevValue = file[(uint8_t)r].value;
    file[(uint8_t)r].value = value;
    file[(uint8_t)r].isDirty = true;
}

void RegisterFile::set(ESegmentRegisterEncoding r, uint16_t value)
{
    segFile[(uint8_t)r].prevValue = segFile[(uint8_t)r].value;
    segFile[(uint8_t)r].value = value;
    segFile[(uint8_t)r].isDirty = true;
}

std::streampos RegisterFile::getIp()
{
    return ip.beforeStartingInstructionExecution;
}

void RegisterFile::setIp(std::streampos value, bool isFirstByteOfInstruction)
{
    if (isFirstByteOfInstruction)
    {
        ip.beforeFetchingInstructionBytes = ip.beforeStartingInstructionExecution;
    }
    ip.beforeStartingInstructionExecution = value;

    ip.isDirty = true;
}

void RegisterFile::modifyIp(int16_t value, bool isFirstByteOfInstruction)
{
    if (isFirstByteOfInstruction)
    {
        ip.beforeFetchingInstructionBytes = ip.beforeStartingInstructionExecution;
    }
    ip.beforeStartingInstructionExecution += value;

    ip.isDirty = true;
}

bool RegisterFile::getFlag(EFlag f) 
{ 
    return (((flags & f) > 0) ? 1 : 0); 
}

void RegisterFile::setFlag(EFlag f, bool v) 
{ 
    ((v) ? flags |= f : flags &= ~f); 
}

void RegisterFile::setFlags(uint16_t result)
{
    if (result == 0)
    {
        setFlag(EFlag::Zero, true);
    }
    else
    {
        setFlag(EFlag::Zero, false);
    }

    if ((result & 0x8000) > 0)
    {
        setFlag(EFlag::Signed, true);
    }
    else
    {
        setFlag(EFlag::Signed, false);
    }
}

void RegisterFile::setFlags(uint8_t result)
{
    if (result == 0)
    {
        setFlag(EFlag::Zero, true);
    }
    else
    {
        setFlag(EFlag::Zero, false);
    }

    if ((result & 0x80) > 0)
    {
        setFlag(EFlag::Signed, true);
    }
    else
    {
        setFlag(EFlag::Signed, false);
    }
}

std::stringstream RegisterFile::getFlagsStream()
{
    std::stringstream s;
    s << "flags=[";

    if (getFlag(EFlag::Zero))
    {
        s << "Z";
    }

    if (getFlag(EFlag::Carry))
    {
        s << "C";
    }

    if (getFlag(EFlag::Signed))
    {
        s << "S";
    }
    s << "]; ";

    return s;
}

std::stringstream RegisterFile::getAllRegisterFileStream(bool dirtyOnly)
{
    std::stringstream s;
    for (uint8_t index = 0; index < file.size(); ++index)
    {
        RegisterEntry& r = file[index];
        if (dirtyOnly)
        {
            if (r.isDirty)
            {
                s << getRegNameFromThreeBitEncodingWordOp(static_cast<ERegThreeBitEncodingWordOp>(index)) << "="
                    << STREAM_WORD(r.prevValue) << "->" << STREAM_WORD(r.value) << "; ";
                r.isDirty = false;
            }
        }
        else
        {
            s << '\t' << getRegNameFromThreeBitEncodingWordOp(static_cast<ERegThreeBitEncodingWordOp>(index)) << "="
                << STREAM_WORD(r.value) << '\n';
        }
    }

    for (uint8_t index = 0; index < segFile.size(); ++index)
    {
        RegisterEntry& r = segFile[index];
        if (dirtyOnly)
        {
            if (r.isDirty)
            {
                s << getSegmentRegisterEncoding(static_cast<ESegmentRegisterEncoding>(index)) << "="
                    << STREAM_WORD(r.prevValue) << "->" << STREAM_WORD(r.value) << "; ";
                r.isDirty = false;
            }
        }
        else
        {
            s << '\t' << getSegmentRegisterEncoding(static_cast<ESegmentRegisterEncoding>(index)) << "="
                << STREAM_WORD(r.value) << '\n';
        }
    }

    // ip
    if (dirtyOnly)
    {
        if (ip.isDirty)
        {
            s << "ip=" << STREAM_WORD(ip.beforeFetchingInstructionBytes) << "->" << STREAM_WORD(ip.beforeStartingInstructionExecution) << "; ";
            ip.isDirty = false;
        }
    }
    else
    {
        s << '\n';
        s << '\t' << "ip=" << STREAM_WORD(ip.beforeStartingInstructionExecution) << '\n';
    }

    return s;
}

std::stringstream InstructionMetadata::getInstructionCostStream()
{
    std::stringstream s;

    if (instructionBaseCostMap.count(instructionString))
    {
        s << "cost : base=" << instructionBaseCostMap[instructionString][instructionOperandLayout] 
            << " ea=" << effectiveAddressCost << " ";
    }
    else
    {
        s << "cost : base=" << 0 << " ea=" << effectiveAddressCost << " ";
    }

    if (hasDisplacement)
    {
        s << "disp+=4" << "; ";
    }

    return s;
}

void executePartOne(int argc, char* argv[])
{
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

}
