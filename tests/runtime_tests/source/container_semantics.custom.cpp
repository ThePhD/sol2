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

#include <catch.hpp>

#include <iterator>
#include <numeric> // std::iota


struct my_object {
private:
	std::vector<int> mdata;

public:
	static const void* last_printed;

	my_object(int sz) : mdata() {
		mdata.resize(sz);
		std::iota(mdata.begin(), mdata.end(), 1);
	}

	void operator()(std::size_t count, int value) {
		for (; count > 0; --count) {
			mdata.push_back(value);
		}
	}

public: // Container requirements, as per the C++ standard
	using value_type = int;
	using reference = value_type&;
	using const_reference = const value_type&;
	using iterator = decltype(mdata)::iterator;
	using const_iterator = decltype(mdata)::const_iterator;
	using difference_type = decltype(mdata)::difference_type;
	using size_type = decltype(mdata)::size_type;

	iterator begin() {
		return iterator(mdata.begin());
	}
	iterator end() {
		return iterator(mdata.end());
	}
	const_iterator begin() const {
		return const_iterator(mdata.begin());
	}
	const_iterator end() const {
		return const_iterator(mdata.end());
	}
	const_iterator cbegin() const {
		return begin();
	}
	const_iterator cend() const {
		return end();
	}
	size_type size() const noexcept {
		return mdata.size();
	}
	size_type max_size() const noexcept {
		return mdata.max_size();
	}
	void push_back(const value_type& v) {
		mdata.push_back(v);
	}
	void insert(const_iterator where, const value_type& v) {
		mdata.insert(where, v);
	}
	bool empty() const noexcept {
		return mdata.empty();
	}
	bool operator==(const my_object& right) const {
		return mdata == right.mdata;
	}
	bool operator!=(const my_object& right) const noexcept {
		return mdata != right.mdata;
	}

	std::vector<int>& data() {
		return mdata;
	}

	const std::vector<int>& data() const {
		return mdata;
	}
};

const void* my_object::last_printed = nullptr;

std::ostream& operator<<(std::ostream& ostr, const my_object& mo) {
	my_object::last_printed = static_cast<const void*>(&mo);
	ostr << "{ ";
	const auto& v = mo.data();
	if (v.empty()) {
		ostr << "empty";
	}
	else {
		ostr << v[0];
		for (std::size_t i = 1; i < v.size(); ++i) {
			ostr << ", " << v[i];
		}
	}
	ostr << " }";

	return ostr;
}

namespace sol {
	template <>
	struct is_container<my_object> : std::false_type {};
} // namespace sol

TEST_CASE("containers/as_container reference", "test that we can force a container to be treated like one despite is_container being false_type") {
	sol::state lua;
	lua.open_libraries(sol::lib::base);

	lua.new_usertype<my_object>("my_object",
	     sol::constructors<my_object(int)>(),
	     sol::call_constructor,
	     sol::constructors<my_object(int)>(),
	     "size",
	     &my_object::size,
	     "iterable",
	     [](my_object& mo) { return sol::as_container(mo); });

#if SOL_LUA_VERSION > 501
	auto result1 = lua.safe_script(R"(
mop = my_object.new(20)
for i, v in pairs(mop) do
	assert(i == v)
end
print(mop)
	)",
	     sol::script_pass_on_error);
	REQUIRE(result1.valid());
	REQUIRE_NOTHROW([&]() {
		my_object& mo = lua["mop"];
		REQUIRE((&mo == my_object::last_printed));
	}());
#endif
	auto result2 = lua.safe_script(R"(
mo = my_object(10)
c_mo = mo
c_iterable = mo:iterable()
)",
	     sol::script_pass_on_error);
	REQUIRE(result2.valid());

	REQUIRE_NOTHROW([&]() {
		my_object& mo = lua["c_mo"];
		my_object& mo_iterable = lua["c_iterable"];
		REQUIRE(&mo == &mo_iterable);
		REQUIRE(mo == mo_iterable);
	}());

	auto result3 = lua.safe_script(R"(
s1 = c_mo:size()
s1_len = #c_mo
s1_iterable = c_iterable:size()
s1_iterable_len = #c_iterable
)");
	REQUIRE(result3.valid());

	REQUIRE_NOTHROW([&]() {
		std::size_t s1 = lua["s1"];
		std::size_t s1_len = lua["s1_len"];
		std::size_t s1_iterable = lua["s1_iterable"];
		std::size_t s1_iterable_len = lua["s1_iterable_len"];
		REQUIRE(s1 == 10);
		REQUIRE(s1 == s1_len);
		REQUIRE(s1 == s1_iterable_len);
		REQUIRE(s1_iterable == s1_iterable_len);
	}());

	auto result4 = lua.safe_script(R"(
for i=1,#c_mo do
	v_iterable = c_iterable[i]
	assert(v_iterable == i)
end
)",
	     sol::script_pass_on_error);
	REQUIRE(result4.valid());

	auto result5 = lua.safe_script(R"(
mo(5, 20)
c_iterable:insert(1, 100)
v1 = c_iterable[1]
s2 = c_mo:size()
s2_len = #c_mo
s2_iterable = c_iterable:size()
s2_iterable_len = #c_iterable
print(mo)
	)",
	     sol::script_pass_on_error);
	REQUIRE(result5.valid());

	int v1 = lua["v1"];
	std::size_t s2 = lua["s2"];
	std::size_t s2_len = lua["s2_len"];
	std::size_t s2_iterable = lua["s2_iterable"];
	std::size_t s2_iterable_len = lua["s2_iterable_len"];
	REQUIRE(v1 == 100);
	REQUIRE(s2 == 16);
	REQUIRE(s2 == s2_len);
	REQUIRE(s2 == s2_iterable_len);
	REQUIRE(s2_iterable == s2_iterable_len);

	my_object& mo = lua["mo"];
	REQUIRE(&mo == my_object::last_printed);
}

TEST_CASE("containers/as_container", "test that we can force a container to be treated like one despite the trait being false using the proper marker") {
	sol::state lua;
	lua.open_libraries(sol::lib::base);

	lua.set_function("f", [](int v) { return sol::as_container(my_object(v)); });

#if SOL_LUA_VERSION > 501
	auto result1 = lua.safe_script(R"(
mop = f(20)
for i, v in pairs(mop) do
	assert(i == v)
end
	)");
	REQUIRE(result1.valid());
#endif
	auto result2 = lua.safe_script(R"(
mo = f(10)
c_iterable = mo
)");
	REQUIRE(result2.valid());

	{
		my_object& mo = lua["mo"];
		my_object& mo_iterable = lua["c_iterable"];
		REQUIRE(&mo == &mo_iterable);
		REQUIRE(mo == mo_iterable);
	}

	auto result3 = lua.safe_script(R"(
s1_iterable = c_iterable:size()
s1_iterable_len = #c_iterable
)");
	REQUIRE(result3.valid());

	{
		std::size_t s1_iterable = lua["s1_iterable"];
		std::size_t s1_iterable_len = lua["s1_iterable_len"];
		REQUIRE(s1_iterable == 10);
		REQUIRE(s1_iterable == s1_iterable_len);
	}

	auto result4 = lua.safe_script(R"(
for i=1,#c_iterable do
	v_iterable = c_iterable[i]
	assert(v_iterable == i)
end
)");
	REQUIRE(result4.valid());

	auto result5 = lua.safe_script(R"(
c_iterable:insert(1, 100)
v1 = c_iterable:get(1)
s2_iterable = c_iterable:size()
s2_iterable_len = #c_iterable
	)");
	REQUIRE(result5.valid());

	{
		int v1 = lua["v1"];
		std::size_t s2_iterable = lua["s2_iterable"];
		std::size_t s2_iterable_len = lua["s2_iterable_len"];
		REQUIRE(v1 == 100);
		REQUIRE(s2_iterable_len == 11);
		REQUIRE(s2_iterable == s2_iterable_len);
	}
}
