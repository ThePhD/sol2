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

#include <catch.hpp>

struct unregister_me {
	double b = 5.5;
	std::string f_val = "registered";

	unregister_me() {
	}

	std::string f() {
		return f_val;
	}
};

TEST_CASE("usertypes/unregister", "make sure that a class can still be bound but that it becomes completely unregistered") {
	const sol::string_view line1 = "assert(u:f() == 'registered')";
	const sol::string_view line2 = "assert(urm.a() == 20)";
	const sol::string_view line3 = "assert(u.a() == 20) assert(u:a() == 20)";
	const sol::string_view line4 = "assert(u.b == 5.5)";

	sol::state lua;
	lua.open_libraries();
	auto register_urm = [&lua, &line1, &line2, &line3, &line4]() {
		lua.new_usertype<unregister_me>("urm", "f", &unregister_me::f, "a", []() { return 20; }, "b", &unregister_me::b);
		{
			sol::object urm_obj = lua["urm"];
			REQUIRE(urm_obj.get_type() == sol::type::table);
			REQUIRE(urm_obj.is<sol::table>());
			REQUIRE(urm_obj.is<sol::metatable>());
			REQUIRE(urm_obj.is<sol::usertype<unregister_me>>());
		}

		lua["urm_unregister"] = [](sol::this_state ts) {
			sol::state_view current_lua = ts;
			sol::usertype<unregister_me> urm = current_lua["urm"];
			urm.unregister();
		};

		auto sresult0 = lua.safe_script("u = urm.new()", sol::script_pass_on_error);
		REQUIRE(sresult0.valid());
		auto sresult1 = lua.safe_script(line1, sol::script_pass_on_error);
		REQUIRE(sresult1.valid());
		auto sresult2 = lua.safe_script(line2, sol::script_pass_on_error);
		REQUIRE(sresult2.valid());
		auto sresult3 = lua.safe_script(line3, sol::script_pass_on_error);
		REQUIRE(sresult3.valid());
		auto sresult4 = lua.safe_script(line4, sol::script_pass_on_error);
		REQUIRE(sresult4.valid());

		unregister_me& u_orig = lua["u"];
		REQUIRE(u_orig.b == 5.5);
		REQUIRE(u_orig.f() == "registered");
	};
	SECTION("unregister C++") {
		register_urm();

		{
			sol::usertype<unregister_me> urm = lua["urm"];
			urm.unregister();
		}

		auto result0 = lua.safe_script("u_fail = urm.new()", sol::script_pass_on_error);
		REQUIRE_FALSE(result0.valid());
		auto result1 = lua.safe_script(line1, sol::script_pass_on_error);
		REQUIRE_FALSE(result1.valid());
		auto result2 = lua.safe_script(line2, sol::script_pass_on_error);
		REQUIRE_FALSE(result2.valid());
		auto result3 = lua.safe_script(line3, sol::script_pass_on_error);
		REQUIRE_FALSE(result3.valid());
		auto result4 = lua.safe_script(line4, sol::script_pass_on_error);
		REQUIRE_FALSE(result4.valid());

		unregister_me& u = lua["u"];
		REQUIRE(u.b == 5.5);
		REQUIRE(u.f() == "registered");
	}
	SECTION("re-register") {
		register_urm();

		sol::protected_function urm_unregister_func = lua["urm_unregister"];
		auto unregister_result = urm_unregister_func();
		REQUIRE(unregister_result.valid());

		auto result0 = lua.safe_script("u_fail2 = urm.new()", sol::script_pass_on_error);
		REQUIRE_FALSE(result0.valid());
		auto result1 = lua.safe_script(line1, sol::script_pass_on_error);
		REQUIRE_FALSE(result1.valid());
		auto result2 = lua.safe_script(line2, sol::script_pass_on_error);
		REQUIRE_FALSE(result2.valid());
		auto result3 = lua.safe_script(line3, sol::script_pass_on_error);
		REQUIRE_FALSE(result3.valid());
		auto result4 = lua.safe_script(line4, sol::script_pass_on_error);
		REQUIRE_FALSE(result4.valid());

		unregister_me& u = lua["u"];
		REQUIRE(u.b == 5.5);
		REQUIRE(u.f() == "registered");

		register_urm();
	}
	SECTION("unregister lua") {
		register_urm();

		auto unregister_result = lua.safe_script("urm_unregister()", sol::script_pass_on_error);
		REQUIRE(unregister_result.valid());

		auto result0 = lua.safe_script("u_fail2 = urm.new()", sol::script_pass_on_error);
		REQUIRE_FALSE(result0.valid());
		auto result1 = lua.safe_script(line1, sol::script_pass_on_error);
		REQUIRE_FALSE(result1.valid());
		auto result2 = lua.safe_script(line2, sol::script_pass_on_error);
		REQUIRE_FALSE(result2.valid());
		auto result3 = lua.safe_script(line3, sol::script_pass_on_error);
		REQUIRE_FALSE(result3.valid());
		auto result4 = lua.safe_script(line4, sol::script_pass_on_error);
		REQUIRE_FALSE(result4.valid());

		unregister_me& u = lua["u"];
		REQUIRE(u.b == 5.5);
		REQUIRE(u.f() == "registered");
	}
}
