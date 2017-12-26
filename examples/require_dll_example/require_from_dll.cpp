#define SOL_CHECK_ARGUMENTS 1
#include <sol.hpp>

#include "my_object.hpp"
#include "../assert.hpp"

#include <iostream>

int main(int, char*[]) {
	std::cout << "=== require from DLL example ===" << std::endl;

	sol::state lua;
	lua.open_libraries(sol::lib::package);

	lua.script_file(R"(
mo = require("my_object")

obj = mo.test.new(24)
print(obj.value)
)");

	my_object::test& obj = lua["obj"];
	c_assert(obj.value == 24);

	return 0;
}