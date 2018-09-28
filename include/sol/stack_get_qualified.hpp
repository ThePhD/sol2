// sol3

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

#ifndef SOL_STACK_QUALIFIED_GET_HPP
#define SOL_STACK_QUALIFIED_GET_HPP

#include "stack_get_unqualified.hpp"

namespace sol {
namespace stack {

	template <typename X>
	struct qualified_getter<X, std::enable_if_t<
		!std::is_reference<X>::value 
		&& is_unique_usertype<meta::unqualified_t<X>>::value
		&& !std::is_void<typename unique_usertype_traits<meta::unqualified_t<X>>::template rebind_base<void>>::value
	>> {
		typedef unique_usertype_traits<meta::unqualified_t<X>> u_traits;
		typedef typename u_traits::type T;
		typedef typename u_traits::actual_type Real;
		typedef typename u_traits::template rebind_base<void> rebind_t;

		static Real get(lua_State* L, int index, record& tracking) {
			tracking.use(1);
			void* memory = lua_touserdata(L, index);
			memory = detail::align_usertype_unique_destructor(memory);
			detail::unique_destructor& pdx = *static_cast<detail::unique_destructor*>(memory);
			if (&detail::usertype_unique_alloc_destroy<T, X> == pdx) {
				memory = detail::align_usertype_unique_tag<true, false>(memory);
				memory = detail::align_usertype_unique<Real, true, false>(memory);
				Real* mem = static_cast<Real*>(memory);
				return *mem;
			}
			Real r(nullptr);
			if (!derive<T>::value) {
				// TODO: abort / terminate, maybe only in debug modes?
				return r;
			}
			memory = detail::align_usertype_unique_tag<true, false>(memory);
			detail::unique_tag& ic = *reinterpret_cast<detail::unique_tag*>(memory);
			memory = detail::align_usertype_unique<Real, true, false>(memory);
			string_view ti = usertype_traits<T>::qualified_name();
			string_view rebind_ti = usertype_traits<rebind_t>::qualified_name();
			int cast_operation = ic(memory, &r, ti, rebind_ti);
			switch (cast_operation) {
			case 1: {
				// it's a perfect match,
				// alias memory directly
				Real* mem = static_cast<Real*>(memory);
				return *mem;
			}
			case 2:
				// it's a base match, return the
				// aliased creation
				return std::move(r);
			default:
				// uh oh..
				break;
			}
			// TODO: abort / terminate, maybe only in debug modes?
			return r;
		}
	};

	template <typename T>
	struct qualified_getter<T, std::enable_if_t<
		!std::is_reference<T>::value 
		&& is_container<meta::unqualified_t<T>>::value 
		&& !is_lua_primitive<T>::value 
		&& !is_transparent_argument<T>::value 
	>> {
		static T get(lua_State* L, int index, record& tracking) {
			if (type_of(L, index) == type::userdata) {
				return stack_detail::unchecked_unqualified_get<T>(L, index, tracking);
			}
			else {
				return stack_detail::unchecked_unqualified_get<sol::nested<T>>(L, index, tracking);
			}
		}
	};
}
} // namespace sol::stack

#endif // SOL_STACK_QUALIFIED_GET_HPP

