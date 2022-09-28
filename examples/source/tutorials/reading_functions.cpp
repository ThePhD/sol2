#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

int main() {

	sol::state lua;

	lua.script(R"(
			function f (a)
				return a + 5
			end
		)");

	// Get and immediately call
	int x = lua["f"](30);
	SOL_ASSERT(x == 35);

	// Store it into a variable first, then call
	sol::unsafe_function f = lua["f"];
	int y = f(20);
	SOL_ASSERT(y == 25);

	// Store it into a variable first, then call
	sol::protected_function safe_f = lua["f"];
	int z = safe_f(45);
	SOL_ASSERT(z == 50);

	return 0;
}
