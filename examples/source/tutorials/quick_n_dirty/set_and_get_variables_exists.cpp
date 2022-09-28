#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>


int main(int, char*[]) {
	sol::state lua;
	lua.open_libraries(sol::lib::base);

	lua.script("exists = 250");

	int first_try = lua.get_or("exists", 322);
	SOL_ASSERT(first_try == 250);

	lua.set("exists", sol::lua_nil);
	int second_try = lua.get_or("exists", 322);
	SOL_ASSERT(second_try == 322);

	return 0;
}
