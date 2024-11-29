#pragma once

#include "../common_utils.h"

namespace PartOne
{

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

enum EInstructionOperandLayout : uint8_t
{
	RegisterToRegister = 0,
	MemoryToRegister,
	RegisterToMemory,

	MemoryToAccumulator,
	AccumulatorToMemory,
	ImmediateToAccumulator,

	ImmediateToMemory,
    ImmediateToRegister,

    JmpTaken,
    JmpNotTaken,

    InvalidOperandLayout
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

extern std::unordered_map<ERegThreeBitEncodingByteOp, EGeneralPurposeRegisterAccess> generalPurposeRegisterAccess;

extern std::unordered_map<ERegThreeBitEncodingByteOp, std::string> byteRegisterEncodingToString;

extern std::unordered_map<ERegThreeBitEncodingWordOp, std::string> wordRegisterEncodingToString;

extern std::unordered_map<ESegmentRegisterEncoding, std::string> segmentRegisterEncodingToString;

extern std::unordered_map<std::string, std::unordered_map<EInstructionOperandLayout, uint16_t>> instructionBaseCostMap;

struct Memory
{
    struct Slot
    {
        uint8_t value;
    };

    Memory()
    {
        slots.resize(MEGABYTE);
    }

    Slot& operator[](std::size_t index) 
    {
        if (index >= slots.size()) 
        {
            throw std::out_of_range("memory index out of bounds");
        }
        return slots[index];
    }

    std::vector<Slot> slots;
};
extern Memory memory;

struct RegisterFile
{
    struct RegisterEntry
    {
        uint16_t prevValue;
        uint16_t value;
        bool isDirty;
    };

    // ip : cpu is already pointing to the instruction it wants to fetch, decode and execute. after fetching and decoding, it
    // increments ip by the number of bytes it decoded "before" executing the instruction. as a consequence, ip is already pointing 
    // to the next instruction while executing the current one.
    struct IpRegisterEntry
    {
        std::streampos beforeFetchingInstructionBytes;
        std::streampos beforeStartingInstructionExecution;
        bool isDirty;
    };

    RegisterFile(uint16_t initialValue);
    
    uint8_t get(ERegThreeBitEncodingByteOp r);
    uint16_t get(ERegThreeBitEncodingWordOp r);
    uint16_t get(ESegmentRegisterEncoding r);

    void set(ERegThreeBitEncodingByteOp r, uint8_t value);
	void set(ERegThreeBitEncodingWordOp r, uint16_t value);
    void set(ESegmentRegisterEncoding r, uint16_t value);

    std::streampos getIp();
    void setIp(std::streampos value, bool isFirstByteOfInstruction = false);
    void modifyIp(int16_t value, bool isFirstByteOfInstruction = false);

    bool getFlag(EFlag f);
    void setFlag(EFlag f, bool v);
    void setFlags(uint16_t result);
    void setFlags(uint8_t result);

    std::stringstream getFlagsStream();
    std::stringstream getAllRegisterFileStream(bool dirtyOnly = false);

private:
    std::vector<RegisterEntry> file;
    std::vector<RegisterEntry> segFile;
    IpRegisterEntry ip;

    uint8_t flags;
};
extern RegisterFile registers;

struct InstructionMetadata
{     
    InstructionMetadata()
    {}

    InstructionMetadata(EInstruction i, const std::string& is, int8_t ms) :
        instruction(i),
        instructionString(is),
        mnemonicSize(ms)
    {}

    std::stringstream getInstructionCostStream(); 

    int8_t mnemonicSize = 0;
    EInstruction instruction = EInstruction::InstructionInvalid;
    EInstructionFormat instructionFormat = EInstructionFormat::InstructionFormatInvalid;
    EInstructionOperandLayout instructionOperandLayout = EInstructionOperandLayout::InvalidOperandLayout;
    std::string instructionString = "";

    EDBit dBit = EDBit::DBitInvalid;
    EWBit wBit = EWBit::WBitInvalid;
    ESBit sBit = ESBit::SBitInvalid;

    bool hasDisplacement = false;
    EModField modField = EModField::MemModeNoDisp;
    uint8_t regField = UINT8_MAX;
    uint8_t rmField = UINT8_MAX;

    std::string effectiveAddress;
    uint16_t effectiveAddressValue;

    // if the instruction has two registers, 0_src, 1_dst. else, based on instruction_format
    std::vector<uint8_t> registers;
    std::vector<std::string> stringifiedRegisters;

    std::string immediateValue;

    // costs
    uint16_t effectiveAddressCost = 0;
    uint16_t totalInstructionCost = 0;
};

extern std::unordered_map<uint64_t, InstructionMetadata> mnemonicToInstructionMetadata;

EGeneralPurposeRegisterAccess getGeneralPurposeRegisterAccess(ERegThreeBitEncodingByteOp e);
std::string& getRegNameFromThreeBitEncodingByteOp(ERegThreeBitEncodingByteOp e);
std::string& getRegNameFromThreeBitEncodingWordOp(ERegThreeBitEncodingWordOp e);
std::string& getSegmentRegisterEncoding(ESegmentRegisterEncoding e);
std::string getEffectiveAddressBaseEquationString(uint8_t registerOrMemoryFieldValue, EModField mod);
uint16_t getEffectiveAddressBaseValue(uint8_t registerOrMemoryFieldValue, EModField mod);
uint16_t getEACostWithoutDisplacement(uint8_t registerOrMemoryFieldValue, EModField mod);
void setNetInstructionCost(InstructionMetadata& metadata);

std::stringstream getDisassStream(InstructionMetadata& metadata, bool finishWithNewline = true);

void simulateInstruction(InstructionMetadata& metadata);
bool getBitsetFromInstructionByteStream(std::ifstream& file, InstructionMetadata& metadata, ByteBitset& byteBitset, bool isFirstByteOfInstruction = false);
void constructImmediateValueFromOperationWidth(std::ifstream& file, InstructionMetadata& ongoingInstructionMetadata, EWBit w);
void constructEffectiveAddressFromMode(std::ifstream& file, InstructionMetadata& metadata, EBitsDisplacement disp);
InstructionMetadata& getInstructionMetadataFromMnemonic(ByteBitset bitSet);
void decodeSecondInstructionByte(const ByteBitset& byteBitset, std::ifstream& file, InstructionMetadata& ongoingInstructionMetadata);
void decodeFirstInstructionByte(const ByteBitset& byteBitset, std::ifstream& file, InstructionMetadata& ongoingInstructionMetadata);

void executePartOne(int argc, char* argv[]);

} // PartOne
