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

#ifndef SOL_ERROR_HANDLER_HPP
#define SOL_ERROR_HANDLER_HPP

#include "types.hpp"
#include "demangle.hpp"

namespace sol {

	namespace detail {
		constexpr const char* not_a_number = "not a numeric type";
		constexpr const char* not_a_number_or_number_string = "not a numeric type or numeric string";
		constexpr const char* not_a_number_integral = "not a numeric type that fits exactly an integer (has significant decimals)";
		constexpr const char* not_a_number_or_number_string_integral
		     = "not a numeric type or a numeric string that fits exactly an integer (has significant decimals)";

		constexpr const char* not_enough_stack_space = "not enough space left on Lua stack";
		constexpr const char* not_enough_stack_space_floating = "not enough space left on Lua stack for a floating point number";
		constexpr const char* not_enough_stack_space_integral = "not enough space left on Lua stack for an integral number";
		constexpr const char* not_enough_stack_space_string = "not enough space left on Lua stack for a string";
		constexpr const char* not_enough_stack_space_meta_function_name = "not enough space left on Lua stack for the name of a meta_function";
		constexpr const char* not_enough_stack_space_userdata = "not enough space left on Lua stack to create a sol3 userdata";
		constexpr const char* not_enough_stack_space_generic = "not enough space left on Lua stack to push valuees";
		constexpr const char* not_enough_stack_space_environment = "not enough space left on Lua stack to retrieve environment";
		constexpr const char* protected_function_error = "caught (...) unknown error during protected_function call";

		inline void accumulate_and_mark(const std::string& n, std::string& addendum, int& marker) {
			if (marker > 0) {
				addendum += ", ";
			}
			addendum += n;
			++marker;
		}
	}

	inline std::string associated_type_name(lua_State* L, int index, type t) {
		switch (t) {
		case type::poly:
			return "anything";
		case type::userdata:
		{
#if defined(SOL_SAFE_STACK_CHECK) && SOL_SAFE_STACK_CHECK
			luaL_checkstack(L, 2, "not enough space to push get the type name");
#endif // make sure stack doesn't overflow
			if (lua_getmetatable(L, index) == 0) {
				break;
			}
			lua_pushlstring(L, "__name", 6);
			lua_rawget(L, -2);
			size_t sz;
			const char* name = lua_tolstring(L, -1, &sz);
			std::string tn(name, static_cast<std::string::size_type>(sz));
			lua_pop(L, 2);
			return tn;
		}
		default:
			break;
		}
		return lua_typename(L, static_cast<int>(t));
	}

	inline int type_panic_string(lua_State* L, int index, type expected, type actual, const std::string& message = "") noexcept(false) {
		const char* err = message.empty() ? "stack index %d, expected %s, received %s" : "stack index %d, expected %s, received %s: %s";
		std::string actualname = associated_type_name(L, index, actual);
		return luaL_error(L, err, index,
			expected == type::poly ? "anything" : lua_typename(L, static_cast<int>(expected)),
			actualname.c_str(),
			message.c_str());
	}

	inline int type_panic_c_str(lua_State* L, int index, type expected, type actual, const char* message = nullptr) noexcept(false) {
		const char* err = message == nullptr || (std::char_traits<char>::length(message) == 0) ? "stack index %d, expected %s, received %s" : "stack index %d, expected %s, received %s: %s";
		std::string actualname = associated_type_name(L, index, actual);
		return luaL_error(L, err, index,
			expected == type::poly ? "anything" : lua_typename(L, static_cast<int>(expected)),
			actualname.c_str(),
			message);
	}

	struct type_panic_t {
		int operator()(lua_State* L, int index, type expected, type actual) const noexcept(false) {
			return type_panic_c_str(L, index, expected, actual, nullptr);
		}
		int operator()(lua_State* L, int index, type expected, type actual, const char* message) const noexcept(false) {
			return type_panic_c_str(L, index, expected, actual, message);
		}
		int operator()(lua_State* L, int index, type expected, type actual, const std::string& message) const noexcept(false) {
			return type_panic_string(L, index, expected, actual, message);
		}
	};

	const type_panic_t type_panic = {};

	struct constructor_handler {
		int operator()(lua_State* L, int index, type expected, type actual, const std::string& message) const noexcept(false) {
			std::string str = "(type check failed in constructor)";
			return type_panic_string(L, index, expected, actual, message.empty() ? str : message + " " + str);
		}
	};

	template <typename F = void>
	struct argument_handler {
		int operator()(lua_State* L, int index, type expected, type actual, const std::string& message) const noexcept(false) {
			std::string str = "(bad argument to variable or function call)";
			return type_panic_string(L, index, expected, actual, message.empty() ? str : message + " " + str );
		}
	};

	template <typename R, typename... Args>
	struct argument_handler<types<R, Args...>> {
		int operator()(lua_State* L, int index, type expected, type actual, const std::string& message) const noexcept(false) {
			std::string addendum = "(bad argument into '";
			addendum += detail::demangle<R>();
			addendum += "(";
			int marker = 0;
			(void)detail::swallow{int(), (detail::accumulate_and_mark(detail::demangle<Args>(), addendum, marker), int())...};
			addendum += ")')";
			return type_panic_string(L, index, expected, actual, message.empty() ? addendum : message + " " + addendum);
		}
	};

	// Specify this function as the handler for lua::check if you know there's nothing wrong
	inline int no_panic(lua_State*, int, type, type, const char* = nullptr) noexcept {
		return 0;
	}

	inline void type_error(lua_State* L, int expected, int actual) noexcept(false) {
		luaL_error(L, "expected %s, received %s", lua_typename(L, expected), lua_typename(L, actual));
	}

	inline void type_error(lua_State* L, type expected, type actual) noexcept(false) {
		type_error(L, static_cast<int>(expected), static_cast<int>(actual));
	}

	inline void type_assert(lua_State* L, int index, type expected, type actual) noexcept(false) {
		if (expected != type::poly && expected != actual) {
			type_panic_c_str(L, index, expected, actual, nullptr);
		}
	}

	inline void type_assert(lua_State* L, int index, type expected) {
		type actual = type_of(L, index);
		type_assert(L, index, expected, actual);
	}

} // namespace sol

#endif // SOL_ERROR_HANDLER_HPP
