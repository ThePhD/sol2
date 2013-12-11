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

void run_script(sol::state& lua) {
    lua.script("assert(os.fun() == \"test\")\n"
        "assert(os.name == \"windows\")");
}

TEST_CASE( "simple/set_global", "Check if the set_global works properly." ) {
	sol::state lua;

	lua.set( "a", 9 );
	REQUIRE_NOTHROW( lua.script( "if a ~= 9 then error('wrong value') end" ) );

	lua.set( "d", "hello" );
	REQUIRE_NOTHROW( lua.script( "if d ~= 'hello' then error('expected \\'hello\\', got '.. tostring(d)) end" ) );

	lua.set( "e", std::string( "hello" ) );
	REQUIRE_NOTHROW( lua.script( "if d ~= 'hello' then error('expected \\'hello\\', got '.. tostring(d)) end" ) );

	lua.set( "f", true );
	REQUIRE_NOTHROW( lua.script( "if f ~= true then error('wrong value') end" ) );
}

TEST_CASE( "simple/get", "Tests if the get function works properly." ) {
	sol::state lua;

	lua.script( "a = 9" );
	auto a = lua.get<int>( "a" );
	REQUIRE( a == 9.0 );

	lua.script( "b = nil" );
	REQUIRE_NOTHROW( auto b = lua.get<sol::nil_t>( "b" ) );
	
	lua.script( "d = 'hello'" );
	auto d = lua.get<std::string>( "d" );
	REQUIRE( d == "hello" );

	lua.script( "e = true" );
	auto e = lua.get<bool>( "e" );
	REQUIRE( e == true );
}

TEST_CASE( "simple/addition", "" ) {
	sol::state lua;

	lua.set( "b", 0.2 );
	lua.script( "c = 9 + b" );
	auto c = lua.get<double>( "c" );

	REQUIRE( c == 9.2 );
}

/*
	lua.get<sol::table>( "os" ).set( "name", "windows" );

	SECTION( "" )
	lua.get<sol::table>( "os" ).set_function( "fun",
		[ ] ( ) {
		std::cout << "stateless lambda()" << std::endl;
		return "test";
	}
	);
	run_script( lua );

	lua.get<sol::table>( "os" ).set_function( "fun", &free_function );
	run_script( lua );

	// l-value, can optomize
	auto lval = object( );
	lua.get<sol::table>( "os" ).set_function( "fun", &object::operator(), lval );
	run_script( lua );

	// Tests will begin failing here
	// stateful lambda: non-convertible, unoptomizable
	int breakit = 50;
	lua.get<sol::table>( "os" ).set_function( "fun",
		[ &breakit ] ( ) {
		std::cout << "stateless lambda()" << std::endl;
		return "test";
	}
	);
	run_script( lua );

	// r-value, cannot optomize
	lua.get<sol::table>( "os" ).set_function( "fun", &object::operator(), object( ) );
	run_script( lua );

	// r-value, cannot optomize
	auto rval = object( );
	lua.get<sol::table>( "os" ).set_function( "fun", &object::operator(), std::move( rval ) );
	run_script( lua );

}
*/
