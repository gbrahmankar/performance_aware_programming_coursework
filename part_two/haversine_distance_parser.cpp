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

    void GetNextToken(JsonToken& token)
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

        JsonToken token;
        GetNextToken(token);
        std::cout << token.value << '\n';

        file.close();
        return;
    }
}
