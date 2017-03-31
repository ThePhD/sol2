#include <sol.hpp>

#include <iostream>

int main() {
	std::cout << "=== basic example ===" << std::endl;
	// create an empty lua state
    sol::state lua;

    // by default, libraries are not opened
    // you can open libraries by using open_libraries
    // the libraries reside in the sol::lib enum class
    lua.open_libraries(sol::lib::base);
	// you can open all libraries by passing no arguments
	//lua.open_libraries();

    // call lua code directly
	lua.script("print('hello world')");

	// call lua code, and check to make sure it has loaded and run properly:
	auto handler = &sol::default_on_error;
	lua.script("print('hello again, world')", handler);

	// Use a custom error handler if you need it
	// This gets called when the result is bad
	auto simple_handler = [](lua_State*, sol::protected_function_result result) {
		// You can just pass it through to let the call-site handle it
		return result;
	};
	auto result = lua.script("print('hello hello again, world') \n return 24", simple_handler);
	if (result.valid()) {
		std::cout << "the code worked, and a double-hello statement should appear above this one!" << std::endl;
		int value = result;
		assert(value == 24);
	}
	else {
		std::cout << "the code failed, check the result type for more information!" << std::endl;
	}

	std::cout << std::endl;
}