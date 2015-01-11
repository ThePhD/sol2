#include <sol.hpp>
#include <string>
#include <iostream>

/// this example shows how to read data in from a lua table
using namespace std;

//compiles on linux with: g++ -std=c++11 -I.. table.cpp -o tables -llua

int main(int argc, char * argv[])
{
  sol::state lua;
  //table used as an array
  lua.script("table1 = {\"hello\", \"table\"}");
  //table with a nested table and the key value syntax
  lua.script("table2 = {[\"nestedTable\"]={[\"key1\"]=\"value1\", [\"key2\"]=\"value2\"},"
    " [\"name\"]=\"table2\"}");
  
  
  //using the values stored in table1
  cout << lua.get<sol::table>("table1").get<string>(1)<< " " << 
    lua.get<sol::table>("table1").get<string>(2) << endl;
    
  auto t2 = lua.get<sol::table>("table2");
  auto nestedTable = t2.get<sol::table>("nestedTable");
  //some retrival of values from the nested table
  //the cleaner way of doing things
  cout << "nested table: key1 : " << nestedTable.get<string>("key1") << ", key2: "
    //yes you can chain the get<>() results 
    << lua.get<sol::table>("table2").get<sol::table>("nestedTable").get<string>("key2") 
    << endl;
    
  cout << "name of t2: " << t2.get<string>("name") << endl;
   
  return 0;
}