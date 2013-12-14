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

TEST_CASE("simple/callWithParameters", "Lua function is called with a few parameters from C++") {
    sol::state lua;

    REQUIRE_NOTHROW(lua.script("function my_add(i, j, k) return i + j + k end"));
    auto f = lua.get<sol::function>("my_add");
    REQUIRE_NOTHROW(lua.script("function my_nothing(i, j, k) end"));
    auto fvoid = lua.get<sol::function>("my_nothing");
    int a;
    REQUIRE_NOTHROW(fvoid(1, 2, 3));
    REQUIRE_NOTHROW(a = f.call<int>(1, 2, 3));
    REQUIRE(a == 6);
    REQUIRE_THROWS(a = f.call<int>(1, 2, "arf"));
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

    REQUIRE_NOTHROW(lua.set_function("a", [ ] { return 42; }));
    REQUIRE_NOTHROW(lua.set_function("b", [ ] { return 42u; }));
    REQUIRE_NOTHROW(lua.set_function("c", [ ] { return 3.14; }));
    REQUIRE_NOTHROW(lua.set_function("d", [ ] { return 6.28f; }));
    REQUIRE_NOTHROW(lua.set_function("e", [ ] { return "lol"; }));
    REQUIRE_NOTHROW(lua.set_function("f", [ ] { return true; }));
    REQUIRE_NOTHROW(lua.set_function("g", [ ] { return std::string("str"); }));
    REQUIRE_NOTHROW(lua.set_function("h", [ ] { }));
    REQUIRE_NOTHROW(lua.set_function("i", [ ] { return sol::nil; }));
    REQUIRE_NOTHROW(lua.set_function("j", [ ] { return std::make_tuple(1, 6.28f, 3.14, std::string( "heh" )); } ));
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

TEST_CASE("libraries", "Check if we can open libraries through sol") {
    sol::state lua;
    REQUIRE_NOTHROW(lua.open_libraries(sol::lib::base, sol::lib::os));
}

TEST_CASE("tables/variables", "Check if tables and variables work as intended") {
    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::os);
    lua.get<sol::table>("os").set("name", "windows");
    REQUIRE_NOTHROW(lua.script("assert(os.name == \"windows\")"));    
}

TEST_CASE("tables/functions_variables", "Check if tables and function calls work as intended") {
    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::os);
    auto run_script = [ ] (sol::state& lua) -> void {
        lua.script("assert(os.fun() == \"test\")");
    };

    lua.get<sol::table>("os").set_function("fun",
        [ ] () {
            std::cout << "stateless lambda()" << std::endl;
            return "test";
        }
    );
    REQUIRE_NOTHROW(run_script(lua));

    lua.get<sol::table>("os").set_function("fun", &free_function);
    REQUIRE_NOTHROW(run_script(lua));

    // l-value, can optomize
    auto lval = object();
    lua.get<sol::table>("os").set_function("fun", &object::operator(), lval);
    REQUIRE_NOTHROW((lua));

    // stateful lambda: non-convertible, unoptomizable
    int breakit = 50;
    lua.get<sol::table>("os").set_function("fun",
        [ &breakit ] () {
        std::cout << "stateless lambda()" << std::endl;
        return "test";
    }
    );
    REQUIRE_NOTHROW(run_script(lua));

    // r-value, cannot optomize
    lua.get<sol::table>("os").set_function("fun", &object::operator(), object());
    REQUIRE_NOTHROW(run_script(lua));

    // r-value, cannot optomize
    auto rval = object();
    lua.get<sol::table>("os").set_function("fun", &object::operator(), std::move(rval));
    REQUIRE_NOTHROW(run_script(lua));
}

TEST_CASE("tables/operator[]", "Check if operator[] retrieval and setting works properly") {
    sol::state lua;
    lua.open_libraries(sol::lib::base);

    lua.script("foo = 20\nbar = \"hello world\"");
    // basic retrieval 
    std::string bar = lua["bar"];
    int foo = lua["foo"];
    REQUIRE(bar == "hello world");
    REQUIRE(foo == 20);

    // basic setting
    lua["bar"] = 20.4;
    lua["foo"] = "goodbye";

    // doesn't modify the actual values obviously.
    REQUIRE(bar == "hello world");
    REQUIRE(foo == 20);

    // function setting
    lua["test"] = plop_xyz;
    REQUIRE_NOTHROW(lua.script("assert(test(10, 11, \"hello\") == 11)"));

    // function retrieval
    sol::function test = lua["test"];
    REQUIRE(test.call<int>(10, 11, "hello") == 11);

    // setting a lambda
    lua["lamb"] = [](int x) {
        return x * 2;
    };

    REQUIRE_NOTHROW(lua.script("assert(lamb(220) == 440)"));

    // function retrieval of a lambda
    sol::function lamb = lua["lamb"];
    REQUIRE(lamb.call<int>(220) == 440);

    // test const table retrieval
    auto assert1 = [](const sol::table& t) {
        std::string a = t["foo"];
        int b = t["bar"];
        std::cout << a << ',' << b << '\n';
    };

    REQUIRE_NOTHROW(assert1(lua.global_table()));
}