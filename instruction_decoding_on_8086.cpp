#include <fstream>
#include <iostream>
#include <sstream>
#include <stdint.h>
#include <string>
#include <vector>

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

    char byte;
    while (file.read(&byte, sizeof(byte))) 
    {
        std::bitset<8> binary(byte);
        std::cout << binary << std::endl;
    }

    return 0;    
}
