// sol2

// The MIT License (MIT)

// Copyright (c) 2013-2022 Rapptz, ThePhD and contributors

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

#include <sol/forward.hpp>
#include <sol/reference.hpp>
#include <sol/utility/to_string.hpp>

#include <catch2/catch_all.hpp>

namespace {
	int someFunc() {
		return 5;
	}
} // namespace

TEST_CASE("to_string") {
	using Catch::Matchers::ContainsSubstring;

	sol::state lua;

	SECTION("boolean") {
		auto object = sol::make_object(lua, true);
		CHECK(sol::utility::to_string(object) == "true");

		auto pp = sol::stack::push_pop(object);
		CHECK(sol::utility::to_string(sol::stack_object(lua, -1)) == "true");
	}

	SECTION("string") {
		auto object = sol::make_object(lua, "Hello");
		CHECK(sol::utility::to_string(object) == "Hello");

		auto pp = sol::stack::push_pop(object);
		CHECK(sol::utility::to_string(sol::stack_object(lua, -1)) == "Hello");
	}

	SECTION("number") {
		auto object = sol::make_object(lua, 5);
		CHECK(sol::utility::to_string(object) == "5");

		auto pp = sol::stack::push_pop(object);
		CHECK(sol::utility::to_string(sol::stack_object(lua, -1)) == "5");
	}

	SECTION("table") {
		auto object = lua.create_table();
		CHECK_THAT(sol::utility::to_string(object), ContainsSubstring("table"));

		auto pp = sol::stack::push_pop(object);
		CHECK_THAT(sol::utility::to_string(sol::stack_object(lua, -1)), ContainsSubstring("table"));
	}

	SECTION("function") {
		auto object = sol::make_object(lua, &someFunc);
		CHECK_THAT(sol::utility::to_string(object), ContainsSubstring("function"));

		auto pp = sol::stack::push_pop(object);
		CHECK_THAT(sol::utility::to_string(sol::stack_object(lua, -1)), ContainsSubstring("function"));
	}
}
