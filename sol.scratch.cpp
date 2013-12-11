#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <sol.hpp>

std::string free_function() {
    std::cout << "free_function()" << std::endl;
    return "test";
}

struct object {

    std::string operator() () {
        std::cout << "member_test()" << std::endl;
        return "test";
    }

};

int plop_xyz(int x, int y, std::string z) {
    std::cout << x << " " << y << " " << z << std::endl;
    return 11;
}

void run_script(sol::state& lua) {
    lua.script("assert(os.fun() == \"test\")\n"
        "assert(os.name == \"windows\")");
}

TEST_CASE("simple/set_global", "Check if the set_global works properly.") {
    sol::state lua;

    lua.set("a", 9);
    REQUIRE_NOTHROW(lua.script("if a ~= 9 then error('wrong value') end"));

    lua.set("d", "hello");
    REQUIRE_NOTHROW(lua.script("if d ~= 'hello' then error('expected \\'hello\\', got '.. tostring(d)) end"));

    lua.set("e", std::string("hello"));
    REQUIRE_NOTHROW(lua.script("if d ~= 'hello' then error('expected \\'hello\\', got '.. tostring(d)) end"));

    lua.set("f", true);
    REQUIRE_NOTHROW(lua.script("if f ~= true then error('wrong value') end"));
}

TEST_CASE("simple/get", "Tests if the get function works properly.") {
    sol::state lua;

    lua.script("a = 9");
    auto a = lua.get<int>("a");
    REQUIRE(a == 9.0);

    lua.script("b = nil");
    REQUIRE_NOTHROW(lua.get<sol::nil_t>("b"));
    
    lua.script("d = 'hello'");
    auto d = lua.get<std::string>("d");
    REQUIRE(d == "hello");

    lua.script("e = true");
    auto e = lua.get<bool>("e");
    REQUIRE(e == true);
}

TEST_CASE("simple/addition", "") {
    sol::state lua;

    lua.set("b", 0.2);
    lua.script("c = 9 + b");
    auto c = lua.get<double>("c");

    REQUIRE(c == 9.2);
}

TEST_CASE("simple/if", "") {
    sol::state lua;

    std::string program = "if true then f = 0.1 else f = 'test' end";
    lua.script(program);
    auto f = lua.get<double>("f");

    REQUIRE(f == 0.1);
}

TEST_CASE("simple/evalStream", "The VM evaluates a stream's contents using a reader") {
    sol::state lua;

    std::stringstream sscript;
    int g = 9;
    sscript << "g = " << g << ";";

    REQUIRE_NOTHROW(lua.script(sscript.str()));
    REQUIRE(lua.get<double>("g") == 9.0);
}

TEST_CASE("simple/callWithParameters", "Lua function is called with a few parameters from C++") {
    sol::state lua;

    REQUIRE_NOTHROW(lua.script("function my_add(i, j, k) return i + j + k end"));
    auto f = lua.get<sol::function>("my_add");
    REQUIRE_NOTHROW(f.call<int>(1, 2, 3));
    REQUIRE_THROWS(f.call<int>(1, 2, "arf"));
}

TEST_CASE("simple/callCppFunction", "C++ function is called from lua") {
    sol::state lua;

    lua.set_function("plop_xyz", plop_xyz);
    lua.script("x = plop_xyz(2, 6, 'hello')");
 
    REQUIRE(lua.get<int>("x") == 11);
}

TEST_CASE("simple/callLambda", "A C++ lambda is exposed to lua and called") {
    sol::state lua;

    int x = 0;

    lua.set_function("foo", [ &x ] { x = 1; });

    lua.script("foo()");

    REQUIRE(x == 1);
}

TEST_CASE("advanced/callLambdaReturns", "Checks for lambdas returning values") {
    sol::state lua;

    lua.set_function("a", [ ] { return 42; });
    lua.set_function("b", [ ] { return 42u; });
    lua.set_function("c", [ ] { return 3.14; });
    lua.set_function("d", [ ] { return 6.28f; });
    lua.set_function("e", [ ] { return "lol"; });
    lua.set_function("f", [ ] { return true; });
    lua.set_function("g", [ ] { return std::string("str"); });
    lua.set_function("h", [ ] { });
    lua.set_function("i", [ ] { return sol::nil; });
}

TEST_CASE("advanced/callLambda2", "A C++ lambda is exposed to lua and called") {
    sol::state lua;

    int x = 0;
    lua.set_function("set_x", [ &] (int new_x) {
        x = new_x;
        return 0;
    });

    lua.script("set_x(9)");
    REQUIRE(x == 9);
}

TEST_CASE("negative/basicError", "Check if error handling works correctly") {
    sol::state lua;

    REQUIRE_THROWS(lua.script("nil[5]"));
}

/*
    lua.get<sol::table>("os").set("name", "windows");

    SECTION("")
    lua.get<sol::table>("os").set_function("fun",
        [ ] () {
        std::cout << "stateless lambda()" << std::endl;
        return "test";
    }
    );
    run_script(lua);

    lua.get<sol::table>("os").set_function("fun", &free_function);
    run_script(lua);

    // l-value, can optomize
    auto lval = object();
    lua.get<sol::table>("os").set_function("fun", &object::operator(), lval);
    run_script(lua);

    // Tests will begin failing here
    // stateful lambda: non-convertible, unoptomizable
    int breakit = 50;
    lua.get<sol::table>("os").set_function("fun",
        [ &breakit ] () {
        std::cout << "stateless lambda()" << std::endl;
        return "test";
    }
    );
    run_script(lua);

    // r-value, cannot optomize
    lua.get<sol::table>("os").set_function("fun", &object::operator(), object());
    run_script(lua);

    // r-value, cannot optomize
    auto rval = object();
    lua.get<sol::table>("os").set_function("fun", &object::operator(), std::move(rval));
    run_script(lua);

}
*/
