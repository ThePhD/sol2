#include <sol/sol.hpp>

#if SOL_TESTS_SIZEOF_VOID_P == 4

inline namespace sol2_regression_test_1192 {
	struct Test {
		std::uint64_t dummy;
	};

	struct alignas(1024) Test2 {
		char dummy[1024];
	};
} // namespace sol2_regression_test_1192

unsigned int regression_1192() {
	sol::state lua;

	static_assert(sizeof(Test) == 8);
	static_assert(alignof(Test) == 8);
	static_assert(sizeof(Test*) == 4);
	static_assert(alignof(Test*) == 4);

	/// [sol2] An error occurred and panic has been invoked: aligned allocation of userdata block (data section) for 'sol2_regression_test_1192::Test' failed
	/// Note: may not panic depending on alignment of local variable `alignment_shim` in sol::detail::aligned_space_for
	lua["test"] = Test {};

	// Test also unique and over-aligned userdata
	lua["test"] = std::make_unique<Test>();
	lua["test"] = Test2 {};

	return 0;
}

#else

unsigned int regression_1192() {
	return 0;
}

#endif
