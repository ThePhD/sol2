#define SOL_CHECK_ARGUMENTS

#include <catch.hpp>
#include <sol.hpp>
#include <iostream>

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

struct A {
    int a = 0xA; int bark() { return 1; }
};

std::tuple<int, int> bark(int num_value, A* a) {
    return std::tuple<int, int>(num_value * 2, a->bark());
}

void test_free_func(std::function<void()> f) {
    f();
}

void test_free_func2(std::function<int(int)> f, int arg1) {
    int val = f(arg1);
    if(val != arg1)
        throw sol::error("failed function call!");
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
    lua.set_function("overloaded", sol::resolve<int(int)>(overloaded));
    REQUIRE_NOTHROW(lua.script("print(overloaded(1))"));

    lua.set_function("overloaded", sol::resolve<int(int, int)>(overloaded));
    REQUIRE_NOTHROW(lua.script("print(overloaded(1, 2))"));

    lua.set_function("overloaded", sol::resolve<int(int, int, int)>(overloaded));
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

TEST_CASE("functions/pair-and-tuple-and-proxy-tests", "Check if sol::reference and sol::proxy can be passed to functions as arguments") {
    sol::state lua;
    lua.new_usertype<A>("A",
        "bark", &A::bark);
    lua.script(R"( function f (num_value, a)
    return num_value * 2, a:bark()
end 
nested = { variables = { no = { problem = 10 } } } )");
    lua.set_function("g", bark);

    sol::function cpp_bark = lua["g"];
    sol::function lua_bark = lua["f"];
    
    sol::reference lua_variable_x = lua["nested"]["variables"]["no"]["problem"];
    A cpp_variable_y;
    
    std::tuple<int, int> ab = cpp_bark(lua_variable_x, cpp_variable_y);
    std::pair<int, int> cd = lua_bark(lua["nested"]["variables"]["no"]["problem"], cpp_variable_y);
    
    static const std::tuple<int, int> abdesired( 20, 1 );
    static const std::pair<int, int> cddesired = { 20, 1 };
    REQUIRE(ab == abdesired);
    REQUIRE(cd == cddesired);
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

TEST_CASE( "functions/function_result-protected_function_result", "Function result should be the beefy return type for sol::function that allows for error checking and error handlers" ) {
    sol::state lua;
    lua.open_libraries( sol::lib::base, sol::lib::debug );
    static const char unhandlederrormessage[] = "true error message";
    static const char handlederrormessage[] = "doodle";

    // Some function; just using a lambda to be cheap
    auto doomfx = []() {
        std::cout << "doomfx called" << std::endl;
        throw std::runtime_error( unhandlederrormessage );
    };
    auto luadoomfx = [&lua]() {
        std::cout << "luadoomfx called" << std::endl;
        // Does not bypass error function, will call it
        luaL_error( lua.lua_state(), unhandlederrormessage );
    };
    lua.set_function("doom", doomfx);
    lua.set_function("luadoom", luadoomfx);

    auto cpphandlerfx = []( std::string x ) {
        std::cout << "c++ handler called with: " << x << std::endl;
        return handlederrormessage;
    };
    lua.set_function( "cpphandler", cpphandlerfx );
    lua.script(
        std::string( "function luahandler ( message )" )
        + "    print('lua handler called with: ' .. message)"
        + "    return '" + handlederrormessage + "'"
        + "end"
        );
    auto nontrampolinefx = [](lua_State*) -> int { throw "x";};
    lua_CFunction c_nontrampolinefx = nontrampolinefx;
    lua.set("nontrampoline", c_nontrampolinefx);

    sol::protected_function doom = lua[ "doom" ];
    sol::protected_function luadoom = lua["luadoom"];
    sol::protected_function nontrampoline = lua["nontrampoline"];
    sol::function luahandler = lua["luahandler"];
    sol::function cpphandler = lua[ "cpphandler" ];
    doom.error_handler = luahandler;
    luadoom.error_handler = cpphandler;
    nontrampoline.error_handler = cpphandler;
    
    {
        sol::protected_function_result result = doom();
        REQUIRE(!result.valid());
        std::string errorstring = result;
        REQUIRE(errorstring == handlederrormessage);
    }
    {
        sol::protected_function_result result = luadoom();
        REQUIRE(!result.valid());
        std::string errorstring = result;
        REQUIRE(errorstring == handlederrormessage);
    }
    {
        sol::protected_function_result result = nontrampoline();
        REQUIRE(!result.valid());
        std::string errorstring = result;
        REQUIRE(errorstring == handlederrormessage);
    }
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
        REQUIRE(destroyed == 1);
    }
}


TEST_CASE("functions/all-kinds", "Register all kinds of functions, make sure they all compile and work") {
    sol::state lua;

    struct test_1 {
        int a = 0xA;
        virtual int bark() {
            return a;
        }

        int bark_mem() {
            return a;
        }

        static std::tuple<int, int> x_bark(int num_value, test_1* a) {
            return std::tuple<int, int>(num_value * 2, a->a);
        }
    };

    struct test_2 {
        int a = 0xC;
        int bark() { 
            return 20; 
        }
    };

    auto a = []() { return 500; };
    auto b = [&]() { return 501; };
    auto c = [&]() { return 502; };
    auto d = []() { return 503; };

    lua.new_usertype<test_1>("test_1",
        "bark", sol::c_call<decltype(&test_1::bark_mem), &test_1::bark_mem>
        );
    lua.new_usertype<test_2>("test_2",
        "bark", sol::c_call<decltype(&test_2::bark), &test_2::bark>
        );
    test_2 t2;

    lua.set_function("a", a);
    lua.set_function("b", b);
    lua.set_function("c", std::ref(c));
    lua.set_function("d", std::ref(d));
    lua.set_function("f", &test_1::bark);
    lua.set_function("g", test_1::x_bark);
    lua.set_function("h", sol::c_call<decltype(&test_1::bark_mem), &test_1::bark_mem>);
    lua.set_function("i", &test_2::bark, test_2());
    lua.set_function("j", &test_2::a, test_2());
    lua.set_function("k", &test_2::a);
    lua.set_function("l", sol::c_call<decltype(&test_1::a), &test_1::a>);
    lua.set_function("m", &test_2::a, &t2);

    lua.script(R"(
o1 = test_1.new()
o2 = test_2.new()

ob = o1:bark()

A = a()
B = b()
C = c()
D = d()
F = f(o1)
G0, G1 = g(2, o1)
H = h(o1)
I = i(o1)
I = i(o1)

J0 = j()
j(24)
J1 = j()

K0 = k(o2)
k(o2, 1024)
K1 = k(o2)

L0 = l(o1)
l(o1, 678)
L1 = l(o1)


M0 = m()
m(256)
M1 = m()
    )");
    int ob, A, B, C, D, F, G0, G1, H, I, J0, J1, K0, K1, L0, L1, M0, M1;
    std::tie( ob, A, B, C, D, F, G0, G1, H, I, J0, J1, K0, K1, L0, L1, M0, M1 ) 
    = lua.get<int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int>(
        "ob", "A", "B", "C", "D", "F", "G0", "G1", "H", "I", "J0", "J1", "K0", "K1", "L0", "L1", "M0", "M1"
    );
   
    REQUIRE(ob == 0xA);
    
    REQUIRE( A == 500 );
    REQUIRE( B == 501 );
    REQUIRE( C == 502 );
    REQUIRE( D == 503 );

    REQUIRE( F == 0xA );
    REQUIRE( G0 == 4 );
    REQUIRE( G1 == 0xA );
    REQUIRE( H == 0xA );
    REQUIRE( I == 20 );

    REQUIRE( J0 == 0xC );
    REQUIRE( J1 == 24 );

    REQUIRE( K0 == 0xC );
    REQUIRE( K1 == 1024 );

    REQUIRE( L0 == 0xA );
    REQUIRE( L1 == 678 );

    REQUIRE( M0 == 0xC );
    REQUIRE( M1 == 256 );
}
