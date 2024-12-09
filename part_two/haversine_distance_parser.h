#pragma once

#include "../common_utils.h"

namespace PartTwo
{
	enum TokenType
	{
		TokenTypeOpenBrace,
		TokenTypeCloseBrace,

		TokenTypeOpenBracket,
		TokenTypeCloseBracket,

        TokenTypeNumber,
        TokenTypeString,
        TokenTypeBool,

        TokenTypeComma,

		TokenTypeCount,
		TokenTypeInvalid
	};

    struct CoordinatePair
    {
        f64 x0;
        f64 y0;

        f64 x1;
        f64 y1;
    };

    struct JsonToken
    {
        TokenType type;
        std::string value;
    };

    extern std::string jsonFileBuffer;
    extern u64 parseIndex;

    void GetNextToken(JsonToken& token);
	void parseHaversineInput(int argc, char* argv[]);
}
