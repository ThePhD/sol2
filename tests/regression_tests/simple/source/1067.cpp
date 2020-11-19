#define SOL_ALL_SAFETIES_ON 1

#include <sol/sol.hpp>

int regression_1067() {
	sol::state lua;

	lua.open_libraries(sol::lib::base);
	lua["fct"] = std::function<int()> { []() { return 42; } };
	lua.script("print(fct())");

	return 0;
}
