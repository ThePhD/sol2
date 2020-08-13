#define SOL_ALL_SAFETIES_ON 1

#include <sol/sol.hpp>

int regression_1008() {

	sol::state lua;
	lua.create_named_table("t");

	sol::table t = lua["t"];
	t["f"] = sol::as_function([]() {});

	return 0;
}
