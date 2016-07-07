#define SOL_CHECK_ARGUMENTS

#include <sol.hpp>
#include <catch.hpp>

#include <iostream>
#include <mutex>

TEST_CASE("usertypes/simple_usertypes", "Ensure that simple usertypes properly work here") {
	struct marker {
		bool value = false;
	};
	struct bark {
		int var = 50;
		marker mark;

		void fun() {
			var = 51;
		}

		int get() const {
			return var;
		}

		int set(int x) {
			var = x;
			return var;
		}

		std::string special() const {
			return mark.value ? "woof" : "pantpant";
		}

		const marker& the_marker() const {
			return mark;
		}
	};

	sol::state lua;
	lua.new_simple_usertype<bark>("bark",
		"fun", &bark::fun,
		"get", &bark::get,
		"var", &bark::var,
		"the_marker", &bark::the_marker,
		"x", sol::property(&bark::get),
		"y", sol::property(&bark::set),
		"z", sol::property(&bark::get, &bark::set)
		);

	lua.script("b = bark.new()");
	bark& b = lua["b"];

	lua.script("b:fun()");
	int var = b.var;
	REQUIRE(var == 51);

	lua.script("b:var(20)");
	lua.script("v = b:var()");
	int v = lua["v"];
	REQUIRE(v == 20);

	lua.script("m = b:the_marker()");
	marker& m = lua["m"];
	REQUIRE_FALSE(b.mark.value);
	REQUIRE_FALSE(m.value);
	m.value = true;
	REQUIRE(&b.mark == &m);
	REQUIRE(b.mark.value);

	sol::table barktable = lua["bark"];
	barktable["special"] = &bark::special;

	lua.script("s = b:special()");
	std::string s = lua["s"];
	REQUIRE(s == "woof");

	lua.script("b:y(24)");
	lua.script("x = b:x()");
	int x = lua["x"];
	REQUIRE(x == 24);

	lua.script("z = b:z(b:z() + 5)");
	int z = lua["z"];
	REQUIRE(z == 29);
}
