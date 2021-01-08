#include <sol/sol.hpp>

unsigned int regression_1067() {
	sol::state lua;

	lua.open_libraries(sol::lib::base);
	lua["fct"] = std::function<int()> { []() { return 42; } };
	sol::optional<sol::error> maybe_error = lua.safe_script("assert(fct() == 42)", sol::script_pass_on_error);

	return maybe_error.has_value() ? 1 : 0;
}
