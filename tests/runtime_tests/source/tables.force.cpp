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

#include <catch.hpp>

#include <iostream>

TEST_CASE("tables/force", "allow force by way of key") {
	sol::state lua;
	lua.open_libraries(sol::lib::base, sol::lib::io);

	sol::optional<int> not_there = lua["a"]["b"]["c"];
	REQUIRE_FALSE(static_cast<bool>(not_there));
	lua["a"].force()["b"].force()["c"] = 357;
	sol::optional<int> totally_there = lua["a"]["b"]["c"];
	REQUIRE(static_cast<bool>(totally_there));
	REQUIRE(*totally_there == 357);
}

TEST_CASE("tables/proxy force", "allow proxies to force creation of tables with explicit function") {
	SECTION("explicit") {
		sol::state lua;

		sol::optional<int> not_there = lua["a"]["b"]["c"];
		REQUIRE_FALSE(static_cast<bool>(not_there));
		lua["a"].force()["b"].force()["c"] = 357;
		sol::optional<int> totally_there = lua["a"]["b"]["c"];
		REQUIRE(static_cast<bool>(totally_there));
		REQUIRE(*totally_there == 357);
	}
	SECTION("key type") {
		sol::state lua;

		sol::optional<int> not_there = lua["a"]["b"]["c"];
		REQUIRE_FALSE(static_cast<bool>(not_there));
		lua[sol::force]["a"]["b"]["c"] = 357;
		sol::optional<int> totally_there = lua["a"]["b"]["c"];
		REQUIRE(static_cast<bool>(totally_there));
		REQUIRE(*totally_there == 357);
	}
}
