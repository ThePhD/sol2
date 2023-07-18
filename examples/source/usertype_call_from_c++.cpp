#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include <iostream>

int main(int, char*[]) {
	std::cout << "=== usertype call from C++ ===" << std::endl;

	sol::state lua;
	lua.open_libraries(sol::lib::base);

	struct cpp_object {
		int value = 5;
	};

	struct test {
		int value = 0;

		int func(const cpp_object& obj) {
			std::cout << "func\t" << obj.value << std::endl;
			value += obj.value;
			return value;
		}
	};

	lua.new_usertype<cpp_object>(
	     "cpp_object", "value", &cpp_object::value);
	lua.new_usertype<test>("test", "func", &test::func);
	lua.script(
	     "function test:lua_func(obj) print('lua_func', "
	     "obj.value) return obj.value end");

	lua["obj"] = test {};
	cpp_object cppobj;

	int obj_func_call = lua["obj"]["func"](lua["obj"], cppobj);
	int obj_lua_func_call
	     = lua["obj"]["lua_func"](lua["obj"], cppobj);

	int test_func_call = lua["test"]["func"](lua["obj"], cppobj);
	int test_lua_func_call
	     = lua["test"]["lua_func"](lua["obj"], cppobj);

	SOL_ASSERT(obj_func_call == 5);
	SOL_ASSERT(obj_lua_func_call == 5);
	SOL_ASSERT(test_func_call == 10);
	SOL_ASSERT(test_lua_func_call == 5);

	// crashes
	// lua["obj"]["func"](cppobj);
	// lua["obj"]["lua_func"](cppobj);

	// crashes
	// lua["test"]["func"](cppobj);
	// lua["test"]["lua_func"](cppobj);

	return 0;
}
