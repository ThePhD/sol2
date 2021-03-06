#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

template <typename A, typename B>
auto my_add(A a, B b) {
	return a + b;
}

int main() {

	sol::state lua;

	// adds 2 integers
	lua["my_int_add"] = &my_add<int, int>;

	// concatenates 2 strings
	lua["my_string_combine"] = &my_add<std::string, std::string>;

	lua.script("my_num = my_int_add(1, 2)");
	int my_num = lua["my_num"];
	sol_c_assert(my_num == 3);

	lua.script("my_str = my_string_combine('bark bark', ' woof woof')");
	std::string my_str = lua["my_str"];
	sol_c_assert(my_str == "bark bark woof woof");

	return 0;
}
