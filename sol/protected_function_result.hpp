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

#ifndef SOL_PROTECTED_FUNCTION_RESULT_HPP
#define SOL_PROTECTED_FUNCTION_RESULT_HPP

#include "reference.hpp"
#include "tuple.hpp"
#include "stack.hpp"
#include "proxy_base.hpp"
#include <cstdint>

namespace sol {
	struct protected_function_result : public proxy_base<protected_function_result> {
	private:
		lua_State* L;
		int index;
		int returncount;
		int popcount;
		call_status err;

		template <typename T>
		decltype(auto) tagged_get(types<optional<T>>) const {
			if (!valid()) {
				return optional<T>(nullopt);
			}
			return stack::get<optional<T>>(L, index);
		}

		template <typename T>
		decltype(auto) tagged_get(types<T>) const {
#ifdef SOL_CHECK_ARGUMENTS
			if (!valid()) {
				type_panic_c_str(L, index, type_of(L, index), type::none);
			}
#endif // Check Argument Safety
			return stack::get<T>(L, index);
		}

		optional<error> tagged_get(types<optional<error>>) const {
			if (valid()) {
				return nullopt;
			}
			return error(detail::direct_error, stack::get<std::string>(L, index));
		}

		error tagged_get(types<error>) const {
#ifdef SOL_CHECK_ARGUMENTS
			if (valid()) {
				type_panic_c_str(L, index, type_of(L, index), type::none);
			}
#endif // Check Argument Safety
			return error(detail::direct_error, stack::get<std::string>(L, index));
		}

	public:
		protected_function_result() = default;
		protected_function_result(lua_State* Ls, int idx = -1, int retnum = 0, int popped = 0, call_status pferr = call_status::ok) noexcept
		: L(Ls), index(idx), returncount(retnum), popcount(popped), err(pferr) {
		}
		protected_function_result(const protected_function_result&) = default;
		protected_function_result& operator=(const protected_function_result&) = default;
		protected_function_result(protected_function_result&& o) noexcept
		: L(o.L), index(o.index), returncount(o.returncount), popcount(o.popcount), err(o.err) {
			// Must be manual, otherwise destructor will screw us
			// return count being 0 is enough to keep things clean
			// but we will be thorough
			o.abandon();
		}
		protected_function_result& operator=(protected_function_result&& o) noexcept {
			L = o.L;
			index = o.index;
			returncount = o.returncount;
			popcount = o.popcount;
			err = o.err;
			// Must be manual, otherwise destructor will screw us
			// return count being 0 is enough to keep things clean
			// but we will be thorough
			o.abandon();
			return *this;
		}

		protected_function_result(const function_result& o) = delete;
		protected_function_result& operator=(const function_result& o) = delete;
		protected_function_result(function_result&& o) noexcept;
		protected_function_result& operator=(function_result&& o) noexcept;

		call_status status() const noexcept {
			return err;
		}

		bool valid() const noexcept {
			return status() == call_status::ok || status() == call_status::yielded;
		}

		template <typename T>
		decltype(auto) get() const {
			return tagged_get(types<meta::unqualified_t<T>>());
		}

		lua_State* lua_state() const noexcept {
			return L;
		};
		int stack_index() const noexcept {
			return index;
		};
		int return_count() const noexcept {
			return returncount;
		};
		int pop_count() const noexcept {
			return popcount;
		};
		void abandon() noexcept {
			//L = nullptr;
			index = 0;
			returncount = 0;
			popcount = 0;
			err = call_status::runtime;
		}
		~protected_function_result() {
			stack::remove(L, index, popcount);
		}
	};

	namespace stack {
		template <>
		struct pusher<protected_function_result> {
			static int push(lua_State* L, const protected_function_result& pfr) {
				int p = 0;
				for (int i = 0; i < pfr.pop_count(); ++i) {
					lua_pushvalue(L, i + pfr.stack_index());
					++p;
				}
				return p;
			}
		};
	} // namespace stack
} // namespace sol

#endif // SOL_PROTECTED_FUNCTION_RESULT_HPP
