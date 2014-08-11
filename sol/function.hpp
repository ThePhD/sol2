// The MIT License (MIT)

// Copyright (c) 2013 Danny Y., Rapptz

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

#ifndef SOL_FUNCTION_HPP
#define SOL_FUNCTION_HPP

#include "reference.hpp"
#include "tuple.hpp"
#include "stack.hpp"
#include "function_types.hpp"
#include "userdata_traits.hpp"
#include "resolve.hpp"
#include <cstdint>
#include <functional>
#include <memory>

namespace sol {
class function : public reference {
private:
    void luacall(std::size_t argcount, std::size_t resultcount) const {
        lua_call(state(), static_cast<uint32_t>(argcount), static_cast<uint32_t>(resultcount));
    }

    template<typename... Ret>
    std::tuple<Ret...> invoke(types<Ret...>, std::size_t n) const {
        luacall(n, sizeof...(Ret));
        return stack::pop_reverse_call(state(), std::make_tuple<Ret...>, types<Ret...>());
    }

    template<typename Ret>
    Ret invoke(types<Ret>, std::size_t n) const {
        luacall(n, 1);
        return stack::pop<Ret>(state());
    }

    void invoke(types<void>, std::size_t n) const {
        luacall(n, 0);
    }

    void invoke(types<>, std::size_t n) const {
        luacall(n, 0);
    }

public:
    function() = default;
    function(lua_State* L, int index = -1): reference(L, index) {
        type_assert(L, index, type::function);
    }
    function(const function&) = default;
    function& operator=(const function&) = default;

    template<typename... Args>
    void operator()(Args&&... args) const {
        call<>(std::forward<Args>(args)...);
    }

    template<typename... Ret, typename... Args>
    typename return_type<Ret...>::type operator()(types<Ret...>, Args&&... args) const {
        return call<Ret...>(std::forward<Args>(args)...);
    }

    template<typename... Ret, typename... Args>
    typename return_type<Ret...>::type call(Args&&... args) const {
        push();
        stack::push_args(state(), std::forward<Args>(args)...);
        return invoke(types<Ret...>(), sizeof...(Args));
    }
};

namespace stack {
template<typename... Sigs>
struct pusher<function_sig_t<Sigs...>> {

    template<typename R, typename... Args, typename Fx, typename = typename std::result_of<Fx(Args...)>::type>
    static void set_memfx(types<R(Args...)> t, lua_State* L, Fx&& fx) {
        typedef Decay<Unwrap<Fx>> raw_fx_t;
        typedef R(* fx_ptr_t)(Args...);
        typedef std::is_convertible<raw_fx_t, fx_ptr_t> is_convertible;
        set_isconvertible_fx(is_convertible(), t, L, std::forward<Fx>(fx));
    }

    template<typename... Args, typename Fx, typename R = typename std::result_of<Fx(Args...)>::type>
    static void set_memfx(types<Args...>, lua_State* L, Fx&& fx){
        set_memfx(types<R(Args...)>(), L, std::forward<Fx>(fx));
    }

    template<typename Fx>
    static void set_memfx(types<>, lua_State* L, Fx&& fx) {
        typedef Unqualified<Unwrap<Fx>> fx_t;
        typedef decltype(&fx_t::operator()) Sig;
        set_memfx(types<function_signature_t<Sig>>(), L, std::forward<Fx>(fx));
    }

    template<typename... Args, typename R>
    static void set(lua_State* L, R fxptr(Args...)){
        set_fx(std::false_type(), L, fxptr);
    }

    template<typename Sig>
    static void set(lua_State* L, Sig* fxptr){
       set_fx(std::false_type(), L, fxptr);
    }

    template<typename... Args, typename R, typename C, typename T>
    static void set(lua_State* L, R (C::*memfxptr)(Args...), T&& obj) {
        typedef Bool<is_specialization_of<T, std::reference_wrapper>::value || std::is_pointer<T>::value> is_reference;
        set_reference_fx(is_reference(), L, memfxptr, std::forward<T>(obj));
    }

    template<typename Sig, typename C, typename T>
    static void set(lua_State* L, Sig C::* memfxptr, T&& obj) {
        typedef Bool<is_specialization_of<T, std::reference_wrapper>::value || std::is_pointer<T>::value> is_reference;
        set_reference_fx(is_reference(), L, memfxptr, std::forward<T>(obj));
    }

    template<typename... Sig, typename Fx>
    static void set(lua_State* L, Fx&& fx) {
        set_memfx(types<Sig...>(), L, std::forward<Fx>(fx));
    }

    template<typename Fx, typename R, typename... Args>
    static void set_isconvertible_fx(std::true_type, types<R(Args...)>, lua_State* L, Fx&& fx) {
        typedef R(* fx_ptr_t)(Args...);
        fx_ptr_t fxptr = unwrapper(std::forward<Fx>(fx));
        set(L, fxptr);
    }

    template<typename Fx, typename R, typename... Args>
    static void set_isconvertible_fx(std::false_type, types<R(Args...)>, lua_State* L, Fx&& fx) {
        typedef Decay<Unwrap<Fx>> fx_t;
        std::unique_ptr<base_function> sptr(new functor_function<fx_t>(std::forward<Fx>(fx)));
        set_fx<Fx>(L, std::move(sptr));
    }

    template<typename Fx, typename T>
    static void set_reference_fx(std::true_type, lua_State* L, Fx&& fx, T&& obj) {
        set_fx(std::true_type(), L, std::forward<Fx>(fx), std::forward<T>(obj));
    }

    template<typename Fx, typename T>
    static void set_reference_fx(std::false_type, lua_State* L, Fx&& fx, T&& obj) {
        typedef typename std::remove_pointer<Decay<Fx>>::type clean_fx;
        std::unique_ptr<base_function> sptr(new member_function<clean_fx, T>(std::forward<T>(obj), std::forward<Fx>(fx)));
        return set_fx<Fx>(L, std::move(sptr));
    }

    template<typename Fx, typename T>
    static void set_fx(std::true_type, lua_State* L, Fx&& fx, T&& obj) {
        // Layout:
        // idx 1...n: verbatim data of member function pointer
        // idx n + 1: is the object's void pointer
        // We don't need to store the size, because the other side is templated
        // with the same member function pointer type
        Decay<Fx> memfxptr(std::forward<Fx>(fx));
        auto userptr = sol::detail::get_ptr(obj);
        void* userobjdata = static_cast<void*>(userptr);
        lua_CFunction freefunc = &static_member_function<Decay<decltype(*userptr)>, Fx>::call;

        int upvalues = stack::detail::push_as_upvalues(L, memfxptr);
        stack::push(L, userobjdata);
        ++upvalues;
        stack::push(L, freefunc, upvalues);
    }

    template<typename Fx>
    static void set_fx(std::false_type, lua_State* L, Fx&& fx) {
        Decay<Fx> target(std::forward<Fx>(fx));
        lua_CFunction freefunc = &static_function<Fx>::call;

        int upvalues = stack::detail::push_as_upvalues(L, target);
        stack::push(L, freefunc, upvalues);
    }

    template<typename Fx>
    static void set_fx(lua_State* L, std::unique_ptr<base_function> luafunc) {
        auto&& metakey = userdata_traits<Unqualified<Fx>>::metatable;
        const char* metatablename = std::addressof(metakey[0]);
        base_function* target = luafunc.release();
        void* userdata = reinterpret_cast<void*>(target);
        lua_CFunction freefunc = &base_function::call;

        if(luaL_newmetatable(L, metatablename) == 1) {
            lua_pushstring(L, "__gc");
            stack::push(L, &base_function::gc);
            lua_settable(L, -3);
            lua_pop(L, 1);
        }

        stack::detail::push_userdata<void*>(L, metatablename, userdata);
        stack::push(L, freefunc, 1);
    }

    template<typename... Args>
    static void push(lua_State* L, Args&&... args) {
        set(L, std::forward<Args>(args)...);
    }
};

template<typename Signature>
struct pusher<std::function<Signature>> {
    static void push(lua_State* L, std::function<Signature> fx) {
        pusher<function_t>{}.push(L, std::move(fx));
    }
};

template<typename Signature>
struct getter<std::function<Signature>> {
    typedef function_traits<Signature> fx_t;
    typedef typename fx_t::args_type args_t;
    typedef typename tuple_types<typename fx_t::return_type>::type ret_t;

    template<typename... FxArgs, typename... Ret>
    static std::function<Signature> get_std_func(types<FxArgs...>, types<Ret...>, lua_State* L, int index = -1) {
        typedef typename function_traits<Signature>::return_type return_t;
        sol::function f(L, index);
        auto fx = [f, L, index](FxArgs&&... args) -> return_t {
            return f(types<Ret...>(), std::forward<FxArgs>(args)...);
        };
        return std::move(fx);
    }

    template<typename... FxArgs>
    static std::function<Signature> get_std_func(types<FxArgs...>, types<void>, lua_State* L, int index = -1) {
        sol::function f(L, index);
        auto fx = [f, L, index](FxArgs&&... args) -> void {
            f(std::forward<FxArgs>(args)...);
        };
        return std::move(fx);
    }

    template<typename... FxArgs>
    static std::function<Signature> get_std_func(types<FxArgs...> t, types<>, lua_State* L, int index = -1) {
        return get_std_func(std::move(t), types<void>(), L, index);
    }

    static std::function<Signature> get(lua_State* L, int index) {
        return get_std_func(args_t(), ret_t(), L, index);
    }
};
} // stack
} // sol

#endif // SOL_FUNCTION_HPP
