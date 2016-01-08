#define NONIUS_RUNNER
#include "nonius/nonius.h++"
#include "sol.hpp"

struct sol_function_result_bench {
	void operator () ( nonius::chronometer meter ) const {
		sol::state lua;
		lua.script( R"(
function r ()
	return 1 + 1		
end
)" );
		sol::function r = lua[ "r" ];
		meter.measure( [ & ] ( int run_index ) {
			// Measure cost of lua calling and returning a function
			return r( );
		} );
	}
};

struct sol_direct_bench {
	void operator () ( nonius::chronometer meter ) const {
		sol::state lua;
		lua.script( R"(
function r ()
	return 1 + 1		
end
)" );
		auto fun = [ & ] ( int run_index ) {
			// Measure cost of lua calling and returning a function
			sol::function r = lua[ "r" ];
			return r.call<int>( );
		};
		meter.measure( fun );
	}
};

struct c_direct_bench {
	void operator () ( nonius::chronometer meter ) const {
		sol::state lua;
		lua.script( R"(
function r ()
	return 1 + 1		
end
)" );

		lua_State* L = lua.lua_state( );
		meter.measure( [ & ] ( int run_index ) {
			lua_getglobal( L, "r" );
			lua_call( L, 0, 1 );
			int lua_out = (int)lua_tonumber( L, -1 );
			lua_pop( L, 1 );
			return lua_out;
		} );
	}
};

NONIUS_BENCHMARK( "sol - function_result", sol_function_result_bench() );
NONIUS_BENCHMARK( "sol - int", sol_direct_bench() );
NONIUS_BENCHMARK( "C - int", c_direct_bench( ) );
