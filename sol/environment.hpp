// The MIT License (MIT) 

// Copyright (c) 2013-2016 Rapptz, ThePhD and contributors

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

#ifndef SOL_ENVIRONMENT_HPP
#define SOL_ENVIRONMENT_HPP

#include "table.hpp"

namespace sol {

	template <typename base_type>
	struct basic_environment : basic_table<base_type> {
	private:
		typedef basic_table<base_type> table_t;
	public:
		basic_environment() noexcept = default;
		basic_environment(const basic_environment&) = default;
		basic_environment(basic_environment&&) = default;
		basic_environment& operator=(const basic_environment&) = default;
		basic_environment& operator=(basic_environment&&) = default;

		basic_environment(lua_State* L, sol::new_table t, const sol::reference& fallback) : table_t(L, std::move(t)) {
			sol::stack_table mt(L, sol::new_table(0, 1));
			mt.set(sol::meta_function::index, fallback);
			this->set(metatable_key, mt);
			mt.pop();
		}
		template <typename T, typename... Args, meta::enable<
			meta::neg<std::is_same<meta::unqualified_t<T>, basic_environment>>, 
			meta::boolean<!(sizeof...(Args) == 2 && meta::any_same<new_table, meta::unqualified_t<Args>...>::value)>,
			meta::boolean<!(sizeof...(Args) == 0 && std::is_base_of<proxy_base_tag, meta::unqualified_t<T>>::value)>
		> = meta::enabler>
		basic_environment(T&& arg, Args&&... args) : table_t(std::forward<T>(arg), std::forward<Args>(args)...) { }

		template <typename T>
		void set_on(const T& target) const {
			lua_State* L = target.lua_state();
#if SOL_LUA_VERSION < 502
			// Use lua_setfenv
			target.push();
			this->push();
			lua_setfenv(L, -2);
			target.pop();
#else
			// Use upvalues as explained in Lua 5.2 and beyond's manual
			target.push();
			this->push();
			if (lua_setupvalue(L, -2, 1) == nullptr) {
				this->pop();
			}
			target.pop();
#endif
		}
	};

	template <typename T, typename E>
	void set_environment(const basic_environment<E>& env, const T& target) {
		env.set_on(target);
	}

} // sol

#endif // SOL_ENVIRONMENT_HPP
