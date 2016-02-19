// The MIT License (MIT)

// Copyright (c) 2013-2016 Rapptz and contributors

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

#ifndef SOL_FUNCTION_TYPES_OVERLOAD_HPP
#define SOL_FUNCTION_TYPES_OVERLOAD_HPP

#include "overload.hpp"
#include "function_types_core.hpp"
#include "function_types_usertype.hpp"

namespace sol {
namespace detail {
template <std::size_t... M, typename Match>
inline int match_arity(types<>, std::index_sequence<>, std::index_sequence<M...>, std::ptrdiff_t, lua_State*, Match&&, int) {
    throw error("no matching function call takes this number of arguments and the specified types");
}

template <typename Fx, typename... Fxs, std::size_t I, std::size_t... In, std::size_t... M, typename Match>
inline int match_arity(types<Fx, Fxs...>, std::index_sequence<I, In...>, std::index_sequence<M...>, std::ptrdiff_t fxarity, lua_State* L, Match&& matchfx, int start) {
    typedef function_traits<Fx> traits;
    typedef tuple_types<typename function_traits<Fx>::return_type> return_types;
    typedef typename function_traits<Fx>::args_type args_type;
    typedef typename args_type::indices args_indices;
    // compile-time eliminate any functions that we know ahead of time are of improper arity
    if (find_in_pack_v<Index<traits::arity>, Index<M>...>::value || traits::arity != fxarity) {
        return match_arity(types<Fxs...>(), std::index_sequence<In...>(), std::index_sequence<M...>(), fxarity, L, std::forward<Match>(matchfx), start);
    }
    if (traits::arity != fxarity) {
	    return match_arity(types<Fxs...>(), std::index_sequence<In...>(), std::index_sequence<traits::arity, M...>(), fxarity, L, std::forward<Match>(matchfx), start);
    }
    if (!detail::check_types(args_type(), args_indices(), L, start)) {
        return match_arity(types<Fxs...>(), std::index_sequence<In...>(), std::index_sequence<M...>(), fxarity, L, std::forward<Match>(matchfx), start);
    }
    return matchfx(Index<I>(), fxarity, L, start);
}
} // detail

template <typename... Functions, typename Match>
inline int match(std::ptrdiff_t fxarity, lua_State* L, Match&& matchfx, int start = 1) {
	return detail::match_arity(types<Functions...>(), std::index_sequence_for<Functions...>(), std::index_sequence<>(), fxarity, L, std::forward<Match>(matchfx), start);
}

template <typename... Functions, typename Match>
inline int match(lua_State* L, Match&& matchfx, int start = 1) {
     std::ptrdiff_t fxarity = lua_gettop(L) - (start - 1);
     return match<Functions...>(fxarity, L, std::forward<Match>(matchfx), start);
}

template <typename... Functions>
struct overloaded_function : base_function {
    typedef std::tuple<Functions...> overload_list;
    overload_list overloads;

    overloaded_function(overload_set<Functions...> set)
    : overloaded_function(indices(), set) {}

    template <std::size_t... I>
    overloaded_function(std::index_sequence<I...>, overload_set<Functions...> set)
    : overloaded_function(std::get<In>(set)...) {}

    overloaded_function(Functions... fxs)
    : overloads(fxs...) {

    }

    template <std::size_t I>
    int call(Index<I>, int, lua_State* L, int) {
	   auto& func = std::get<I>(overloads);
	   typedef Unqualified<decltype(func)> Fx;
        typedef function_traits<Fx> traits;
        typedef tuple_types<typename function_traits<Fx>::return_type> return_types;
        typedef typename function_traits<Fx>::args_type args_type;
	   return stack::typed_call<false>(return_types(), args_type(), func, L);
    }

    virtual int operator()(lua_State* L) override {
        auto mfx = [&](auto&&... args){ return call(std::forward<decltype(args)>(args)...); };
        return match<Functions...>(L, mfx);
    }
};

template <typename T, typename... Functions>
struct usertype_overloaded_function : base_function {
    typedef std::tuple<detail::functor<T, Functions>...> overload_list;
    typedef std::index_sequence_for<Functions...> indices;
    overload_list overloads;

    usertype_overloaded_function(overload_set<Functions...> set) : usertype_overloaded_function(indices(), set) {}

    template <std::size_t... I>
    usertype_overloaded_function(std::index_sequence<I...>, overload_set<Functions...> set) : usertype_overloaded_function(std::get<I>(set)...) {}

    usertype_overloaded_function(Functions... fxs) : overloads(fxs...) {}

    template <std::size_t I>
    int call(Index<I>, int, lua_State* L, int) {
        auto& func = std::get<I>(overloads);
        typedef Unqualified<decltype(func)> Fx;
        typedef typename Fx::traits_type traits;
        typedef tuple_types<typename Fx::return_type> return_types;
        typedef typename Fx::args_type args_type;
        func.item = ptr(stack::get<T>(L, 1));
        return stack::typed_call<false>(return_types(), args_type(), func, L);
    }

    virtual int operator()(lua_State* L) override {
        auto mfx = [&](auto&&... args){ return call(std::forward<decltype(args)>(args)...); };
        return match<Functions...>(L, mfx, 2);
    }

};
} // sol

#endif // SOL_FUNCTION_TYPES_OVERLOAD_HPP
