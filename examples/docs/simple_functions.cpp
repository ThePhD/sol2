#define SOL_CHECK_ARGUMENTS 1

#include <sol.hpp>
#include <cassert>

int main() {
	sol::state lua;
	int x = 0;
	lua.set_function("beep", [&x]{ ++x; });
	lua.script("beep()");
	assert(x == 1);

	sol::function beep = lua["beep"];
	beep();
	assert(x == 2);

	return 0;
}
