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

#include <memory>

inline namespace sol2_regression_test_1757 {
	struct interface_1757 {
		virtual int value() const = 0;

	protected:
		~interface_1757() = default; // not to be deleted through the interface
	};

	struct impl_1757 final : interface_1757 {
		int value() const override {
			return 42;
		}
	};
} // namespace sol2_regression_test_1757

TEST_CASE("issue #1757 - shared_ptr to a type with a protected destructor is released by the garbage collector",
     "[sol2][regression][issue1757]") {
	static_assert(!std::is_destructible_v<interface_1757>);

	sol::state lua;
	lua.open_libraries(sol::lib::base);
	lua.new_usertype<interface_1757>("interface_1757", "value", &interface_1757::value);

	std::shared_ptr<interface_1757> p = std::make_shared<impl_1757>();
	lua["get"] = [&p]() { return p; };

	auto result = lua.safe_script("for i = 1, 10 do local o = get(); assert(o:value() == 42) end", sol::script_pass_on_error);
	REQUIRE(result.valid());
	lua.collect_garbage();
	lua.collect_garbage();
	REQUIRE(p.use_count() == 1);
}
