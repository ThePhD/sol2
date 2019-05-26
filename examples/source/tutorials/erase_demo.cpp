#define SOL_ALL_SAFETIES_ON 1
#include <sol.hpp>

int main() {

	sol::state lua;
	lua["bark"] = 50;
	sol::optional<int> x = lua["bark"];
	// x will have a value

	lua["bark"] = sol::lua_nil;
	sol::optional<int> y = lua["bark"];
	// y will not have a value

	return 0;
}
