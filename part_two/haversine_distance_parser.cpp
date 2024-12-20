#include "../common_utils.h"
#include "haversine_distance_calculator.h"
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
    
	void printTimeElapsed(const std::string& label, u64 totalTSCElapsed, u64 begin, u64 end)
	{
		u64 elapsed = end - begin;
		f64 percent = 100.0 * ((f64)elapsed / (f64)totalTSCElapsed);
        std::cout << label << " : elapsed=" << elapsed << " | percent=" << std::setprecision(2) << percent << '\n';
	}

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
        if (currentScope->parentScope == nullptr)
        {
            std::cout << "warning : entering child_scope without a parent_scope" << '\n';
        }
    }

    bool enterParentScope()
    {
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
		currentScope->sibling = std::make_unique<InternalJsonRepresentation>();
        currentScope->sibling->scopeType = currentScope->scopeType;
		currentScope->sibling->parentScope = currentScope->parentScope;
    }

    void enterSiblingScope()
    {
		currentScope = currentScope->sibling.get();
    }

    void parseScope()
    {
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
                    std::cout << "------------------creating root_scope" << '\n';

					rootScope = std::make_unique<InternalJsonRepresentation>();
					rootScope->key = "base";
                    rootScope->scopeType = ScopeTypeJson;
					currentScope = rootScope.get();
				}
                else
                {
                    std::cout << "------------------creating and entering child_json_scope" << '\n';

                    createChildScope(ScopeTypeJson);
                    enterChildScope();
                }

				parseScope();

				break;
			}
			case (TokenTypeOpenBrace) :
            {
				std::cout << "------------------creating and entering child_array_scope" << '\n';

				createChildScope(ScopeTypeArray);
				enterChildScope();

				parseScope();

                break;
            }
			case (TokenTypeCloseBrace) :
            {
				std::cout << "------------------entering parent_scope_array" << '\n';

				enterParentScope();

                parseScope();

				break;
			}
			case (TokenTypeCloseBracket) :
			{
				std::cout << "------------------entering parent_scope_json" << '\n';

				if (!enterParentScope())
                {
				    std::cout << "------------------parsing has ended" << '\n';
                    return;
                }

                parseScope();

				break;
			}
            case (TokenTypeComma) :
            {
                if (currentScope == nullptr)
                {
                    std::cout << "wtffff" << '\n';
                }

                if (currentScope->scopeType == ScopeTypeJson)
                {
                    std::cout << "------------------creating and entering a sibling in a json" << '\n';
                }
                else
                {
                    std::cout << "------------------creating and entering a sibling in an array" << '\n';
                }

				createSiblingScope();
				enterSiblingScope();

				parseScope();

                break;
            }
            case (TokenTypeColon) :
            {
                currentScope->beforeColon = false;

				parseScope();

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
                
                parseScope();

                break;
            }
		}
    }

    InternalJsonRepresentation* getPairsArrayScope()
    {
        if (rootScope != nullptr)
        {
            return rootScope->childScope.get();
        }
        else
        {
            return nullptr;
        }
    }

	std::unique_ptr<CoordinatePair> getCoordinatePairFromArrayScope(u64 index)
    {
        InternalJsonRepresentation* pairsArrayScope = getPairsArrayScope();

        currentScope = pairsArrayScope;
        u64 currentIndex = 0;

        while (currentScope != nullptr && currentIndex < index)
        {
            currentScope = currentScope->sibling.get();
            currentIndex += 1;
        }

        if (currentScope == nullptr)
        {
            return nullptr;
        }

        std::unique_ptr<CoordinatePair> pair = std::make_unique<CoordinatePair>();
        if (currentIndex == index && currentScope)
        {
            currentScope = currentScope->childScope.get();
            pair->x0 = std::stod(currentScope->value);
            currentScope = currentScope->sibling.get();
            pair->y0 = std::stod(currentScope->value);
            currentScope = currentScope->sibling.get();
            pair->x1 = std::stod(currentScope->value);
            currentScope = currentScope->sibling.get();
            pair->y1 = std::stod(currentScope->value);

            return pair;
        }

        return nullptr;
    }

    void parseHaversineInput(int argc, char* argv[])
    {
        // profile
        u64 timeBegin = Profiler::ReadCPUTimer();
        u64 timeEnd = 0;
        u64 timeReadFile = 0;
        u64 timeParseScope = 0;
        u64 timeComputeSum = 0;

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

        file.close();

        // profile
        timeReadFile = Profiler::ReadCPUTimer();

		std::cout << "starting_to_parse=" << '\n' << jsonFileBuffer << '\n';
        parseScope();

        // profile
        timeParseScope = Profiler::ReadCPUTimer();

        f64 sum = 0;

        u64 pairIndex = 0;
        std::unique_ptr<CoordinatePair> pair = getCoordinatePairFromArrayScope(0);
        while (pair != nullptr)
        {
            f64 haversineDistance = ReferenceHaversine(pair->x0, pair->y0, pair->x1, pair->y1, 6372.8);

            std::cout << "pair_index=" << pairIndex 
                << ", [x0 = " << STREAM_16BIT_PRECISION_FP(pair->x0) 
                << ", y0 = " << STREAM_16BIT_PRECISION_FP(pair->y0)
                << " | x1=" << STREAM_16BIT_PRECISION_FP(pair->x1)
                << ", y1=" << STREAM_16BIT_PRECISION_FP(pair->y1) << "] -" 
                << "- [d=" << STREAM_16BIT_PRECISION_FP(haversineDistance) << "]" << '\n';

            sum += haversineDistance;

            pairIndex += 1;
            pair = getCoordinatePairFromArrayScope(pairIndex);
        }
        
        f64 sumCoef = 1.0 / (f64)pairIndex;
        f64 avgSum = sumCoef * sum;

        // profile
        timeComputeSum = Profiler::ReadCPUTimer();

        std::cout << "haversine_avg_sum=" << STREAM_16BIT_PRECISION_FP(avgSum);

        // profile
        timeEnd = Profiler::ReadCPUTimer();

        u64 totalTimeElapsed = timeEnd - timeBegin;
        u64 cPUFreq = Profiler::estimateCPUFrequency();
        if(cPUFreq)
        {
            std::cout << "total_time=" << std::setprecision(4) << 1000.0 * (f64)totalTimeElapsed / (f64)cPUFreq << "ms" << " | cpu_freq=" << cPUFreq << '\n';;
        }

        printTimeElapsed("read_file", totalTimeElapsed, timeBegin, timeReadFile);
        printTimeElapsed("parse", totalTimeElapsed, timeReadFile, timeParseScope);
        printTimeElapsed("compute_sum", totalTimeElapsed, timeParseScope, timeComputeSum);

        return;
    }
}
