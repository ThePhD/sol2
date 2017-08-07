#define SOL_CHECK_ARGUMENTS

#include <sol.hpp>
#include <catch.hpp>

#include <iterator>
#include <vector>
#include <list>
#include <forward_list>
#include <deque>
#include <set>
#include <map>
#include <array>
#include <unordered_map>
#include <unordered_set>

template <typename T>
void sequence_container_check(sol::state& lua, T& items) {
	{
		auto r1 = lua.script(R"(
for i=1,#c do 
	v = c[i] 
	assert(v == (i + 10)) 
end
		)", sol::script_pass_on_error);
		REQUIRE(r1.valid());
	}
	{
		auto ffind = [&]() {
			auto r1 = lua.script("i1 = c:find(11)", sol::script_pass_on_error);
			REQUIRE(r1.valid());
			auto r2 = lua.script("i2 = c:find(14)", sol::script_pass_on_error);
			REQUIRE(r2.valid());
		};
		auto fget = [&]() {
			auto r1 = lua.script("v1 = c:get(1)", sol::script_pass_on_error);
			REQUIRE(r1.valid());
			auto r2 = lua.script("v2 = c:get(3)", sol::script_pass_on_error);
			REQUIRE(r2.valid());
		};
		auto fset = [&]() {
			auto r1 = lua.script("c:set(2, 20)", sol::script_pass_on_error);
			REQUIRE(r1.valid());
			auto r2 = lua.script("c:set(6, 16)", sol::script_pass_on_error);
			REQUIRE(r2.valid());
		};
		auto ferase = [&]() {
			auto r5 = lua.script("s1 = #c", sol::script_pass_on_error);
			REQUIRE(r5.valid());
			auto r1 = lua.script("c:erase(i1)", sol::script_pass_on_error);
			REQUIRE(r1.valid());
			auto r3 = lua.script("s2 = #c", sol::script_pass_on_error);
			REQUIRE(r3.valid());
			auto r2 = lua.script("c:erase(i2)", sol::script_pass_on_error);
			REQUIRE(r2.valid());
			auto r4 = lua.script("s3 = #c", sol::script_pass_on_error);
			REQUIRE(r4.valid());
		};
		auto fadd = [&]() {
			auto r = lua.script("c:add(17)", sol::script_pass_on_error);
			REQUIRE(r.valid());
		};
		auto fopset = [&]() {
			auto r = lua.script("c[#c + 1] = 18", sol::script_pass_on_error);
			REQUIRE(r.valid());
		};
		auto fopget = [&]() {
			auto r = lua.script("v3 = c[#c]", sol::script_pass_on_error);
			REQUIRE(r.valid());
		};
		REQUIRE_NOTHROW(ffind());
		REQUIRE_NOTHROW(fget());
		REQUIRE_NOTHROW(fset());
		REQUIRE_NOTHROW(ferase());
		REQUIRE_NOTHROW(fadd());
		REQUIRE_NOTHROW(fopset());
		REQUIRE_NOTHROW(fopget());
	}
	auto backit = items.begin();
	std::size_t len = 0;
	{
		auto e = items.end();
		auto last = backit;
		for (; backit != e; ++backit, ++len) {
			if (backit == e) {
				break;
			}
			last = backit;
		}
		backit = last;
	}
	const int& first = *items.begin();
	const int& last = *backit;
	std::size_t i1 = lua["i1"];
	std::size_t i2 = lua["i2"];
	std::size_t s1 = lua["s1"];
	std::size_t s2 = lua["s2"];
	std::size_t s3 = lua["s3"];
	int v1 = lua["v1"];
	int v2 = lua["v2"];
	int v3 = lua["v3"];
	int values[6] = {
		20, 13, 14, 16, 17, 18
	};
	{
		std::size_t idx = 0;
		for (const auto& i : items) {
			const auto& v = values[idx];
			REQUIRE((i == v));
			++idx;
		}
	}
	REQUIRE((s1 == 6));
	REQUIRE((s2 == 5));
	REQUIRE((s3 == 4));
	REQUIRE((len == 6));
	REQUIRE((first == 20));
	REQUIRE((last == 18));
	REQUIRE((i1 == 1));
	REQUIRE((i2 == 4));
	REQUIRE((v1 == 11));
	REQUIRE((v2 == 13));
	REQUIRE((v3 == 18));
}

template <typename T>
void ordered_container_check(sol::state& lua, T& items) {
	{
		auto r1 = lua.script(R"(
for i=1,#c do 
	v = c[(i + 10)] 
	assert(v == (i + 10)) 
end
		)", sol::script_pass_on_error);
		REQUIRE(r1.valid());
	}
	{
		auto ffind = [&]() {
			auto r1 = lua.script("i1 = c:find(11)", sol::script_pass_on_error);
			REQUIRE(r1.valid());
			auto r2 = lua.script("i2 = c:find(14)", sol::script_pass_on_error);
			REQUIRE(r2.valid());
		};
		auto fget = [&]() {
			auto r1 = lua.script("v1 = c:get(11)", sol::script_pass_on_error);
			REQUIRE(r1.valid());
			auto r2 = lua.script("v2 = c:get(13)", sol::script_pass_on_error);
			REQUIRE(r2.valid());
		};
		auto fset = [&]() {
			auto r1 = lua.script("c:set(20)", sol::script_pass_on_error);
			REQUIRE(r1.valid());
			auto r2 = lua.script("c:set(16)", sol::script_pass_on_error);
			REQUIRE(r2.valid());
		};
		auto ferase = [&]() {
			auto r5 = lua.script("s1 = #c", sol::script_pass_on_error);
			REQUIRE(r5.valid());
			auto r1 = lua.script("c:erase(i1)", sol::script_pass_on_error);
			REQUIRE(r1.valid());
			auto r3 = lua.script("s2 = #c", sol::script_pass_on_error);
			REQUIRE(r3.valid());
			auto r2 = lua.script("c:erase(i2)", sol::script_pass_on_error);
			REQUIRE(r2.valid());
			auto r4 = lua.script("s3 = #c", sol::script_pass_on_error);
			REQUIRE(r4.valid());
		};
		auto fadd = [&]() {
			auto r = lua.script("c:add(17)", sol::script_pass_on_error);
			REQUIRE(r.valid());
		};
		auto fopset = [&]() {
			auto r = lua.script("c[18] = true", sol::script_pass_on_error);
			REQUIRE(r.valid());
		};
		auto fopget = [&]() {
			auto r = lua.script("v3 = c[20]", sol::script_pass_on_error);
			REQUIRE(r.valid());
		};
		REQUIRE_NOTHROW(ffind());
		REQUIRE_NOTHROW(fget());
		REQUIRE_NOTHROW(fset());
		REQUIRE_NOTHROW(ferase());
		REQUIRE_NOTHROW(fadd());
		REQUIRE_NOTHROW(fopset());
		REQUIRE_NOTHROW(fopget());
	}
	auto backit = items.begin();
	std::size_t len = 0;
	{
		auto e = items.end();
		auto last = backit;
		for (; backit != e; ++backit, ++len) {
			if (backit == e) {
				break;
			}
			last = backit;
		}
		backit = last;
	}
	const int& first = *items.begin();
	const int& last = *backit;
	int i1 = lua["i1"];
	int i2 = lua["i2"];
	std::size_t s1 = lua["s1"];
	std::size_t s2 = lua["s2"];
	std::size_t s3 = lua["s3"];
	int v1 = lua["v1"];
	int v2 = lua["v2"];
	int v3 = lua["v3"];
	int values[] = {
		12, 13, 15, 16, 17, 18, 20
	};
	{
		std::size_t idx = 0;
		for (const auto& i : items) {
			const auto& v = values[idx];
			REQUIRE((i == v));
			++idx;
		}
	}
	REQUIRE((s1 == 7));
	REQUIRE((s2 == 6));
	REQUIRE((s3 == 5));
	REQUIRE((len == 7));
	REQUIRE((first == 12));
	REQUIRE((last == 20));
	REQUIRE((i1 == 11));
	REQUIRE((i2 == 14));
	REQUIRE((v1 == 11));
	REQUIRE((v2 == 13));
	REQUIRE((v3 == 20));
}

template <typename T>
void unordered_container_check(sol::state& lua, T& items) {
	{
		auto ffind = [&]() {
			auto r1 = lua.script("i1 = c:find(11)", sol::script_pass_on_error);
			REQUIRE(r1.valid());
			auto r2 = lua.script("i2 = c:find(14)", sol::script_pass_on_error);
			REQUIRE(r2.valid());
		};
		auto fget = [&]() {
			auto r1 = lua.script("v1 = c:get(11)", sol::script_pass_on_error);
			REQUIRE(r1.valid());
			auto r2 = lua.script("v2 = c:get(13)", sol::script_pass_on_error);
			REQUIRE(r2.valid());
		};
		auto fset = [&]() {
			auto r1 = lua.script("c:set(20)", sol::script_pass_on_error);
			REQUIRE(r1.valid());
			auto r2 = lua.script("c:set(16)", sol::script_pass_on_error);
			REQUIRE(r2.valid());
		};
		auto ferase = [&]() {
			auto r5 = lua.script("s1 = #c", sol::script_pass_on_error);
			REQUIRE(r5.valid());
			auto r1 = lua.script("c:erase(i1)", sol::script_pass_on_error);
			REQUIRE(r1.valid());
			auto r3 = lua.script("s2 = #c", sol::script_pass_on_error);
			REQUIRE(r3.valid());
			auto r2 = lua.script("c:erase(i2)", sol::script_pass_on_error);
			REQUIRE(r2.valid());
			auto r4 = lua.script("s3 = #c", sol::script_pass_on_error);
			REQUIRE(r4.valid());
		};
		auto fadd = [&]() {
			auto r = lua.script("c:add(17)", sol::script_pass_on_error);
			REQUIRE(r.valid());
		};
		auto fopset = [&]() {
			auto r = lua.script("c[18] = true", sol::script_pass_on_error);
			REQUIRE(r.valid());
		};
		auto fopget = [&]() {
			auto r = lua.script("v3 = c[20]", sol::script_pass_on_error);
			REQUIRE(r.valid());
		};
		REQUIRE_NOTHROW(ffind());
		REQUIRE_NOTHROW(fget());
		REQUIRE_NOTHROW(fset());
		REQUIRE_NOTHROW(ferase());
		REQUIRE_NOTHROW(fadd());
		REQUIRE_NOTHROW(fopset());
		REQUIRE_NOTHROW(fopget());
	}
	std::size_t len = items.size();
	int i1 = lua["i1"];
	int i2 = lua["i2"];
	std::size_t s1 = lua["s1"];
	std::size_t s2 = lua["s2"];
	std::size_t s3 = lua["s3"];
	int v1 = lua["v1"];
	int v2 = lua["v2"];
	int v3 = lua["v3"];
	int values[] = {
		12, 13, 15, 16, 17, 18, 20
	};
	{
		std::size_t idx = 0;
		for (const auto& i : items) {
			const auto& v = values[idx];
			auto it = items.find(v);
			REQUIRE((it != items.cend()));
			REQUIRE((*it == v));
			++idx;
		}
	}
	REQUIRE((s1 == 7));
	REQUIRE((s2 == 6));
	REQUIRE((s3 == 5));
	REQUIRE((len == 7));
	REQUIRE((i1 == 11));
	REQUIRE((i2 == 14));
	REQUIRE((v1 == 11));
	REQUIRE((v2 == 13));
	REQUIRE((v3 == 20));
}

template <typename T>
void associative_ordered_container_check(sol::state& lua, T& items) {
	{
		auto r1 = lua.script(R"(
for i=1,#c do 
	v = c[(i + 10)] 
	assert(v == (i + 20))
end
		)", sol::script_pass_on_error);
		REQUIRE(r1.valid());
	}
	{
		auto ffind = [&]() {
			auto r1 = lua.script("i1 = c:find(11)", sol::script_pass_on_error);
			REQUIRE(r1.valid());
			auto r2 = lua.script("i2 = c:find(14)", sol::script_pass_on_error);
			REQUIRE(r2.valid());
		};
		auto fget = [&]() {
			auto r1 = lua.script("v1 = c:get(11)", sol::script_pass_on_error);
			REQUIRE(r1.valid());
			auto r2 = lua.script("v2 = c:get(13)", sol::script_pass_on_error);
			REQUIRE(r2.valid());
		};
		auto fset = [&]() {
			auto r1 = lua.script("c:set(20, 30)", sol::script_pass_on_error);
			REQUIRE(r1.valid());
			auto r2 = lua.script("c:set(16, 26)", sol::script_pass_on_error);
			REQUIRE(r2.valid());
			auto r3 = lua.script("c:set(12, 31)", sol::script_pass_on_error);
			REQUIRE(r3.valid());
		};
		auto ferase = [&]() {
			auto r5 = lua.script("s1 = #c", sol::script_pass_on_error);
			REQUIRE(r5.valid());
			auto r1 = lua.script("c:erase(11)", sol::script_pass_on_error);
			REQUIRE(r1.valid());
			auto r3 = lua.script("s2 = #c", sol::script_pass_on_error);
			REQUIRE(r3.valid());
			auto r2 = lua.script("c:erase(14)", sol::script_pass_on_error);
			REQUIRE(r2.valid());
			auto r4 = lua.script("s3 = #c", sol::script_pass_on_error);
			REQUIRE(r4.valid());
		};
		auto fadd = [&]() {
			auto r = lua.script("c:add(17, 27)", sol::script_pass_on_error);
			REQUIRE(r.valid());
		};
		auto fopset = [&]() {
			auto r = lua.script("c[18] = 28", sol::script_pass_on_error);
			REQUIRE(r.valid());
		};
		auto fopget = [&]() {
			auto r = lua.script("v3 = c[20]", sol::script_pass_on_error);
			REQUIRE(r.valid());
		};
		REQUIRE_NOTHROW(ffind());
		REQUIRE_NOTHROW(fget());
		REQUIRE_NOTHROW(fset());
		REQUIRE_NOTHROW(ferase());
		REQUIRE_NOTHROW(fadd());
		REQUIRE_NOTHROW(fopset());
		REQUIRE_NOTHROW(fopget());
	}
	auto backit = items.begin();
	std::size_t len = 0;
	{
		auto e = items.end();
		auto last = backit;
		for (; backit != e; ++backit, ++len) {
			if (backit == e) {
				break;
			}
			last = backit;
		}
		backit = last;
	}
	const std::pair<const short, int>& first = *items.begin();
	const std::pair<const short, int>& last = *backit;
	int i1 = lua["i1"];
	int i2 = lua["i2"];
	std::size_t s1 = lua["s1"];
	std::size_t s2 = lua["s2"];
	std::size_t s3 = lua["s3"];
	int v1 = lua["v1"];
	int v2 = lua["v2"];
	int v3 = lua["v3"];
	std::pair<const short, int> values[] = {
		{ 12, 31 },
		{ 13, 23 },
		{ 15, 25 },
		{ 16, 26 },
		{ 17, 27 },
		{ 18, 28 },
		{ 20, 30 }
	};
	{
		std::size_t idx = 0;
		for (const auto& i : items) {
			const auto& v = values[idx];
			REQUIRE((i == v));
			++idx;
		}
	}
	REQUIRE((s1 == 7));
	REQUIRE((s2 == 6));
	REQUIRE((s3 == 5));
	REQUIRE((len == 7));
	REQUIRE((first.first == 12));
	REQUIRE((last.first == 20));
	REQUIRE((first.second == 31));
	REQUIRE((last.second == 30));
	REQUIRE((i1 == 21));
	REQUIRE((i2 == 24));
	REQUIRE((v1 == 21));
	REQUIRE((v2 == 23));
	REQUIRE((v3 == 30));
}

template <typename T>
void associative_unordered_container_check(sol::state& lua, T& items) {
	{
		auto ffind = [&]() {
			auto r1 = lua.script("i1 = c:find(11)", sol::script_pass_on_error);
			REQUIRE(r1.valid());
			auto r2 = lua.script("i2 = c:find(14)", sol::script_pass_on_error);
			REQUIRE(r2.valid());
		};
		auto fget = [&]() {
			auto r1 = lua.script("v1 = c:get(11)", sol::script_pass_on_error);
			REQUIRE(r1.valid());
			auto r2 = lua.script("v2 = c:get(13)", sol::script_pass_on_error);
			REQUIRE(r2.valid());
		};
		auto fset = [&]() {
			auto r1 = lua.script("c:set(20, 30)", sol::script_pass_on_error);
			REQUIRE(r1.valid());
			auto r2 = lua.script("c:set(16, 26)", sol::script_pass_on_error);
			REQUIRE(r2.valid());
			auto r3 = lua.script("c:set(12, 31)", sol::script_pass_on_error);
			REQUIRE(r3.valid());
		};
		auto ferase = [&]() {
			auto r5 = lua.script("s1 = #c", sol::script_pass_on_error);
			REQUIRE(r5.valid());
			auto r1 = lua.script("c:erase(11)", sol::script_pass_on_error);
			REQUIRE(r1.valid());
			auto r3 = lua.script("s2 = #c", sol::script_pass_on_error);
			REQUIRE(r3.valid());
			auto r2 = lua.script("c:erase(14)", sol::script_pass_on_error);
			REQUIRE(r2.valid());
			auto r4 = lua.script("s3 = #c", sol::script_pass_on_error);
			REQUIRE(r4.valid());
		};
		auto fadd = [&]() {
			auto r = lua.script("c:add(17, 27)", sol::script_pass_on_error);
			REQUIRE(r.valid());
		};
		auto fopset = [&]() {
			auto r = lua.script("c[18] = 28", sol::script_pass_on_error);
			REQUIRE(r.valid());
		};
		auto fopget = [&]() {
			auto r = lua.script("v3 = c[20]", sol::script_pass_on_error);
			REQUIRE(r.valid());
		};
		REQUIRE_NOTHROW(ffind());
		REQUIRE_NOTHROW(fget());
		REQUIRE_NOTHROW(fset());
		REQUIRE_NOTHROW(ferase());
		REQUIRE_NOTHROW(fadd());
		REQUIRE_NOTHROW(fopset());
		REQUIRE_NOTHROW(fopget());
	}
	std::size_t len = items.size();
	int i1 = lua["i1"];
	int i2 = lua["i2"];
	std::size_t s1 = lua["s1"];
	std::size_t s2 = lua["s2"];
	std::size_t s3 = lua["s3"];
	int v1 = lua["v1"];
	int v2 = lua["v2"];
	int v3 = lua["v3"];
	std::pair<const short, int> values[] = {
		{ 12, 31 },
		{ 13, 23 },
		{ 15, 25 },
		{ 16, 26 },
		{ 17, 27 },
		{ 18, 28 },
		{ 20, 30 }
	};
	std::pair<const short, int> item_values[7];
	{
		std::size_t idx = 0;
		for (const auto& i : items) {
			const auto& v = values[idx];
			auto it = items.find(v.first);
			REQUIRE((it != items.cend()));
			REQUIRE((it->second == v.second));
			++idx;
		}
	}
	REQUIRE((s1 == 7));
	REQUIRE((s2 == 6));
	REQUIRE((s3 == 5));
	REQUIRE((len == 7));
	REQUIRE((i1 == 21));
	REQUIRE((i2 == 24));
	REQUIRE((v1 == 21));
	REQUIRE((v2 == 23));
	REQUIRE((v3 == 30));
}

template <typename T>
void fixed_container_check(sol::state& lua, T& items) {
	{
		auto r1 = lua.script(R"(
for i=1,#c do 
	v = c[i] 
	assert(v == (i + 10)) 
end
		)", sol::script_pass_on_error);
		REQUIRE(r1.valid());
	}
	{
		auto ffind = [&]() {
			auto r1 = lua.script("i1 = c:find(11)", sol::script_pass_on_error);
			REQUIRE(r1.valid());
			auto r2 = lua.script("i2 = c:find(14)", sol::script_pass_on_error);
			REQUIRE(r2.valid());
		};
		auto fget = [&]() {
			auto r1 = lua.script("v1 = c:get(2)", sol::script_pass_on_error);
			REQUIRE(r1.valid());
			auto r2 = lua.script("v2 = c:get(5)", sol::script_pass_on_error);
			REQUIRE(r2.valid());
		};
		auto fset = [&]() {
			auto r1 = lua.script("c:set(2, 20)", sol::script_pass_on_error);
			REQUIRE(r1.valid());
			auto r2 = lua.script("c:set(6, 16)", sol::script_pass_on_error);
			REQUIRE_FALSE(r2.valid());
		};
		auto ferase = [&]() {
			auto r5 = lua.script("s1 = #c", sol::script_pass_on_error);
			REQUIRE(r5.valid());
			auto r1 = lua.script("c:erase(i1)", sol::script_pass_on_error);
			REQUIRE_FALSE(r1.valid());
			auto r3 = lua.script("s2 = #c", sol::script_pass_on_error);
			REQUIRE(r3.valid());
			auto r2 = lua.script("c:erase(i2)", sol::script_pass_on_error);
			REQUIRE_FALSE(r2.valid());
			auto r4 = lua.script("s3 = #c", sol::script_pass_on_error);
			REQUIRE(r4.valid());
		};
		auto fadd = [&]() {
			auto r = lua.script("c:add(17)", sol::script_pass_on_error);
			REQUIRE_FALSE(r.valid());
		};
		auto fopset = [&]() {
			auto r = lua.script("c[5] = 18", sol::script_pass_on_error);
			REQUIRE(r.valid());
		};
		auto fopget = [&]() {
			auto r = lua.script("v3 = c[4]", sol::script_pass_on_error);
			REQUIRE(r.valid());
		};
		REQUIRE_NOTHROW(ffind());
		REQUIRE_NOTHROW(fget());
		REQUIRE_NOTHROW(fset());
		REQUIRE_NOTHROW(ferase());
		REQUIRE_NOTHROW(fadd());
		REQUIRE_NOTHROW(fopset());
		REQUIRE_NOTHROW(fopget());
	}
	auto backit = std::begin(items);
	std::size_t len = 0;
	{
		auto e = std::end(items);
		auto last = backit;
		for (; backit != e; ++backit, ++len) {
			if (backit == e) {
				break;
			}
			last = backit;
		}
		backit = last;
	}
	const int& first = *std::begin(items);
	const int& last = *backit;
	int i1 = lua["i1"];
	int i2 = lua["i2"];
	std::size_t s1 = lua["s1"];
	std::size_t s2 = lua["s2"];
	std::size_t s3 = lua["s3"];
	int v1 = lua["v1"];
	int v2 = lua["v2"];
	int v3 = lua["v3"];
	int values[] = {
		11, 20, 13, 14, 18
	};
	{
		std::size_t idx = 0;
		for (const auto& i : items) {
			const auto& v = values[idx];
			REQUIRE((i == v));
			++idx;
		}
	}
	REQUIRE((first == 11));
	REQUIRE((last == 18));
	REQUIRE((s1 == 5));
	REQUIRE((s2 == 5));
	REQUIRE((s3 == 5));
	REQUIRE((len == 5));
	REQUIRE((i1 == 1));
	REQUIRE((i2 == 4));
	REQUIRE((v1 == 12));
	REQUIRE((v2 == 15));
	REQUIRE((v3 == 14));
}

TEST_CASE("containers/sequence containers", "check all of the functinos for every single container") {
	SECTION("vector") {
		sol::state lua;
		lua.open_libraries(sol::lib::base);

		std::vector<int> items{ 11, 12, 13, 14, 15 };
		lua["c"] = &items;
		sequence_container_check(lua, items);
	}
	SECTION("list") {
		sol::state lua;
		lua.open_libraries(sol::lib::base);
		
		std::list<int> items{ 11, 12, 13, 14, 15 };
		lua["c"] = &items;
		sequence_container_check(lua, items);
	}
	SECTION("forward_list") {
		sol::state lua;
		lua.open_libraries(sol::lib::base);

		std::forward_list<int> items{ 11, 12, 13, 14, 15 };
		lua["c"] = &items;
		sequence_container_check(lua, items);
	}
	SECTION("deque") {
		sol::state lua;
		lua.open_libraries(sol::lib::base);

		std::deque<int> items{ 11, 12, 13, 14, 15 };
		lua["c"] = &items;
		sequence_container_check(lua, items);
	}
}

TEST_CASE("containers/fixed containers", "check immutable container types") {
	SECTION("array") {
		sol::state lua;
		lua.open_libraries(sol::lib::base);

		std::array<int, 5> items{ 11, 12, 13, 14, 15 };
		lua["c"] = &items;
		fixed_container_check(lua, items);
	}
	SECTION("array ref") {
		sol::state lua;
		lua.open_libraries(sol::lib::base);

		std::array<int, 5> items{ 11, 12, 13, 14, 15 };
		lua["c"] = std::ref(items);
		fixed_container_check(lua, items);
	}
	SECTION("c array") {
		sol::state lua;
		lua.open_libraries(sol::lib::base);

		int items[5] = { 11, 12, 13, 14, 15 };
		lua["c"] = &items;
		fixed_container_check(lua, items);
	}
	SECTION("c array ref") {
		sol::state lua;
		lua.open_libraries(sol::lib::base);

		int items[5] = { 11, 12, 13, 14, 15 };
		lua["c"] = std::ref(items);
		fixed_container_check(lua, items);
	}
}

TEST_CASE("containers/ordered lookup containers", "check ordered container types") {
	SECTION("set") {
		sol::state lua;
		lua.open_libraries(sol::lib::base);

		std::set<int> items{ 11, 12, 13, 14, 15 };
		lua["c"] = &items;
		ordered_container_check(lua, items);
	}
	SECTION("multiset") {
		sol::state lua;
		lua.open_libraries(sol::lib::base);

		std::multiset<int> items{ 11, 12, 13, 14, 15 };
		lua["c"] = &items;
		ordered_container_check(lua, items);
	}
}

TEST_CASE("containers/unordered lookup containers", "check ordered container types") {
	SECTION("unordered_set") {
		sol::state lua;
		lua.open_libraries(sol::lib::base);

		std::unordered_set<int> items{ 11, 12, 13, 14, 15 };
		lua["c"] = &items;
		unordered_container_check(lua, items);
	}
	SECTION("unordered_multiset") {
		sol::state lua;
		lua.open_libraries(sol::lib::base);

		std::unordered_multiset<int> items{ 11, 12, 13, 14, 15 };
		lua["c"] = &items;
		unordered_container_check(lua, items);
	}
}

TEST_CASE("containers/associative ordered containers", "check associative (map) containers that are ordered fulfill basic functionality requirements") {
	SECTION("map") {
		sol::state lua;
		lua.open_libraries(sol::lib::base);

		std::map<short, int> items{
			{ 11, 21 },
			{ 12, 22 },
			{ 13, 23 },
			{ 14, 24 },
			{ 15, 25 }
		};
		lua["c"] = &items;
		associative_ordered_container_check(lua, items);
	}
	SECTION("multimap") {
		sol::state lua;
		lua.open_libraries(sol::lib::base);

		std::multimap<short, int> items{
			{ 11, 21 },
			{ 12, 22 },
			{ 13, 23 },
			{ 14, 24 },
			{ 15, 25 }
		};
		lua["c"] = &items;
		associative_ordered_container_check(lua, items);
	}
}

TEST_CASE("containers/associative unordered containers", "check associative (map) containers that are ordered that they fulfill basic functionality requirements") {
	SECTION("unordered_map") {
		sol::state lua;
		lua.open_libraries(sol::lib::base);

		std::unordered_map<short, int> items{
			{ 11, 21 },
			{ 12, 22 },
			{ 13, 23 },
			{ 14, 24 },
			{ 15, 25 }
		};
		lua["c"] = &items;
		associative_unordered_container_check(lua, items);
	}
	SECTION("unordered_multimap") {
		sol::state lua;
		lua.open_libraries(sol::lib::base);

		std::unordered_multimap<short, int> items{
			{ 11, 21 },
			{ 12, 22 },
			{ 13, 23 },
			{ 14, 24 },
			{ 15, 25 }
		};
		lua["c"] = &items;
		associative_unordered_container_check(lua, items);
	}
}

TEST_CASE("containers/auxiliary functions test", "make sure the manipulation functions are present and usable and working across various container types") {
	sol::state lua;
	lua.open_libraries();

	lua.script(R"(
function g (x)
	x:add(20)
end

function h (x)
	x:add(20, 40)
end

function i (x)
	x:clear()
end

function sf (x,v)
	return x:find(v)
end

)");

	// Have the function we 
	// just defined in Lua
	sol::function g = lua["g"];
	sol::function h = lua["h"];
	sol::function i = lua["i"];
	sol::function sf = lua["sf"];

	// Set a global variable called 
	// "arr" to be a vector of 5 lements
	lua["c_arr"] = std::array<int, 5>{ { 2, 4, 6, 8, 10 } };
	lua["arr"] = std::vector<int>{ 2, 4, 6, 8, 10 };
	lua["map"] = std::map<int, int>{ { 1 , 2 },{ 2, 4 },{ 3, 6 },{ 4, 8 },{ 5, 10 } };
	lua["set"] = std::set<int>{ 2, 4, 6, 8, 10 };
	std::array<int, 5>& c_arr = lua["c_arr"];
	std::vector<int>& arr = lua["arr"];
	std::map<int, int>& map = lua["map"];
	std::set<int>& set = lua["set"];
	REQUIRE(c_arr.size() == 5);
	REQUIRE(arr.size() == 5);
	REQUIRE(map.size() == 5);
	REQUIRE(set.size() == 5);

	g(lua["set"]);
	g(lua["arr"]);
	h(lua["map"]);
	REQUIRE(arr.size() == 6);
	REQUIRE(map.size() == 6);
	REQUIRE(set.size() == 6);

	{
		int r = sf(set, 8);
		REQUIRE(r == 8);
		sol::object rn = sf(set, 9);
		REQUIRE(rn == sol::nil);
	}

	{
		int r = sf(map, 3);
		REQUIRE(r == 6);
		sol::object rn = sf(map, 9);
		REQUIRE(rn == sol::nil);
	}

	i(lua["arr"]);
	i(lua["map"]);
	i(lua["set"]);
	REQUIRE(arr.empty());
	REQUIRE(map.empty());
	REQUIRE(set.empty());

	REQUIRE_NOTHROW([&]() {
		lua.script(R"(
c_arr[1] = 7
c_arr[2] = 7
c_arr[3] = 7
)");
	}());
}

TEST_CASE("containers/indices test", "test indices on fixed array types") {
	SECTION("zero index test") {
		sol::state lua;
		lua["c_arr"] = std::array<int, 5>{ { 2, 4, 6, 8, 10 } };
		auto result = lua.safe_script(R"(
c_arr[0] = 7
)", sol::script_pass_on_error);
		REQUIRE_FALSE(result.valid());
	}

	SECTION("negative index test") {
		sol::state lua;
		lua["c_arr"] = std::array<int, 5>{ { 2, 4, 6, 8, 10 } };
		auto result = lua.safe_script(R"(
c_arr[-1] = 7
)", sol::script_pass_on_error);
		REQUIRE_FALSE(result.valid());
	}
}
