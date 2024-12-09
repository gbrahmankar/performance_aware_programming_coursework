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

    InternalJsonRepresentation* currentScope = nullptr;
    std::unique_ptr<InternalJsonRepresentation> rootScope = nullptr;
    
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

    void createChildScope(ScopeType scopeType)
    {
		currentScope->childScope = std::make_unique<InternalJsonRepresentation>();
        currentScope->childScope->scopeType = scopeType;
		currentScope->childScope->parentScope = currentScope;
    }

    void enterChildScope()
    {
		currentScope = currentScope->childScope.get();
    }

    void enterParentScope()
    {
		currentScope = currentScope->parentScope;
    }

    void createSiblingScope()
    {
		currentScope->sibling = std::make_unique<InternalJsonRepresentation>();
        currentScope->sibling->scopeType = currentScope->scopeType;
    }

    void enterSiblingScope()
    {
		currentScope = currentScope->sibling.get();
    }

    void parseScope()
    {
		std::cout << "started parsing a scope :" << '\n';

		JsonToken token;
		getNextToken(token);
		if (token.type == TokenTypeInvalid)
		{
			return;
		}

		switch (token.type)
		{
			case (TokenTypeOpenBracket) :
			{
				if (rootScope == nullptr)
				{
					rootScope = std::make_unique<InternalJsonRepresentation>();
					rootScope->key = "base";
					currentScope = rootScope.get();

                    std::cout << "creating root_scope" << '\n';

					break;
				}

				std::cout << "creating and entering child_json_scope" << '\n';

				createChildScope(ScopeTypeJson);
				enterChildScope();
				parseScope();

				break;
			}
			case (TokenTypeOpenBrace) :
            {
				std::cout << "creating and entering child_array_scope" << '\n';

				createChildScope(ScopeTypeArray);
				enterChildScope();
				parseScope();

                break;
            }
			case (TokenTypeCloseBrace) :
			case (TokenTypeCloseBracket) :
			{
				std::cout << "entering parent_scope" << '\n';

				enterParentScope();

				break;
			}
            case (TokenTypeComma) :
            {
                if (currentScope->scopeType == ScopeTypeJson)
                {
                    std::cout << "creating and entering a sibling in a json" << '\n';
                }
                else
                {
                    std::cout << "creating and entering a sibling in an array" << '\n';
                }

				createSiblingScope();
				enterSiblingScope();
				parseScope();

                break;
            }
            case (TokenTypeColon) :
            {
                currentScope->beforeColon = false;

				std::cout << "switching from before to after a colon in a scope" << '\n';
                break;
            }
            case (TokenTypeNumber):
            case (TokenTypeString):
            {
                if (currentScope->beforeColon && currentScope->scopeType == ScopeTypeJson)
                {
                    currentScope->key = token.value;
				    std::cout << "populating key=" << token.value << '\n';
                }
                else
                {
                    currentScope->value = token.value;
				    std::cout << "populating value=" << token.value << '\n';
                }

                break;
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

		std::cout << "starting_to_parse=" << '\n' << jsonFileBuffer << '\n';
        parseScope();   

        file.close();
        return;
    }
}
