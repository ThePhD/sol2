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
#include <vector>
#include <memory>
#include <iterator>

inline namespace sol2_regression_test_coroutines_array_proxy_lifetime {
	struct A {
	public:
		A(int value_) : value(value_) {
		}
		int value;

		int getValue() {
			return value;
		}
		std::vector<std::shared_ptr<A>> children;
	};

	struct ArrayProxy {
		using value_type = std::weak_ptr<A>;

		struct iterator {
			using iterator_category = std::random_access_iterator_tag;
			using difference_type = std::ptrdiff_t;
			using value_type = std::weak_ptr<A>;
			using pointer = std::weak_ptr<A>*;
			using reference = std::weak_ptr<A>&;

			const ArrayProxy& a;
			size_t index;

			iterator(const ArrayProxy& a_, size_t index_) : a(a_), index(index_) {
			}

			value_type operator*() const {
				size_t size = a.mpParent.children.size();
				if (index < size) {
					return a.mpParent.children[index];
				}
				return std::weak_ptr<A>();
			}

			// Operators : arithmetic
			inline iterator& operator++() {
				++index;
				return *this;
			}
			inline iterator& operator--() {
				--index;
				return *this;
			}
			inline iterator& operator+=(const size_t& rhs) {
				index += rhs;
				return *this;
			}
			inline iterator& operator-=(const size_t& rhs) {
				index -= rhs;
				return *this;
			}

			// Operators : comparison
			inline bool operator==(const iterator& rhs) {
				return index == rhs.index;
			}
			inline bool operator!=(const iterator& rhs) {
				return index != rhs.index;
			}
			inline bool operator>(const iterator& rhs) {
				return index > rhs.index;
			}
			inline bool operator<(const iterator& rhs) {
				return index < rhs.index;
			}
			inline bool operator>=(const iterator& rhs) {
				return index >= rhs.index;
			}
			inline bool operator<=(const iterator& rhs) {
				return index <= rhs.index;
			}
		};

		ArrayProxy(const A& parent) : mpParent(parent) {
		}

		~ArrayProxy() {
		}

		auto begin() const {
			return iterator(*this, 0);
		}
		auto end() const {
			return iterator(*this, mpParent.children.size());
		}
		size_t size() const {
			return mpParent.children.size();
		}

		const A& mpParent;
	};
	static ArrayProxy getChildren(const A& a) {
		return ArrayProxy(a);
	}
} // namespace sol2_regression_test_coroutines_array_proxy_lifetime

namespace sol {
	template <typename T>
	struct unique_usertype_traits<std::weak_ptr<T>> {
		static T* get(lua_State*, const std::weak_ptr<T>& ptr) noexcept {
			return ptr.lock().get();
		}

		static bool is_null(lua_State*, const std::weak_ptr<T>& ptr) noexcept {
			return ptr.expired();
		}
	};

	template <>
	struct is_container<ArrayProxy> : std::true_type { };
} // namespace sol

TEST_CASE("test for issue #1400 - array proxy tests", "[sol2][regression][issue-1400]") {
	sol::state lua;
	lua.open_libraries(
	     sol::lib::base, sol::lib::package, sol::lib::table, sol::lib::debug, sol::lib::string, sol::lib::math, sol::lib::bit32, sol::lib::coroutine);

	const int outer = 10;
	const int inner = 100;
	A a(0);
	for (int i = 0; i < outer; i++) {
		auto child = std::make_shared<A>(i * outer);
		for (int j = 0; j < inner; j++) {
			auto child2 = std::make_shared<A>(i * outer + j);
			child->children.push_back(child2);
		}
		a.children.push_back(child);
	}

	lua.new_usertype<A>("A", "value", sol::property(&A::getValue), "children", sol::property(&getChildren));

	lua.globals()["A"] = &a;

	const auto& code = R"(
print(A.value)
for i=1, 10 do
	co = coroutine.create( function()
		print("A")
		for _, child in pairs(A.children) do
			for _, child2 in pairs(child.children) do
				print(child2.value)
			end
		end
		end)
	coroutine.resume(co)
end
    )";

	// call lua code directly
	sol::optional<sol::error> result = lua.safe_script(code);
	REQUIRE_FALSE(result.has_value());
}
