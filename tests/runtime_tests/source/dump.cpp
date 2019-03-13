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

TEST_CASE("dump/dump transfer", "test that a function can be transferred from one place to another") {
	sol::state lua;
	sol::state lua2;
	lua2.open_libraries(sol::lib::base);

	sol::load_result lr = lua.load("a = function (v) print(v) return v end");
	REQUIRE(lr.valid());
	sol::protected_function target = lr;
	sol::bytecode target_bc = target.dump();

	auto result2 = lua2.safe_script(target_bc.as_string_view(), sol::script_pass_on_error);
	REQUIRE(result2.valid());
	sol::protected_function pf = lua2["a"];
	int v = pf(25557);
	REQUIRE(v == 25557);
}
