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
        TokenTypeColon,

		TokenTypeCount,
		TokenTypeInvalid
	};

    enum ScopeType
    {
        ScopeTypeArray,
        ScopeTypeJson,

        ScopeTypeCount,
        ScopeTypeInvalid
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
        TokenType type = TokenTypeInvalid;
        std::string value;
    };

    struct InternalJsonRepresentation
    {
        bool beforeColon = true;
        ScopeType scopeType = ScopeTypeInvalid;

        std::string key;
        std::string value;

        std::unique_ptr<InternalJsonRepresentation> sibling = nullptr;
        std::unique_ptr<InternalJsonRepresentation> childScope = nullptr;
        InternalJsonRepresentation* parentScope = nullptr;
    };

    extern std::string jsonFileBuffer;
    extern u64 parseIndex;

    void printToken(const JsonToken& token);
    void getNextToken(JsonToken& token);
	void parseHaversineInput(int argc, char* argv[]);

    void createChildScope(ScopeType scopeType);
    void enterChildScope();
    void enterParentScope();

    void createSiblingScope();
    void enterSiblingScope();

    void parseScope();
}
