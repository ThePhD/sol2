#include <sol/sol.hpp>

int regression_1067() {
	sol::state lua;

	lua.open_libraries(sol::lib::base);
	lua["fct"] = std::function<int()> { []() { return 42; } };
	sol::optional<sol::error> maybe_error = lua.safe_script("print(fct())");

	return maybe_error.has_value() ? 1 : 0;
}
