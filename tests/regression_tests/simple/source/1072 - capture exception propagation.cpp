#include <sol/sol.hpp>

unsigned int regression_1072() {
	auto st = sol::state {};
	// If you remove the capture, the problem goes away.
	st["foo"] = [&] { throw std::runtime_error(""); };
	sol::protected_function_result result = st.safe_script("foo()", sol::script_pass_on_error);
	if (!result.valid()) {
		sol::error err = result;
		return 0;
	}
	return 1;
}