// sol3

// The MIT License (MIT)

// Copyright (c) 2013-2019 Rapptz, ThePhD and contributors

// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "sol_test.hpp"

#include "common_classes.hpp"

#include <catch.hpp>


struct matrix_xf {
	float a, b;

	static matrix_xf from_lua_table(sol::table t) {
		matrix_xf m;
		m.a = t[1][1];
		m.b = t[1][2];
		return m;
	}
};

struct matrix_xi {
	int a, b;

	static matrix_xi from_lua_table(sol::table t) {
		matrix_xi m;
		m.a = t[1][1];
		m.b = t[1][2];
		return m;
	}
};

TEST_CASE("usertype/call_constructor", "make sure lua types can be constructed with function call constructors") {
	sol::state lua;
	lua.open_libraries(sol::lib::base);

	lua.new_usertype<thing>("thing", "v", &thing::v, sol::call_constructor, sol::constructors<sol::types<>, sol::types<int>>());

	lua.safe_script(R"(
t = thing(256)
)");

	thing& y = lua["t"];
	INFO(y.v);
	REQUIRE(y.v == 256);
}

TEST_CASE("usertype/call_constructor factories", "make sure tables can be passed to factory-based call constructors") {
	sol::state lua;
	lua.open_libraries();

	lua.new_usertype<matrix_xf>("mat", sol::call_constructor, sol::factories(&matrix_xf::from_lua_table));

	lua.safe_script("m = mat{ {1.1, 2.2} }");

	lua.new_usertype<matrix_xi>("mati", sol::call_constructor, sol::factories(&matrix_xi::from_lua_table));

	lua.safe_script("mi = mati{ {1, 2} }");

	matrix_xf& m = lua["m"];
	REQUIRE(m.a == 1.1f);
	REQUIRE(m.b == 2.2f);
	matrix_xi& mi = lua["mi"];
	REQUIRE(mi.a == 1);
	REQUIRE(mi.b == 2);
}

TEST_CASE("usertype/call_constructor metatable check", "prevent metatable regression") {
	class class01 {
	public:
		int x = 57;
		class01() {
		}
	};

	class class02 {
	public:
		int x = 50;
		class02() {
		}
		class02(const class01& other) : x(other.x) {
		}
	};

	sol::state lua;

	lua.new_usertype<class01>("class01", sol::call_constructor, sol::constructors<sol::types<>, sol::types<const class01&>>());

	lua.new_usertype<class02>("class02", sol::call_constructor, sol::constructors<sol::types<>, sol::types<const class02&>, sol::types<const class01&>>());

	REQUIRE_NOTHROW(lua.safe_script(R"(
x = class01()
y = class02(x)
)"));
	class02& y = lua["y"];
	REQUIRE(y.x == 57);
}

TEST_CASE("usertype/blank_constructor", "make sure lua types cannot be constructed with arguments if a blank / empty constructor is provided") {
	sol::state lua;
	lua.open_libraries(sol::lib::base);

	lua.new_usertype<thing>("thing", "v", &thing::v, sol::call_constructor, sol::constructors<>());

	auto result = lua.safe_script("t = thing(256)", sol::script_pass_on_error);
	REQUIRE_FALSE(result.valid());
}

TEST_CASE("usertype/no_constructor", "make sure lua types cannot be constructed if a blank / empty constructor is provided") {
	SECTION("order1") {
		sol::state lua;
		lua.open_libraries(sol::lib::base);
		lua.new_usertype<thing>("thing", "v", &thing::v, sol::call_constructor, sol::no_constructor);
		auto result = lua.safe_script("t = thing()", sol::script_pass_on_error);
		REQUIRE_FALSE(result.valid());
	}

	SECTION("order2") {
		sol::state lua;
		lua.open_libraries(sol::lib::base);

		lua.new_usertype<thing>("thing", sol::call_constructor, sol::no_constructor, "v", &thing::v);
		auto result = lua.safe_script("t = thing.new()", sol::script_pass_on_error);
		REQUIRE_FALSE(result.valid());
	}

	SECTION("new no_constructor") {
		sol::state lua;
		lua.open_libraries(sol::lib::base);

		lua.new_usertype<thing>("thing", sol::meta_function::construct, sol::no_constructor);
		auto result = lua.safe_script("t = thing.new()", sol::script_pass_on_error);
		REQUIRE_FALSE(result.valid());
	}

	SECTION("call no_constructor") {
		sol::state lua;
		lua.open_libraries(sol::lib::base);

		lua.new_usertype<thing>("thing", sol::call_constructor, sol::no_constructor);
		auto result = lua.safe_script("t = thing()", sol::script_pass_on_error);
		REQUIRE_FALSE(result.valid());
	}
}

TEST_CASE("usertype/constructor list", "Show that we can create classes from usertype and use them with multiple constructors") {

	sol::state lua;

	sol::constructors<sol::types<>, sol::types<int>, sol::types<int, int>> con;
	sol::usertype<crapola::fuser> lc = lua.new_usertype<crapola::fuser>("fuser", con, "add", &crapola::fuser::add, "add2", &crapola::fuser::add2);

	lua.safe_script(
	     "a = fuser.new(2)\n"
	     "u = a:add(1)\n"
	     "v = a:add2(1)\n"
	     "b = fuser:new()\n"
	     "w = b:add(1)\n"
	     "x = b:add2(1)\n"
	     "c = fuser.new(2, 3)\n"
	     "y = c:add(1)\n"
	     "z = c:add2(1)\n");
	sol::object a = lua.get<sol::object>("a");
	auto atype = a.get_type();
	REQUIRE((atype == sol::type::userdata));
	sol::object u = lua.get<sol::object>("u");
	sol::object v = lua.get<sol::object>("v");
	REQUIRE((u.as<int>() == 3));
	REQUIRE((v.as<int>() == 5));

	sol::object b = lua.get<sol::object>("b");
	auto btype = b.get_type();
	REQUIRE((btype == sol::type::userdata));
	sol::object w = lua.get<sol::object>("w");
	sol::object x = lua.get<sol::object>("x");
	REQUIRE((w.as<int>() == 1));
	REQUIRE((x.as<int>() == 3));

	sol::object c = lua.get<sol::object>("c");
	auto ctype = c.get_type();
	REQUIRE((ctype == sol::type::userdata));
	sol::object y = lua.get<sol::object>("y");
	sol::object z = lua.get<sol::object>("z");
	REQUIRE((y.as<int>() == 7));
	REQUIRE((z.as<int>() == 9));
}
