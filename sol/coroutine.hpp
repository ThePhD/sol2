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

#ifndef SOL_COROUTINE_HPP
#define SOL_COROUTINE_HPP

#include "reference.hpp"
#include "stack.hpp"
#include "function_result.hpp"
#include "thread.hpp"

namespace sol {
	template <typename base_t>
	class basic_coroutine : public base_t {
	private:
		call_status stats = call_status::yielded;

		void luacall(std::ptrdiff_t argcount, std::ptrdiff_t) {
			stats = static_cast<call_status>(lua_resume(lua_state(), nullptr, static_cast<int>(argcount)));
		}

		template <std::size_t... I, typename... Ret>
		auto invoke(types<Ret...>, std::index_sequence<I...>, std::ptrdiff_t n) {
			luacall(n, sizeof...(Ret));
			return stack::pop<std::tuple<Ret...>>(lua_state());
		}

		template <std::size_t I, typename Ret>
		Ret invoke(types<Ret>, std::index_sequence<I>, std::ptrdiff_t n) {
			luacall(n, 1);
			return stack::pop<Ret>(lua_state());
		}

		template <std::size_t I>
		void invoke(types<void>, std::index_sequence<I>, std::ptrdiff_t n) {
			luacall(n, 0);
		}

		protected_function_result invoke(types<>, std::index_sequence<>, std::ptrdiff_t n) {
			int firstreturn = 1;
			luacall(n, LUA_MULTRET);
			int poststacksize = lua_gettop(lua_state());
			int returncount = poststacksize - (firstreturn - 1);
			if (error()) {
				return protected_function_result(lua_state(), lua_absindex(lua_state(), -1), 1, returncount, status());
			}
			return protected_function_result(lua_state(), firstreturn, returncount, returncount, status());
		}

	public:
		using base_t::lua_state;
		
		basic_coroutine() noexcept = default;
		basic_coroutine(const basic_coroutine&) noexcept = default;
		basic_coroutine(basic_coroutine&&) noexcept = default;
		basic_coroutine& operator=(const basic_coroutine&) noexcept = default;
		basic_coroutine& operator=(basic_coroutine&&) noexcept = default;
		template <typename T, meta::enable<meta::neg<std::is_same<meta::unqualified_t<T>, basic_coroutine>>, is_lua_reference<meta::unqualified_t<T>>> = meta::enabler>
		basic_coroutine(T&& r)
		: base_t(std::forward<T>(r)) {
		}
		basic_coroutine(lua_nil_t r)
		: base_t(r) {
		}
		basic_coroutine(const stack_reference& r) noexcept
		: basic_coroutine(r.lua_state(), r.stack_index()) {
		}
		basic_coroutine(stack_reference&& r) noexcept
		: basic_coroutine(r.lua_state(), r.stack_index()) {
		}
		template <typename T, meta::enable<is_lua_reference<meta::unqualified_t<T>>> = meta::enabler>
		basic_coroutine(lua_State* L, T&& r)
		: base_t(L, std::forward<T>(r)) {
#ifdef SOL_SAFE_REFERENCES
			auto pp = stack::push_pop(*this);
			constructor_handler handler{};
			stack::check<basic_coroutine>(lua_state(), -1, handler);
#endif // Safety
		}
		basic_coroutine(lua_State* L, int index = -1)
		: base_t(L, index) {
#ifdef SOL_SAFE_REFERENCES
			constructor_handler handler{};
			stack::check<basic_coroutine>(lua_state(), index, handler);
#endif // Safety
		}
		basic_coroutine(lua_State* L, ref_index index)
		: base_t(L, index) {
#ifdef SOL_SAFE_REFERENCES
			auto pp = stack::push_pop(*this);
			constructor_handler handler{};
			stack::check<basic_coroutine>(lua_state(), -1, handler);
#endif // Safety
		}

		call_status status() const noexcept {
			return stats;
		}

		bool error() const noexcept {
			call_status cs = status();
			return cs != call_status::ok && cs != call_status::yielded;
		}

		bool runnable() const noexcept {
			return base_t::valid()
				&& (status() == call_status::yielded);
		}

		explicit operator bool() const noexcept {
			return runnable();
		}

		template <typename... Args>
		protected_function_result operator()(Args&&... args) {
			return call<>(std::forward<Args>(args)...);
		}

		template <typename... Ret, typename... Args>
		decltype(auto) operator()(types<Ret...>, Args&&... args) {
			return call<Ret...>(std::forward<Args>(args)...);
		}

		template <typename... Ret, typename... Args>
		decltype(auto) call(Args&&... args) {
			// some users screw up coroutine.create
			// and try to use it with sol::coroutine without ever calling the first resume in Lua
			// this makes the stack incompatible with other kinds of stacks: protect against this
			// make sure coroutines don't screw us over
			base_t::push();
			int pushcount = stack::multi_push_reference(lua_state(), std::forward<Args>(args)...);
			return invoke(types<Ret...>(), std::make_index_sequence<sizeof...(Ret)>(), pushcount);
		}
	};
} // namespace sol

#endif // SOL_COUROUTINE_HPP
