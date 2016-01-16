#pragma once

#include "sol.hpp"
#include "nonius/nonius.h++"

struct woof {
	int var;

	int func( int x ) {
		return var + x;
	}

	int operator + (int x) const {
		return var + x;
	}
};

static sol::state prepare_usertype_state( ) {
	sol::state lua;
	lua.new_usertype<woof>( "woof",
		"var", &woof::var,
		"func", &woof::func,
		sol::meta_function::addition, &woof::operator+ );
	lua.script( R"(
r = woof:new()
)" );
	return lua;
}

struct sol_usertype_special_bench {
	void operator () ( nonius::chronometer meter ) const {
		sol::state lua = prepare_usertype_state( );
		auto measurement = [ & ] ( int run_index ) {
			lua.script("x = r + 2");
		};
		meter.measure( measurement );
	}
};

struct sol_usertype_regular_bench {
	void operator () ( nonius::chronometer meter ) const {
		sol::state lua = prepare_usertype_state( );
		auto measurement = [ & ] ( int run_index ) {
			lua.script("x = r:func( 2 )");
		};
		meter.measure( measurement );
	}
};

struct sol_usertype_variable_bench {
	void operator () ( nonius::chronometer meter ) const {
		sol::state lua = prepare_usertype_state( );
		auto measurement = [ & ] ( int run_index ) {
			lua.script("x = r.var + 2");
		};
		meter.measure( measurement );
	}
};

struct c_usertype_special_bench {
	void operator () ( nonius::chronometer meter ) const {
		sol::state lua = prepare_usertype_state( );
		lua_State* L = lua.lua_state( );
		auto measurement = [ & ] ( int run_index ) {
			
		};
		meter.measure( measurement );
	}
};

struct c_usertype_regular_bench {
	void operator () ( nonius::chronometer meter ) const {
		sol::state lua = prepare_usertype_state( );
		lua_State* L = lua.lua_state( );
		auto measurement = [ & ] ( int run_index ) {
		};
		meter.measure( measurement );
	}
};

struct c_usertype_variable_bench {
	void operator () ( nonius::chronometer meter ) const {
		sol::state lua = prepare_usertype_state( );
		lua_State* L = lua.lua_state( );
		auto measurement = [ & ] ( int run_index ) {
		};
		meter.measure( measurement );
	}
};

void bench_usertype( const std::string& dir, std::string& configurationname, const std::string& platformname ) {
	nonius::configuration cfg;
	cfg.output_file = dir + "sol.usertype - " + configurationname + " " + platformname + ".html";
	cfg.title = "sol usertype (" + configurationname + " " + platformname + ")";
	cfg.samples = 100;
	nonius::benchmark benchmarks [] = {
		nonius::benchmark( "sol - usertype special", sol_usertype_special_bench( ) ),
		nonius::benchmark( "sol - usertype regular", sol_usertype_regular_bench( ) ),
		nonius::benchmark( "sol - usertype variable", sol_usertype_variable_bench( ) ),
		//nonius::benchmark( "plain C - usertype special", c_usertype_special_bench( ) ),
		//nonius::benchmark( "plain C - usertype regular", c_usertype_regular_bench( ) ),
		//nonius::benchmark( "plain C - usertype variable", c_usertype_variable_bench( ) ),
	};
	nonius::go( cfg, std::begin( benchmarks ), std::end( benchmarks ), nonius::html_reporter( ) );
}
