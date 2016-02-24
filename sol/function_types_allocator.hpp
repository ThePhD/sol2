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

#ifndef SOL_FUNCTION_TYPES_ALLOCATOR_HPP
#define SOL_FUNCTION_TYPES_ALLOCATOR_HPP

#include "raii.hpp"
#include "function_types_overload.hpp"

namespace sol {
namespace detail {
template <typename T, typename List>
struct void_call;

template <typename T, typename... Args>
struct void_call<T, types<Args...>> {
    static void call(Args... args) {}
};

template <typename T>
struct constructor_match {
    T* obj;

    constructor_match(T* obj) : obj(obj) {}

    template <bool b, typename Fx, std::size_t I, typename... R, typename... Args>
    int operator()(meta::Bool<b>, types<Fx>, Index<I>, types<R...> r, types<Args...> a, lua_State* L, int, int start) const {
        default_construct func{};
        return stack::typed_call<b ? false : stack::stack_detail::default_check_arguments>(r, a, func, L, start, obj);
    }
};
} // detail

namespace function_detail {
template <typename T, typename... TypeLists, typename Match>
inline int construct(Match&& matchfx, lua_State* L, int fxarity, int start) {
    // use same overload resolution matching as all other parts of the framework
    return overload_match_arity<decltype(detail::void_call<T, TypeLists>::call)...>(std::forward<Match>(matchfx), L, fxarity, start);
}

template <typename T, typename... TypeLists>
inline int construct(lua_State* L) {
    static const auto& meta = usertype_traits<T>::metatable;
    call_syntax syntax = stack::get_call_syntax(L, meta);
    int argcount = lua_gettop(L) - static_cast<int>(syntax);

    T** pointerpointer = reinterpret_cast<T**>(lua_newuserdata(L, sizeof(T*) + sizeof(T)));
    T*& referencepointer = *pointerpointer;
    T* obj = reinterpret_cast<T*>(pointerpointer + 1);
    referencepointer = obj;
    reference userdataref(L, -1);
    userdataref.pop();
        
    construct<T, TypeLists...>(detail::constructor_match<T>(obj), L, argcount, 1 + static_cast<int>(syntax));

    userdataref.push();
    luaL_getmetatable(L, &meta[0]);
    if (stack::get<type>(L) == type::nil) {
        lua_pop(L, 1);
        std::string err = "unable to get usertype metatable for ";
        err += meta;
        throw error(err);
    }

    lua_setmetatable(L, -2);
    return 1;
}

template <typename T>
inline int destruct(lua_State* L) {
    userdata udata = stack::get<userdata>(L, 1);
    // The first sizeof(T*) bytes are the reference: the rest is
    // the actual data itself (if there is a reference at all)
    T** pobj = reinterpret_cast<T**>(udata.value);
    T*& obj = *pobj;
    std::allocator<T> alloc{};
    alloc.destroy(obj);
    return 0;
}

template <typename T, typename... Functions>
struct usertype_constructor_function : base_function {
    typedef std::tuple<Functions...> overload_list;
    typedef std::index_sequence_for<Functions...> indices;
    overload_list overloads;

    usertype_constructor_function(constructor_wrapper<Functions...> set) : usertype_constructor_function(indices(), set) {}

    template <std::size_t... I>
    usertype_constructor_function(std::index_sequence<I...>, constructor_wrapper<Functions...> set) : usertype_constructor_function(std::get<I>(set)...) {}

    usertype_constructor_function(Functions... fxs) : overloads(fxs...) {}

    template <bool b, typename Fx, std::size_t I, typename... R, typename... Args>
    int call(meta::Bool<b>, types<Fx>, Index<I>, types<R...> r, types<Args...> a, lua_State* L, int, int start) {
        static const auto& meta = usertype_traits<T>::metatable;
        T** pointerpointer = reinterpret_cast<T**>(lua_newuserdata(L, sizeof(T*) + sizeof(T)));
        T*& referencepointer = *pointerpointer;
        T* obj = reinterpret_cast<T*>(pointerpointer + 1);
        referencepointer = obj;
        reference userdataref(L, -1);
        userdataref.pop();

        auto& func = std::get<I>(overloads);
        stack::typed_call<b ? false : stack::stack_detail::default_check_arguments>(r, a, func, L, start, function_detail::implicit_wrapper<T>(obj));

        userdataref.push();
        luaL_getmetatable(L, &meta[0]);
        if (stack::get<type>(L) == type::nil) {
            lua_pop(L, 1);
            std::string err = "unable to get usertype metatable for ";
            err += meta;
            throw error(err);
        }
        lua_setmetatable(L, -2);
            
        return 1;
    }

    virtual int operator()(lua_State* L) override {
        static const auto& meta = usertype_traits<T>::metatable;
        call_syntax syntax = stack::get_call_syntax(L, meta);
        int argcount = lua_gettop(L) - static_cast<int>(syntax);
        auto mfx = [&](auto&&... args) { return this->call(std::forward<decltype(args)>(args)...); };
        return construct<T, meta::pop_front_type_t<meta::function_args_t<Functions>>...>(mfx, L, argcount, 1 + static_cast<int>(syntax));
    }
};
} // function_detail
} // sol

#endif // SOL_FUNCTION_TYPES_ALLOCATOR_HPP
