#include <sol.hpp>

#include <iostream>

int main() {
	std::cout << "=== protected_functions example ===" << std::endl;

	sol::state lua;

	// A complicated function which can error out
	// We define both in terms of Lua code

	lua.script(R"(
			function handler (message)
				return "Handled this message: " .. message
			end

			function f (a)
				if a < 0 then
					error("negative number detected")
				end
				return a + 5
			end
		)");

	// Get a protected function out of Lua
	sol::protected_function f = lua["f"];
	// Set a non-default error handler
	f.error_handler = lua["handler"];

	sol::protected_function_result result = f(-500);
	if (result.valid()) {
		// Call succeeded
		int x = result;
		std::cout << x << std::endl;
	}
	else {
		// Call failed
		sol::error err = result;
		std::string what = err.what();
		std::cout << what << std::endl;
		// 'what' Should read 
		// "Handled this message: negative number detected"
	}

	std::cout << std::endl;
}
