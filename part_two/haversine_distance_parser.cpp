#include "../common_utils.h"
#include "haversine_distance_calculator.h"
#include "haversine_distance_parser.h"

namespace PartTwo
{
    u64 parseIndex = 0;

    InternalJsonRepresentation* currentScope = nullptr;
    InternalJsonRepresentation* rootScope = nullptr;
    
    void printToken(const JsonToken& token)
    {
        switch (token.type)
        {
			case(TokenType::TokenTypeBool) :
			{
				break;
			}
			case(TokenType::TokenTypeOpenBrace) :
			{
				break;
			}
			case(TokenType::TokenTypeCloseBrace) :
			{
				break;
			}
			case(TokenType::TokenTypeOpenBracket) :
			{
				break;
			}
			case(TokenType::TokenTypeCloseBracket) :
			{
				break;
			}
			case(TokenType::TokenTypeNumber) :
			{
				break;
			}
			case(TokenType::TokenTypeString) :
			{
				break;
			}
			case(TokenType::TokenTypeComma) :
			{
				break;
			}
			case(TokenType::TokenTypeColon) :
			{
				break;
			}
        }

    }

    void getNextToken(const std::string& jsonFileBuffer, JsonToken& token)
    {
        // profile
        TimeFunction;

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
        // profile
        TimeFunction;

		currentScope->childScope = new InternalJsonRepresentation();
        currentScope->childScope->scopeType = scopeType;
		currentScope->childScope->parentScope = currentScope;
    }

    void enterChildScope()
    {
        // profile
        TimeFunction;

		currentScope = currentScope->childScope;
        if (currentScope->parentScope == nullptr)
        {
            std::cout << "warning : entering child_scope without a parent_scope" << '\n';
        }
    }

    bool enterParentScope()
    {
        // profile
        TimeFunction;

        if (currentScope)
        {
            currentScope = currentScope->parentScope;
            return true;
        }
        else
        {
            return false;
        }
    }

    void createSiblingScope()
    {
        // profile
        TimeFunction;

		currentScope->sibling = new InternalJsonRepresentation();
        currentScope->sibling->scopeType = currentScope->scopeType;
		currentScope->sibling->parentScope = currentScope->parentScope;
    }

    void enterSiblingScope()
    {
        // profile
        TimeFunction;

		currentScope = currentScope->sibling;
    }

    bool parseScope(const std::string& jsonFileBuffer)
    {
        // profile
        TimeFunction;

		JsonToken token;
		getNextToken(jsonFileBuffer, token);
		if (token.type == TokenTypeInvalid)
		{
			return false;
		}

		switch (token.type)
		{
			case (TokenTypeOpenBracket) :
			{
				if (rootScope == nullptr)
				{
					rootScope = new InternalJsonRepresentation();
					rootScope->key = "base";
                    rootScope->scopeType = ScopeTypeJson;
					currentScope = rootScope;
				}
                else
                {
                    createChildScope(ScopeTypeJson);
                    enterChildScope();
                }

				break;
			}
			case (TokenTypeOpenBrace) :
            {
				createChildScope(ScopeTypeArray);
				enterChildScope();

                break;
            }
			case (TokenTypeCloseBrace) :
            {
				enterParentScope();

				break;
			}
			case (TokenTypeCloseBracket) :
			{
				if (!enterParentScope())
                {
                    return false;
                }

				break;
			}
            case (TokenTypeComma) :
            {
                if (currentScope == nullptr)
                {
                    return false;
                }

				createSiblingScope();
				enterSiblingScope();

                break;
            }
            case (TokenTypeColon) :
            {
                currentScope->beforeColon = false;

                break;
            }
            case (TokenTypeNumber):
            case (TokenTypeString):
            {
                if (currentScope->beforeColon && currentScope->scopeType == ScopeTypeJson)
                {
                    currentScope->key = token.value;
                }
                else
                {
                    currentScope->value = token.value;
                }
                
                break;
            }
            default :
                return false;
		}

        return true;
    }

    InternalJsonRepresentation* getPairsArrayScope()
    {
        // profile
        TimeFunction;

        if (rootScope != nullptr)
        {
            return rootScope->childScope;
        }
        else
        {
            return nullptr;
        }
    }

    void freeJsonScopes(InternalJsonRepresentation* scope)
    {
        // profile
        TimeFunction;

        if (scope != nullptr)
        {
            do
            {
                InternalJsonRepresentation* sibling = scope->sibling;

                freeJsonScopes(scope->childScope);
                free(scope);

                scope = sibling;
            } while (scope != nullptr);
        }
    }

	CoordinatePair* getCoordinatePairAtIndexInArrayScope(u64 index)
    {
        // profile
        TimeFunction;

        InternalJsonRepresentation* currScope = getPairsArrayScope();
        u64 currentIndex = 0;

        while (currScope != nullptr && currentIndex < index)
        {
            currScope = currScope->sibling;
            currentIndex += 1;
        }

        if (currScope == nullptr)
        {
            return nullptr;
        }

        CoordinatePair* pair = new CoordinatePair();
        if (currentIndex == index && currScope)
        {
            currScope = currScope->childScope;
            pair->x0 = std::stod(currScope->value);
            currScope = currScope->sibling;
            pair->y0 = std::stod(currScope->value);
            currScope = currScope->sibling;
            pair->x1 = std::stod(currScope->value);
            currScope = currScope->sibling;
            pair->y1 = std::stod(currScope->value);

            return pair;
        }

        return nullptr;
    }

    CoordinatePair* getCoordinatePairFromArrayScope(InternalJsonRepresentation* currScope)
    {
        // profile
        TimeFunction;

        if (currScope != nullptr)
        {
            currScope = currScope->childScope;
        }

        if (currScope == nullptr)
        {
            return nullptr;
        }

        CoordinatePair* pair = new CoordinatePair();
        pair->x0 = std::stod(currScope->value);
        currScope = currScope->sibling;
        pair->y0 = std::stod(currScope->value);
        currScope = currScope->sibling;
        pair->x1 = std::stod(currScope->value);
        currScope = currScope->sibling;
        pair->y1 = std::stod(currScope->value);

        return pair;
    }

    void parseHaversineInput(int argc, char* argv[])
    {
		// profile
        Profiler::beginProfile();

        // block : parseHaversineInput
        {
			// profile
            TimeFunction;

            std::string jsonFileBuffer;

            // block : read_file 
            {
                // profile
                TimeBlock("read_file");

                std::string inputJsonFileName = std::string(argv[3]);
                std::ifstream file(inputJsonFileName, std::ios::binary);
                if (!file)
                {
                    std::cerr << "error: could not open the file=" << inputJsonFileName << '\n';
                    return;
                }

                jsonFileBuffer = std::string((std::istreambuf_iterator<char>(file)),
                    std::istreambuf_iterator<char>());

                file.close();
            }

            // block : parse_json 
            {
                // profile
                TimeBlock("parse_json");

                bool parseRetVal = false;
                do
                {
                    parseRetVal = parseScope(jsonFileBuffer);
                } while (parseRetVal);
            }

            f64 sum = 0;
            f64 avgSum = 0;
            u64 pairIndex = 0;

            // block : calculate_sum 
            {
                // profile
                TimeBlock("calculate_sum");

                currentScope = getPairsArrayScope();
                CoordinatePair* pair = getCoordinatePairAtIndexInArrayScope(0);
                while (pair != nullptr)
                {
                    f64 haversineDistance = ReferenceHaversine(pair->x0, pair->y0, pair->x1, pair->y1, 6372.8);
                    sum += haversineDistance;

                    if (currentScope)
                    {
                        currentScope = currentScope->sibling;
                    }

                    pairIndex += 1;
                    pair = getCoordinatePairFromArrayScope(currentScope);
                }

                f64 sumCoef = 1.0 / (f64)pairIndex;
                avgSum = sumCoef * sum;
            }
            std::cout << "haversine_avg_sum=" << STREAM_16BIT_PRECISION_FP(avgSum) << '\n';

            // block : free_allocs
            {
                // profile
                TimeBlock("free_allocs");

                freeJsonScopes(rootScope);
            }
        }

		// profile
	    Profiler::endAndPrintProfile();

        return;
    }
}
