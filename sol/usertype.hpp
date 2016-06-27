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

#ifndef SOL_USERTYPE_HPP
#define SOL_USERTYPE_HPP

#include "usertype_metatable.hpp"
#include "stack.hpp"
#include <memory>

namespace sol {
namespace usertype_detail {
struct add_destructor_tag {};
struct check_destructor_tag {};
struct verified_tag {} const verified {};

template <typename T>
struct is_constructor : std::false_type {};

template <typename... Args>
struct is_constructor<constructors<Args...>> : std::true_type {};

template <typename... Args>
struct is_constructor<constructor_wrapper<Args...>> : std::true_type {};

template <>
struct is_constructor<no_construction> : std::true_type {};

template <typename... Args>
using has_constructor = meta::any<is_constructor<meta::unqualified_t<Args>>...>;

template <typename T>
struct is_destructor : std::false_type {};

template <typename Fx>
struct is_destructor<destructor_wrapper<Fx>> : std::true_type {};

template <typename... Args>
using has_destructor = meta::any<is_destructor<meta::unqualified_t<Args>>...>;
} // usertype_detail

template<typename T>
class usertype {
private:
	std::unique_ptr<usertype_detail::registrar> metatableregister;

	template<typename... Args>
	usertype(usertype_detail::verified_tag, Args&&... args) : metatableregister( std::make_unique<usertype_metatable<T, std::make_index_sequence<sizeof...(Args) / 2>, std::decay_t<Args>...>>(std::forward<Args>(args)...) ) {}

	template<typename... Args>
	usertype(usertype_detail::add_destructor_tag, Args&&... args) : usertype(usertype_detail::verified, std::forward<Args>(args)..., "__gc", default_destructor) {}

	template<typename... Args>
	usertype(usertype_detail::check_destructor_tag, Args&&... args) : usertype(meta::condition<meta::all<std::is_destructible<T>, meta::neg<usertype_detail::has_destructor<Args...>>>, usertype_detail::add_destructor_tag, usertype_detail::verified_tag>(), std::forward<Args>(args)...) {}

public:

	template<typename... Args>
	usertype(Args&&... args) : usertype(meta::condition<meta::all<std::is_default_constructible<T>, meta::neg<usertype_detail::has_constructor<Args...>>>, decltype(default_constructor), usertype_detail::check_destructor_tag>(), std::forward<Args>(args)...) {}

	template<typename... Args, typename... CArgs>
	usertype(constructors<CArgs...> constructorlist, Args&&... args) : usertype(usertype_detail::check_destructor_tag(), std::forward<Args>(args)..., "new", constructorlist) {}

	template<typename... Args, typename... Fxs>
	usertype(constructor_wrapper<Fxs...> constructorlist, Args&&... args) : usertype(usertype_detail::check_destructor_tag(), std::forward<Args>(args)..., "new", constructorlist) {}

	int push(lua_State* L) {
		return metatableregister->push_um(L);
	}
};

namespace stack {
template<typename T>
struct pusher<usertype<T>> {
    static int push(lua_State* L, usertype<T>& user) {
        return user.push(L);
    }
};
} // stack
} // sol

#endif // SOL_USERTYPE_HPP
