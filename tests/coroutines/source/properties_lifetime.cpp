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

inline namespace sol2_regression_test_coroutines_properties_lifetime {
	struct A {
	public:
		A() {
		}

		sol::object index(sol::object key) {
			if (mProperties.valid()) {
				auto prop = mProperties[key];
				return prop.get<sol::object>();
			}
			return sol::object();
		}

		void new_index(sol::object key, sol::object value, sol::this_main_state L) {
			if (!mProperties.valid()) {
				sol::state_view lua(L);
				mProperties = lua.create_table();
			}
			if (mProperties.lua_state() != key.lua_state()) {
				std::cout << "scream A" << std::endl;
			}
			if (mProperties.lua_state() != value.lua_state()) {
				std::cout << "scream B" << std::endl;
			}
			if (mProperties.lua_state() != L.lua_state()) {
				std::cout << "scream C" << std::endl;
			}
			mProperties[key] = value;
		}

	protected:
		// ❗❗ Here ❗❗
		sol::main_table mProperties;
	};
} // namespace sol2_regression_test_coroutines_properties_lifetime

TEST_CASE("issue #1374 - coroutine lifetime tests (using main_* classes)", "[sol2][regression][issue-1374]") {
	sol::state lua;

	A a;

	lua.open_libraries(
	     sol::lib::base, sol::lib::package, sol::lib::table, sol::lib::debug, sol::lib::string, sol::lib::math, sol::lib::bit32, sol::lib::coroutine);

	lua.new_usertype<A>("A", sol::meta_function::index, &A::index, sol::meta_function::new_index, &A::new_index);

	lua["A"] = &a;

	const auto& code = R"(
for i=1, 300 do
	co = coroutine.create( function()
		A.foo = 7
		A.bar = {}
		for j=1, 170 do
			A.bar[j] = j
		end
	end)
	coroutine.resume(co)
	print(A.foo)
	for _, value in pairs(A.bar) do
		print(value)
	end
end)";

	// call lua code directly
	sol::optional<sol::error> result = lua.safe_script(code);
	REQUIRE_FALSE(result.has_value());
}
