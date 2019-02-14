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

#include <iterator>
#include <vector>
#include <list>
#include <forward_list>
#include <map>
#include <deque>
#include <array>
#include <unordered_map>
#include <set>
#include <unordered_set>


auto test_table_return_one() {
	return sol::as_table(std::vector<int>{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 });
}

auto test_table_return_two() {
	return sol::as_table(std::vector<std::pair<std::string, int>>{ { "one", 1 }, { "two", 2 }, { "three", 3 } });
}

auto test_table_return_three() {
	return sol::as_table(std::map<std::string, std::string>{ { "name", "Rapptz" }, { "friend", "ThePhD" }, { "project", "sol" } });
}

auto test_table_return_four() {
	return sol::as_table(std::array<std::pair<std::string, int>, 4>{ { { "one", 1 }, { "two", 2 }, { "three", 3 }, { "four", 4 } } });
}

template <typename S, typename T>
void check_ordered_values(S& src, T& target) {
	std::size_t idx = 0;
	auto b = std::begin(target);
	auto e = std::end(target);
	for (; b != e; ++b, ++idx) {
		const auto& v = src[idx];
		REQUIRE((*b == v));
	}
}

template <typename S, typename T>
void table_check_unordered_values(S& src, T& target) {
	std::size_t idx = 0;
	auto b = std::begin(target);
	auto e = std::end(target);
	for (; b != e; ++b, ++idx) {
		auto sb = std::begin(src);
		auto se = std::end(src);
		auto it = std::find(sb, se, *b);
		REQUIRE((it != se));
	}
}

TEST_CASE("containers/arbitrary creation", "userdata and tables should be usable from standard containers") {
	sol::state lua;
	lua.open_libraries(sol::lib::base);
	lua.set_function("test_one", test_table_return_one);
	lua.set_function("test_two", test_table_return_two);
	lua.set_function("test_three", test_table_return_three);
	lua.set_function("test_four", test_table_return_four);

	{
		auto result = lua.safe_script("a = test_one()", sol::script_pass_on_error);
		REQUIRE(result.valid());
	}
	{
		auto result = lua.safe_script("b = test_two()", sol::script_pass_on_error);
		REQUIRE(result.valid());
	}
	{
		auto result = lua.safe_script("c = test_three()", sol::script_pass_on_error);
		REQUIRE(result.valid());
	}
	{
		auto result = lua.safe_script("d = test_four()", sol::script_pass_on_error);
		REQUIRE(result.valid());
	}

	{
		auto result = lua.safe_script("assert(#a == 10, 'error')", sol::script_pass_on_error);
		REQUIRE(result.valid());
	}
	{
		auto result = lua.safe_script("assert(a[3] == 3, 'error')", sol::script_pass_on_error);
		REQUIRE(result.valid());
	}
	{
		auto result = lua.safe_script("assert(b.one == 1, 'error')", sol::script_pass_on_error);
		REQUIRE(result.valid());
	}
	{
		auto result = lua.safe_script("assert(b.three == 3, 'error')", sol::script_pass_on_error);
		REQUIRE(result.valid());
	}
	{
		auto result = lua.safe_script("assert(c.name == 'Rapptz', 'error')", sol::script_pass_on_error);
		REQUIRE(result.valid());
	}
	{
		auto result = lua.safe_script("assert(c.project == 'sol', 'error')", sol::script_pass_on_error);
		REQUIRE(result.valid());
	}
	{
		auto result = lua.safe_script("assert(d.one == 1, 'error')", sol::script_pass_on_error);
		REQUIRE(result.valid());
	}
	{
		auto result = lua.safe_script("assert(d.three == 3, 'error')", sol::script_pass_on_error);
		REQUIRE(result.valid());
	}
	{
		auto result = lua.safe_script("assert(d.four == 4, 'error')", sol::script_pass_on_error);
		REQUIRE(result.valid());
	}

	sol::table a = lua.get<sol::table>("a");
	sol::table b = lua.get<sol::table>("b");
	sol::table c = lua.get<sol::table>("c");
	sol::table d = lua["d"];

	REQUIRE(a.size() == 10ULL);
	REQUIRE(a.get<int>(3) == 3);
	REQUIRE(b.get<int>("one") == 1);
	REQUIRE(b.get<int>("three") == 3);
	REQUIRE(c.get<std::string>("name") == "Rapptz");
	REQUIRE(c.get<std::string>("project") == "sol");
	REQUIRE(d.get<int>("one") == 1);
	REQUIRE(d.get<int>("three") == 3);
	REQUIRE(d.get<int>("four") == 4);
}

TEST_CASE("containers/table serialization", "ensure types can be serialized as tables still") {
	typedef std::vector<int> woof;
	sol::state lua;
	lua.open_libraries();
	lua.set("b", sol::as_table(woof{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30 }));
	{
		auto result = lua.safe_script("for k, v in ipairs(b) do assert(k == v) end", sol::script_pass_on_error);
		REQUIRE(result.valid());
	}
	woof w{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30 };
	lua.set("b", sol::as_table(w));
	{
		auto result = lua.safe_script("for k, v in ipairs(b) do assert(k == v) end", sol::script_pass_on_error);
		REQUIRE(result.valid());
	}
	lua.set("b", sol::as_table(&w));
	{
		auto result = lua.safe_script("for k, v in ipairs(b) do assert(k == v) end", sol::script_pass_on_error);
		REQUIRE(result.valid());
	}
	lua.set("b", sol::as_table(std::ref(w)));
	{
		auto result = lua.safe_script("for k, v in ipairs(b) do assert(k == v) end", sol::script_pass_on_error);
		REQUIRE(result.valid());
	}
}

TEST_CASE("containers/initializer-list", "test initializer lists get pushed as tables directly rather than userdata") {
	SECTION("array-like") {
		sol::state lua;
		lua.open_libraries(sol::lib::base, sol::lib::table);

		lua["c"] = { 1, 2, 3, 4, 5 };
		auto result1 = lua.safe_script(R"lua(
for k, v in pairs(c) do
  assert(k == v)
end
)lua",
		     sol::script_pass_on_error);
		sol::as_table_t<std::vector<int>> t1vector = lua["c"];
		sol::as_table_t<std::deque<int>> t1deque = lua["c"];
		sol::as_table_t<std::list<int>> t1list = lua["c"];
		sol::as_table_t<std::forward_list<int>> t1flist = lua["c"];
		sol::as_table_t<std::set<int>> t1set = lua["c"];
		const int src[5] = { 1, 2, 3, 4, 5 };
		check_ordered_values(src, t1vector.source);
		check_ordered_values(src, t1deque.source);
		check_ordered_values(src, t1list.source);
		check_ordered_values(src, t1flist.source);
		check_ordered_values(src, t1set.source);
	}
	SECTION("map-like") {
		sol::state lua;
		lua.open_libraries(sol::lib::base, sol::lib::table);
		std::pair<const std::string, int> src[5]{ { "a", 21 }, { "b", 22 }, { "c", 23 }, { "d", 24 }, { "e", 25 } };

		lua["c"] = std::initializer_list<std::pair<std::string, int>>{ { "a", 21 }, { "b", 22 }, { "c", 23 }, { "d", 24 }, { "e", 25 } };

		sol::as_table_t<std::unordered_map<std::string, int>> t1umap = lua["c"];
		sol::as_table_t<std::unordered_multimap<std::string, int>> t1ummap = lua["c"];
		table_check_unordered_values(src, t1umap.source);
		table_check_unordered_values(src, t1ummap.source);
	}
}
