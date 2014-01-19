#include <sol.hpp>

int main() {
    // create an empty lua state
    sol::state lua;

    // by default, libraries are not opened
    // you can open libraries by using open_libraries
    // the libraries reside in the sol::lib enum class
    lua.open_libraries(sol::lib::base);

    // call lua code directly
    lua.script("print('hello world')");
}