// sol3

// The MIT License (MIT)

// Copyright (c) 2013-2020 Rapptz, ThePhD and contributors

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

#ifndef SOL_PROTECTED_HANDLER_HPP
#define SOL_PROTECTED_HANDLER_HPP

#include <sol/reference.hpp>
#include <sol/stack.hpp>
#include <sol/protected_function_result.hpp>
#include <sol/unsafe_function.hpp>
#include <cstdint>

namespace sol { namespace detail {
	inline const char (&default_handler_name())[9] {
		static const char name[9] = "sol.\xF0\x9F\x94\xA9";
		return name;
	}

	template <bool b, typename target_t = reference>
	struct protected_handler {
		typedef is_stack_based<target_t> is_stack;
		const target_t& target;
		int stack_index;

		protected_handler(std::false_type, const target_t& target_) : target(target_), stack_index(0) {
			if (b) {
				stack_index = lua_gettop(target.lua_state()) + 1;
				target.push();
			}
		}

		protected_handler(std::true_type, const target_t& target_) : target(target_), stack_index(0) {
			if (b) {
				stack_index = target.stack_index();
			}
		}

		protected_handler(const target_t& target_) : protected_handler(is_stack(), target_) {
		}

		bool valid() const noexcept {
			return b;
		}

		~protected_handler() {
			if constexpr (!is_stack::value) {
				if (stack_index != 0) {
					lua_remove(target.lua_state(), stack_index);
				}
			}
		}
	};

	template <typename base_t, typename T>
	basic_function<base_t> force_cast(T& p) {
		return p;
	}

	template <typename Reference, bool is_main_ref = false>
	static Reference get_default_handler(lua_State* L) {
		if (is_stack_based<Reference>::value || L == nullptr)
			return Reference(L, lua_nil);
		L = is_main_ref ? main_thread(L, L) : L;
		lua_getglobal(L, default_handler_name());
		auto pp = stack::pop_n(L, 1);
		return Reference(L, -1);
	}

	template <typename T>
	static void set_default_handler(lua_State* L, const T& ref) {
		if (L == nullptr) {
			return;
		}
		if (!ref.valid()) {
#if SOL_IS_ON(SOL_SAFE_STACK_CHECK_I_)
			luaL_checkstack(L, 1, detail::not_enough_stack_space_generic);
#endif // make sure stack doesn't overflow
			lua_pushnil(L);
			lua_setglobal(L, default_handler_name());
		}
		else {
			ref.push(L);
			lua_setglobal(L, default_handler_name());
		}
	}
}} // namespace sol::detail

#endif // SOL_PROTECTED_HANDLER_HPP
