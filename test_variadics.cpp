#define SOL_CHECK_ARGUMENTS

#include <catch.hpp>
#include <sol.hpp>

#include <deque>
#include <set>
#include <functional>
#include <string>

TEST_CASE("variadics/variadic_args", "Check to see we can receive multiple arguments through a variadic") {
	struct structure {
		int x;
		bool b;
	};

	sol::state lua;
	lua.open_libraries(sol::lib::base);
	lua.set_function("v", [](sol::this_state, sol::variadic_args va) -> structure {
		int r = 0;
		for (auto v : va) {
			int value = v;
			r += value;
		}
		return{ r, r > 200 };
	});

	lua.script("x = v(25, 25)");
	lua.script("x2 = v(25, 25, 100, 50, 250, 150)");
	lua.script("x3 = v(1, 2, 3, 4, 5, 6)");

	structure& lx = lua["x"];
	structure& lx2 = lua["x2"];
	structure& lx3 = lua["x3"];
	REQUIRE(lx.x == 50);
	REQUIRE(lx2.x == 600);
	REQUIRE(lx3.x == 21);
	REQUIRE_FALSE(lx.b);
	REQUIRE(lx2.b);
	REQUIRE_FALSE(lx3.b);
}

TEST_CASE("variadics/required with variadic_args", "Check if a certain number of arguments can still be required even when using variadic_args") {
	sol::state lua;
	lua.set_function("v",
		[](sol::this_state, sol::variadic_args, int, int) {
	}
	);
	REQUIRE_NOTHROW(lua.script("v(20, 25, 30)"));
	REQUIRE_NOTHROW(lua.script("v(20, 25)"));
#ifndef SOL_LUAJIT
	REQUIRE_THROWS(lua.script("v(20)"));
#endif // LuaJIT has problems with exceptions, as fucking usual
}

TEST_CASE("variadics/variadic_args get type", "Make sure we can inspect types proper from variadic_args") {
	sol::state lua;

	lua.set_function("f", [](sol::variadic_args va) {
		sol::type types[] = {
			sol::type::number,
			sol::type::string,
			sol::type::boolean
		};
		bool working = true;
		auto b = va.begin();
		for (std::size_t i = 0; i < va.size(); ++i, ++b) {
			sol::type t1 = va.get_type(i);
			sol::type t2 = b->get_type();
			working &= types[i] == t1;
			working &= types[i] == t2;
		}
		REQUIRE(working);
	});

	lua.script("f(1, 'bark', true)");
	lua.script("f(2, 'wuf', false)");
}

TEST_CASE("variadics/variadic_results", "returning a variable amount of arguments from C++") {
	SECTION("as_returns - containers") {
		sol::state lua;

		lua.set_function("f", []() {
			std::set<std::string> results{ "arf", "bark", "woof" };
			return sol::as_returns(std::move(results));
		});
		lua.set_function("g", []() {
			static const std::deque<int> results{ 25, 82 };
			return sol::as_returns(std::ref(results));
		});

		REQUIRE_NOTHROW([&]() {
			lua.script(R"(
	v1, v2, v3 = f()
	v4, v5 = g()
)");
		}());

		std::string v1 = lua["v1"];
		std::string v2 = lua["v2"];
		std::string v3 = lua["v3"];
		int v4 = lua["v4"];
		int v5 = lua["v5"];

		REQUIRE(v1 == "arf");
		REQUIRE(v2 == "bark");
		REQUIRE(v3 == "woof");
		REQUIRE(v4 == 25);
		REQUIRE(v5 == 82);
	}
	SECTION("variadic_results - variadic_args") {
		sol::state lua;

		lua.set_function("f", [](sol::variadic_args args) {
			return sol::variadic_results(args.cbegin(), args.cend());
		});

		REQUIRE_NOTHROW([&]() {
			lua.script(R"(
	v1, v2, v3 = f(1, 'bark', true)
	v4, v5 = f(25, 82)
)");
		}());

		int v1 = lua["v1"];
		std::string v2 = lua["v2"];
		bool v3 = lua["v3"];
		int v4 = lua["v4"];
		int v5 = lua["v5"];

		REQUIRE(v1 == 1);
		REQUIRE(v2 == "bark");
		REQUIRE(v3);
		REQUIRE(v4 == 25);
		REQUIRE(v5 == 82);
	}
	SECTION("variadic_results") {
		sol::state lua;

		lua.set_function("f", [](sol::this_state ts, bool maybe) {
			if (maybe) {
				sol::variadic_results vr;
				vr.push_back({ ts, sol::in_place, 1 });
				vr.push_back({ ts, sol::in_place, 2 });
				vr.insert(vr.cend(), { ts, sol::in_place, 3 });
				return vr;
			}
			else {
				sol::variadic_results vr;
				vr.push_back({ ts, sol::in_place, "bark" });
				vr.push_back({ ts, sol::in_place, "woof" });
				vr.insert(vr.cend(), { ts, sol::in_place, "arf" });
				vr.push_back({ ts, sol::in_place, "borf" });
				return vr;
			}
		});

		REQUIRE_NOTHROW([&]() {
			lua.script(R"(
	v1, v2, v3 = f(true)
	v4, v5, v6, v7 = f(false)
)");
		}());

		int v1 = lua["v1"];
		int v2 = lua["v2"];
		int v3 = lua["v3"];
		std::string v4 = lua["v4"];
		std::string v5 = lua["v5"];
		std::string v6 = lua["v6"];
		std::string v7 = lua["v7"];

		REQUIRE(v1 == 1);
		REQUIRE(v2 == 2);
		REQUIRE(v3 == 3);
		REQUIRE(v4 == "bark");
		REQUIRE(v5 == "woof");
		REQUIRE(v6 == "arf");
		REQUIRE(v7 == "borf");
	}
}
