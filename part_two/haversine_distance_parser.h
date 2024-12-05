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

		TokenTypeCount,
		TokenTypeInvalid
	};

	extern bool isValidJson;

	void parseHaversineInput(int argc, char* argv[]);
}
