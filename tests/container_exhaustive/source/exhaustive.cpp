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

#include <cstdint>
#include <limits>

inline namespace sol2_tests_exhaustive {
	struct Link {
		int value;
	};

	struct NonLink {
		void* value;
	};

	inline constexpr int FAILURE_CONSTANT = 1000;

	inline constexpr int LINK_CONSTANT = 0xA837;

	inline int ex_f0(sol::exhaustive<std::vector<Link>> ex_vec) {
		const auto& vec = ex_vec.value();
		for (const auto& elem : vec) {
			REQUIRE(elem.value == LINK_CONSTANT);
		}
		return 0;
	}

	inline int ex_f1(sol::exhaustive<std::vector<uint64_t>> ex_vec) {
		const auto& vec = ex_vec.value();
		for (std::size_t i = 0; i < vec.size(); ++i) {
			const auto& elem = vec[i];
			REQUIRE(elem == static_cast<uint64_t>(i + 1));
		}
		return 1;
	}

	inline int ex_f2(sol::exhaustive<std::vector<NonLink>> ex_vec, sol::this_state this_lua) {
		const auto& vec = ex_vec.value();
		void* address = static_cast<void*>(this_lua.lua_state());
		for (const auto& elem : vec) {
			REQUIRE(elem.value == address);
		}
		return 2;
	}

	inline int ex_ffail(sol::object o) {
		REQUIRE(o.get_type() == sol::type::table);
		return FAILURE_CONSTANT;
	}
} // namespace sol2_tests_exhaustive

TEST_CASE("large_integer/bool", "pass bool integral value to and from lua") {
	sol::state lua;
	lua.open_libraries(sol::lib::base);

	void* address = lua.lua_state();

	lua["FAILURE_CONSTANT"] = FAILURE_CONSTANT;
	lua["link_obj"] = Link { LINK_CONSTANT };
	lua["nonlink_obj"] = NonLink { address };

	sol::optional<sol::error> setup_result = lua.safe_script(R"(
		expect0_0 = { link_obj, link_obj, link_obj, link_obj }
		expect0_1 = { link_obj, 2, 3, 4 }
		expect1_0 = { 1, 2, 3, 4, 5 }
		expect1_1 = { 1, link_obj, 3, 4, 5 }
		expect2_0 = { nonlink_obj, nonlink_obj, nonlink_obj, nonlink_obj }
		expect2_1 = { nonlink_obj, 2, 3, 4, 5 }
	)");
	REQUIRE_FALSE(setup_result.has_value());

	lua.set_function("get_table_exhaustive", sol::overload(ex_f0, ex_f1, ex_f2, ex_ffail));
	{
		sol::optional<sol::error> result = lua.safe_script(R"(
			result0_0 = get_table_exhaustive(expect0_0)
			assert(result0_0 == 0)
			result0_1 = get_table_exhaustive(expect0_1)
			assert(result0_1 == FAILURE_CONSTANT)
		)");
		REQUIRE_FALSE(result.has_value());
	}
	{
		sol::optional<sol::error> result = lua.safe_script(R"(
			result1_0 = get_table_exhaustive(expect1_0)
			assert(result1_0 == 1)
			result1_1 = get_table_exhaustive(expect1_1)
			assert(result1_1 == FAILURE_CONSTANT)
		)");
		REQUIRE_FALSE(result.has_value());
	}
	{
		sol::optional<sol::error> result = lua.safe_script(R"(
			result2_0 = get_table_exhaustive(expect2_0)
			assert(result2_0 == 2)
			result2_1 = get_table_exhaustive(expect2_1)
			assert(result2_1 == FAILURE_CONSTANT)
		)");
		REQUIRE_FALSE(result.has_value());
	}
}
