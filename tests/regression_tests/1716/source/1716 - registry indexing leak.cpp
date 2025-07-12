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

#include <catch2/catch_all.hpp>

#include <sol/sol.hpp>

TEST_CASE("#1716 - index and newindex should not leak values into the registry", "[sol2][regression][issue1716]") {
	class vector {
	public:
		vector() = default;

		static int my_index(vector&, int) {
			return 0;
		}

		static void my_new_index(vector&, int, int) {
			return;
		}
	};

	sol::state lua;
	lua.open_libraries(sol::lib::base);
	lua.new_usertype<vector>("vector",
	     sol::constructors<sol::types<>>(), //
	     sol::meta_function::index,
	     &vector::my_index, //
	     sol::meta_function::new_index,
	     &vector::my_new_index);
	auto begin = lua.registry().size();
	lua.script(R"(
			local v = vector.new()
			local unused = 0
			for i=1,100 do
				unused = v[1]
				v[1] = 1
			end
		)");

	REQUIRE(lua.registry().size() <= begin + 1);
}
