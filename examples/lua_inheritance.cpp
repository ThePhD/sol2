#define SOL_CHECK_ARGUMENTS 1
#include <sol.hpp>

#include <cassert>
#include <iostream>

struct cpp_class {
	int value = 0;
};

int main(int, char*[]) {
	std::cout << "=== runtime_additions example ===" << std::endl;

	sol::state lua;
	lua.open_libraries(sol::lib::base);

	lua.new_usertype<cpp_class>("object");

	// runtime additions: through the sol API
	lua["object"]["func"] = [](cpp_class& o) {
		++o.value;
		return o.value;
	};
	// runtime additions: through a lua script
	lua.script(R"(
function object:print () 
	print(self:func())
end
	)");

	// see it work
	lua.script(R"(
obj = object.new()
obj:print()
	)");

	cpp_class& obj = lua["obj"];
	assert(obj.value == 1);

	return 0;
}
