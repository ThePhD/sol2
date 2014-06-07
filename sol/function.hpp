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
#include <cstdint>
#include <functional>
#include <memory>

namespace sol {
class function : public reference {
private:
    void luacall (std::size_t argcount, std::size_t resultcount) const {
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
    function() : reference() {}
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
template <>
struct pusher<function_t> {

    template<typename TFx>
    static void set_isfunction_fx(std::true_type, lua_State* L, TFx&& fx) {
        set_fx(std::false_type(), L, std::forward<TFx>(fx));
    }

    template<typename TFx>
    static void set_isfunction_fx(std::false_type, lua_State* L, TFx&& fx) {
        typedef Decay<TFx> clean_lambda;
        typedef typename function_traits<decltype(&clean_lambda::operator())>::free_function_pointer_type raw_func_t;
        typedef std::is_convertible<clean_lambda, raw_func_t> is_convertible;
        set_isconvertible_fx(is_convertible(), L, std::forward<TFx>(fx));
    }

    template<typename TFx>
    static void set_isconvertible_fx(std::true_type, lua_State* L, TFx&& fx) {
        typedef Decay<TFx> clean_lambda;
        typedef typename function_traits<decltype(&clean_lambda::operator())>::free_function_pointer_type raw_func_t;
        set_isfunction_fx(std::true_type(), L, raw_func_t(std::forward<TFx>(fx)));
    }

    template<typename TFx>
    static void set_isconvertible_fx(std::false_type, lua_State* L, TFx&& fx) {
        typedef typename std::remove_pointer<Decay<TFx>>::type clean_fx;
        std::unique_ptr<base_function> sptr(new functor_function<clean_fx>(std::forward<TFx>(fx)));
        set_fx<TFx>(L, std::move(sptr));
    }

    template<typename TFx, typename TObj>
    static void set_lvalue_fx(std::true_type, lua_State* L, TFx&& fx, TObj&& obj) {
        set_fx(std::true_type(), L, std::forward<TFx>(fx), std::forward<TObj>(obj));
    }

    template<typename TFx, typename TObj>
    static void set_lvalue_fx(std::false_type, lua_State* L, TFx&& fx, TObj&& obj) {
        typedef typename std::remove_pointer<Decay<TFx>>::type clean_fx;
        std::unique_ptr<base_function> sptr(new member_function<clean_fx, TObj>(std::forward<TObj>(obj), std::forward<TFx>(fx)));
        return set_fx<TFx>(L, std::move(sptr));
    }

    template<typename TFx, typename TObj>
    static void set_fx(std::true_type, lua_State* L, TFx&& fx, TObj&& obj) {
        // Layout:
        // idx 1...n: verbatim data of member function pointer
        // idx n + 1: is the object's void pointer
        // We don't need to store the size, because the other side is templated
        // with the same member function pointer type
        Decay<TFx> fxptr(std::forward<TFx>(fx));
        void* userobjdata = static_cast<void*>(sol::detail::get_ptr(obj));
        lua_CFunction freefunc = &static_member_function<Decay<TObj>, TFx>::call;

        int upvalues = stack::detail::push_as_upvalues(L, fxptr);
        stack::push(L, userobjdata);
        stack::pusher<lua_CFunction>{}.push(L, freefunc, upvalues);
    }

    template<typename TFx>
    static void set_fx(lua_State* L, std::false_type, TFx&& fx) {
        Decay<TFx> target(std::forward<TFx>(fx));
        lua_CFunction freefunc = &static_function<TFx>::call;

        int upvalues = stack::detail::push_as_upvalues(L, target);
        stack::pusher<lua_CFunction>{}.push(L, freefunc, upvalues);
    }

    template<typename TFx>
    static void set_fx(lua_State* L, std::unique_ptr<base_function> luafunc) {
        auto&& metakey = userdata_traits<Unqualified<TFx>>::metatable;
        const char* metatablename = std::addressof(metakey[0]);
        base_function* target = luafunc.release();
        void* userdata = reinterpret_cast<void*>(target);
        lua_CFunction freefunc = &base_function::call;

        if (luaL_newmetatable(L, metatablename) == 1) {
            lua_pushstring(L, "__gc");
            stack::push(L, &base_function::gc);
            lua_settable(L, -3);
        }

        stack::detail::push_userdata<void*>(L, metatablename, userdata);
        stack::pusher<lua_CFunction>{}.push(L, freefunc, 1);
    }

    template<typename TFx>
    static void set_function(lua_State* L, TFx&& fx) {
        typedef typename std::remove_pointer<Decay<TFx>>::type clean_fx;
        set_isfunction_fx(std::is_function<clean_fx>(), L, std::forward<TFx>(fx));
    }

    template<typename TFx, typename TObj>
    static void set_function(lua_State* L, TFx&& fx, TObj&& obj) {
        set_lvalue_fx(Bool<std::is_lvalue_reference<TObj>::value || std::is_pointer<TObj>::value>(),
            L, std::forward<TFx>(fx), std::forward<TObj>(obj));
    }

    template <typename... Args>
    static void push(lua_State* L, Args&&... args) {
        set_function(L, std::forward<Args>(args)...);
    }
};

template <typename Signature>
struct pusher<std::function<Signature>> {
    static void push(lua_State* L, std::function<Signature> fx) {
        pusher<function_t>{}.push(L, std::move(fx));
    }
};

template <typename Signature>
struct getter<std::function<Signature>> {
    typedef function_traits<Signature> fx_t;
    typedef typename fx_t::args_type args_t;
    typedef typename tuple_types<typename fx_t::return_type>::type ret_t;

    template <typename... FxArgs, typename... Ret>
    static std::function<Signature> get_std_func(types<FxArgs...>, types<Ret...>, lua_State* L, int index = -1) {
        typedef typename function_traits<Signature>::return_type return_t;
        sol::function f(L, index);
        auto fx = [ f, L, index ] (FxArgs&&... args) -> return_t {
            return f(types<Ret...>(), std::forward<FxArgs>(args)...);
        };
        return std::move(fx);
    }

    template <typename... FxArgs>
    static std::function<Signature> get_std_func(types<FxArgs...>, types<void>, lua_State* L, int index = -1) {
        sol::function f(L, index);
        auto fx = [ f, L, index ] (FxArgs&&... args) -> void {
            f(std::forward<FxArgs>(args)...);
        };
        return std::move(fx);
    }

    template <typename... FxArgs>
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
