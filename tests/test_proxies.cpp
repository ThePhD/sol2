// sol2 

// The MIT License (MIT)

// Copyright (c) 2013-2017 Rapptz, ThePhD and contributors

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

#define SOL_CHECK_ARGUMENTS 1
#define SOL_ENABLE_INTEROP 1

#include <sol.hpp>

#include <catch.hpp>

#include <iostream>
#include "test_stack_guard.hpp"

TEST_CASE("proxy/function results", "make sure that function results return proper proxies and can be indexed nicely") {
	sol::state lua;
	SECTION("unsafe_function_result") {
		auto ufr = lua.script("return 1, 2, 3, 4");
		int accum = 0;
		for (const auto& r : ufr) {
			int v = r;
			accum += v;
		}
		REQUIRE(accum == 10);
	}
	SECTION("protected_function_result") {
		auto pfr = lua.safe_script("return 1, 2, 3, 4");
		int accum = 0;
		for (const auto& r : pfr) {
			int v = r;
			accum += v;
		}
		REQUIRE(accum == 10);
	}
}
