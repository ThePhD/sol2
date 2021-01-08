#include <sol/sol.hpp>

inline namespace sol2_regression_test_1087 {
	struct MyStruct {
		int prop = 10;
	};
} // namespace sol2_regression_test_1087

unsigned int regression_1087() {
	sol::state lua;
	lua.open_libraries(sol::lib::base,
	     sol::lib::package,
	     sol::lib::coroutine,
	     sol::lib::string,
	     sol::lib::os,
	     sol::lib::math,
	     sol::lib::table,
	     sol::lib::io,
	     sol::lib::debug);
	lua.new_usertype<MyStruct>("MyStruct", "prop", sol::readonly_property(&MyStruct::prop));
	sol::optional<sol::error> maybe_error = lua.safe_script("local s = MyStruct.new(); s.prop = 20", sol::script_pass_on_error);
	if (maybe_error.has_value()) {
		// good: cannot write to this property
		return 0;
	}
	return 1;
}