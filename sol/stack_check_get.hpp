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

#ifndef SOL_STACK_CHECK_GET_HPP
#define SOL_STACK_CHECK_GET_HPP

#include "stack_core.hpp"
#include "stack_get.hpp"
#include "stack_check.hpp"
#include "optional.hpp"
#include <cstdlib>
#include <cmath>

namespace sol {
namespace stack {
	template <typename T, typename>
	struct check_getter {
		typedef decltype(stack_detail::unchecked_get<T>(nullptr, 0, std::declval<record&>())) R;

		template <typename Handler>
		static optional<R> get(lua_State* L, int index, Handler&& handler, record& tracking) {
			if (!check<T>(L, index, std::forward<Handler>(handler))) {
				tracking.use(static_cast<int>(!lua_isnone(L, index)));
				return nullopt;
			}
			return stack_detail::unchecked_get<T>(L, index, tracking);
		}
	};

	template <typename T>
	struct check_getter<optional<T>> {
		template <typename Handler>
		static decltype(auto) get(lua_State* L, int index, Handler&&, record& tracking) {
			return check_get<T>(L, index, no_panic, tracking);
		}
	};

	template <typename T>
	struct check_getter<T, std::enable_if_t<std::is_integral<T>::value && lua_type_of<T>::value == type::number>> {
		template <typename Handler>
		static optional<T> get(lua_State* L, int index, Handler&& handler, record& tracking) {
#if SOL_LUA_VERSION >= 503
			if (lua_isinteger(L, index) != 0) {
				tracking.use(1);
				return static_cast<T>(lua_tointeger(L, index));
			}
#endif
			int isnum = 0;
			const lua_Number value = lua_tonumberx(L, index, &isnum);
			if (isnum != 0) {
#if defined(SOL_CHECK_ARGUMENTS) && !defined(SOL_NO_CHECK_NUMBER_PRECISION)
				const auto integer_value = llround(value);
				if (static_cast<lua_Number>(integer_value) == value) {
					tracking.use(1);
					return static_cast<T>(integer_value);
				}
#else
				tracking.use(1);
				return static_cast<T>(value);
#endif
			}
			const type t = type_of(L, index);
			tracking.use(static_cast<int>(t != type::none));
			handler(L, index, type::number, t, "not an integer");
			return nullopt;
		}
	};

	template <typename T>
	struct check_getter<T, std::enable_if_t<std::is_enum<T>::value && !meta::any_same<T, meta_function, type>::value>> {
		template <typename Handler>
		static optional<T> get(lua_State* L, int index, Handler&& handler, record& tracking) {
			int isnum = 0;
			lua_Integer value = lua_tointegerx(L, index, &isnum);
			if (isnum == 0) {
				type t = type_of(L, index);
				tracking.use(static_cast<int>(t != type::none));
				handler(L, index, type::number, t, "not a valid enumeration value");
				return nullopt;
			}
			tracking.use(1);
			return static_cast<T>(value);
		}
	};

	template <typename T>
	struct check_getter<T, std::enable_if_t<std::is_floating_point<T>::value>> {
		template <typename Handler>
		static optional<T> get(lua_State* L, int index, Handler&& handler, record& tracking) {
			int isnum = 0;
			lua_Number value = lua_tonumberx(L, index, &isnum);
			if (isnum == 0) {
				type t = type_of(L, index);
				tracking.use(static_cast<int>(t != type::none));
				handler(L, index, type::number, t, "not a valid floating point number");
				return nullopt;
			}
			tracking.use(1);
			return static_cast<T>(value);
		}
	};

	template <typename T>
	struct getter<optional<T>> {
		static decltype(auto) get(lua_State* L, int index, record& tracking) {
			return check_get<T>(L, index, no_panic, tracking);
		}
	};

#ifdef SOL_CXX17_FEATURES
	template <typename... Tn>
	struct check_getter<std::variant<Tn...>> {
		typedef std::variant<Tn...> V;
		typedef std::variant_size<V> V_size;
		typedef std::integral_constant<bool, V_size::value == 0> V_is_empty;

		template <typename Handler>
		static optional<V> get_empty(std::true_type, lua_State* L, int index, Handler&& handler, record& tracking) {
			return nullopt;
		}

		template <typename Handler>
		static optional<V> get_empty(std::false_type, lua_State* L, int index, Handler&& handler, record& tracking) {
			typedef std::variant_alternative_t<0, V> T;
			// This should never be reached...
			// please check your code and understand what you did to bring yourself here
			handler(L, index, type::poly, type_of(L, index), "this variant code should never be reached: if it has, you have done something so terribly wrong");
			return nullopt;
		}

		template <typename Handler>
		static optional<V> get_one(std::integral_constant<std::size_t, 0>, lua_State* L, int index, Handler&& handler, record& tracking) {
			return get_empty(V_is_empty(), L, index, std::forward<Handler>(handler), tracking);
		}

		template <std::size_t I, typename Handler>
		static optional<V> get_one(std::integral_constant<std::size_t, I>, lua_State* L, int index, Handler&& handler, record& tracking) {
			typedef std::variant_alternative_t<I - 1, V> T;
			if (stack::check<T>(L, index, no_panic, tracking)) {
				return V(std::in_place_index<I - 1>, stack::get<T>(L, index));
			}
			return get_one(std::integral_constant<std::size_t, I - 1>(), L, index, std::forward<Handler>(handler), tracking);
		}

		template <typename Handler>
		static optional<V> get(lua_State* L, int index, Handler&& handler, record& tracking) {
			return get_one(std::integral_constant<std::size_t, V_size::value>(), L, index, std::forward<Handler>(handler), tracking);
		}
	};
#endif // C++17
}
} // namespace sol::stack

#endif // SOL_STACK_CHECK_GET_HPP
