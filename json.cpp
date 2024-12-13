#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <tuple>

using namespace std;

class JSONToken;

class JSONToken {
  public:
    string tokenType;
    string tokenValue;
    JSONToken(string t_type, string t_value){
      tokenType = t_type;
      tokenValue = t_value;
    }
};

class JSONValue {
  public:
    string data_type;
    
    double d_value; // represents doubles
    bool b_value; // represents booleans
    string str_value; // represents strings
    vector<JSONValue> arr{};
    map<string, JSONValue> dict;
    JSONValue* c_value; // represents complex types

    // string instance
    JSONValue(string d_type, const char* d_value) {
      data_type = d_type;
      str_value = string(d_value);
    }

    JSONValue(string d_type, string d_value) {
      data_type = d_type;
      str_value = d_value;
    }

    JSONValue(string d_type, double value) {
      data_type = d_type;
      d_value = value;
    }

    JSONValue(string d_type, bool value) {
      data_type = d_type;
      b_value = value;
    }

    JSONValue(string d_type, vector<JSONValue> d_value) {
      data_type = d_type;
      arr = d_value;
    }

    JSONValue(string d_type, map<string, JSONValue> d_value) {
      data_type = d_type;
      dict = d_value;
    }

    JSONValue(){
      data_type = "NULL";
    }
  
    JSONValue operator [](int position){
      if(data_type != "ARRAY"){
        throw runtime_error("Cannot use int index in non-array object");
      }
      if(position >= arr.size()){
        throw runtime_error("Array index position greater than array size");
      }
      return arr[position];
    }

    JSONValue operator [](string key){
      if(data_type != "OBJECT"){
        throw runtime_error("Cannot use int index in non-dict object");
      }
      if(!dict.contains(key)){
        throw runtime_error("Key does not exist in dictionary object");
      }
      return dict[key];
    }

    string getString(){
      return str_value;
    }

    double getNumber(){
      return d_value;
    }

    bool getBoolean(){
      return b_value;
    }

    string getType(){
      return data_type;
    }
};

class JSON {

  public:
    string stringify(JSONValue value) {
      if(value.getType() == "STRING"){
        return "\"" + value.getString() + "\"";
      } else if(value.getType() == "NUMBER"){
        if((value.getNumber() - int(value.getNumber())) == 0){
          return to_string(int(value.getNumber()));
        }
        return to_string(round(value.getNumber() * 10000.0) / 10000.0);
      } else if(value.getType() == "BOOLEAN"){
        return value.getBoolean() == true ? "true" : "false";
      } else if(value.getType() == "NULL"){
        return "null";
      } else if(value.getType() == "ARRAY"){
        string result = "[";
        
        for(int i = 0; i < value.arr.size(); i++){
          if(i != 0){
            result += ",";
          }
          result += stringify(value[i]);
        }
        result += "]";
        return result;
      } else {
        string result = "{";
        int counter = 0;
        for(auto const& [key,val] : value.dict){
          if(counter != 0){
            result += ",";
          }
          result += "\"" + key + "\"";
          result += " : ";
          result += stringify(val);
          counter += 1;
        }
        result += "}";
        return result;
      }
    };

    JSONValue parse(string value){
      if(value.length() == 0){
        throw runtime_error("Invalid JSON string provided.");
      }
      auto tokens = scanTokens(value);

      // for(int i = 0; i < tokens.size(); i++){
      //   std::cout<< "Token type is: " << tokens[i].tokenType << " and value is " << tokens[i].tokenValue << std::endl;
      // }
      // JSONValue t = parseTokens(tokens, 0);
      // std::cout << "Number type is " << t.getNumber() <<  " is it " << endl;
      int start = 0;
      auto result = parseTokens(tokens, start);

      if(start < tokens.size() - 1){
        throw runtime_error("Invalid item " +  tokens[start].tokenValue + " in json string ");
      }
      return result;
    };

  private:
    bool isNumeric(char c){
      return c >= '0' && c <= '9';
    }

    bool isAlpha(char c){
      return (c >= 'a' && c <= 'z');
    }

    tuple<string, int> consumeString(int position, string text){
      string value {""};
      while(text[position] != '"'){
        value += text[position];
        position += 1;
      }
      return {value, position + 1};
    }

    tuple<string, int> consumeIdentifier(int position, string text){
      string value {""};
      while(isAlpha(text[position])){
        value += text[position];
        position += 1;
      }

      if(value == "true" || value == "false" || value == "null"){
        return {value, position };
      }
      
      throw runtime_error("Invalid identifier " + value + " provided");
    }

    tuple<string, int> consumeNumeric(int position, string text){
      string value {""};
      while(isNumeric(text[position])){
        value += text[position];
        position += 1;
      }
      // consume decimal part of a number
      if(text[position] == '.'){
        position += 1;
        value += '.';
        while(isNumeric(text[position])){
          value += text[position];
          position += 1;
        }
      }

      return {value, position};
    }

    vector<JSONToken> scanTokens(string text){
      vector<JSONToken> tokens {};
      /*
        types:
        number = 13.32
        null = null
        boolean = true | false
        string = "dajfaj"
        array = ["ajahd"]
        object = {"ajah": "cute", "age": 23}
      */
      for(int i = 0; i < text.length(); ){
        switch (text[i]){
        case '{': // opening brace for dicts
          /* code */
          tokens.push_back(JSONToken("OPENING_BRACE", "{"));
          i += 1;
          break;
        case '}': // closing brace for dicts
          tokens.push_back(JSONToken("CLOSING_BRACE", "}"));
          i += 1;
          break;
        case '[': // opening square brackets for arrays
          tokens.push_back(JSONToken("OPENING_SQUARE_BRACKET", "["));
          i += 1;
          break;
        case ']': // closing square brackets for arrays
          tokens.push_back(JSONToken("CLOSING_SQUARE_BRACKET", "]"));
          i += 1;
          break;
        case ',': // closing square brackets for arrays
          tokens.push_back(JSONToken("COMMA", ","));
          i += 1;
          break;
        case '\n':
          i += 1;
          break;
        case '\t':
          i += 1;
          break;
        case ' ':
          i += 1;
          break;
        case '-':
        {
          if(isNumeric(text[i+1])){
            auto response = consumeNumeric(i+1, text);
            i = get<int>(response);
            tokens.push_back(JSONToken("NUMBER", "-" + get<string>(response)));
          }
          break;
        }
        case ':': // colon for key value pair separation
          tokens.push_back(JSONToken("COLON", ":"));
          i += 1;
          break;
        case '"':
        {
          auto response = consumeString(i+1, text);
          i = get<int>(response);
          tokens.push_back(JSONToken("STRING", get<string>(response)));
          break;
        } // opening and closing quotation marks for strings
        default:
        {
          if(isNumeric(text[i])){
            auto response = consumeNumeric(i, text);
            i = get<int>(response);
            tokens.push_back(JSONToken("NUMBER", get<string>(response)));
          } else if(isAlpha(text[i])){
            auto response = consumeIdentifier(i, text);
            i = get<int>(response);
            string value = get<string>(response);
            tokens.push_back(JSONToken( value == "null" ? "NULL" : "BOOLEAN", value));
          } 
          break;
        }
        }
      }
      return tokens;
    };

    JSONValue parseTokens(vector<JSONToken> tokens, int& i){
      
      JSONToken token = tokens[i];

      if(token.tokenType == "STRING"){
        return JSONValue(token.tokenType, token.tokenValue);
      } else if(token.tokenType == "NUMBER"){
        return JSONValue(token.tokenType, std::stod(token.tokenValue));
      } else if(token.tokenType == "BOOLEAN"){
        return JSONValue(token.tokenType, token.tokenValue == "true" ? true : false);
      } else if(token.tokenType == "NULL"){
        return JSONValue();
      } else if(token.tokenType == "OPENING_SQUARE_BRACKET"){
        // beginning of array
        vector<JSONValue> items {};
        i += 1;
        if(tokens[i].tokenType == "CLOSING_SQUARE_BRACKET"){
          return JSONValue("ARRAY", items);
        }
        
        JSONValue item = parseTokens(tokens, i);
        i += 1;
        items.push_back(item);
        while(tokens[i].tokenType != "CLOSING_SQUARE_BRACKET"){
          if(tokens[i].tokenType != "COMMA"){
            throw runtime_error("Expected comma after json array object");
          }
          i += 1;
          JSONValue item = parseTokens(tokens, i);
          i += 1;
          items.push_back(item);
        }
        // move forward since we found closing bracket
        return JSONValue("ARRAY", items);
      } else if(token.tokenType == "OPENING_BRACE") {
        // beginning of json object
        map<string,JSONValue> items {};
        i += 1;
        if(tokens[i].tokenType == "CLOSING_BRACE"){
          return JSONValue("OBJECT", items);
        }

        if(tokens[i].tokenType != "STRING"){
          throw runtime_error("JSON object key should be a string");
        }
        auto key = tokens[i];
        i += 1;
        if(tokens[i].tokenType != "COLON"){
          throw runtime_error("Expected : after json object key");
        }
        i += 1;
        JSONValue item = parseTokens(tokens, i);
        i += 1;
        items[key.tokenValue] = item;
        
        while(tokens[i].tokenType != "CLOSING_BRACE"){
          if(tokens[i].tokenType != "COMMA"){
            throw runtime_error("Expected comma after json dict object " + tokens[i-1].tokenType);
          }
          // consume the comma
          i += 1; 
          if(tokens[i].tokenType != "STRING"){
            throw runtime_error("JSON object key should be a string");
          }
          // consume the key
          auto key = tokens[i];
          i += 1;
          
          // consume the colon separator
          if(tokens[i].tokenType != "COLON"){
            throw runtime_error("Expected : after json object key");
          }
          i += 1;

          // consume the value
          JSONValue item = parseTokens(tokens, i);
          i += 1;
          items[key.tokenValue] = item;
        }
        // i += 1;
        return JSONValue("OBJECT", items);
      } else {
        throw runtime_error("Invalid item " +  tokens[i].tokenValue + "in json string ");
      }
    }

};

int main(int argc, char* argv[]){
  JSON parser {};
  //"{\"age\":-23.32, \"is_man\": true, \"who\": null, \"name\": {\"last\": \"Ajah\", \"first\": [\"Chuks\"]}}"
  auto t = parser.parse(argv[1]);
  // cout << t["age"].getNumber() << " that is the age" << endl;
  // cout << t["name"].getString() << " that is the name" << endl;
  // cout << "first item is " << t[0].getNumber() << endl;
  // cout << "3rd item is " << t[2].getString() << endl;
  // cout << "the key to my heart is " << t[3][2]["key"]["key"].getString() << endl;
  // vector<JSONValue> arr {};
  // arr.push_back(JSONValue("STRING", "Chuks"));
  // arr.push_back(JSONValue("STRING", "Mercy"));
  // arr.push_back(JSONValue("STRING", "Grace"));
  // map<string, JSONValue> family{};
  // family["children"] = JSONValue("ARRAY", arr);
  // family["father"] = JSONValue("STRING", "Patrick");
  // family["mother"] = JSONValue("STRING", "Priscillia");
  // family["is_wealthy"] = JSONValue("BOOLEAN", "true");
  // JSONValue j = JSONValue("OBJECT", family);
  // JSONValue s = JSONValue("STRING", "Chuks");
  // cout << "First child is " << j["children"][0].getString() << endl;

  // cout << "First child from array is " << arr[0].data_type << endl;

  // cout << "First child from STRING is " << j.getType() << endl;
  cout << "output of stringify is" << parser.stringify(t) << endl;
  // JSONValue s = JSONValue("STRING", string("CHUKS"));
  //   JSONValue n = JSONValue("NUMBER", 23.322);
  //   JSONValue b = JSONValue("BOOLEAN", true);
  //   cout << " Value is " << s.str_value<< endl;
  //   cout << " Value for num is " << n.d_value << endl;
  //   cout << " Value for bool is " << b.b_value << endl;
  return 0;
}