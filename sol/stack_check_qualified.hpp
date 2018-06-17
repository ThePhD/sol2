// sol2 

// The MIT License (MIT)

// Copyright (c) 2013-2018 Rapptz, ThePhD and contributors

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

#ifndef SOL_STACK_CHECK_QUALIFIED_HPP
#define SOL_STACK_CHECK_QUALIFIED_HPP

#include "stack_check_unqualified.hpp"

namespace sol {
namespace stack {

	template <typename X>
	struct qualified_checker<X, type::userdata, std::enable_if_t<
		is_unique_usertype<X>::value
		&& !std::is_reference<X>::value
	>> {
		typedef unique_usertype_traits<meta::unqualified_t<X>> u_traits;
		typedef typename u_traits::type T;
		typedef typename u_traits::template rebind_base<void> rebind_t;
		
		template <typename Handler>
		static bool check(std::false_type, lua_State* L, int index, Handler&& handler, record& tracking) {
			return stack::unqualified_check<X>(L, index, std::forward<Handler>(handler), tracking);
		}

		template <typename Handler>
		static bool check(std::true_type, lua_State* L, int index, Handler&& handler, record& tracking) {
			// we have a unique pointer type that can be 
			// rebound to a base/derived type
			const type indextype = type_of(L, index);
			tracking.use(1);
			if (indextype != type::userdata) {
				handler(L, index, type::userdata, indextype, "value is not a userdata");
				return false;
			}
			void* memory = lua_touserdata(L, index);
			memory = detail::align_usertype_unique_destructor(memory);
			detail::unique_destructor& pdx = *static_cast<detail::unique_destructor*>(memory);
			if (&detail::usertype_unique_alloc_destroy<T, X> == pdx) {
				return true;
			}
			if (derive<T>::value) {
				memory = detail::align_usertype_unique_tag<true, false>(memory);
				detail::unique_tag& ic = *reinterpret_cast<detail::unique_tag*>(memory);
				string_view ti = usertype_traits<T>::qualified_name();
				string_view rebind_ti = usertype_traits<rebind_t>::qualified_name();
				if (ic(nullptr, nullptr, ti, rebind_ti) != 0) {
					return true;
				}
			}
			handler(L, index, type::userdata, indextype, "value is a userdata but is not the correct unique usertype");
			return false;
		}
		
		template <typename Handler>
		static bool check(lua_State* L, int index, Handler&& handler, record& tracking) {
			return check(meta::neg<std::is_void<rebind_t>>(), L, index, std::forward<Handler>(handler), tracking);
		}
	};

	template <typename X>
	struct qualified_checker<X, type::userdata, std::enable_if_t<is_container<meta::unqualified_t<X>>::value && !std::is_reference<X>::value>> {
		template <typename Handler>
		static bool check(lua_State* L, int index, Handler&& handler, record& tracking) {
			if (type_of(L, index) == type::userdata) {
				return stack::unqualified_check<X>(L, index, std::forward<Handler>(handler), tracking);
			}
			else {
				return stack::unqualified_check<nested<X>>(L, index, std::forward<Handler>(handler), tracking);
			}
		}
	};
}
} // namespace sol::stack

#endif // SOL_STACK_CHECK_HPP
