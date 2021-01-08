#include <sol/sol.hpp>

#include <iostream>
#include <string>

inline namespace sol2_regression_test_1096 {
	const double constant_float_v = 3;
	double test_value() {
		return constant_float_v;
	}
} // namespace sol2_regression_test_1096

unsigned int regression_1096() {
	sol::state lua;
	lua.open_libraries(sol::lib::base);

	lua["test0"] = lua.create_table_with("value", [] { return constant_float_v; });
	lua["test1"] = lua.create_table_with("value", [] { return constant_float_v; });
	lua["test2"] = lua.create_table_with("value", [&] { return constant_float_v; });
	lua["test3"] = lua.create_table_with("value", test_value);
	lua["test4"] = lua.create_table_with("value", &test_value);
	lua["test5"] = sol::create;
	lua["test6"] = sol::create;
	lua["test7"] = sol::create;
	lua["test5"]["value"] = [] { return constant_float_v; };
	lua["test6"]["value"] = [] { return constant_float_v; };
	lua["test7"]["value"] = [&] { return constant_float_v; };

	unsigned int accumulated_errors = 0;
	for (std::size_t i = 0; i < 8; ++i) {
		std::string num = std::to_string(i);
		std::string val = std::to_string(constant_float_v);
		std::string code = "local val = test" + num + ".value()\nassert(val == " + val + ")";
		sol::optional<sol::error> maybe_error = lua.safe_script(code, sol::script_pass_on_error);
		if (maybe_error.has_value()) {
			const sol::error& err = *maybe_error;
			std::cerr << "regression 1096: nested function call using test" << i << " failed:\n" << err.what() << std::endl;
			++accumulated_errors;
		}
	}
	return accumulated_errors;
}
