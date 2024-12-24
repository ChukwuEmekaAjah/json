# JSON parser
JSON parser is an implementation of the JSON RFC (https://datatracker.ietf.org/doc/html/rfc8259) in C++. It is developed as part of projects to learn the C++ programming language and it's not stress-tested for production usage or even environments where high performance is expected. 
However, if you want to get an idea on how JSON parsers work under the hood across languages, you can use this code as a starting point after reading through the RFC.

## API
The API of this JSON implementation follows the very simple API of the JSON parser in JavaScript which has only 2 public API methods. The methods are `parse` which transforms a JSON string to the relevant programming language equivalent data types and `stringify` which converts data in the programming language data types into a string that can be shared between services/processes.

`parse` : Converts a valid JSON string into a `JSONValue` type.
`stringify` : Converts a `JSONValue` type into a valid JSON string.

```C++
#include <iostream>
#include <string>

auto jsonArray = JSON::parse("[78, 29.293, \"json\", {\"name\": \"Nobody\", \"location\":null}]");
double firstArrayItem = jsonArray[0];
std::cout << firstArrayItem << "\n";
// 78

double decimalValue = jsonArray[1];
std::cout << decimalValue << "\n";
// 29.293

std::string stringItem = jsonArray[2];
std::cout << stringItem << "\n";
// json

auto jsonDictionary = jsonArray[3];
std::cout << jsonDictionary["name"].getString() << "\n";
// Nobody
std::cout << jsonDictionary["location"].getString() << "\n";
// null


JSONValue integerNumber = JSONValue("NUMBER", 78);
JSONValue decimalNumber = JSONValue("NUMBER", 29.293);
JSONValue stringValue = JSONValue("STRING", "json");
JSONValue jsonDictionary = 
map<string, JSONValue> jsonDictionary{};
jsonDictionary["name"] = JSONValue("STRING", "Nobody");
jsonDictionary["location"] =  JSONValue("NULL", "\0");
vector<JSONValue> arr {integerNumber, decimalNumber, stringValue, jsonDictionary};

std::cout << JSON::stringify(arr) << std::endl;
// [78, 29.293, "json", {"name": "Nobody", "location":null}]
```

## TODO
1. Write more tests
2. Use C++ enums to represent JSON value types
3. Extend documentation