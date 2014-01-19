#include <sol.hpp>
#include <cassert>

int main() {
    sol::state lua;

    // need the base library for assertions
    lua.open_libraries(sol::lib::base);

    // basic setting of a variable
    // through multiple ways
    lua["x"] = 10;
    lua.set("y", "hello");

    // assert values are as given
    lua.script("assert(x == 10)");
    lua.script("assert(y == 'hello')");


    // basic retrieval of a variable
    // through multiple ways
    int x = lua["x"];
    auto y = lua.get<std::string>("y");

    int x2;
    std::string y2;
    std::tie(x2, y2) = lua.get<int, std::string>("x", "y");

    // assert the values
    assert(x == 10);
    assert(y == "hello");
    assert(x2 == 10);
    assert(y2 == "hello");
}