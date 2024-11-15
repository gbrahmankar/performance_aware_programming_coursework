#include <bitset>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdint.h>
#include <string>
#include <unordered_map>
#include <vector>

#define STREAM_BYTE(X) "0x" << std::hex << std::setw(2) << std::setfill('0') << unsigned(X)
#define STREAM_WORD(X) "0x" << std::hex << std::setw(4) << std::setfill('0') << X

using NibbleBitset = std::bitset<4>;
using ByteBitset = std::bitset<8>;
using WordBitset = std::bitset<16>;

using MnemonicBitset = std::bitset<8>;

enum EInstruction
{
    /* 
    mov_regmem_to_from_reg_layout : [[100010][1b_d][1b_w]] [[2b_mod][3b_reg][3b_r/m]] [8b_disp_low] [8b_disp_high]
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
    mov_regmem_to_segment_reg : [10001110] [[2b_mod][0][2b_seg_reg][3b_r/m]] [8b_disp_low] [8b_disp_high]
    */
    MovRegMemToSegReg,
    /* 
    mov_segment_reg_to_regmem : [10001100] [[2b_mod][0][2b_seg_reg][3b_r/m]] [8b_disp_low] [8b_disp_high]
    */
    MovSegRegToRegMem,
    /*
    add_reg_mem_with_reg_to_either : [[000000][1b_d][1b_w]] [[2b_mod][3b_reg][3b_r/m]] [8b_disp_low] [8b_disp_high]
    */
    AddRegMemWithRegToEither,
    /*
    add_imm_to_acc : [[0000010][1b_w]] [8b_data_low] [8b_data_high]
    */
    AddImmToAcc,
    /*
    add_imm_to_rm : [[100000][1b_s][1b_w]] [[2b_mod][000][3b_r/m]] [8b_disp_low] [8b_disp_high] [8b_data_low] [8b_data_high]
    sub_imm_from_rm : [[100000][1b_s][1b_w]] [[2b_mod][101][3b_r/m]] [8b_disp_low] [8b_disp_high] [8b_data_low] [8b_data_high]
    cmp_imm_from_rm : [[100000][1b_s][1b_w]] [[2b_mod][111][3b_r/m]] [8b_disp_low] [8b_disp_high] [8b_data_low] [8b_data_high]
    */
    AddSubCmpImmToRM,
    /*
    sub_reg_mem_with_reg_to_either : [[001010][1b_d][1b_w]] [[2b_mod][3b_reg][3b_r/m]] [8b_disp_low] [8b_disp_high]
    */
    SubRegMemWithRegToEither,
    /*
    sub_imm_from_acc : [[0010110][1b_w]] [8b_data_low] [8b_data_high]
    */
    SubImmFromAcc,
    /*
    cmp_reg_mem_with_reg : [[001110][1b_d][1b_w]] [[2b_mod][3b_reg][3b_r/m]] [8b_disp_low] [8b_disp_high]
    */
    CmpRegMemAndReg,
    /*
    cmp_imm_with_acc : [[0011110][1b_w]] [8b_data_low] [8b_data_high]
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

    SegRegReg,
    SegMemReg,

    RegImm,
    MemImm,

    Imm,

    InstructionFormatInvalid
};

enum EFlag
{
    Zero = (1 << 0),
    Carry = (1 << 1),
    Signed = (1 << 2)
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

    RegFieldsByteOperationCount,
    RegFieldsByteOperationInvalid
};

enum EGeneralPurposeRegisterAccess
{
    HighByteAccess,
    LowByteAccess,
    GeneralPurposeRegisterAccessInvalid
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

    RegFieldsWordOperationCount,
    RegFieldsWordOperationInvalid
};

enum ESegmentRegisterEncoding : uint8_t
{
    es = 0,
    cs,
    ss,
    ds,

    SegmentRegisterCount,
    SegmentRegisterInvalid
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

std::unordered_map<ESegmentRegisterEncoding, std::string> segmentRegisterEncodingToString =
{
    { ESegmentRegisterEncoding::es, "es" },
    { ESegmentRegisterEncoding::cs, "cs" },
    { ESegmentRegisterEncoding::ss, "ss" },
    { ESegmentRegisterEncoding::ds, "ds" }
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

struct InstructionMetadata
{
    InstructionMetadata()
    {}

    InstructionMetadata(EInstruction i, const std::string& is, int8_t ms) :
        instruction(i),
        instructionString(is),
        mnemonicSize(ms)
    {}

    int8_t mnemonicSize = 0;
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

struct RegisterFile
{
    struct RegisterEntry
    {
        uint16_t prevValue;
        uint16_t value;
        bool isDirty;
    };

    RegisterFile(uint16_t initialValue)
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

        setFlag(EFlag::Carry, false);
        setFlag(EFlag::Signed, false);
        setFlag(EFlag::Zero, false);
    }

    uint8_t get(ERegThreeBitEncodingByteOp r)
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

    uint16_t get(ERegThreeBitEncodingWordOp r)
    {
        return file[(uint8_t)r].value;
    }

    uint16_t get(ESegmentRegisterEncoding r)
    {
        return segFile[(uint8_t)r].value;
    }

    void set(ERegThreeBitEncodingByteOp r, uint8_t value)
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

	void set(ERegThreeBitEncodingWordOp r, uint16_t value)
	{
        file[(uint8_t)r].prevValue = file[(uint8_t)r].value;
        file[(uint8_t)r].value = value;
        file[(uint8_t)r].isDirty = true;
    }

    void set(ESegmentRegisterEncoding r, uint16_t value)
	{
        segFile[(uint8_t)r].prevValue = segFile[(uint8_t)r].value;
        segFile[(uint8_t)r].value = value;
        segFile[(uint8_t)r].isDirty = true;
    }

    bool getFlag(EFlag f) 
    { 
        return (((flags & f) > 0) ? 1 : 0); 
    }

    void setFlag(EFlag f, bool v) 
    { 
        ((v) ? flags |= f : flags &= ~f); 
    }

    void setFlags(uint16_t result)
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

    void setFlags(uint8_t result)
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

    std::stringstream getFlagsStream()
    {
        std::stringstream s;
        s << "flags : [";

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
        s << "]";

        return s;
    }

    std::stringstream getAllRegisterFileStream(bool dirtyOnly = false)
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

        return s;
    }

    std::vector<RegisterEntry> file;
    std::vector<RegisterEntry> segFile;

    uint8_t flags;
};
RegisterFile registers(0x0000);

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

std::stringstream getDisassStream(InstructionMetadata& metadata, bool finishWithNewline = true)
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
    {
    case (EInstructionFormat::Imm):
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

void simulateInstruction(const InstructionMetadata& metadata)
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

       break;
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
				   }
				   else if (metadata.instructionString == "sub")
				   {
					   result = leftHandOperand - rightHandOperand;
				   }
				   else if (metadata.instructionString == "cmp")
				   {
                       registers.setFlags(result); 
					   break;
                   }

                   registers.setFlags(result); 
                   registers.set(leftHandOperandRegisterEncoding, result);
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
				   }
				   else if (metadata.instructionString == "sub")
				   {
					   result = leftHandOperand - rightHandOperand;
				   }
				   else if (metadata.instructionString == "cmp")
				   {
                       registers.setFlags(result); 
					   break;
				   }

                   registers.setFlags(result); 
				   registers.set(leftHandOperandRegisterEncoding, result);
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
				   }
				   else if (metadata.instructionString == "sub")
				   {
					   result = leftHandOperand - rightHandOperand;
				   }
				   else if (metadata.instructionString == "cmp")
				   {
                       registers.setFlags(result); 
					   break;
				   }

                   registers.setFlags(result); 
                   registers.set(leftHandOperandRegisterEncoding, result);
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
				   }
				   else if (metadata.instructionString == "sub")
				   {
					   result = leftHandOperand - rightHandOperand;
				   }
				   else if (metadata.instructionString == "cmp")
				   {
                       registers.setFlags(result); 
					   break;
				   }

                   registers.setFlags(result); 
				   registers.set(leftHandOperandRegisterEncoding, result);
               }
           }
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
				   }
				   else if (metadata.instructionString == "sub")
				   {
					   result = leftHandOperand - rightHandOperand;
				   }
				   else if (metadata.instructionString == "cmp")
				   {
                       registers.setFlags((uint16_t)result); 
					   break;
				   }
               }
               else
               {
                   int16_t rightHandOperand = static_cast<int16_t>(std::stoul(metadata.immediateValue));
                   if (metadata.instructionString == "add")
				   {
					   result = leftHandOperand + rightHandOperand;
				   }
				   else if (metadata.instructionString == "sub")
				   {
					   result = leftHandOperand - rightHandOperand;
				   }
				   else if (metadata.instructionString == "cmp")
				   {
                       registers.setFlags((uint16_t)result); 
					   break;
				   }
               }

               registers.setFlags((uint16_t)result); 
			   registers.set(leftHandOperandRegisterEncoding, result);
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
               }
               else if (metadata.instructionString == "sub")
               {
                   result = leftHandOperand - rightHandOperand;
               }
               else if (metadata.instructionString == "cmp")
               {
                   registers.setFlags(result); 
                   break;
               }

               registers.setFlags(result); 
               registers.set(leftHandOperandRegisterEncoding, result);
		   }
       }

       break;
   }
   default:
       return;
   }
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
		char byte;
		file.read(&byte, sizeof(byte));
		ByteBitset secondByteBitset(byte);
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
		char byte;
		file.read(&byte, sizeof(byte));
		ByteBitset secondByteBitset(byte);
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

        if (argc >= 3 && std::string(argv[2]) == "simulate")
        {
            simulateInstruction(ongoingInstructionMetadata);
            std::cout << getDisassStream(ongoingInstructionMetadata, false).str() 
                << registers.getAllRegisterFileStream(true).str() 
                << registers.getFlagsStream().str() << '\n';
        }
        else
        {
            std::cout << getDisassStream(ongoingInstructionMetadata, true).str();
        }
    }

    std::cout << "Final registers:" << '\n';
    std::cout << registers.getAllRegisterFileStream(false).str();

    return 0;    
}
