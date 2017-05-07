#define SOL_CHECK_ARGUMENTS
#include <sol.hpp>

#include <cassert>
#include <iostream>

// NOTE:
// THIS IS A LOW-LEVEL EXAMPLE, using pieces of sol2
// to facilitate better usage
// If you need to do this often, you can copy this code and paste it in a utility function for yourself
// so you can grab the environment whenever you need to

void some_function_called_by_sol2(sol::this_state ts) {
	lua_State* L = ts;

	lua_Debug info;
	// Level 0 means current function (this C function, which is useless for our purposes)
	// Level 1 means next call frame up the stack. This is probably the environment we're looking for?
	int level = 1;
	int pre_stack_size = lua_gettop(L);
	if (lua_getstack(L, level, &info) != 1) {
		// failure: call it quits
		std::cout << "error: unable to traverse the stack" << std::endl;
		lua_settop(L, pre_stack_size);
		return;
	}
	// the "f" identifier is the most important here
	// it pushes the function running at `level` onto the stack:
	// we can get the environment from this
	// the rest is for printing / debugging purposes
	if (lua_getinfo(L, "fnluS", &info) == 0) {
		// failure?
		std::cout << "error: unable to get stack information" << std::endl;
		lua_settop(L, pre_stack_size);
		return;
	}

	// Okay, so all the calls worked.
	// Print out some information about this "level"
	std::cout << "[" << level << "] " << info.short_src << ":" << info.currentline
		<< " -- " << (info.name ? info.name : "<unknown>") << "[" << info.what << "]" << std::endl;

	// Grab the function off the top of the stack
	// remember: -1 means top, -2 means 1 below the top, and so on...
	// 1 means the very bottom of the stack, 
	// 2 means 1 more up, and so on to the top value...
	sol::function f(L, -1);
	// The environment can now be ripped out of the function
	sol::environment env(sol::env_key, f);
	if (!env.valid()) {
		std::cout << "error: no environment to get" << std::endl;
		lua_settop(L, pre_stack_size);
		return;
	}
	sol::state_view lua(L);
	sol::environment freshenv = lua["freshenv"];
	bool is_same_env = freshenv == env;
	std::cout << "env == freshenv : " << is_same_env << std::endl;
}

int main() {
	std::cout << "=== environment snooping example ===" << std::endl;
	sol::state lua;

	sol::environment freshenv(lua, sol::create, lua.globals());
	lua["freshenv"] = freshenv;
	lua.set_function("f", some_function_called_by_sol2);

	lua.script("f()", freshenv);

	return 0;
}
