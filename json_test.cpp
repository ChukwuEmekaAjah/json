#include <cassert>
#include <iostream>
#include "json.cpp"

int main(){
  JSON parser {};
  
  auto t = parser.parse("\"name\"");
  assert(t.getString() == "name");
  std::cout << "Test that a simple string is a valid json was successful" << std::endl;
  
  t = parser.parse("23");
  assert(t.getNumber() == 23);
  std::cout << "Test that a simple integer number is a valid json was successful" << std::endl;
  
  t = parser.parse("23.32");
  assert(t.getNumber() == 23.32);
  std::cout << "Test that a simple real number is a valid json was successful" << std::endl;
  

  t = parser.parse("23.32E2");
  assert(t.getNumber() == 2332);
  std::cout << "Test that a simple exponential number is a valid json was successful" << std::endl;


  t = parser.parse("[78,\"json\"]");
  assert(t[0].getNumber() == 78);
  assert(t[1].getString() == "json");
  std::cout << "Test that an array containing multiple types is a valid json was successful" << std::endl;
  return 0;
}