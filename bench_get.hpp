#pragma once

#include "sol.hpp"
#include "nonius/nonius.h++"

static sol::state prepare_get_state( ) {
	sol::state lua;
	lua.set( "r", 25 );
	return lua;
}

struct sol_get_proxy_bench {
	void operator () ( nonius::chronometer meter ) const {
		sol::state lua = prepare_get_state( );
		auto measurement = [ & ] ( int run_index ) {
			int value = lua[ "r" ];
			return value;
		};
		meter.measure( measurement );
	}
};

struct sol_get_typed_bench {
	void operator () ( nonius::chronometer meter ) const {
		sol::state lua = prepare_get_state( );
		auto measurement = [ & ] ( int run_index ) {
			int value = lua.get<int>( "r" );
			return value;
		};
		meter.measure( measurement );
	}
};

struct c_get_bench {
	void operator () ( nonius::chronometer meter ) const {
		sol::state lua = prepare_get_state( );
		lua_State* L = lua.lua_state( );
		auto measurement = [ & ] ( int run_index ) {
			lua_getglobal( L, "r" );
			int value = (int)lua_tonumber( L, -1 );
			lua_pop( L, 1 );
			return value;
		};
		meter.measure( measurement );
	}
};

void bench_get( const std::string& dir, std::string& configurationname, const std::string& platformname ) {
	nonius::configuration cfg;
	cfg.output_file = dir + "sol.get - " + configurationname + " " + platformname + ".html";
	cfg.title = "sol get (" + configurationname + " " + platformname + ")";
	cfg.samples = 100;
	nonius::benchmark benchmarks [] = {
		nonius::benchmark( "sol - proxy get", sol_get_proxy_bench( ) ),
		nonius::benchmark( "sol - typed get", sol_get_typed_bench( ) ),
		nonius::benchmark( "plain C", c_get_bench( ) ),
	};
	nonius::go( cfg, std::begin( benchmarks ), std::end( benchmarks ), nonius::html_reporter( ) );
}
