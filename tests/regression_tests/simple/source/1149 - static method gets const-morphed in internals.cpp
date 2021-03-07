#include <sol/sol.hpp>

inline namespace sol2_regression_test_1149 {
	struct Test {
		static Test* staticCreate() {
			return new Test();
		}
		Test* create() {
			return new Test();
		}
	};
} // namespace sol2_regression_test_1149

unsigned int regression_1149() {
	sol::state lua = {};
	auto T = lua.new_usertype<Test>("Test");
	// compile ok.
	T.set_function("create", &Test::create);
	// compile error.
	T.set_function("staticCreate", &Test::staticCreate);

	return 0;
}
