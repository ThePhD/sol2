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


TEST_CASE("containers/vector table roundtrip", "make sure vectors can be round-tripped") {
	sol::state lua;
	std::vector<int> v{ 1, 2, 3 };
	lua.set_function("f", [&]() {
		return sol::as_table(v);
	});
	auto result1 = lua.safe_script("x = f()", sol::script_pass_on_error);
	REQUIRE(result1.valid());
	sol::as_table_t<std::vector<int>> x = lua["x"];
	bool areequal = x.value() == v;
	REQUIRE(areequal);
}

TEST_CASE("containers/deque table roundtrip", "make sure deques can be round-tripped") {
	sol::state lua;
	std::deque<int> v{ 1, 2, 3 };
	lua.set_function("f", [&]() {
		return sol::as_table(v);
	});
	auto result1 = lua.safe_script("x = f()", sol::script_pass_on_error);
	REQUIRE(result1.valid());
	sol::as_table_t<std::deque<int>> x = lua["x"];
	bool areequal = x.value() == v;
	REQUIRE(areequal);
}

TEST_CASE("containers/array table roundtrip", "make sure arrays can be round-tripped") {
	sol::state lua;
	std::array<int, 3> v{ { 1, 2, 3 } };
	lua.set_function("f", [&]() {
		return sol::as_table(v);
	});
	auto result1 = lua.safe_script("x = f()", sol::script_pass_on_error);
	REQUIRE(result1.valid());
	sol::as_table_t<std::array<int, 3>> x = lua["x"];
	bool areequal = x.value() == v;
	REQUIRE(areequal);
}

TEST_CASE("containers/list table roundtrip", "make sure lists can be round-tripped") {
	sol::state lua;
	std::list<int> v{ 1, 2, 3 };
	lua.set_function("f", [&]() {
		return sol::as_table(v);
	});
	auto result1 = lua.safe_script("x = f()", sol::script_pass_on_error);
	REQUIRE(result1.valid());
	sol::as_table_t<std::list<int>> x = lua["x"];
	bool areequal = x.value() == v;
	REQUIRE(areequal);
}

TEST_CASE("containers/forward_list table roundtrip", "make sure forward_lists can be round-tripped") {
	sol::state lua;
	std::forward_list<int> v{ 1, 2, 3 };
	lua.set_function("f", [&]() {
		return sol::as_table(v);
	});
	auto result1 = lua.safe_script("x = f()", sol::script_pass_on_error);
	REQUIRE(result1.valid());
	sol::as_table_t<std::forward_list<int>> x = lua["x"];
	bool areequal = x.value() == v;
	REQUIRE(areequal);
}

TEST_CASE("containers/map table roundtrip", "make sure maps can be round-tripped") {
	sol::state lua;
	std::map<std::string, int> v{ { "a", 1 }, { "b", 2 }, { "c", 3 } };
	lua.set_function("f", [&]() {
		return sol::as_table(v);
	});
	auto result1 = lua.safe_script("x = f()", sol::script_pass_on_error);
	REQUIRE(result1.valid());
	sol::as_table_t<std::map<std::string, int>> x = lua["x"];
	bool areequal = x.value() == v;
	REQUIRE(areequal);
}

TEST_CASE("containers/unordered_map table roundtrip", "make sure unordered_maps can be round-tripped") {
	sol::state lua;
	std::unordered_map<std::string, int> v{ { "a", 1 }, { "b", 2 }, { "c", 3 } };
	lua.set_function("f", [&]() {
		return sol::as_table(v);
	});
	auto result1 = lua.safe_script("x = f()", sol::script_pass_on_error);
	REQUIRE(result1.valid());
	sol::as_table_t<std::unordered_map<std::string, int>> x = lua["x"];
	bool areequal = x.value() == v;
	REQUIRE(areequal);
}

TEST_CASE("containers/unordered_set table roundtrip", "make sure unordered_sets can be round-tripped") {
	sol::state lua;
	std::unordered_set<int> v{ 1, 2, 3 };
	lua.set_function("f", [&]() {
		return sol::as_table(v);
	});
	auto result1 = lua.safe_script("x = f()", sol::script_pass_on_error);
	REQUIRE(result1.valid());
	sol::as_table_t<std::unordered_set<int>> x = lua["x"];
	bool areequal = x.value() == v;
	REQUIRE(areequal);
}

TEST_CASE("containers/set table roundtrip", "make sure sets can be round-tripped") {
	sol::state lua;
	std::set<int> v{ 1, 2, 3 };
	lua.set_function("f", [&]() {
		return sol::as_table(v);
	});
	auto result1 = lua.safe_script("x = f()", sol::script_pass_on_error);
	REQUIRE(result1.valid());
	sol::as_table_t<std::set<int>> x = lua["x"];
	bool areequal = x.value() == v;
	REQUIRE(areequal);
}

TEST_CASE("containers/table conversions (lvalue)", "test table conversions with as_table and nested, when not directly serializing a temporary / new value") {
	sol::state lua;

	auto f = []() {
		std::vector<std::string> response_words;
		response_words.push_back("a");
		response_words.push_back("b");
		response_words.push_back("c");
		return sol::as_table(response_words);
	};
	auto g = []() {
		std::vector<std::string> response_words;
		response_words.push_back("a");
		response_words.push_back("b");
		response_words.push_back("c");
		return sol::as_nested(response_words);
	};

	lua["f"] = std::ref(f);
	lua["g"] = std::ref(g);

	sol::safe_function sff = lua["f"];
	sol::safe_function sfg = lua["g"];
	sol::table tf = sff();
	sol::table tg = sfg();

	std::string af = tf[1];
	std::string bf = tf[2];
	std::string cf = tf[3];
	std::string ag = tf[1];
	std::string bg = tf[2];
	std::string cg = tf[3];
	REQUIRE(tf.size() == 3);
	REQUIRE(af == "a");
	REQUIRE(bf == "b");
	REQUIRE(cf == "c");
	REQUIRE(tg.size() == 3);
	REQUIRE(ag == "a");
	REQUIRE(bg == "b");
	REQUIRE(cg == "c");
}

TEST_CASE("containers/table conversions (std::ref)", "test table conversions with as_table and nested, when not directly serializing a temporary / new value") {
	sol::state lua;

	std::vector<std::string> response_words;
	response_words.push_back("a");
	response_words.push_back("b");
	response_words.push_back("c");
	auto f = [&response_words]() { return sol::as_table(std::ref(response_words)); };
	auto g = [&response_words]() { return sol::as_nested(std::ref(response_words)); };

	lua["f"] = std::ref(f);
	lua["g"] = std::ref(g);

	sol::safe_function sff = lua["f"];
	sol::safe_function sfg = lua["g"];
	sol::table tf = sff();
	sol::table tg = sfg();

	std::string af = tf[1];
	std::string bf = tf[2];
	std::string cf = tf[3];
	std::string ag = tf[1];
	std::string bg = tf[2];
	std::string cg = tf[3];
	REQUIRE(tf.size() == 3);
	REQUIRE(af == "a");
	REQUIRE(bf == "b");
	REQUIRE(cf == "c");
	REQUIRE(tg.size() == 3);
	REQUIRE(ag == "a");
	REQUIRE(bg == "b");
	REQUIRE(cg == "c");
}

TEST_CASE("containers/table conversion", "test table conversions with as_table and nested") {
	sol::state lua;
	lua.open_libraries(sol::lib::base);

	lua.set_function("bark", []() { return sol::as_nested(std::vector<std::string>{ "bark", "woof" }); });

	lua.set_function("woof", []() { return sol::as_nested(std::vector<std::string>{ "bark", "woof" }); });

	auto result1 = lua.safe_script("v1 = bark()", sol::script_pass_on_error);
	REQUIRE(result1.valid());
	auto result2 = lua.safe_script("v2 = woof()", sol::script_pass_on_error);
	REQUIRE(result2.valid());

	sol::as_table_t<std::vector<std::string>> as_table_strings = lua["v1"];
	sol::nested<std::vector<std::string>> nested_strings = lua["v2"];

	std::vector<std::string> expected_values{ "bark", "woof" };
	REQUIRE(as_table_strings.value() == expected_values);
	REQUIRE(nested_strings.value() == expected_values);
}

TEST_CASE("containers/from table argument conversions", "test table conversions without as_table and nested for function args") {
	const std::vector<std::string> expected_values{ "bark", "woof" };

	sol::state lua;
	lua.open_libraries(sol::lib::base);

	lua.set_function("f", [&](std::vector<std::string> t) { return t == expected_values; });

	auto result0 = lua.safe_script("t = { \"bark\", \"woof\" }");
	REQUIRE(result0.valid());

	auto result1 = lua.safe_script("assert(f(t))", sol::script_pass_on_error);
	REQUIRE(result1.valid());

	sol::function f = lua["f"];
	sol::table t = lua["t"];
	bool passed = f(t);
	REQUIRE(passed);
}

TEST_CASE("containers/deeply nested", "make sure nested works for deeply-nested C++ containers and works as advertised") {
	typedef std::map<const char*, std::string> info_t;
	typedef std::vector<info_t> info_vector;

	class ModList {
	public:
		info_vector list;

		ModList() {
			list.push_back(info_t{ { "a", "b" } });
		}

		sol::nested<info_vector&> getList() {
			return sol::nested<info_vector&>(list);
		}
	};

	sol::state lua;
	lua.open_libraries(sol::lib::base);

	lua.new_usertype<ModList>("ModList", "getList", &ModList::getList);

	sol::string_view code = R"(
mods = ModList.new()
local modlist = mods:getList()
print(modlist[1])
assert(type(modlist) == "table")
assert(type(modlist[1]) == "table")
)";

	auto result1 = lua.safe_script(code, sol::script_pass_on_error);
	REQUIRE(result1.valid());
}
