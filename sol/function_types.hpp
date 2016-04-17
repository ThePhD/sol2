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

#ifndef SOL_FUNCTION_TYPES_HPP
#define SOL_FUNCTION_TYPES_HPP

#include "function_types_core.hpp"
#include "function_types_templated.hpp"
#include "function_types_basic.hpp"
#include "function_types_allocator.hpp"
#include "function_types_member.hpp"
#include "function_types_usertype.hpp"
#include "function_types_overload.hpp"
#include "function_types_allocator.hpp"
#include "resolve.hpp"

namespace sol {
template <typename Sig, typename... Ps>
struct function_arguments {
    std::tuple<Ps...> params;
    template <typename... Args>
    function_arguments(Args&&... args) : params(std::forward<Args>(args)...) {}
};

template <typename Sig = function_sig<>, typename... Args>
function_arguments<Sig, Args...> function_args( Args&&... args ) { 
    return function_arguments<Sig, Args...>(std::forward<Args>(args)...);
}

// Allow someone to make a member variable readonly (const)
template <typename R, typename T>
auto readonly( R T::* v ) {
    typedef const R C;
    return static_cast<C T::*>( v );
}

namespace stack {
template<typename... Sigs>
struct pusher<function_sig<Sigs...>> {
    template <typename... Sig, typename Fx, typename... Args>
    static void select_convertible(std::false_type, types<Sig...>, lua_State* L, Fx&& fx, Args&&... args) {
        typedef std::remove_pointer_t<std::decay_t<Fx>> clean_fx;
        std::unique_ptr<function_detail::base_function> sptr = std::make_unique<function_detail::functor_function<clean_fx>>(std::forward<Fx>(fx), std::forward<Args>(args)...);
        set_fx(L, std::move(sptr));
    }

    template <typename R, typename... A, typename Fx, typename... Args>
    static void select_convertible(std::true_type, types<R(A...)>, lua_State* L, Fx&& fx, Args&&... args) {
        using fx_ptr_t = R(*)(A...);
        fx_ptr_t fxptr = detail::unwrap(std::forward<Fx>(fx));
        select_function(std::true_type(), L, fxptr, std::forward<Args>(args)...);
    }

    template <typename R, typename... A, typename Fx, typename... Args>
    static void select_convertible(types<R(A...)> t, lua_State* L, Fx&& fx, Args&&... args) {
        typedef std::decay_t<meta::Unwrapped<meta::Unqualified<Fx>>> raw_fx_t;
        typedef R(* fx_ptr_t)(A...);
        typedef std::is_convertible<raw_fx_t, fx_ptr_t> is_convertible;
        select_convertible(is_convertible(), t, L, std::forward<Fx>(fx), std::forward<Args>(args)...);
    }

    template <typename Fx, typename... Args>
    static void select_convertible(types<>, lua_State* L, Fx&& fx, Args&&... args) {
        typedef meta::function_signature_t<meta::Unwrapped<meta::Unqualified<Fx>>> Sig;
        select_convertible(types<Sig>(), L, std::forward<Fx>(fx), std::forward<Args>(args)...);
    }

    template <typename Fx, typename T, typename... Args>
    static void select_reference_member_variable(std::false_type, lua_State* L, Fx&& fx, T&& obj, Args&&... args) {
        typedef std::remove_pointer_t<std::decay_t<Fx>> clean_fx;
        std::unique_ptr<function_detail::base_function> sptr = std::make_unique<function_detail::member_variable<meta::Unqualified<T>, clean_fx>>(std::forward<Fx>(fx), std::forward<T>(obj), std::forward<Args>(args)... );
        set_fx(L, std::move(sptr));
    }

    template <typename Fx, typename T, typename... Args>
    static void select_reference_member_variable(std::true_type, lua_State* L, Fx&& fx, T&& obj, Args&&... args) {
        typedef std::decay_t<Fx> dFx;
        dFx memfxptr(std::forward<Fx>(fx));
        auto userptr = detail::ptr(std::forward<T>(obj), std::forward<Args>(args)...);
        lua_CFunction freefunc = &function_detail::upvalue_member_variable<std::decay_t<decltype(*userptr)>, meta::Unqualified<Fx>>::call;

        int upvalues = stack::stack_detail::push_as_upvalues(L, memfxptr);
        upvalues += stack::push(L, light_userdata_value(static_cast<void*>(userptr)));        
        stack::push(L, c_closure(freefunc, upvalues));
    }

    template <typename Fx, typename... Args>
    static void select_member_variable(std::false_type, lua_State* L, Fx&& fx, Args&&... args) {
        select_convertible(types<Sigs...>(), L, std::forward<Fx>(fx), std::forward<Args>(args)...);
    }

    template <typename Fx, typename T, typename... Args>
    static void select_member_variable(std::true_type, lua_State* L, Fx&& fx, T&& obj, Args&&... args) {
        typedef meta::Bool<meta::is_specialization_of<std::reference_wrapper, meta::Unqualified<T>>::value || std::is_pointer<T>::value> is_reference;
        select_reference_member_variable(is_reference(), L, std::forward<Fx>(fx), std::forward<T>(obj), std::forward<Args>(args)...);
    }

    template <typename Fx>
    static void select_member_variable(std::true_type, lua_State* L, Fx&& fx) {
        typedef typename meta::bind_traits<meta::Unqualified<Fx>>::object_type C;
        lua_CFunction freefunc = &function_detail::upvalue_this_member_variable<C, Fx>::call;
        int upvalues = stack::stack_detail::push_as_upvalues(L, fx);
        stack::push(L, c_closure(freefunc, upvalues));
    }

    template <typename Fx, typename T, typename... Args>
    static void select_reference_member_function(std::false_type, lua_State* L, Fx&& fx, T&& obj, Args&&... args) {
        typedef std::remove_pointer_t<std::decay_t<Fx>> clean_fx;
        std::unique_ptr<function_detail::base_function> sptr = std::make_unique<function_detail::member_function<meta::Unwrapped<meta::Unqualified<T>>, clean_fx>>(std::forward<Fx>(fx), std::forward<T>(obj), std::forward<Args>(args)... );
        set_fx(L, std::move(sptr));
    }

    template <typename Fx, typename T, typename... Args>
    static void select_reference_member_function(std::true_type, lua_State* L, Fx&& fx, T&& obj, Args&&... args) {
        typedef std::decay_t<Fx> dFx;
        dFx memfxptr(std::forward<Fx>(fx));
        auto userptr = detail::ptr(std::forward<T>(obj), std::forward<Args>(args)...);
        lua_CFunction freefunc = &function_detail::upvalue_member_function<std::decay_t<decltype(*userptr)>, meta::Unqualified<Fx>>::call;

        int upvalues = stack::stack_detail::push_as_upvalues(L, memfxptr);
        upvalues += stack::push(L, light_userdata_value(static_cast<void*>(userptr)));        
        stack::push(L, c_closure(freefunc, upvalues));
    }

    template <typename Fx, typename... Args>
    static void select_member_function(std::false_type, lua_State* L, Fx&& fx, Args&&... args) {
        select_member_variable(std::is_member_object_pointer<meta::Unqualified<Fx>>(), L, std::forward<Fx>(fx), std::forward<Args>(args)...);
    }

    template <typename Fx, typename T, typename... Args>
    static void select_member_function(std::true_type, lua_State* L, Fx&& fx, T&& obj, Args&&... args) {
        typedef meta::Bool<meta::is_specialization_of<std::reference_wrapper, meta::Unqualified<T>>::value || std::is_pointer<T>::value> is_reference;
        select_reference_member_function(is_reference(), L, std::forward<Fx>(fx), std::forward<T>(obj), std::forward<Args>(args)...);
    }

    template <typename Fx>
    static void select_member_function(std::true_type, lua_State* L, Fx&& fx) {
        typedef typename meta::bind_traits<meta::Unqualified<Fx>>::object_type C;
        lua_CFunction freefunc = &function_detail::upvalue_this_member_function<C, Fx>::call;
        int upvalues = stack::stack_detail::push_as_upvalues(L, fx);
        stack::push(L, c_closure(freefunc, upvalues));
    }

    template <typename Fx, typename... Args>
    static void select_function(std::false_type, lua_State* L, Fx&& fx, Args&&... args) {
        select_member_function(std::is_member_function_pointer<meta::Unqualified<Fx>>(), L, std::forward<Fx>(fx), std::forward<Args>(args)...);
    }

    template <typename Fx, typename... Args>
    static void select_function(std::true_type, lua_State* L, Fx&& fx, Args&&... args) {
        std::decay_t<Fx> target(std::forward<Fx>(fx), std::forward<Args>(args)...);
        lua_CFunction freefunc = &function_detail::upvalue_free_function<Fx>::call;

        int upvalues = stack::stack_detail::push_as_upvalues(L, target);
        stack::push(L, freefunc, upvalues);
    }

    static void select_function(std::true_type, lua_State* L, lua_CFunction f) {
        stack::push(L, f);
    }

    template <typename Fx, typename... Args>
    static void select(lua_State* L, Fx&& fx, Args&&... args) {
        select_function(std::is_function<meta::Unqualified<Fx>>(), L, std::forward<Fx>(fx), std::forward<Args>(args)...);
    }

    static void set_fx(lua_State* L, std::unique_ptr<function_detail::base_function> luafunc) {
        function_detail::base_function* target = luafunc.release();
        void* targetdata = static_cast<void*>(target);
        lua_CFunction freefunc = function_detail::call;

        stack::push(L, userdata_value(targetdata));
        function_detail::free_function_cleanup(L);
        lua_setmetatable(L, -2);
        stack::push(L, freefunc, 1);
    }

    template<typename... Args>
    static int push(lua_State* L, Args&&... args) {
        // Set will always place one thing (function) on the stack
        select(L, std::forward<Args>(args)...);
        return 1;
    }
};

template<typename T, typename... Args>
struct pusher<function_arguments<T, Args...>> {
    template <std::size_t... I, typename FP>
    static int push_func(std::index_sequence<I...>, lua_State* L, FP&& fp) {
        return stack::push<T>(L, detail::forward_get<I>(fp.params)...);
    }

    template <typename FP>
    static int push(lua_State* L, FP&& fp) {
        return push_func(std::make_index_sequence<sizeof...(Args)>(), L, std::forward<FP>(fp));
    }
};

template<typename Signature>
struct pusher<std::function<Signature>> {
    static int push(lua_State* L, std::function<Signature> fx) {
        return pusher<function_sig<Signature>>{}.push(L, std::move(fx));
    }
};

template<typename Signature>
struct pusher<Signature, std::enable_if_t<std::is_member_pointer<Signature>::value>> {
    template <typename F>
    static int push(lua_State* L, F&& f) {
        return pusher<function_sig<>>{}.push(L, std::forward<F>(f));
    }
};

template<typename Signature>
struct pusher<Signature, std::enable_if_t<meta::And<std::is_function<Signature>, meta::Not<std::is_same<Signature, lua_CFunction>>, meta::Not<std::is_same<Signature, std::remove_pointer_t<lua_CFunction>>>>::value>> {
    template <typename F>
    static int push(lua_State* L, F&& f) {
        return pusher<function_sig<>>{}.push(L, std::forward<F>(f));
    }
};

template<typename... Functions>
struct pusher<overload_set<Functions...>> {
    static int push(lua_State* L, overload_set<Functions...>&& set) {
        pusher<function_sig<>>{}.set_fx(L, std::make_unique<function_detail::overloaded_function<Functions...>>(std::move(set.set)));
        return 1;
    }

    static int push(lua_State* L, const overload_set<Functions...>& set) {
        pusher<function_sig<>>{}.set_fx(L, std::make_unique<function_detail::overloaded_function<Functions...>>(set.set));
        return 1;
    }
};

} // stack
} // sol

#endif // SOL_FUNCTION_TYPES_HPP
