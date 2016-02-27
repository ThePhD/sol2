#define CATCH_CONFIG_MAIN
#define SOL_CHECK_ARGUMENTS
#include <catch.hpp>
#include <sol.hpp>
#include <vector>
#include <map>

struct stack_guard {
    lua_State* L;
    int& begintop;
    int& endtop;
    stack_guard(lua_State* L, int& begintop, int& endtop) : L(L), begintop(begintop), endtop(endtop) { 
        begintop = lua_gettop(L);
    }
    ~stack_guard() { endtop = lua_gettop(L); }
};

void test_free_func(std::function<void()> f) {
    f();
}

void test_free_func2(std::function<int(int)> f, int arg1) {
    int val = f(arg1);
    if(val != arg1)
        throw sol::error("failed function call!");
}

std::function<int()> makefn() {
    auto fx = []() -> int {
        return 0x1456789;
    };
    return fx;
}

void takefn(std::function<int()> purr) {
    if (purr() != 0x1456789)
        throw 0;
}

std::string free_function() {
    std::cout << "free_function()" << std::endl;
    return "test";
}

int overloaded(int x) {
    std::cout << x << std::endl;
    return 3;
}

int overloaded(int x, int y) {
    std::cout << x << " " << y << std::endl;
    return 7;
}

int overloaded(int x, int y, int z) {
    std::cout << x << " " << y << " " << z << std::endl;
    return 11;
}

int non_overloaded(int x, int y, int z) {
    std::cout << x << " " << y << " " << z << std::endl;
    return 13;
}

std::vector<int> test_table_return_one() {
    return { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
}

std::vector<std::pair<std::string, int>> test_table_return_two() {
    return {{ "one", 1 }, { "two", 2 }, { "three", 3 }};
}

std::map<std::string, std::string> test_table_return_three() {
    return {{ "name", "Rapptz" }, { "friend", "ThePhD" }, { "project", "sol" }};
}

struct self_test {
    int bark;

    self_test() : bark(100) {

    }

    void g(const std::string& str) {
        std::cout << str << '\n';
        bark += 1;
    }

    void f(const self_test& t) {
        std::cout << "got test" << '\n';
        if (t.bark != bark)
            throw sol::error("bark values are not the same for self_test f function");
        if (&t != this)
            throw sol::error("call does not reference self for self_test f function");
    }
};

int func_1(int) {
    return 1;
}

std::string func_1s(std::string a) {
    return "string: " + a;
}

int func_2(int, int) {
    return 2;
}

void func_3(int, int, int) {

}

struct vars {
    vars () {

    }

    int boop = 0;

    ~vars () {

    }
};

struct object {
    std::string operator() () {
        std::cout << "member_test()" << std::endl;
        return "test";
    }
};

struct fuser {
    int x;
    fuser() : x(0) {}

    fuser(int x) : x(x) {}

    int add(int y) {
       return x + y;
    }

    int add2(int y) {
       return x + y + 2;
    }
};

namespace crapola {
struct fuser {
    int x;
    fuser() : x(0) {}
    fuser(int x) : x(x) {}
    fuser(int x, int x2) : x(x * x2) {}

    int add(int y) {
       return x + y;
    }
    int add2(int y) {
       return x + y + 2;
    }
};
} // crapola

int plop_xyz(int x, int y, std::string z) {
    std::cout << x << " " << y << " " << z << std::endl;
    return 11;
}

class Base {
public:
    Base(int a_num) : m_num(a_num) { }

    int get_num() {
        return m_num;
    }

protected:
    int m_num;
};

class Derived : public Base {
public:
    Derived(int a_num) : Base(a_num) { }

    int get_num_10() {
        return 10 * m_num;
    }
};

struct Vec {
  float x, y, z;
  Vec(float x, float y, float z) : x{x}, y{y}, z{z} {}
  float length() {
    return sqrtf(x*x + y*y + z*z);
  }
  Vec normalized() {
    float invS = 1 / length();
    return {x * invS, y * invS, z * invS};
  }
};

struct giver {
    int a = 0;

    giver () {

    }

    void gief () {
        a = 1;
    }

    static void stuff () {

    }

    static void gief_stuff (giver& t, int a) {
        t.a = a;
    }

    ~giver () {

    }

};

struct factory_test {
private:
    factory_test() { a = true_a; }
    ~factory_test() { a = 0; }
public:
    static int num_saved;
    static int num_killed;

    struct deleter {
        void operator()(factory_test* f) {
            f->~factory_test();
        }
    };

    static const int true_a;
    int a;

    static std::unique_ptr<factory_test, deleter> make() {
        return std::unique_ptr<factory_test, deleter>( new factory_test(), deleter());
    }

    static void save(factory_test& f) {
        new(&f)factory_test();
        ++num_saved;
    }

    static void kill(factory_test& f) {
        f.~factory_test();
        ++num_killed;
    }
};

int factory_test::num_saved = 0;
int factory_test::num_killed = 0;
const int factory_test::true_a = 156;

TEST_CASE("table/traversal", "ensure that we can chain requests and tunnel down into a value if we desire") {

    sol::state lua;
    int begintop = 0, endtop = 0;

    lua.script("t1 = {t2 = {t3 = 24}};");
    {
        stack_guard g(lua.lua_state(), begintop, endtop);
        int traversex24 = lua.traverse_get<int>("t1", "t2", "t3");
        REQUIRE(traversex24 == 24);
    } REQUIRE(begintop == endtop);

    {
        stack_guard g(lua.lua_state(), begintop, endtop);
        int x24 = lua["t1"]["t2"]["t3"];
        REQUIRE(x24 == 24);
    } REQUIRE(begintop == endtop);

    {
        stack_guard g(lua.lua_state(), begintop, endtop);
        lua["t1"]["t2"]["t3"] = 64;
        int traversex64 = lua.traverse_get<int>("t1", "t2", "t3");
        REQUIRE(traversex64 == 64);
    } REQUIRE(begintop == endtop);

    {
        stack_guard g(lua.lua_state(), begintop, endtop);
        int x64 = lua["t1"]["t2"]["t3"];
        REQUIRE(x64 == 64);
    } REQUIRE(begintop == endtop);

    {
        stack_guard g(lua.lua_state(), begintop, endtop);
        lua.traverse_set("t1", "t2", "t3", 13);
        int traversex13 = lua.traverse_get<int>("t1", "t2", "t3");
        REQUIRE(traversex13 == 13);
    } REQUIRE(begintop == endtop);

    {
        stack_guard g(lua.lua_state(), begintop, endtop);
        int x13 = lua["t1"]["t2"]["t3"];
        REQUIRE(x13 == 13);
    } REQUIRE(begintop == endtop);
}

TEST_CASE("simple/set", "Check if the set works properly.") {
    sol::state lua;
    int begintop = 0, endtop = 0;
    {
        stack_guard g(lua.lua_state(), begintop, endtop);
        lua.set("a", 9);
    } REQUIRE(begintop == endtop);
    REQUIRE_NOTHROW(lua.script("if a ~= 9 then error('wrong value') end"));
    {
        stack_guard g(lua.lua_state(), begintop, endtop);
        lua.set("d", "hello");
    } REQUIRE(begintop == endtop);
    REQUIRE_NOTHROW(lua.script("if d ~= 'hello' then error('expected \\'hello\\', got '.. tostring(d)) end"));

    {
        stack_guard g(lua.lua_state(), begintop, endtop);
        lua.set("e", std::string("hello"), "f", true);
    } REQUIRE(begintop == endtop);
    REQUIRE_NOTHROW(lua.script("if d ~= 'hello' then error('expected \\'hello\\', got '.. tostring(d)) end"));
    REQUIRE_NOTHROW(lua.script("if f ~= true then error('wrong value') end"));
}

TEST_CASE("simple/get", "Tests if the get function works properly.") {
    sol::state lua;
    int begintop = 0, endtop = 0;

    lua.script("a = 9");
    {
        stack_guard g(lua.lua_state(), begintop, endtop);
        auto a = lua.get<int>("a");
        REQUIRE(a == 9.0);
    } REQUIRE(begintop == endtop);

    lua.script("b = nil");
    {
        stack_guard g(lua.lua_state(), begintop, endtop);
        REQUIRE_NOTHROW(lua.get<sol::nil_t>("b"));
    } REQUIRE(begintop == endtop);

    lua.script("d = 'hello'");
    lua.script("e = true");
    {
        stack_guard g(lua.lua_state(), begintop, endtop);
        std::string d;
        bool e;
        std::tie( d, e ) = lua.get<std::string, bool>("d", "e");
        REQUIRE(d == "hello");
        REQUIRE(e == true);
    } REQUIRE(begintop == endtop);
}

TEST_CASE("simple/set-get-global-integer", "Tests if the get function works properly with global integers") {
    sol::state lua;
    lua[1] = 25.4;
    lua.script("b = 1");
    double a = lua.get<double>(1);
    double b = lua.get<double>("b");
    REQUIRE(a == 25.4);
    REQUIRE(b == 1);
}

TEST_CASE("simple/addition", "check if addition works and can be gotten through lua.get and lua.set") {
    sol::state lua;

    lua.set("b", 0.2);
    lua.script("c = 9 + b");
    auto c = lua.get<double>("c");

    REQUIRE(c == 9.2);
}

TEST_CASE("simple/if", "check if if statements work through lua") {
    sol::state lua;

    std::string program = "if true then f = 0.1 else f = 'test' end";
    lua.script(program);
    auto f = lua.get<double>("f");

    REQUIRE(f == 0.1);
    REQUIRE((f == lua["f"]));
}

TEST_CASE("simple/call-with-parameters", "Lua function is called with a few parameters from C++") {
    sol::state lua;

    REQUIRE_NOTHROW(lua.script("function my_add(i, j, k) return i + j + k end"));
    auto f = lua.get<sol::function>("my_add");
    REQUIRE_NOTHROW(lua.script("function my_nothing(i, j, k) end"));
    auto fvoid = lua.get<sol::function>("my_nothing");
    int a;
    REQUIRE_NOTHROW(fvoid(1, 2, 3));
    REQUIRE_NOTHROW(a = f.call<int>(1, 2, 3));
    REQUIRE(a == 6);
    REQUIRE_THROWS(a = f(1, 2, "arf"));
}

TEST_CASE("simple/call-c++-function", "C++ function is called from lua") {
    sol::state lua;

    lua.set_function("plop_xyz", plop_xyz);
    lua.script("x = plop_xyz(2, 6, 'hello')");

    REQUIRE(lua.get<int>("x") == 11);
}

TEST_CASE("simple/call-lambda", "A C++ lambda is exposed to lua and called") {
    sol::state lua;

    int x = 0;

    lua.set_function("foo", [&x] { x = 1; });

    lua.script("foo()");

    REQUIRE(x == 1);
}

TEST_CASE("advanced/get-and-call", "Checks for lambdas returning values after a get operation") {
    const static std::string lol = "lol", str = "str";
    const static std::tuple<int, float, double, std::string> heh_tuple = std::make_tuple(1, 6.28f, 3.14, std::string("heh"));
    sol::state lua;

    REQUIRE_NOTHROW(lua.set_function("a", [] { return 42; }));
    REQUIRE(lua.get<sol::function>("a").call<int>() == 42);

    REQUIRE_NOTHROW(lua.set_function("b", [] { return 42u; }));
    REQUIRE(lua.get<sol::function>("b").call<unsigned int>() == 42u);

    REQUIRE_NOTHROW(lua.set_function("c", [] { return 3.14; }));
    REQUIRE(lua.get<sol::function>("c").call<double>() == 3.14);

    REQUIRE_NOTHROW(lua.set_function("d", [] { return 6.28f; }));
    REQUIRE(lua.get<sol::function>("d").call<float>() == 6.28f);

    REQUIRE_NOTHROW(lua.set_function("e", [] { return "lol"; }));
    REQUIRE(lua.get<sol::function>("e").call<std::string>() == lol);

    REQUIRE_NOTHROW(lua.set_function("f", [] { return true; }));
    REQUIRE(lua.get<sol::function>("f").call<bool>());

    REQUIRE_NOTHROW(lua.set_function("g", [] { return std::string("str"); }));
    REQUIRE(lua.get<sol::function>("g").call<std::string>() == str);

    REQUIRE_NOTHROW(lua.set_function("h", [] { }));
    REQUIRE_NOTHROW(lua.get<sol::function>("h").call());

    REQUIRE_NOTHROW(lua.set_function("i", [] { return sol::nil; }));
    REQUIRE(lua.get<sol::function>("i").call<sol::nil_t>() == sol::nil);
    REQUIRE_NOTHROW(lua.set_function("j", [] { return std::make_tuple(1, 6.28f, 3.14, std::string("heh")); }));
    REQUIRE((lua.get<sol::function>("j").call<int, float, double, std::string>() == heh_tuple));
}

TEST_CASE("advanced/operator[]-call", "Checks for lambdas returning values using operator[]") {
    const static std::string lol = "lol", str = "str";
    const static std::tuple<int, float, double, std::string> heh_tuple = std::make_tuple(1, 6.28f, 3.14, std::string("heh"));
    sol::state lua;

    REQUIRE_NOTHROW(lua.set_function("a", [] { return 42; }));
    REQUIRE(lua["a"].call<int>() == 42);

    REQUIRE_NOTHROW(lua.set_function("b", [] { return 42u; }));
    REQUIRE(lua["b"].call<unsigned int>() == 42u);

    REQUIRE_NOTHROW(lua.set_function("c", [] { return 3.14; }));
    REQUIRE(lua["c"].call<double>() == 3.14);

    REQUIRE_NOTHROW(lua.set_function("d", [] { return 6.28f; }));
    REQUIRE(lua["d"].call<float>() == 6.28f);

    REQUIRE_NOTHROW(lua.set_function("e", [] { return "lol"; }));
    REQUIRE(lua["e"].call<std::string>() == lol);

    REQUIRE_NOTHROW(lua.set_function("f", [] { return true; }));
    REQUIRE(lua["f"].call<bool>());

    REQUIRE_NOTHROW(lua.set_function("g", [] { return std::string("str"); }));
    REQUIRE(lua["g"].call<std::string>() == str);

    REQUIRE_NOTHROW(lua.set_function("h", [] { }));
    REQUIRE_NOTHROW(lua["h"].call());

    REQUIRE_NOTHROW(lua.set_function("i", [] { return sol::nil; }));
    REQUIRE(lua["i"].call<sol::nil_t>() == sol::nil);
    REQUIRE_NOTHROW(lua.set_function("j", [] { return std::make_tuple(1, 6.28f, 3.14, std::string("heh")); }));
    REQUIRE((lua["j"].call<int, float, double, std::string>() == heh_tuple));
}

TEST_CASE("advanced/call-lambdas", "A C++ lambda is exposed to lua and called") {
    sol::state lua;

    int x = 0;
    lua.set_function("set_x", [&] (int new_x) {
        x = new_x;
        return 0;
    });

    lua.script("set_x(9)");
    REQUIRE(x == 9);
}

TEST_CASE("advanced/call-referenced_obj", "A C++ object is passed by pointer/reference_wrapper to lua and invoked") {
    sol::state lua;

    int x = 0;
    auto objx = [&](int new_x) {
        x = new_x;
        return 0;
    };
    lua.set_function("set_x", std::ref(objx));

    int y = 0;
    auto objy = [&](int new_y) {
        y = new_y;
        return std::tuple<int, int>(0, 0);
    };
    lua.set_function("set_y", &decltype(objy)::operator(), std::ref(objy));

    lua.script("set_x(9)");
    lua.script("set_y(9)");
    REQUIRE(x == 9);
    REQUIRE(y == 9);
}

TEST_CASE("negative/basic_errors", "Check if error handling works correctly") {
    sol::state lua;

    REQUIRE_THROWS(lua.script("nil[5]"));
}

TEST_CASE("libraries", "Check if we can open libraries") {
    sol::state lua;
    REQUIRE_NOTHROW(lua.open_libraries(sol::lib::base, sol::lib::os));
}

TEST_CASE("tables/variables", "Check if tables and variables work as intended") {
    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::os);
    lua.get<sol::table>("os").set("name", "windows");
    REQUIRE_NOTHROW(lua.script("assert(os.name == \"windows\")"));
}

TEST_CASE("tables/functions-variables", "Check if tables and function calls work as intended") {
    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::os);
    auto run_script = [] (sol::state& lua) -> void {
        lua.script("assert(os.fun() == \"test\")");
    };

    lua.get<sol::table>("os").set_function("fun",
        [] () {
            std::cout << "stateless lambda()" << std::endl;
            return "test";
        }
    );
    REQUIRE_NOTHROW(run_script(lua));

    lua.get<sol::table>("os").set_function("fun", &free_function);
    REQUIRE_NOTHROW(run_script(lua));

    // l-value, canNOT optimise
    // prefer value semantics unless wrapped with std::reference_wrapper
    {
    auto lval = object();
    lua.get<sol::table>("os").set_function("fun", &object::operator(), lval);
    }
    REQUIRE_NOTHROW(run_script(lua));

    auto reflval = object();
    lua.get<sol::table>("os").set_function("fun", &object::operator(), std::ref(reflval));
    REQUIRE_NOTHROW(run_script(lua));


    // stateful lambda: non-convertible, cannot be optimised
    int breakit = 50;
    lua.get<sol::table>("os").set_function("fun",
        [&breakit] () {
            std::cout << "stateful lambda()" << std::endl;
            return "test";
        }
    );
    REQUIRE_NOTHROW(run_script(lua));

    // r-value, cannot optimise
    lua.get<sol::table>("os").set_function("fun", &object::operator(), object());
    REQUIRE_NOTHROW(run_script(lua));

    // r-value, cannot optimise
    auto rval = object();
    lua.get<sol::table>("os").set_function("fun", &object::operator(), std::move(rval));
    REQUIRE_NOTHROW(run_script(lua));
}

TEST_CASE("functions/overload-resolution", "Check if overloaded function resolution templates compile/work") {
    sol::state lua;
    lua.open_libraries(sol::lib::base);

    lua.set_function("non_overloaded", non_overloaded);
    REQUIRE_NOTHROW(lua.script("x = non_overloaded(1, 2, 3)\nprint(x)"));

    /*
    // Cannot reasonably support: clang++ refuses to try enough
    // deductions to make this work
    lua.set_function<int>("overloaded", overloaded);
    REQUIRE_NOTHROW(lua.script("print(overloaded(1))"));

    lua.set_function<int, int>("overloaded", overloaded);
    REQUIRE_NOTHROW(lua.script("print(overloaded(1, 2))"));

    lua.set_function<int, int, int>("overloaded", overloaded);
    REQUIRE_NOTHROW(lua.script("print(overloaded(1, 2, 3))"));
    */
    lua.set_function<int(int)>("overloaded", overloaded);
    REQUIRE_NOTHROW(lua.script("print(overloaded(1))"));

    lua.set_function<int(int, int)>("overloaded", overloaded);
    REQUIRE_NOTHROW(lua.script("print(overloaded(1, 2))"));

    lua.set_function<int(int, int, int)>("overloaded", overloaded);
    REQUIRE_NOTHROW(lua.script("print(overloaded(1, 2, 3))"));
}

TEST_CASE("functions/return-order-and-multi-get", "Check if return order is in the same reading order specified in Lua") {
    const static std::tuple<int, int, int> triple = std::make_tuple(10, 11, 12);
    const static std::tuple<int, float> paired = std::make_tuple(10, 10.f);
    sol::state lua;
    lua.set_function("f", [] {
        return std::make_tuple(10, 11, 12);
    } );
    int a = 0;
    lua.set_function( "h", []() {
        return std::make_tuple( 10, 10.0f );
    } );
    lua.script("function g() return 10, 11, 12 end\nx,y,z = g()");
    auto tcpp = lua.get<sol::function>("f").call<int, int, int>();
    auto tlua = lua.get<sol::function>( "g" ).call<int, int, int>();
    auto tcpp2 = lua.get<sol::function>( "h" ).call<int, float>();
    auto tluaget = lua.get<int, int, int>( "x", "y", "z" );
    REQUIRE(tcpp == triple);
    REQUIRE(tlua == triple);
    REQUIRE(tluaget == triple);
    REQUIRE(tcpp2 == paired);
}

TEST_CASE("functions/deducing-return-order-and-multi-get", "Check if return order is in the same reading order specified in Lua, with regular deducing calls") {
    const static std::tuple<int, int, int> triple = std::make_tuple(10, 11, 12);
    sol::state lua;
    lua.set_function( "f_string", []() { return "this is a string!"; } );
    sol::function f_string = lua[ "f_string" ];
    
    // Make sure there are no overload collisions / compiler errors for automatic string conversions
    std::string f_string_result = f_string();
    REQUIRE(f_string_result == "this is a string!");
    f_string_result = f_string();
    REQUIRE(f_string_result == "this is a string!");

    lua.set_function("f", [] {
        return std::make_tuple(10, 11, 12);
    });
    lua.script("function g() return 10, 11, 12 end\nx,y,z = g()");
    std::tuple<int, int, int> tcpp = lua.get<sol::function>("f")();
    std::tuple<int, int, int> tlua = lua.get<sol::function>("g")();
    std::tuple<int, int, int> tluaget = lua.get<int, int, int>("x", "y", "z");
    std::cout << "cpp: " << std::get<0>(tcpp) << ',' << std::get<1>(tcpp) << ',' << std::get<2>(tcpp) << std::endl;
    std::cout << "lua: " << std::get<0>(tlua) << ',' << std::get<1>(tlua) << ',' << std::get<2>(tlua) << std::endl;
    std::cout << "lua xyz: " << lua.get<int>("x") << ',' << lua.get<int>("y") << ',' << lua.get<int>("z") << std::endl;
    REQUIRE(tcpp == triple);
    REQUIRE(tlua == triple);
    REQUIRE(tluaget == triple);
}

TEST_CASE("functions/sol::function-to-std::function", "check if conversion to std::function works properly and calls with correct arguments") {
    sol::state lua;
    lua.open_libraries(sol::lib::base);

    lua.set_function("testFunc", test_free_func);
    lua.set_function("testFunc2", test_free_func2);
    lua.script(
        "testFunc(function() print(\"hello std::function\") end)"
      );
    lua.script(
        "function m(a)\n"
        "     print(\"hello std::function with arg \", a)\n"
        "     return a\n"
        "end\n"
        "\n"
        "testFunc2(m, 1)"
      );
}

TEST_CASE("functions/returning-functions-from-C++-and-gettin-in-lua", "check to see if returning a functor and getting a functor from lua is possible") {
    sol::state lua;
    lua.open_libraries(sol::lib::base);

    lua.set_function("makefn", makefn);
    lua.set_function("takefn", takefn);
    lua.script("afx = makefn()\n"
                "print(afx())\n"
                "takefn(afx)\n");
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
    // test operator= for stringification
    // errors due to ambiguous operators
    bar = lua["bar"];

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
        double b = t["bar"];
       REQUIRE(a == "goodbye");
       REQUIRE(b == 20.4);
    };

    REQUIRE_NOTHROW(assert1(lua.global()));
}

TEST_CASE("tables/usertype", "Show that we can create classes from usertype and use them") {

    sol::state lua;

    sol::usertype<fuser> lc{ "add", &fuser::add, "add2", &fuser::add2 };
    lua.set_usertype(lc);

    lua.script("a = fuser:new()\n"
               "b = a:add(1)\n"
               "c = a:add2(1)\n");

    sol::object a = lua.get<sol::object>("a");
    sol::object b = lua.get<sol::object>("b");
    sol::object c = lua.get<sol::object>("c");
    REQUIRE((a.is<sol::userdata>()));
    auto atype = a.get_type();
    auto btype = b.get_type();
    auto ctype = c.get_type();
    REQUIRE((atype == sol::type::userdata));
    REQUIRE((btype == sol::type::number));
    REQUIRE((ctype == sol::type::number));
    int bresult = b.as<int>();
    int cresult = c.as<int>();
    REQUIRE(bresult == 1);
    REQUIRE(cresult == 3);
}

TEST_CASE("tables/usertype-constructors", "Show that we can create classes from usertype and use them with multiple constructors") {

    sol::state lua;

    sol::constructors<sol::types<>, sol::types<int>, sol::types<int, int>> con;
    sol::usertype<crapola::fuser> lc(con, "add", &crapola::fuser::add, "add2", &crapola::fuser::add2);
    lua.set_usertype(lc);

    lua.script(
        "a = crapola_fuser.new(2)\n"
        "u = a:add(1)\n"
        "v = a:add2(1)\n"
        "b = crapola_fuser:new()\n"
        "w = b:add(1)\n"
        "x = b:add2(1)\n"
        "c = crapola_fuser.new(2, 3)\n"
        "y = c:add(1)\n"
        "z = c:add2(1)\n");
    sol::object a = lua.get<sol::object>("a");
    auto atype = a.get_type();
    REQUIRE((atype == sol::type::userdata));
    sol::object u = lua.get<sol::object>("u");
    sol::object v = lua.get<sol::object>("v");
    REQUIRE((u.as<int>() == 3));
    REQUIRE((v.as<int>() == 5));

    sol::object b = lua.get<sol::object>("b");
    auto btype = b.get_type();
    REQUIRE((btype == sol::type::userdata));
    sol::object w = lua.get<sol::object>("w");
    sol::object x = lua.get<sol::object>("x");
    REQUIRE((w.as<int>() == 1));
    REQUIRE((x.as<int>() == 3));

    sol::object c = lua.get<sol::object>("c");
    auto ctype = c.get_type();
    REQUIRE((ctype == sol::type::userdata));
    sol::object y = lua.get<sol::object>("y");
    sol::object z = lua.get<sol::object>("z");
    REQUIRE((y.as<int>() == 7));
    REQUIRE((z.as<int>() == 9));
}

TEST_CASE("tables/usertype-utility", "Show internal management of classes registered through new_usertype") {
    sol::state lua;

    lua.new_usertype<fuser>("fuser", "add", &fuser::add, "add2", &fuser::add2);

    lua.script("a = fuser.new()\n"
        "b = a:add(1)\n"
        "c = a:add2(1)\n");

    sol::object a = lua.get<sol::object>("a");
    sol::object b = lua.get<sol::object>("b");
    sol::object c = lua.get<sol::object>("c");
    REQUIRE((a.is<sol::userdata>()));
    auto atype = a.get_type();
    auto btype = b.get_type();
    auto ctype = c.get_type();
    REQUIRE((atype == sol::type::userdata));
    REQUIRE((btype == sol::type::number));
    REQUIRE((ctype == sol::type::number));
    int bresult = b.as<int>();
    int cresult = c.as<int>();
    REQUIRE(bresult == 1);
    REQUIRE(cresult == 3);
}

TEST_CASE("tables/usertype-utility-derived", "usertype classes must play nice when a derived class does not overload a publically visible base function") {
    sol::state lua;
    lua.open_libraries(sol::lib::base);
    sol::constructors<sol::types<int>> basector;
    sol::usertype<Base> baseusertype(basector, "get_num", &Base::get_num);

    lua.set_usertype(baseusertype);

    lua.script("base = Base.new(5)");
    REQUIRE_NOTHROW(lua.script("print(base:get_num())"));

    sol::constructors<sol::types<int>> derivedctor;
    sol::usertype<Derived> derivedusertype(derivedctor, 
        "get_num_10", &Derived::get_num_10, 
        "get_num", &Derived::get_num
    );

    lua.set_usertype(derivedusertype);

    lua.script("derived = Derived.new(7)");
    Derived& derived = lua["derived"];
    lua.script("dgn = derived:get_num()\n"
        "print(dgn)");
    lua.script("dgn10 = derived:get_num_10()\n"
        "print(dgn10)");

    REQUIRE((lua.get<int>("dgn10") == 70));
    REQUIRE((lua.get<int>("dgn") == 7));
}

TEST_CASE("tables/self-referential usertype", "usertype classes must play nice when C++ object types are requested for C++ code") {
    sol::state lua;
    lua.open_libraries(sol::lib::base);

    lua.new_usertype<self_test>("test", "g", &self_test::g, "f", &self_test::f);

    lua.script(
        "local a = test.new()\n"
        "a:g(\"woof\")\n"
        "a:f(a)\n"
      );
}

TEST_CASE("tables/arbitrary-creation", "tables should be created from standard containers") {
    sol::state lua;
    lua.open_libraries(sol::lib::base);
    lua.set_function("test_one", test_table_return_one);
    lua.set_function("test_two", test_table_return_two);
    lua.set_function("test_three", test_table_return_three);

    REQUIRE_NOTHROW(lua.script("a = test_one()"));
    REQUIRE_NOTHROW(lua.script("b = test_two()"));
    REQUIRE_NOTHROW(lua.script("c = test_three()"));

    REQUIRE_NOTHROW(lua.script("assert(#a == 10, 'error')"));
    REQUIRE_NOTHROW(lua.script("assert(a[3] == 3, 'error')"));
    REQUIRE_NOTHROW(lua.script("assert(b.one == 1, 'error')"));
    REQUIRE_NOTHROW(lua.script("assert(b.three == 3, 'error')"));
    REQUIRE_NOTHROW(lua.script("assert(c.name == 'Rapptz', 'error')"));
    REQUIRE_NOTHROW(lua.script("assert(c.project == 'sol', 'error')"));

    auto&& a = lua.get<sol::table>("a");
    auto&& b = lua.get<sol::table>("b");
    auto&& c = lua.get<sol::table>("c");

    REQUIRE(a.size() == 10ULL);
    REQUIRE(a.get<int>(3) == 3);
    REQUIRE(b.get<int>("one") == 1);
    REQUIRE(b.get<int>("three") == 3);
    REQUIRE(c.get<std::string>("name") == "Rapptz");
    REQUIRE(c.get<std::string>("project") == "sol");
}

TEST_CASE("tables/for-each", "Testing the use of for_each to get values from a lua table") {
    sol::state lua;
    lua.open_libraries(sol::lib::base);

    lua.script("arr = {\n"
        "[0] = \"Hi\",\n"
        "[1] = 123.45,\n"
        "[2] = \"String value\",\n"
        // Does nothing
        //"[3] = nil,\n"
        //"[nil] = 3,\n"
        "[\"WOOF\"] = 123,\n"
        "}");
    sol::table tbl = lua[ "arr" ];
    std::size_t tablesize = 4;
    std::size_t iterations = 0;
    tbl.for_each(
        [&iterations](sol::object key, sol::object value) {
        ++iterations;
        sol::type keytype = key.get_type();
        switch (keytype) {
        case sol::type::number:
            switch (key.as<int>()) {
            case 0:
                REQUIRE((value.as<std::string>() == "Hi"));
                break;
            case 1:
                REQUIRE((value.as<double>() == 123.45));
                break;
            case 2:
                REQUIRE((value.as<std::string>() == "String value"));
                break;
            case 3:
                REQUIRE((value.is<sol::nil_t>()));
                break;
            }
            break;
        case sol::type::string:
            if (key.as<std::string>() == "WOOF") {
                REQUIRE((value.as<double>() == 123));
            }
            break;
        case sol::type::nil:
            REQUIRE((value.as<double>() == 3));
            break;
        default:
            break;
        }
    }
    );
    REQUIRE(iterations == tablesize);
}

TEST_CASE("tables/issue-number-twenty-five", "Using pointers and references from C++ classes in Lua") {
    struct test {
        int x = 0;
        test& set() {
            x = 10;
            return *this;
        }

        int get() {
            return x;
        }

        test* pget() {
            return this;
        }

        test create_get() {
            return *this;
        }

        int fun(int xa) {
            return xa * 10;
        }
    };

    sol::state lua;
    lua.open_libraries(sol::lib::base);
    lua.new_usertype<test>("test", "set", &test::set, "get", &test::get, "pointer_get", &test::pget, "fun", &test::fun, "create_get", &test::create_get);
    REQUIRE_NOTHROW(lua.script("x = test.new()"));
    REQUIRE_NOTHROW(lua.script("assert(x:set():get() == 10)"));
    REQUIRE_NOTHROW(lua.script("y = x:pointer_get()"));
    REQUIRE_NOTHROW(lua.script("y:set():get()"));
    REQUIRE_NOTHROW(lua.script("y:fun(10)"));
    REQUIRE_NOTHROW(lua.script("x:fun(10)"));
    REQUIRE_NOTHROW(lua.script("assert(y:fun(10) == x:fun(10), '...')"));
    REQUIRE_NOTHROW(lua.script("assert(y:fun(10) == 100, '...')"));
    REQUIRE_NOTHROW(lua.script("assert(y:set():get() == y:set():get(), '...')"));
    REQUIRE_NOTHROW(lua.script("assert(y:set():get() == 10, '...')"));
}

TEST_CASE("usertype/issue-number-thirty-five", "using value types created from lua-called C++ code, fixing user-defined types with constructors") {
    sol::state lua;
    lua.open_libraries(sol::lib::base);

    sol::constructors<sol::types<float, float, float>> ctor;
    sol::usertype<Vec> udata(ctor, "normalized", &Vec::normalized, "length", &Vec::length);
    lua.set_usertype(udata);

    REQUIRE_NOTHROW(lua.script("v = Vec.new(1, 2, 3)\n"
               "print(v:length())"));
    REQUIRE_NOTHROW(lua.script("v = Vec.new(1, 2, 3)\n"
               "print(v:normalized():length())" ));
}

TEST_CASE("usertype/lua-stored-usertype", "ensure usertype values can be stored without keeping usertype object alive") {
    sol::state lua;
    lua.open_libraries(sol::lib::base);

    {
        sol::constructors<sol::types<float, float, float>> ctor;
        sol::usertype<Vec> udata(ctor,
          "normalized", &Vec::normalized,
          "length",     &Vec::length);

        lua.set_usertype(udata);
        // usertype dies, but still usable in lua!
    }

    REQUIRE_NOTHROW(lua.script("collectgarbage()\n"
                "v = Vec.new(1, 2, 3)\n"
                "print(v:length())"));

    REQUIRE_NOTHROW(lua.script("v = Vec.new(1, 2, 3)\n"
                "print(v:normalized():length())" ));
}

TEST_CASE("usertype/member-variables", "allow table-like accessors to behave as member variables for usertype") {
    sol::state lua;
    lua.open_libraries(sol::lib::base);
    sol::constructors<sol::types<float, float, float>> ctor;
    sol::usertype<Vec> udata(ctor,
                             "x", &Vec::x,
                             "y", &Vec::y,
                             "z", &Vec::z,
                             "normalized", &Vec::normalized,
                             "length",     &Vec::length);
    lua.set_usertype(udata);

    REQUIRE_NOTHROW(lua.script("v = Vec.new(1, 2, 3)\n"
               "v2 = Vec.new(0, 1, 0)\n"
               "print(v:length())\n"
               ));
    REQUIRE_NOTHROW(lua.script("v.x = 2\n"
               "v2.y = 2\n"
               "print(v.x, v.y, v.z)\n"
               "print(v2.x, v2.y, v2.z)\n"
               ));
    REQUIRE_NOTHROW(lua.script("assert(v.x == 2)\n"
               "assert(v2.x == 0)\n"
               "assert(v2.y == 2)\n"
               ));
    REQUIRE_NOTHROW(lua.script("v.x = 3\n"
               "local x = v.x\n"
               "assert(x == 3)\n"
               ));
}

TEST_CASE("usertype/nonmember-functions", "let users set non-member functions that take unqualified T as first parameter to usertype") {
    sol::state lua;
    lua.open_libraries( sol::lib::base );

    lua.new_usertype<giver>( "giver",
                "gief_stuff", giver::gief_stuff,
                "gief", &giver::gief,
                "__tostring", [](const giver& t) {
                                    return std::to_string(t.a) + ": giving value";
                                }
    ).get<sol::table>( "giver" )
    .set_function( "stuff", giver::stuff );

    REQUIRE_NOTHROW(lua.script("giver.stuff()"));
    REQUIRE_NOTHROW(lua.script("t = giver.new()\n"
            "print(tostring(t))\n"
            "t:gief()\n"
            "t:gief_stuff(20)\n"));
    REQUIRE((lua.get<giver>("t").a == 20));
}

TEST_CASE("regressions/one", "issue number 48") {
    sol::state lua;
    lua.new_usertype<vars>("vars",
                           "boop", &vars::boop);
    REQUIRE_NOTHROW(lua.script("beep = vars.new()\n"
                               "beep.boop = 1"));
    // test for segfault
    auto my_var = lua.get<vars>("beep");
    REQUIRE(my_var.boop == 1);
    auto* ptr = &my_var;
    REQUIRE(ptr->boop == 1);
}

TEST_CASE("references/get-set", "properly get and set with std::ref semantics. Note that to get, we must not use Unqualified<T> on the type...") {
    sol::state lua;

    lua.new_usertype<vars>("vars",
                           "boop", &vars::boop);
    vars var{};
    vars rvar{};
    lua.set("beep", var);
    lua.set("rbeep", std::ref(rvar));
    auto& my_var = lua.get<vars>("beep");
    auto& ref_var = lua.get<std::reference_wrapper<vars>>("rbeep");
    vars& proxy_my_var = lua["beep"];
    std::reference_wrapper<vars> proxy_ref_var = lua["rbeep"];
    var.boop = 2;
    rvar.boop = 5;

    // Was return as a value: var must be diferent from "beep"
    REQUIRE_FALSE(std::addressof(var) == std::addressof(my_var));
    REQUIRE_FALSE(std::addressof(proxy_my_var) == std::addressof(var));
    REQUIRE((my_var.boop == 0));
    REQUIRE(var.boop != my_var.boop);
    
    REQUIRE(std::addressof(ref_var) == std::addressof(rvar));
    REQUIRE(std::addressof(proxy_ref_var.get()) == std::addressof(rvar));
    REQUIRE(rvar.boop == 5);
    REQUIRE(rvar.boop == ref_var.boop);
}

TEST_CASE("interop/null-to-nil-and-back", "nil should be the given type when a pointer from C++ is returned as nullptr, and nil should result in nullptr in connected C++ code") {
    sol::state lua;
    lua.open_libraries(sol::lib::base);

    lua.set_function("lol", []() -> int* {
        return nullptr;
    });
    lua.set_function("rofl", [](int* x) {
        std::cout << x << std::endl;
    });
    REQUIRE_NOTHROW(lua.script("x = lol()\n"
        "rofl(x)\n"
        "assert(x == nil)"));
}

TEST_CASE( "functions/function_result-protected_function_result", "Function result should be the beefy return type for sol::function that allows for error checking and error handlers" ) {
    sol::state lua;
    lua.open_libraries( sol::lib::base, sol::lib::debug );
    static const char errormessage1[] = "true error message";
    static const char errormessage2[] = "doodle";

    // Some function; just using a lambda to be cheap
    auto doom = []() {
        // Bypasses handler function: puts information directly into lua error
        throw std::runtime_error( errormessage1 );
    };
    auto luadoom = [&lua]() {
        // Does not bypass error function, will call it
        luaL_error( lua.lua_state(), "BIG ERROR MESSAGES!" );
    };
    auto specialhandler = []( std::string ) {
        return errormessage2;
    };

    lua.set_function( "doom", doom );
    lua.set_function( "luadoom", luadoom );
    lua.set_function( "cpphandler", specialhandler );
    lua.script(
        std::string( "function handler ( message )" )
        + "    return '" + errormessage2 + "'"
        + "end"
        );

    sol::protected_function func = lua[ "doom" ];
    sol::protected_function luafunc = lua[ "luadoom" ];
    sol::function luahandler = lua[ "handler" ];
    sol::function cpphandler = lua[ "cpphandler" ];
    func.error_handler = luahandler;
    luafunc.error_handler = cpphandler;

    sol::protected_function_result result1 = func();
    int test = lua_gettop(lua.lua_state());
    REQUIRE(!result1.valid());
    std::string errorstring = result1;
    REQUIRE(errorstring == errormessage1);
    
    sol::protected_function_result result2 = luafunc();
    REQUIRE(!result2.valid());
    errorstring = result2;
    REQUIRE(errorstring == errormessage2);
}

TEST_CASE("functions/destructor-tests", "Show that proper copies / destruction happens") {
    static int created = 0;
    static int destroyed = 0;
    static void* last_call = nullptr;
    static void* static_call = reinterpret_cast<void*>(0x01);
    typedef void(* fptr)();
    struct x { 
        x() {++created;}
        x(const x&) {++created;}
        x(x&&) {++created;}
        x& operator=(const x&) {return *this;}
        x& operator=(x&&) {return *this;}
        void func() {last_call = static_cast<void*>(this);};
        ~x () {++destroyed;} 
    };
    struct y { 
        y() {++created;}
        y(const x&) {++created;}
        y(x&&) {++created;}
        y& operator=(const x&) {return *this;}
        y& operator=(x&&) {return *this;}
        static void func() {last_call = static_call;};
        void operator()() {func();}
        operator fptr () { return func; }
        ~y () {++destroyed;} 
    };

    // stateful functors/member functions should always copy unless specified
    {
        created = 0;
        destroyed = 0;
        last_call = nullptr;
        {
            sol::state lua;
            x x1;
            lua.set_function("x1copy", &x::func, x1);
            lua.script("x1copy()");
            REQUIRE(created == 2);
            REQUIRE(destroyed == 0);
            REQUIRE_FALSE(last_call == &x1);

            lua.set_function("x1ref", &x::func, std::ref(x1));
            lua.script("x1ref()");
            REQUIRE(created == 2);
            REQUIRE(destroyed == 0);
            REQUIRE(last_call == &x1);
        }
        REQUIRE(created == 2);
        REQUIRE(destroyed == 2);
        REQUIRE(created == destroyed);
    }

    // things convertible to a static function should _never_ be forced to make copies
    // therefore, pass through untouched
    {
        created = 0;
        destroyed = 0;
        last_call = nullptr;
        {
            sol::state lua;
            y y1;
            lua.set_function("y1copy", y1);
            lua.script("y1copy()");
            REQUIRE(created == 1);
            REQUIRE(destroyed == 0);
            REQUIRE(last_call == static_call);
            
            last_call = nullptr;
            lua.set_function("y1ref", std::ref(y1));
            lua.script("y1ref()");
            REQUIRE(created == 1);
            REQUIRE(destroyed == 0);
            REQUIRE(last_call == static_call);
        }
        REQUIRE(created == 1);
        REQUIRE(destroyed == 2);
    }
}

TEST_CASE("usertype/destructor-tests", "Show that proper copies / destruction happens") {
    static int created = 0;
    static int destroyed = 0;
    static void* last_call = nullptr;
    struct x { 
        x() {++created;}
        x(const x&) {++created;}
        x(x&&) {++created;}
        x& operator=(const x&) {return *this;}
        x& operator=(x&&) {return *this;}
        ~x () {++destroyed;} 
    };
    {
        sol::state lua;
        lua.new_usertype<x>("x");
        x x1;
        x x2;
        lua.set("x1copy", x1, "x2copy", x2, "x1ref", std::ref(x1));
        x& x1copyref = lua["x1copy"];
        x& x2copyref = lua["x2copy"];
        x& x1ref = lua["x1ref"];
        REQUIRE(created == 4);
        REQUIRE(destroyed == 0);
        REQUIRE(std::addressof(x1) == std::addressof(x1ref));
    }
    REQUIRE(created == 4);
    REQUIRE(destroyed == 4);
    REQUIRE(created == destroyed);
}

TEST_CASE("functions/overloading", "Check if overloading works properly for regular set function syntax") {
    sol::state lua;
    lua.open_libraries(sol::lib::base);

    lua.set_function("func_1", func_1);
    lua.set_function("func", sol::overload(func_1, func_1s, func_2, func_3));

    const std::string string_bark = "string: bark";

    REQUIRE_NOTHROW(lua.script(
"a = func(1)\n"
"b = func('bark')\n"
"c = func(1,2)\n"
"func(1,2,3)\n"
));

    REQUIRE((lua["a"] == 1));
    REQUIRE((lua["b"] == string_bark));
    REQUIRE((lua["c"] == 2));

    REQUIRE_THROWS(lua.script("func(1,2,'meow')"));
}

TEST_CASE("usertype/private-constructible", "Check to make sure special snowflake types from Enterprise thingamahjongs work properly.") {
    int numsaved = factory_test::num_saved;
    int numkilled = factory_test::num_killed;
    {
        sol::state lua;
        lua.open_libraries(sol::lib::base);

        lua.new_usertype<factory_test>("factory_test",
           "new", sol::constructor(factory_test::save),
           "__gc", sol::destructor(factory_test::kill),
          "a", &factory_test::a
        );
    
        std::unique_ptr<factory_test, factory_test::deleter> f = factory_test::make();
        lua.set("true_a", factory_test::true_a, "f", f.get());
        REQUIRE_NOTHROW(lua.script("assert(f.a == true_a)"));

        REQUIRE_NOTHROW(lua.script(
"local fresh_f = factory_test:new()\n"
"assert(fresh_f.a == true_a)\n"));
    }
    int expectednumsaved = numsaved + 1;
    int expectednumkilled = numkilled + 1;
    REQUIRE(expectednumsaved == factory_test::num_saved);
    REQUIRE(expectednumkilled == factory_test::num_killed);
}

TEST_CASE("usertype/overloading", "Check if overloading works properly for usertypes") {
    struct woof {
        int var;

        int func(int x) {
            return var + x;
        }

        double func2(int x, int y) {
            return var + x + y + 0.5;
        }

        std::string func2s(int x, std::string y) {
            return y + " " + std::to_string(x);
        }
    };
    sol::state lua;
    lua.open_libraries(sol::lib::base);

    lua.new_usertype<woof>("woof",
        "var", &woof::var,
        "func", sol::overload(&woof::func, &woof::func2, &woof::func2s)
    );
    
    const std::string bark_58 = "bark 58";
    
    REQUIRE_NOTHROW(lua.script(
"r = woof:new()\n"
"a = r:func(1)\n"
"b = r:func(1, 2)\n"
"c = r:func(58, 'bark')\n"
));
    REQUIRE((lua["a"] == 1));
    REQUIRE((lua["b"] == 3.5));
    REQUIRE((lua["c"] == bark_58));

    REQUIRE_THROWS(lua.script("r:func(1,2,'meow')"));
}

TEST_CASE("threading/coroutines", "ensure calling a coroutine works") {
    const auto& script = R"(counter = 20
 
function loop()
    while counter ~= 30
    do
        print("Sending " .. counter);
        coroutine.yield(counter);
        counter = counter + 1;
    end
    print("Sending " .. counter);
    return counter
end
)";

    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::coroutine);
    lua.script(script);
    sol::coroutine cr = lua["loop"];

    int counter;
    for (counter = 20; counter < 31 && cr; ++counter) {
        int x = cr();
        if (counter != x) {
            throw std::logic_error("fuck");
        }
    }
    counter -= 1;
    REQUIRE(counter == 30);
}

TEST_CASE("threading/new-thread-coroutines", "ensure calling a coroutine works") {
    const auto& script = R"(counter = 20
 
function loop()
    while counter ~= 30
    do
        print("Sending " .. counter);
        coroutine.yield(counter);
        counter = counter + 1;
    end
    print("Sending " .. counter);
    return counter
end
)";

    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::coroutine);
    lua.script(script);
    sol::thread runner = sol::thread::create(lua.lua_state());
    sol::state_view runnerstate = runner.state();
    sol::coroutine cr = lua["loop"];

    int counter;
    for (counter = 20; counter < 31 && cr; ++counter) {
        int x = cr();
        if (counter != x) {
            throw std::logic_error("fuck");
        }
    }
    counter -= 1;
    REQUIRE(counter == 30);
}

TEST_CASE("issues/stack-overflow", "make sure various operations repeated don't trigger stack overflow") {
    sol::state lua;
    lua.script("t = {};t[0]=20");
    lua.script("lua_function=function(i)return i;end");
    
    sol::function f = lua["lua_function"];
    std::string teststring = "testtext";
    REQUIRE_NOTHROW(
    for (int i = 0; i < 1000000; ++i) {
        std::string result = f(teststring);
        if (result != teststring) throw std::logic_error("RIP");
    }
    );
    sol::table t = lua["t"];
    int expected = 20;
    REQUIRE_NOTHROW(
    for (int i = 0; i < 1000000; ++i) {
        int result = t[0];
        t.size();
        if (result != expected)
            throw std::logic_error("RIP");
    }
    );
}
