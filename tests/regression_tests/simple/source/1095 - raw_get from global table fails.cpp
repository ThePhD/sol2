#include <sol/sol.hpp>

#include <iostream>
#include <string>

unsigned int regression_1095() {
	const std::string magic_value = "test_value";

	sol::state lua;
	lua[magic_value] = magic_value;

	if (lua.globals().get<std::string>(magic_value) != magic_value) {
		return 1;
	}
	if (lua.globals().raw_get<std::string>(magic_value) != magic_value) {
		return 2;
	}
	return 0;
}
