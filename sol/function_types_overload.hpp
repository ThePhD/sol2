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
template <typename... Functions>
struct overloaded_function : base_function {
    typedef std::tuple<std::pair<int, Functions>...> overloads_t;
    overloads_t overloads;

    overloaded_function(Functions... fxs)
        : overloads({ function_traits<Unqualified<Functions>>::arity, fxs }...) {

    }

    int match_arity(lua_State* L, std::ptrdiff_t x, indices<>) {
        throw error("no matching function call takes this number of arguments");
    }

    template <std::size_t I, std::size_t... In>
    int match_arity(lua_State* L, std::ptrdiff_t x, indices<I, In...>) {
        // TODO:
        // when we get proper constexpr, search functions only within the specific
        // arity range, instead of all of them by using
        // std::tuple<
        //    std::pair<1 - arity, std::tuple<func_arity_of_1_a, func_arity_of_1_b>>,
        //    std::pair<3 - arity, std::tuple<func_arity_of_3>>,
        //    std::pair<n - arity, std::tuple<func_arity_of_n, ...>>,
        //    ...
        //>
        auto& package = std::get<I>(overloads);
        auto arity = package.first;
        if (arity != x) {
            return match_arity(L, x, indices<In...>());
        }
        auto& func = package.second;
        typedef Unqualified<decltype(func)> fx_t;
        typedef tuple_types<typename function_traits<fx_t>::return_type> return_type;
        typedef typename function_traits<fx_t>::args_type args_type;
        if (!detail::check_types(args_type(), args_type(), L)) {
            return match_arity(L, x, indices<In...>());
        }
        return stack::typed_call(return_type(), args_type(), func, L);
    }

    int match_arity(lua_State* L) {
        std::ptrdiff_t x = lua_gettop(L);
        return match_arity(L, x, build_indices<std::tuple_size<overloads_t>::value>());
    }

    virtual int operator()(lua_State* L) override {
        return match_arity(L);
    }

    virtual int operator()(lua_State* L, detail::ref_call_t) override {
        return match_arity(L);
    }
};

template <typename T, typename... Functions>
struct usertype_overloaded_function : base_function {
    typedef std::tuple<std::pair<int, detail::functor<T, Functions>>...> overloads_t;
    overloads_t overloads;

    usertype_overloaded_function(overload_set<Functions...> set) 
    : usertype_overloaded_function(build_indices<sizeof...(Functions)>(), set) {}

    template<std::size_t... In>
    usertype_overloaded_function(indices<In...>, overload_set<Functions...> set)
    : usertype_overloaded_function(std::get<In>(set)...) {}


    usertype_overloaded_function(Functions... fxs)
    : overloads({function_traits<Functions>::arity, fxs}...) {

    }

    int match_arity(lua_State* L, std::ptrdiff_t x, indices<>) {
        throw error("no matching function call takes this number of arguments");
    }

    template <std::size_t I, std::size_t... In>
    int match_arity(lua_State* L, std::ptrdiff_t x, indices<I, In...>) {
        // TODO:
        // propogate changes from above down here too when they get figured out
        auto& package = std::get<I>(overloads);
        auto arity = package.first;
        if (arity != x) {
            return match_arity(L, x, indices<In...>());
        }
        auto& func = package.second;
        typedef Unqualified<decltype(func)> fx_t;
        typedef tuple_types<typename fx_t::return_type> return_type;
        typedef typename fx_t::args_type args_type;
        if (!detail::check_types(args_type(), args_type(), L, 2)) {
            return match_arity(L, x, indices<In...>());
        }
        func.item = ptr(stack::get<T>(L, 1));
        return stack::typed_call(return_type(), args_type(), func, L);
    }

    int match_arity(lua_State* L) {
        std::ptrdiff_t x = lua_gettop(L) - 1;
        return match_arity(L, x, build_indices<std::tuple_size<overloads_t>::value>());
    }

    virtual int operator()(lua_State* L) override {
        return match_arity(L);
    }

    virtual int operator()(lua_State* L, detail::ref_call_t) override {
        return match_arity(L);
    }
};

template<typename... Functions, typename T>
struct usertype_indexing_function<overload_set<Functions...>, T> : base_function {
    typedef std::tuple<std::pair<int, detail::functor<T, Functions>>...> overloads_t;
    overloads_t overloads;
    std::string name;
    std::unordered_map<std::string, std::pair<std::unique_ptr<base_function>, bool>> functions;
    
    usertype_indexing_function(std::string name, overload_set<Functions...> set) 
    : usertype_indexing_function(build_indices<sizeof...(Functions)>(), std::move(name), set) {}

    template <std::size_t... In>
    usertype_indexing_function(indices<In...>, std::string name, overload_set<Functions...> set)
    : usertype_indexing_function(std::move(name), std::get<In>(set)...) {}

    usertype_indexing_function(std::string name, Functions... fxs)
    : overloads({function_traits<Functions>::arity, fxs}...), name(std::move(name)) {}

    int match_arity(lua_State* L, std::ptrdiff_t x, indices<>) {
        throw error("no matching function call takes this number of arguments");
    }

    template <std::size_t I, std::size_t... In>
    int match_arity(lua_State* L, std::ptrdiff_t x, indices<I, In...>) {
        // TODO:
        // propogate changes from above down here too when they get figured out
        auto& package = std::get<I>(overloads);
        auto arity = package.first;
        if (arity != x) {
            return match_arity(L, x, indices<In...>());
        }
        auto& func = package.second;
        typedef Unqualified<decltype(func)> fx_t;
        typedef tuple_types<typename fx_t::return_type> return_type;
        typedef typename fx_t::args_type args_type;
        if (!detail::check_types(args_type(), args_type(), L, 2)) {
            return match_arity(L, x, indices<In...>());
        }
        func.item = ptr(stack::get<T>(L, 1));
        return stack::typed_call(return_type(), args_type(), func, L);
    }

    int match_arity(lua_State* L) {
        std::ptrdiff_t x = lua_gettop(L) - 1;
        return match_arity(L, x, build_indices<std::tuple_size<overloads_t>::value>());
    }

    int prelude(lua_State* L) {
        std::string accessor = stack::get<std::string>(L, 1 - lua_gettop(L));
        auto function = functions.find(accessor);
        if(function != functions.end()) {
            if(function->second.second) {
                stack::push<upvalue>(L, function->second.first.get());
                stack::push(L, &base_function::usertype<0>::ref_call, 1);
                return 1;
            }
            return (*function->second.first)(L);
        }
        return match_arity(L);
    }

    virtual int operator()(lua_State* L) override {
        return prelude(L);
    }

    virtual int operator()(lua_State* L, detail::ref_call_t) override {
        return prelude(L);
    }
};
} // sol

#endif // SOL_FUNCTION_TYPES_OVERLOAD_HPP
