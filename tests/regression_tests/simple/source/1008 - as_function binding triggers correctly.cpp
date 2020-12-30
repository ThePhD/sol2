#include <sol/sol.hpp>

unsigned int regression_1008() {

	sol::state lua;
	lua.create_named_table("t");

	sol::table t = lua["t"];
	t["f"] = sol::as_function([]() {});

	return 0;
}
