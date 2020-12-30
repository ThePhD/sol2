#include <sol/sol.hpp>

inline namespace sol2_regression_test_1000 {
	struct foo {
		static void register_into(sol::table& table) {
			table.new_usertype<foo>("foo", "id", sol::readonly(&foo::id));
		}
		int id;
	};
} // namespace sol2_regression_test_1000

unsigned int regression_1000() {
	sol::state lua;
	lua.create_named_table("t");

	sol::table t = lua["t"];
	foo::register_into(t);

	return 0;
}
