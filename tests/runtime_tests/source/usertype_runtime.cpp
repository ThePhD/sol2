// sol3

// The MIT License (MIT)

// Copyright (c) 2013-2018 Rapptz, ThePhD and contributors

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

#include <iostream>
#include <list>
#include <memory>


TEST_CASE("usertype/runtime-extensibility", "Check if usertypes are runtime extensible") {
	struct thing {
		int v = 20;
		int func(int a) {
			return a;
		}
	};
	int val = 0;

	class base_a {
	public:
		int x;
	};

	class derived_b : public base_a {};

	SECTION("just functions") {
		sol::state lua;
		lua.open_libraries(sol::lib::base);

		lua.new_usertype<thing>("thing", "func", &thing::func);

		lua.safe_script(R"(
t = thing.new()
		)");

		{
			auto result = lua.safe_script(R"(
t.runtime_func = function (a)
	return a + 50
end
		)",
			     sol::script_pass_on_error);
			REQUIRE_FALSE(result.valid());
		};

		{
			auto result = lua.safe_script(R"(
function t:runtime_func(a)
	return a + 52
end
		)",
			     sol::script_pass_on_error);
			REQUIRE_FALSE(result.valid());
		};

		lua.safe_script("val = t:func(2)");
		val = lua["val"];
		REQUIRE(val == 2);

		REQUIRE_NOTHROW([&lua]() {
			lua.safe_script(R"(
function thing:runtime_func(a)
	return a + 1
end
		)");
		}());

		lua.safe_script("val = t:runtime_func(2)");
		val = lua["val"];
		REQUIRE(val == 3);
	}
	SECTION("with variable") {
		sol::state lua;
		lua.open_libraries(sol::lib::base);

		lua.new_usertype<thing>("thing", "func", &thing::func, "v", &thing::v);

		lua.safe_script(R"(
t = thing.new()
		)");

		{
			auto result = lua.safe_script(R"(
t.runtime_func = function (a)
	return a + 50
end
		)",
			     sol::script_pass_on_error);
			REQUIRE_FALSE(result.valid());
		};

		{
			auto result = lua.safe_script(R"(
function t:runtime_func(a)
	return a + 52
end
		)",
			     sol::script_pass_on_error);
			REQUIRE_FALSE(result.valid());
		};

		lua.safe_script("val = t:func(2)");
		val = lua["val"];
		REQUIRE(val == 2);

		REQUIRE_NOTHROW([&lua]() {
			lua.safe_script(R"(
function thing:runtime_func(a)
	return a + 1
end
		)");
		}());

		lua.safe_script("val = t:runtime_func(2)");
		val = lua["val"];
		REQUIRE(val == 3);
	}
	SECTION("with bases") {
		sol::state lua;
		lua.open_libraries(sol::lib::base);

		lua.new_usertype<base_a>("A", "x", &base_a::x // no crash without this
		);

		lua.new_usertype<derived_b>("B", sol::base_classes, sol::bases<base_a>());

		auto pfr0 = lua.safe_script("function A:c() print('A') return 1 end", sol::script_pass_on_error);
		REQUIRE(pfr0.valid());
		auto pfr1 = lua.safe_script("function B:c() print('B') return 2 end", sol::script_pass_on_error);
		REQUIRE(pfr1.valid());
		auto pfr2 = lua.safe_script("obja = A.new() objb = B.new()", sol::script_default_on_error);
		REQUIRE(pfr2.valid());
		auto pfr3 = lua.safe_script("assert(obja:c() == 1)", sol::script_default_on_error);
		REQUIRE(pfr3.valid());
		auto pfr4 = lua.safe_script("assert(objb:c() == 2)", sol::script_default_on_error);
		REQUIRE(pfr4.valid());
	}
}

TEST_CASE("usertype/runtime-replacement", "ensure that functions can be properly replaced at runtime for non-indexed things") {
	struct heart_base_t {};
	struct heart_t : heart_base_t {
		int x = 0;
		void func() {
		}
	};

	SECTION("plain") {
		sol::state lua;
		lua.open_libraries(sol::lib::base);

		lua.new_usertype<heart_t>("a");
		{
			auto result1 = lua.safe_script("obj = a.new()", sol::script_pass_on_error);
			REQUIRE(result1.valid());
			auto result2 = lua.safe_script("function a:heartbeat () print('arf') return 1 end", sol::script_pass_on_error);
			REQUIRE(result2.valid());
			auto result3 = lua.safe_script("v1 = obj:heartbeat()", sol::script_pass_on_error);
			REQUIRE(result3.valid());
			auto result4 = lua.safe_script("function a:heartbeat () print('bark') return 2 end", sol::script_pass_on_error);
			REQUIRE(result4.valid());
			auto result5 = lua.safe_script("v2 = obj:heartbeat()", sol::script_pass_on_error);
			REQUIRE(result5.valid());
			auto result6 = lua.safe_script("a.heartbeat = function(self) print('woof') return 3 end", sol::script_pass_on_error);
			REQUIRE(result6.valid());
			auto result7 = lua.safe_script("v3 = obj:heartbeat()", sol::script_pass_on_error);
			REQUIRE(result7.valid());
		}
		int v1 = lua["v1"];
		int v2 = lua["v2"];
		int v3 = lua["v3"];
		REQUIRE(v1 == 1);
		REQUIRE(v2 == 2);
		REQUIRE(v3 == 3);
	}
	SECTION("variables") {
		sol::state lua;
		lua.open_libraries(sol::lib::base);

		lua.new_usertype<heart_t>("a", "x", &heart_t::x, sol::base_classes, sol::bases<heart_base_t>());

		{
			auto result1 = lua.safe_script("obj = a.new()", sol::script_pass_on_error);
			REQUIRE(result1.valid());
			auto result2 = lua.safe_script("function a:heartbeat () print('arf') return 1 end", sol::script_pass_on_error);
			REQUIRE(result2.valid());
			auto result3 = lua.safe_script("v1 = obj:heartbeat()", sol::script_pass_on_error);
			REQUIRE(result3.valid());
			auto result4 = lua.safe_script("function a:heartbeat () print('bark') return 2 end", sol::script_pass_on_error);
			REQUIRE(result4.valid());
			auto result5 = lua.safe_script("v2 = obj:heartbeat()", sol::script_pass_on_error);
			REQUIRE(result5.valid());
			auto result6 = lua.safe_script("a.heartbeat = function(self) print('woof') return 3 end", sol::script_pass_on_error);
			REQUIRE(result6.valid());
			auto result7 = lua.safe_script("v3 = obj:heartbeat()", sol::script_pass_on_error);
			REQUIRE(result7.valid());
		}
		int v1 = lua["v1"];
		int v2 = lua["v2"];
		int v3 = lua["v3"];
		REQUIRE(v1 == 1);
		REQUIRE(v2 == 2);
		REQUIRE(v3 == 3);
	}
	SECTION("methods") {
		sol::state lua;
		lua.open_libraries(sol::lib::base);

		lua.new_usertype<heart_t>("a", "func", &heart_t::func, sol::base_classes, sol::bases<heart_base_t>());

		{
			auto result1 = lua.safe_script("obj = a.new()", sol::script_pass_on_error);
			REQUIRE(result1.valid());
			auto result2 = lua.safe_script("function a:heartbeat () print('arf') return 1 end", sol::script_pass_on_error);
			REQUIRE(result2.valid());
			auto result3 = lua.safe_script("v1 = obj:heartbeat()", sol::script_pass_on_error);
			REQUIRE(result3.valid());
			auto result4 = lua.safe_script("function a:heartbeat () print('bark') return 2 end", sol::script_pass_on_error);
			REQUIRE(result4.valid());
			auto result5 = lua.safe_script("v2 = obj:heartbeat()", sol::script_pass_on_error);
			REQUIRE(result5.valid());
			auto result6 = lua.safe_script("a.heartbeat = function(self) print('woof') return 3 end", sol::script_pass_on_error);
			REQUIRE(result6.valid());
			auto result7 = lua.safe_script("v3 = obj:heartbeat()", sol::script_pass_on_error);
			REQUIRE(result7.valid());
		}
		int v1 = lua["v1"];
		int v2 = lua["v2"];
		int v3 = lua["v3"];
		REQUIRE(v1 == 1);
		REQUIRE(v2 == 2);
		REQUIRE(v3 == 3);
	}
}

TEST_CASE("usertype/runtime additions with newindex", "ensure that additions when new_index is overriden don't hit the specified new_index function") {
	class newindex_object {};
	sol::state lua;
	lua.open_libraries(sol::lib::base);
	lua.new_usertype<newindex_object>("object", sol::meta_function::new_index, [](newindex_object&, sol::object, sol::object) { return; });

	lua["object"]["test"] = [](newindex_object&) {
		std::cout << "test" << std::endl;
		return 446;
	};

	auto result1 = lua.safe_script("o = object.new()", sol::script_pass_on_error);
	REQUIRE(result1.valid());
	auto result2 = lua.safe_script("assert(o:test() == 446)", sol::script_pass_on_error);
	REQUIRE(result2.valid());
}

TEST_CASE("usertype/meta-key-retrievals", "allow for special meta keys (__index, __newindex) to trigger methods even if overwritten directly") {
	SECTION("dynamically") {
		static int writes = 0;
		static std::string keys[4] = {};
		static int values[4] = {};
		struct d_sample {
			void foo(std::string k, int v) {
				keys[writes] = k;
				values[writes] = v;
				++writes;
			}
		};

		sol::state lua;
		lua.new_usertype<d_sample>("sample");
		sol::table s = lua["sample"]["new"]();
		s[sol::metatable_key][sol::meta_function::new_index] = &d_sample::foo;
		lua["var"] = s;

		lua.safe_script("var = sample.new()");
		lua.safe_script("var.key = 2");
		lua.safe_script("var.__newindex = 4");
		lua.safe_script("var.__index = 3");
		lua.safe_script("var.__call = 1");
		REQUIRE(values[0] == 2);
		REQUIRE(values[1] == 4);
		REQUIRE(values[2] == 3);
		REQUIRE(values[3] == 1);
		REQUIRE(keys[0] == "key");
		REQUIRE(keys[1] == "__newindex");
		REQUIRE(keys[2] == "__index");
		REQUIRE(keys[3] == "__call");
	}

	SECTION("statically") {
		static int writes = 0;
		static std::string keys[4] = {};
		static int values[4] = {};
		struct sample {
			void foo(std::string k, int v) {
				keys[writes] = k;
				values[writes] = v;
				++writes;
			}
		};

		sol::state lua;
		lua.new_usertype<sample>("sample", sol::meta_function::new_index, &sample::foo);

		lua.safe_script("var = sample.new()");
		lua.safe_script("var.key = 2");
		lua.safe_script("var.__newindex = 4");
		lua.safe_script("var.__index = 3");
		lua.safe_script("var.__call = 1");
		REQUIRE(values[0] == 2);
		REQUIRE(values[1] == 4);
		REQUIRE(values[2] == 3);
		REQUIRE(values[3] == 1);
		REQUIRE(keys[0] == "key");
		REQUIRE(keys[1] == "__newindex");
		REQUIRE(keys[2] == "__index");
		REQUIRE(keys[3] == "__call");
	}
}
