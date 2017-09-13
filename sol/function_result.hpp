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

#ifndef SOL_FUNCTION_RESULT_HPP
#define SOL_FUNCTION_RESULT_HPP

#include "reference.hpp"
#include "tuple.hpp"
#include "stack.hpp"
#include "proxy_base.hpp"
#include <cstdint>

namespace sol {
	struct function_result : public proxy_base<function_result> {
	private:
		lua_State* L;
		int index;
		int returncount;

	public:
		function_result() = default;
		function_result(lua_State* Ls, int idx = -1, int retnum = 0)
		: L(Ls), index(idx), returncount(retnum) {
		}
		function_result(const function_result&) = default;
		function_result& operator=(const function_result&) = default;
		function_result(function_result&& o)
		: L(o.L), index(o.index), returncount(o.returncount) {
			// Must be manual, otherwise destructor will screw us
			// return count being 0 is enough to keep things clean
			// but will be thorough
			o.abandon();
		}
		function_result& operator=(function_result&& o) {
			L = o.L;
			index = o.index;
			returncount = o.returncount;
			// Must be manual, otherwise destructor will screw us
			// return count being 0 is enough to keep things clean
			// but will be thorough
			o.abandon();
			return *this;
		}

		function_result(const protected_function_result& o) = delete;
		function_result& operator=(const protected_function_result& o) = delete;
		function_result(protected_function_result&& o) noexcept;
		function_result& operator=(protected_function_result&& o) noexcept;

		template <typename T>
		decltype(auto) get() const {
			return stack::get<T>(L, index);
		}

		call_status status() const noexcept {
			return call_status::ok;
		}

		bool valid() const noexcept {
			return status() == call_status::ok || status() == call_status::yielded;
		}

		lua_State* lua_state() const {
			return L;
		};
		int stack_index() const {
			return index;
		};
		int return_count() const {
			return returncount;
		};
		void abandon() noexcept {
			//L = nullptr;
			index = 0;
			returncount = 0;
		}
		~function_result() {
			lua_pop(L, returncount);
		}
	};

	namespace stack {
		template <>
		struct pusher<function_result> {
			static int push(lua_State* L, const function_result& fr) {
				int p = 0;
				for (int i = 0; i < fr.return_count(); ++i) {
					lua_pushvalue(L, i + fr.stack_index());
					++p;
				}
				return p;
			}
		};
	} // namespace stack
} // namespace sol

#endif // SOL_FUNCTION_RESULT_HPP
