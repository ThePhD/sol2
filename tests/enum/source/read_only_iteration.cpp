// sol2

// The MIT License (MIT)

// Copyright (c) 2013-2021 Rapptz, ThePhD and contributors

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

#include <catch2/catch.hpp>

#include <sol/sol.hpp>

inline namespace sol2_tests_enum_read_only_iteration {
	enum class color { red, blue };
}


TEST_CASE("environments/sanboxing", "see if environments on functions are working properly") {
	sol::state lua;
	lua.open_libraries(sol::lib::base);
	constexpr bool ro = true;
	lua.new_enum<color, ro>("color", { { "red", color::red }, { "blue", color::blue } });
	auto script = R"lua(
		print( "start" )
		for k, v in pairs( color ) do
			print( tostring(k) .. ": " .. tostring(v) )
		end
		print( "end" )
	)lua";
	auto result = lua.safe_script(script, sol::script_pass_on_error);
	sol::optional<sol::error> maybe_error = result;
	REQUIRE(result.valid());
	REQUIRE(result.status() == sol::call_status::ok);
	REQUIRE_FALSE(maybe_error.has_value());
}
