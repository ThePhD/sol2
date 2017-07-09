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

#ifndef SOL_FUNCTION_HPP
#define SOL_FUNCTION_HPP

#include "unsafe_function.hpp"
#include "protected_function.hpp"
#include "stack.hpp"
#include <functional>

namespace sol {

	namespace stack {
		template<typename Signature>
		struct getter<std::function<Signature>> {
			typedef meta::bind_traits<Signature> fx_t;
			typedef typename fx_t::args_list args_lists;
			typedef meta::tuple_types<typename fx_t::return_type> return_types;

			template<typename... Args, typename... Ret>
			static std::function<Signature> get_std_func(types<Ret...>, types<Args...>, lua_State* L, int index) {
				sol::function f(L, index);
				auto fx = [f, L, index](Args&&... args) -> meta::return_type_t<Ret...> {
					return f.call<Ret...>(std::forward<Args>(args)...);
				};
				return std::move(fx);
			}

			template<typename... FxArgs>
			static std::function<Signature> get_std_func(types<void>, types<FxArgs...>, lua_State* L, int index) {
				sol::function f(L, index);
				auto fx = [f, L, index](FxArgs&&... args) -> void {
					f(std::forward<FxArgs>(args)...);
				};
				return std::move(fx);
			}

			template<typename... FxArgs>
			static std::function<Signature> get_std_func(types<>, types<FxArgs...> t, lua_State* L, int index) {
				return get_std_func(types<void>(), t, L, index);
			}

			static std::function<Signature> get(lua_State* L, int index, record& tracking) {
				tracking.last = 1;
				tracking.used += 1;
				type t = type_of(L, index);
				if (t == type::none || t == type::lua_nil) {
					return nullptr;
				}
				return get_std_func(return_types(), args_lists(), L, index);
			}
		};
	} // stack

} // sol

#endif // SOL_FUNCTION_HPP
