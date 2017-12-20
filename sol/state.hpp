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

#ifndef SOL_STATE_HPP
#define SOL_STATE_HPP

#include "state_view.hpp"
#include "thread.hpp"

namespace sol {

	namespace detail {
		inline int default_at_panic(lua_State* L) {
#ifdef SOL_NO_EXCEPTIONS
			(void)L;
			return -1;
#else
			size_t messagesize;
			const char* message = lua_tolstring(L, -1, &messagesize);
			if (message) {
				std::string err(message, messagesize);
				lua_settop(L, 0);
				throw error(err);
			}
			lua_settop(L, 0);
			throw error(std::string("An unexpected error occurred and forced the lua state to call atpanic"));
#endif
		}

		inline int default_traceback_error_handler(lua_State* L) {
			using namespace sol;
			std::string msg = "An unknown error has triggered the default error handler";
			optional<string_view> maybetopmsg = stack::check_get<string_view>(L, 1);
			if (maybetopmsg) {
				const string_view& topmsg = maybetopmsg.value();
				msg.assign(topmsg.data(), topmsg.size());
			}
			luaL_traceback(L, L, msg.c_str(), 1);
			optional<string_view> maybetraceback = stack::check_get<string_view>(L, -1);
			if (maybetraceback) {
				const string_view& traceback = maybetraceback.value();
				msg.assign(traceback.data(), traceback.size());
			}
			return stack::push(L, msg);
		}
	} // namespace detail

	class state : private std::unique_ptr<lua_State, void (*)(lua_State*)>, public state_view {
	private:
		typedef std::unique_ptr<lua_State, void (*)(lua_State*)> unique_base;

	public:
		state(lua_CFunction panic = detail::default_at_panic)
		: unique_base(luaL_newstate(), lua_close), state_view(unique_base::get()) {
			set_panic(panic);
			lua_CFunction f = c_call<decltype(&detail::default_traceback_error_handler), &detail::default_traceback_error_handler>;
			protected_function::set_default_handler(object(lua_state(), in_place, f));
			stack::register_main_thread(unique_base::get());
			stack::luajit_exception_handler(unique_base::get());
		}

		state(lua_CFunction panic, lua_Alloc alfunc, void* alpointer = nullptr)
		: unique_base(lua_newstate(alfunc, alpointer), lua_close), state_view(unique_base::get()) {
			set_panic(panic);
			lua_CFunction f = c_call<decltype(&detail::default_traceback_error_handler), &detail::default_traceback_error_handler>;
			protected_function::set_default_handler(object(lua_state(), in_place, f));
			stack::register_main_thread(unique_base::get());
			stack::luajit_exception_handler(unique_base::get());
		}

		state(const state&) = delete;
		state(state&&) = default;
		state& operator=(const state&) = delete;
		state& operator=(state&& that) {
			state_view::operator=(std::move(that));
			unique_base::operator=(std::move(that));
			return *this;
		}

		using state_view::get;

		~state() {
		}
	};
} // namespace sol

#endif // SOL_STATE_HPP
