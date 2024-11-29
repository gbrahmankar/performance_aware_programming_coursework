#pragma once

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

constexpr std::size_t BYTE = 1;
constexpr std::size_t KILOBYTE = 1024 * BYTE;
constexpr std::size_t MEGABYTE = 1024 * KILOBYTE;
constexpr std::size_t GIGABYTE = 1024 * MEGABYTE;
constexpr std::size_t TERABYTE = 1024 * GIGABYTE;
