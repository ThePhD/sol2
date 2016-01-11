#pragma once

#include "sol.hpp"
#include "nonius/nonius.h++"

static sol::state prepare_set_state( ) {
	sol::state lua;
	lua.set( "r", 25 );
	return lua;
}

struct sol_set_proxy_bench {
	void operator () ( nonius::chronometer meter ) const {
		sol::state lua = prepare_set_state( );
		auto measurement = [ & ] ( int run_index ) {
			lua[ "r" ] = 40;
		};
		meter.measure( measurement );
	}
};

struct sol_set_typed_bench {
	void operator () ( nonius::chronometer meter ) const {
		sol::state lua = prepare_set_state( );
		auto measurement = [ & ] ( int run_index ) {
			lua.set( "r", 40 );
		};
		meter.measure( measurement );
	}
};

struct c_set_bench {
	void operator () ( nonius::chronometer meter ) const {
		sol::state lua = prepare_set_state( );
		lua_State* L = lua.lua_state( );
		auto measurement = [ & ] ( int run_index ) {
			lua_pushinteger( L, 40 );
			lua_setglobal( L, "r" );
		};
		meter.measure( measurement );
	}
};

void bench_set( const std::string& dir, std::string& configurationname, const std::string& platformname ) {
	nonius::configuration cfg;
	cfg.output_file = dir + "sol.set - " + configurationname + " " + platformname + ".html";
	cfg.title = "sol set (" + configurationname + " " + platformname + ")";
	cfg.samples = 100;
	nonius::benchmark benchmarks [] = {
		nonius::benchmark( "sol - proxy set", sol_set_proxy_bench( ) ),
		nonius::benchmark( "sol - typed set", sol_set_typed_bench( ) ),
		nonius::benchmark( "plain C", c_set_bench( ) ),
	};
	nonius::go( cfg, std::begin( benchmarks ), std::end( benchmarks ), nonius::html_reporter( ) );
}
