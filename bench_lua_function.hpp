#pragma once

#include "sol.hpp"
#include "nonius/nonius.h++"

static sol::state prepare_lua_function_state( ) {
	sol::state lua;
	lua.script( R"(
function r ()
	return 1 + 1
end
)" );
	return lua;
}

struct sol_function_result_lua_bench {
	void operator () ( nonius::chronometer meter ) const {
		sol::state lua = prepare_lua_function_state( );
		auto measurement = [ & ] ( int run_index ) {
			sol::function r = lua[ "r" ];
			int value = r( );
			return value;
		};
		meter.measure( measurement );
	}
};

struct sol_call_lua_bench {
	void operator () ( nonius::chronometer meter ) const {
		sol::state lua = prepare_lua_function_state( );
		auto measurement = [ & ] ( int run_index ) {
			sol::function r = lua[ "r" ];
			int value = r.call<int>( );
			return value;
		};
		meter.measure( measurement );
	}
};

struct sol_protected_function_result_lua_bench {
	void operator () ( nonius::chronometer meter ) const {
		sol::state lua = prepare_lua_function_state( );
		auto measurement = [ & ] ( int run_index ) {
			sol::protected_function r = lua[ "r" ];
			int value = r( );
			return value;
		};
		meter.measure( measurement );
	}
};

struct sol_protected_call_lua_bench {
	void operator () ( nonius::chronometer meter ) const {
		sol::state lua = prepare_lua_function_state( );
		auto measurement = [ & ] ( int run_index ) {
			sol::protected_function r = lua[ "r" ];
			int value = r.call<int>( );
			return value;
		};
		meter.measure( measurement );
	}
};

struct c_direct_lua_bench {
	void operator () ( nonius::chronometer meter ) const {
		sol::state lua = prepare_lua_function_state( );
		lua_State* L = lua.lua_state( );
		auto measurement = [ & ] ( int run_index ) {
			lua_getglobal( L, "r" );
			lua_call( L, 0, 1 );
			int value = (int)lua_tonumber( L, -1 );
			lua_pop( L, 1 );
			return value;
		};
		meter.measure( measurement );
	}
};

void bench_lua_function( const std::string& dir,  std::string& configurationname, const std::string& platformname ) {
	nonius::configuration cfg;
	cfg.output_file = dir + "sol.functions (lua source) - " + configurationname + " " + platformname + ".html";
	cfg.title = "sol::function (lua source) (" + configurationname + " " + platformname + ")";
	cfg.samples = 100;
	nonius::benchmark benchmarks[] = {
		nonius::benchmark("function - function_result", sol_function_result_lua_bench()),
		nonius::benchmark("function - call<>", sol_call_lua_bench()),
		nonius::benchmark("protected_function - function_result", sol_protected_function_result_lua_bench()),
		nonius::benchmark("protected_function - call<>", sol_protected_call_lua_bench()),
		nonius::benchmark( "plain C", c_direct_lua_bench( ) ),
	};
	nonius::go( cfg, std::begin( benchmarks ), std::end( benchmarks ), nonius::html_reporter( ) );
}
