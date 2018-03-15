#define SOL_CHECK_ARGUMENTS 1
#include <sol.hpp>

int main () {
	class B {
	public:
		int bvar = 24;
	};

	sol::state lua;
	lua.open_libraries();
	lua.new_usertype<B>("B", 
		// bind as variable
		"b", &B::bvar,
		// bind as function
		"f", sol::as_function(&B::bvar)
	);

	B b;
	lua.set("b", &b);
	lua.script("x = b:f()");
	lua.script("y = b.b");
	int x = lua["x"];
	int y = lua["y"];
	assert(x == 24);
	assert(y == 24);
}
