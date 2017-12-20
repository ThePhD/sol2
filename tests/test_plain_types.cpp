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

TEST_CASE("plain/alignment", "test that aligned classes in certain compilers don't trigger compiler errors") {
#ifdef _MSC_VER
	__declspec(align(16)) class aligned_class {
		int var;
	};

	class A {
		aligned_class a;
	};

	sol::state lua;
	lua.open_libraries(sol::lib::base, sol::lib::math);

	lua.new_usertype<A>("A");
	A a;
	lua["a"] = &a;
	A& la = lua["a"];
	REQUIRE(&a == &la);
#else
	REQUIRE(true);
#endif // VC++ stuff
}

TEST_CASE("plain/indestructible", "test that we error for types that are innately indestructible") {
	struct indestructible {
	public:
		int v = 20;

		struct insider {
			void operator()(indestructible* i) const {
				i->~indestructible();
			}
		};

	private:
		~indestructible() {
		}
	};

	SECTION("doomed") {
		sol::state lua;
		lua.open_libraries(sol::lib::base);

		std::unique_ptr<indestructible, indestructible::insider> i = sol::detail::make_unique_deleter<indestructible, indestructible::insider>();
		lua["i"] = *i;
		lua.safe_script("i = nil");
		auto result = lua.safe_script("collectgarbage()", sol::script_pass_on_error);
		REQUIRE_FALSE(result.valid());
	}
	SECTION("saved") {
		sol::state lua;
		lua.open_libraries(sol::lib::base);

		std::unique_ptr<indestructible, indestructible::insider> i = sol::detail::make_unique_deleter<indestructible, indestructible::insider>();
		lua["i"] = *i;
		lua.new_usertype<indestructible>("indestructible",
			sol::default_constructor,
			sol::meta_function::garbage_collect, sol::destructor([](indestructible& i) {
				indestructible::insider del;
				del(&i);
			}));
		lua.safe_script("i = nil");
		auto result = lua.safe_script("collectgarbage()", sol::script_pass_on_error);
		REQUIRE(result.valid());
	}
}