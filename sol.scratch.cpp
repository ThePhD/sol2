#include <sol.hpp>
#include <iostream>
#include <tuple>

std::string free_func_yo() {
    std::cout << "free_func_yo()" << std::endl;
    return "test";
}

struct member_test {

    std::string operator() () {
        std::cout << "member_test()" << std::endl;
        return "test";
    }

};

void run_test(sol::state& lua) {
    lua.script("assert(os.fun() == \"test\")\n"
        "assert(os.name == \"windows\")");
}

int main( ) {
	sol::state lua;
	lua.open_libraries( sol::lib::base, sol::lib::os );

	lua.get<sol::table>( "os" ).set( "name", "windows" );

	lua.get<sol::table>( "os" ).set_function( "fun",
		[ ] ( ) {
		std::cout << "stateless lambda()" << std::endl;
		return "test";
	}
	);
	run_test( lua );

	lua.get<sol::table>( "os" ).set_function( "fun", &free_func_yo );
	run_test( lua );

	// l-value, can optomize
	auto lval = member_test( );
	lua.get<sol::table>( "os" ).set_function( "fun", &member_test::operator(), lval );
	run_test( lua );

	// Tests will begin failing here
	// stateful lambda: non-convertible, unoptomizable
	int breakit = 50;
	lua.get<sol::table>( "os" ).set_function( "fun",
		[ &breakit ] ( ) {
		std::cout << "stateless lambda()" << std::endl;
		return "test";
	}
	);
	run_test( lua );

	// r-value, cannot optomize
	lua.get<sol::table>( "os" ).set_function( "fun", &member_test::operator(), member_test( ) );
	run_test( lua );

	// r-value, cannot optomize
	auto rval = member_test( );
	lua.get<sol::table>( "os" ).set_function( "fun", &member_test::operator(), std::move( rval ) );
	run_test( lua );

}