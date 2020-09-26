#define SOL_ALL_SAFETIES_ON 1

#include <sol/sol.hpp>

struct foo1000 {
	static void register_into(sol::table& table) {
		table.new_usertype<foo1000>("foo1000", "id", sol::readonly(&foo1000::foo));
	}
	int foo;
};

int regression_1000() {
	sol::state lua;
	lua.create_named_table("t");

	sol::table t = lua["t"];
	foo1000::register_into(t);

	return 0;
}
