#define SOL_CHECK_ARGUMENTS

#include <catch.hpp>
#include <sol.hpp>

#ifdef SOL_CXX17_FEATURES
#include <string_view>
#include <variant>
#endif


TEST_CASE("utility/variant", "test that variant can be round-tripped") {
#ifdef SOL_CXX17_FEATURES
	SECTION("okay") {
		sol::state lua;
		lua.open_libraries(sol::lib::base);

		lua.set_function("f", [](int v) {
			return v == 2;
		});
		lua.set_function("g", [](std::variant<float, int, std::string> vv) {
			int v = std::get<int>(vv);
			return v == 2;
		});
		lua["v"] = std::variant<float, int, std::string>(2);
		REQUIRE_NOTHROW([&]() {
			lua.script("assert(f(v))");
			lua.script("assert(g(v))");
		}());
	}
	SECTION("throws") {
		sol::state lua;
		lua.open_libraries(sol::lib::base);

		lua.set_function("f", [](int v) {
			return v == 2;
		});
		lua.set_function("g", [](std::variant<float, int, std::string> vv) {
			int v = std::get<int>(vv);
			return v == 2;
		});
		lua["v"] = std::variant<float, int, std::string>(std::string("bark"));
		REQUIRE_THROWS([&]() {
			lua.script("assert(f(v))");
			lua.script("assert(g(v))");
		}());
	}
#else
	REQUIRE(true);
#endif // C++17
}

TEST_CASE("utility/string_view", "test that string_view can be taken as an argument") {
#ifdef SOL_CXX17_FEATURES
	sol::state lua;
	lua.open_libraries(sol::lib::base);

	lua.set_function("f", [](std::string_view v) {
		return v == "bark!";
	});
	lua["v"] = "bark!";

	REQUIRE_NOTHROW([&]() {
		lua.script("assert(f(v))");
	}());
#else
	REQUIRE(true);
#endif // C++17
}