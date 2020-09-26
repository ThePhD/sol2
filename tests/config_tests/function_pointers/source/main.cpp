#include <sol/sol.hpp>

#include <assert.hpp>

#include <iostream>

inline constexpr int magic_value = 24;

using zero_arg_type = int (*)();
using one_arg_type = int (*)(int);
using callback_type = int (*)(one_arg_type);

int free_function(int value) {
	return value + 1;
}

int callback(one_arg_type f) {
	return f(magic_value) + 1;
}

int main() {
	constexpr int expected_value = magic_value;
	constexpr int expected_free_function_value = magic_value + 1;
	constexpr int expected_callback_value = magic_value + 1 + 1;

	sol::state lua;
	lua.open_libraries(sol::lib::base);

	auto lambda = []() { return magic_value; };
	auto lambda_ptr = static_cast<zero_arg_type>(lambda);

	lua["magic_value"] = magic_value;
	lua["expected_value"] = expected_value;
	lua["expected_free_function_value"] = expected_free_function_value;
	lua["expected_callback_value"] = expected_callback_value;

	lua["lambda"] = sol::as_function_reference(lambda);
	lua["lambda_ptr"] = lambda_ptr;
	lua["free_function"] = &free_function;
	lua["callback"] = &callback;

	zero_arg_type lambda_f = lua["lambda"];
	zero_arg_type lambda_ptr_f = lua["lambda_ptr"];
	one_arg_type free_function_f = lua["free_function"];
	callback_type callback_f = lua["callback"];
	sol::function lua_callback_f = lua["callback"];

	int lambda_f_result = lambda_f();
	int lambda_ptr_f_result = lambda_ptr_f();
	int free_function_f_result = free_function_f(magic_value);
	int callback_f_result = callback_f(&free_function);
	int lua_callback_f_result = lua_callback_f(&free_function);
	c_assert(lambda_f_result == expected_value);
	c_assert(lambda_ptr_f_result == expected_value);
	c_assert(free_function_f_result == expected_free_function_value);
	c_assert(callback_f_result == expected_callback_value);
	c_assert(lua_callback_f_result == expected_callback_value);

	const char code[] = R"(
assert(lambda() == expected_value)
assert(lambda_ptr() == expected_value)
assert(free_function(magic_value) == expected_free_function_value)
assert(callback(free_function) == expected_callback_value)
	)";

	sol::optional<sol::error> err = lua.safe_script(code, sol::script_pass_on_error);
	if (err.has_value()) {
		std::cerr << err.value().what() << std::endl;
		return 1;
	}
	c_assert(!err.has_value());

	return 0;
}
