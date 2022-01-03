#include <iostream>
#include <sol/sol.hpp>

inline namespace sol2_regression_test_1144 {
	class MyClass {
	private:
		std::string m_name;

	public:
		MyClass(const std::string& name) : m_name(name) {};
	};

	void bind_class(sol::state_view state) {
		sol::usertype<MyClass> bind_my_class = state.new_usertype<MyClass>("MyClass", sol::call_constructor, sol::constructors<MyClass(std::string)>());
	}
} // namespace sol2_regression_test_1144

unsigned int regression_1144() {
	sol::state state;
	bind_class(state);
	sol::protected_function_result pr = state.do_string("local a = MyClass();");
	if (!pr.valid()) {
		sol::error err = pr;
		std::cout << "An error occurred, as expected:\n" << err.what() << std::endl;
		return 0;
	}

	return 1;
}
