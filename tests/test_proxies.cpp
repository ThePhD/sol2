#define SOL_CHECK_ARGUMENTS 1
#define SOL_ENABLE_INTEROP 1

#include <sol.hpp>

#include <catch.hpp>

#include <iostream>
#include "test_stack_guard.hpp"

TEST_CASE("proxy/function results", "make sure that function results return proper proxies and can be indexed nicely") {
	sol::state lua;
	SECTION("unsafe_function_result") {
		auto ufr = lua.script("return 1, 2, 3, 4");
		int accum = 0;
		for (const auto& r : ufr) {
			int v = r;
			accum += v;
		}
		REQUIRE(accum == 10);
	}
	SECTION("protected_function_result") {
		auto pfr = lua.safe_script("return 1, 2, 3, 4");
		int accum = 0;
		for (const auto& r : pfr) {
			int v = r;
			accum += v;
		}
		REQUIRE(accum == 10);
	}
}
