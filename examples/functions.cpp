#include <sol.hpp>
#include <cassert>

inline int my_add(int x, int y) {
    return x + y;
}

struct multiplier {
    int operator()(int x) {
        return x * 10;
    }

    static int by_five(int x) {
        return x * 5;
    }
};

int main() {
    sol::state lua;
    lua.open_libraries(sol::lib::base);

    // setting a function is simple
    lua.set_function("my_add", my_add);

    // you could even use a lambda
    lua.set_function("my_mul", [](double x, double y) { return x * y; });

    // member function pointers and functors as well
    lua.set_function("mult_by_ten", multiplier{});
    lua.set_function("mult_by_five", &multiplier::by_five);

    // assert that the functions work
    lua.script("assert(my_add(10, 11) == 21)");
    lua.script("assert(my_mul(4.5, 10) == 45)");
    lua.script("assert(mult_by_ten(50) == 500)");
    lua.script("assert(mult_by_five(10) == 50)");

    // using lambdas, functions could have state.
    int x = 0;
    lua.set_function("inc", [&x]() { x += 10; });

    // calling a stateful lambda modifies the value
    lua.script("inc()");
    assert(x == 10);

    // this can be done as many times as you want
    lua.script("inc()\ninc()\ninc()");
    assert(x == 40);

    // retrieval of a function is done similarly
    // to other variables, using sol::function
    sol::function add = lua["my_add"];
    assert(add.call<int>(10, 11) == 21);

    // multi-return functions are supported using
    // std::tuple as the interface.
    lua.set_function("multi", []{ return std::make_tuple(10, "goodbye"); });
    lua.script("x, y = multi()");
    lua.script("assert(x == 10 and y == 'goodbye')");

    auto multi = lua.get<sol::function>("multi");
    int first;
    std::string second;
    std::tie(first, second) = multi.call<int, std::string>();

    // assert the values
    assert(first == 10);
    assert(second == "goodbye");
}