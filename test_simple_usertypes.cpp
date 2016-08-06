#define SOL_CHECK_ARGUMENTS

#include <sol.hpp>
#include <catch.hpp>

#include <iostream>
#include <list>
#include <memory>
#include <mutex>

TEST_CASE("usertypes/simple-usertypes", "Ensure that simple usertypes properly work here") {
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

TEST_CASE("usertypes/simple-usertypes-constructors", "Ensure that calls with specific arguments work") {
	struct marker {
		bool value = false;
	};
	struct bark {
		int var = 50;
		marker mark;

		bark() {}
		bark(int v) : var(v) {}

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
		sol::constructors<sol::types<>, sol::types<int>>(),
		"fun", sol::protect( &bark::fun ),
		"get", &bark::get,
		"var", &bark::var,
		"the_marker", &bark::the_marker,
		"x", sol::property(&bark::get),
		"y", sol::property(&bark::set),
		"z", sol::property(&bark::get, &bark::set)
		);

	lua.script("bx = bark.new(760)");
	bark& bx = lua["bx"];
	REQUIRE(bx.var == 760);

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

TEST_CASE("usertype/simple-shared-ptr-regression", "simple usertype metatables should not screw over unique usertype metatables") {
	static int created = 0;
	static int destroyed = 0;
	struct test {
		test() {
			++created;
		}

		~test() {
			++destroyed;
		}
	};
	{
		std::list<std::shared_ptr<test>> tests;
		sol::state lua;
		lua.open_libraries();

		lua.new_simple_usertype<test>("test",
			"create", [&]() -> std::shared_ptr<test> {
			tests.push_back(std::make_shared<test>());
			return tests.back();
		}
		);
		REQUIRE(created == 0);
		REQUIRE(destroyed == 0);
		lua.script("x = test.create()");
		REQUIRE(created == 1);
		REQUIRE(destroyed == 0);
		REQUIRE_FALSE(tests.empty());
		std::shared_ptr<test>& x = lua["x"];
		std::size_t xuse = x.use_count();
		std::size_t tuse = tests.back().use_count();
		REQUIRE(xuse == tuse);
	}
	REQUIRE(created == 1);
	REQUIRE(destroyed == 1);
}

TEST_CASE("usertypes/simple=vars", "simple usertype vars can bind various values (no ref)") {
	int muh_variable = 10;
	int through_variable = 25;

	sol::state lua;
	lua.open_libraries();

	struct test {};
	lua.new_simple_usertype<test>("test",
		"straight", sol::var(2),
		"global", sol::var(muh_variable),
		"global2", sol::var(through_variable)
		);

	lua.script(R"(
s = test.straight
g = test.global
g2 = test.global2
)");

	int s = lua["s"];
	int g = lua["g"];
	int g2 = lua["g2"];
	REQUIRE(s == 2);
	REQUIRE(g == 10);
	REQUIRE(g2 == 25);
}
