#define SOL_CHECK_ARGUMENTS
#include <sol.hpp>

#include <cassert>
#include <iostream>

int main(int, char**) {
	sol::state lua;
	lua.open_libraries(sol::lib::base);

	// Environments can set on functions (scripts), userdata and threads
	// let's look at functions

	lua.script("f = function() return test end");
	sol::function f = lua["f"];
	
	sol::environment env(lua, sol::create);
	env["test"] = 5;
	sol::set_environment(env, f);

	// the function returns the value from the environment table
	int result = f();
	assert(result == 5);

	// You can also protect from variables
	// being set without the 'local' specifier
	lua.script("g = function() test = 5 end");
	sol::function g = lua["g"];
	sol::environment env_g(lua, sol::create);
	env_g.set_on(g); // same as set_environment

	g();
	// the value can be retrieved from the env table
	int test = env_g["test"];
	assert(test == 5);


	// the global environment
	// is not polluted at all, despite both functions being used and set
	sol::object global_test = lua["test"];
	assert(!global_test.valid());

	return 0;
}
