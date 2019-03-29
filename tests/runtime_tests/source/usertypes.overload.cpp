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

struct overloading_test {
	int print(int i) {
		INFO("Integer print: " << i);
		return 500 + i;
	}
	int print() {
		INFO("No param print.");
		return 500;
	}
};

TEST_CASE("usertype/overloading", "Check if overloading works properly for usertypes") {
	sol::state lua;
	sol::stack_guard luasg(lua);
	lua.open_libraries(sol::lib::base);

	lua.new_usertype<woof>("woof", "var", &woof::var, "func", sol::overload(&woof::func, &woof::func2, &woof::func2s));

	const std::string bark_58 = "bark 58";

	REQUIRE_NOTHROW(
	     lua.safe_script("r = woof:new()\n"
	                     "a = r:func(1)\n"
	                     "b = r:func(1, 2)\n"
	                     "c = r:func(58, 'bark')\n"));
	REQUIRE((lua["a"] == 1));
	REQUIRE((lua["b"] == 3.5));
	REQUIRE((lua["c"] == bark_58));
	auto result = lua.safe_script("r:func(1,2,'meow')", sol::script_pass_on_error);
	REQUIRE_FALSE(result.valid());
	std::cout << "----- end of 7" << std::endl;
}

TEST_CASE("usertype/overloading_values", "ensure overloads handle properly") {
	sol::state lua;
	sol::stack_guard luasg(lua);
	lua.new_usertype<overloading_test>("overloading_test",
	     sol::constructors<>(),
	     "print",
	     sol::overload(
	          static_cast<int (overloading_test::*)(int)>(&overloading_test::print), static_cast<int (overloading_test::*)()>(&overloading_test::print)),
	     "print2",
	     sol::overload(
	          static_cast<int (overloading_test::*)()>(&overloading_test::print), static_cast<int (overloading_test::*)(int)>(&overloading_test::print)));
	lua.set("test", overloading_test());

	sol::function f0_0 = lua.load("return test:print()");
	sol::function f0_1 = lua.load("return test:print2()");
	sol::function f1_0 = lua.load("return test:print(24)");
	sol::function f1_1 = lua.load("return test:print2(24)");
	int res = f0_0();
	int res2 = f0_1();
	int res3 = f1_0();
	int res4 = f1_1();

	REQUIRE(res == 500);
	REQUIRE(res2 == 500);

	REQUIRE(res3 == 524);
	REQUIRE(res4 == 524);
	std::cout << "----- end of 8" << std::endl;
}
