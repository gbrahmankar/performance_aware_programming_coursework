#include "../common_utils.h"
#include "haversine_distance_parser.h"

/*
{
    "person": {
        "name": "Alice",        // String
        "age": 30,             // Number
        "isEmployed": true,    // Boolean
        "address": {
            "street": "123 Maple Street",
            "city": "Wonderland",
            "zipCode": 12345
        },                     // Nested Object
        "skills": [            // Array
            "programming",
            "data analysis",
            "machine learning"
        ],
        "languages": [
            {
                "name": "English",
                "level": "fluent"
            },
            {
                "name": "Spanish",
                "level": "intermediate"
            }
        ]
    },
    "project": {
        "title": "JSON Syntax Example",
        "deadline": null,      // Null value
        "budget": 10000.50     // Floating-point Number
    },
    "company": {
        "name": "Tech Corp",
        "departments": ["HR", "Development", "Marketing"],
        "remote": false        // Boolean
    }
}
*/

namespace PartTwo
{
    std::string jsonFileBuffer;
    u64 parseIndex = 0;

    void printToken(const JsonToken& token)
    {
        switch (token.type)
        {
			case(TokenType::TokenTypeBool) :
			{
                std::cout << "tokentype=bool" << '\n';
				break;
			}
			case(TokenType::TokenTypeOpenBrace) :
			{
                std::cout << "tokentype=open_brace" << '\n';
				break;
			}
			case(TokenType::TokenTypeCloseBrace) :
			{
                std::cout << "tokentype=close_brace" << '\n';
				break;
			}
			case(TokenType::TokenTypeOpenBracket) :
			{
                std::cout << "tokentype=open_bracket" << '\n';
				break;
			}
			case(TokenType::TokenTypeCloseBracket) :
			{
                std::cout << "tokentype=close_bracket" << '\n';
				break;
			}
			case(TokenType::TokenTypeNumber) :
			{
                std::cout << "tokentype=number value=" << token.value << '\n';
				break;
			}
			case(TokenType::TokenTypeString) :
			{
                std::cout << "tokentype=string value=" << token.value << '\n';
				break;
			}
			case(TokenType::TokenTypeComma) :
			{
                std::cout << "tokentype=comma" << '\n';
				break;
			}
			case(TokenType::TokenTypeColon) :
			{
                std::cout << "tokentype=colon" << '\n';
				break;
			}
        }

    }

    void getNextToken(JsonToken& token)
    {
        while (parseIndex < jsonFileBuffer.length())
        {
            if (!std::isspace(jsonFileBuffer[parseIndex])) 
            {
                break;
            }

            parseIndex += 1;
        }

        if (parseIndex >= jsonFileBuffer.length())
        {
            token.type = TokenTypeInvalid;
            return;
        }

        switch(jsonFileBuffer[parseIndex])
        {
			case(':') : 
			{
				token.type = TokenTypeColon;

				parseIndex += 1;
                break;
			}
            case(',') : 
			{
				token.type = TokenTypeComma;

				parseIndex += 1;
                break;
			}
			case('{') :
			{
				token.type = TokenTypeOpenBracket;

				parseIndex += 1;
                break;
            }
			case('}') :
			{
				token.type = TokenTypeCloseBracket;

				parseIndex += 1;
                break;
            }
			case('[') :
			{
				token.type = TokenTypeOpenBrace;

				parseIndex += 1;
                break;
            }
			case(']') :
			{
				token.type = TokenTypeCloseBrace;

				parseIndex += 1;
                break;
            }
			case('"') :
			{
				parseIndex += 1;

				u64 stringStartIndex = parseIndex;
				while (parseIndex < jsonFileBuffer.length() && 
					   jsonFileBuffer[parseIndex] != '"')
				{
					parseIndex += 1;
				}

				token.type = TokenTypeString;
				token.value = jsonFileBuffer.substr(stringStartIndex, parseIndex - stringStartIndex);

				parseIndex += 1;
				break;
			}
            case('t'):
            case('f'):
            {
                token.type = TokenTypeBool;

				while (parseIndex < jsonFileBuffer.length())
				{
					if (std::isspace(jsonFileBuffer[parseIndex])) 
					{
						break;
					}

					parseIndex += 1;
				}

                break;
            }
            case ('-') :
            case ('0') :
            case ('1') :
            case ('2') :
            case ('3') :
            case ('4') :
            case ('5') :
            case ('6') :
            case ('7') :
            case ('8') :
            case ('9') :
            {
				u64 stringStartIndex = parseIndex;
				while (parseIndex < jsonFileBuffer.length())
				{
					if (!std::isdigit(jsonFileBuffer[parseIndex]) &&
                        !(jsonFileBuffer[parseIndex] == '.') &&
                        !(jsonFileBuffer[parseIndex] == '-'))
					{
						break;
					}

					parseIndex += 1;
				}

                token.type = TokenTypeNumber;
				token.value = jsonFileBuffer.substr(stringStartIndex, parseIndex - stringStartIndex);
                
                break;
            }
            default :
            {
                std::cerr << "unidentified_character=" << jsonFileBuffer[parseIndex] << '\n';
            }
        }
    }

    void parseHaversineInput(int argc, char* argv[])
    {
        std::string inputJsonFileName = std::string(argv[3]);
        std::ifstream file(inputJsonFileName);
		if (!file.is_open()) 
		{
			std::cerr << "failed to open the file. file_name=" << argv[2] << '\n';
			return;
		}

        char c;
        while(file.get(c))
        {
            jsonFileBuffer += c;
        }

        while (true)
        {
            JsonToken token;
            getNextToken(token);
            if (token.type == TokenTypeInvalid)
            {
                break;
            }

            printToken(token);
        }

        file.close();
        return;
    }
}
