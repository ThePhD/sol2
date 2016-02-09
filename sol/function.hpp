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

#ifndef SOL_FUNCTION_HPP
#define SOL_FUNCTION_HPP

#include "reference.hpp"
#include "tuple.hpp"
#include "stack.hpp"
#include "function_types.hpp"
#include "usertype_traits.hpp"
#include "resolve.hpp"
#include "function_result.hpp"
#include <cstdint>
#include <functional>
#include <memory>

namespace sol {
class function : public reference {
private:
    void luacall( std::ptrdiff_t argcount, std::ptrdiff_t resultcount ) const {
        lua_callk( lua_state( ), static_cast<int>( argcount ), static_cast<int>( resultcount ), 0, nullptr );
    }

    template<std::size_t... I, typename... Ret>
    std::tuple<Ret...> invoke( indices<I...>, types<Ret...>, std::ptrdiff_t n ) const {
        luacall( n, sizeof...( Ret ) );
        int nreturns = static_cast<int>( sizeof...( Ret ) );
        int stacksize = lua_gettop( lua_state( ) );
        int firstreturn = std::max( 0, stacksize - nreturns ) + 1;
        auto r = std::make_tuple( stack::get<Ret>( lua_state( ), firstreturn + I )... );
        lua_pop( lua_state( ), nreturns );
        return r;
    }

    template<std::size_t I, typename Ret>
    Ret invoke( indices<I>, types<Ret>, std::ptrdiff_t n ) const {
        luacall( n, 1 );
        return stack::pop<Ret>( lua_state( ) );
    }

    template <std::size_t I>
    void invoke( indices<I>, types<void>, std::ptrdiff_t n ) const {
        luacall( n, 0 );
    }

    function_result invoke( indices<>, types<>, std::ptrdiff_t n ) const {
        int stacksize = lua_gettop( lua_state( ) );
        int firstreturn = std::max( 1, stacksize - static_cast<int>( n ) );
        luacall( n, LUA_MULTRET );
        int poststacksize = lua_gettop( lua_state( ) );
        int returncount = poststacksize - firstreturn;
        return function_result( lua_state( ), firstreturn, returncount, returncount, call_error::ok );
    }

public:
    function( ) = default;
    function( lua_State* L, int index = -1 ) : reference( L, index ) {
        type_assert( L, index, type::function );
    }
    function( const function& ) = default;
    function& operator=( const function& ) = default;
    function( function&& ) = default;
    function& operator=( function&& ) = default;

    template<typename... Args>
    function_result operator()( Args&&... args ) const {
        return call<>( std::forward<Args>( args )... );
    }

    template<typename... Ret, typename... Args>
    auto operator()( types<Ret...>, Args&&... args ) const
        -> decltype( invoke( types<Ret...>( ), types<Ret...>( ), 0 ) ) {
        return call<Ret...>( std::forward<Args>( args )... );
    }

    template<typename... Ret, typename... Args>
    auto call( Args&&... args ) const
    -> decltype( invoke( types<Ret...>( ), types<Ret...>( ), 0 ) ) {
        push( );
        int pushcount = stack::push_args( lua_state( ), std::forward<Args>( args )... );
        auto tr = types<Ret...>( );
        return invoke( tr, tr, pushcount );
    }
};

class protected_function : public reference {
private:
    static reference& handler_storage() {
        static sol::reference h;
        return h;
    }

public:
    static const reference& get_default_handler () {
        return handler_storage();
    }

    static void set_default_handler( reference& ref ) {
        handler_storage() = ref;
    }

private:
    struct handler {
        const reference& target;
        int stackindex;
        handler(const reference& target) : target(target), stackindex(0) {
            if (target.valid()) {
                stackindex = lua_gettop(target.lua_state()) + 1;
                target.push();
            }
        }
        ~handler() {
            if (stackindex > 0) {
                lua_remove(target.lua_state(), stackindex);
            }
        }
    };

    int luacall(std::ptrdiff_t argcount, std::ptrdiff_t resultcount, handler& h) const {
        return lua_pcallk(lua_state(), static_cast<int>(argcount), static_cast<int>(resultcount), h.stackindex, 0, nullptr);
    }

    template<std::size_t... I, typename... Ret>
    std::tuple<Ret...> invoke(indices<I...>, types<Ret...>, std::ptrdiff_t n, handler& h) const {
        luacall(n, sizeof...(Ret), h);
        int nreturns = static_cast<int>(sizeof...(Ret));
        int stacksize = lua_gettop(lua_state());
        int firstreturn = std::max(0, stacksize - nreturns) + 1;
        auto r = std::make_tuple(stack::get<Ret>(lua_state(), firstreturn + I)...);
        lua_pop(lua_state(), nreturns);
        return r;
    }

    template<std::size_t I, typename Ret>
    Ret invoke(indices<I>, types<Ret>, std::ptrdiff_t n, handler& h) const {
        luacall(n, 1, h);
        return stack::pop<Ret>(lua_state());
    }

    template <std::size_t I>
    void invoke(indices<I>, types<void>, std::ptrdiff_t n, handler& h) const {
        luacall(n, 0, h);
    }

    function_result invoke(indices<>, types<>, std::ptrdiff_t n, handler& h) const {
        bool handlerpushed = error_handler.valid();
        int stacksize = lua_gettop(lua_state());
        int firstreturn = std::max(1, stacksize - static_cast<int>(n) - 1);
        int returncount = 0;
        call_error code = call_error::ok;
       
        try {
            code = static_cast<call_error>(luacall(n, LUA_MULTRET, h));
            int poststacksize = lua_gettop(lua_state());
            returncount = poststacksize - firstreturn;
        }
        // Handle C++ errors thrown from C++ functions bound inside of lua
        catch (const std::exception& error) {
            h.stackindex = 0;
            stack::push(lua_state(), error.what());
            firstreturn = lua_gettop(lua_state());
            return function_result(lua_state(), firstreturn, 0, 1, call_error::runtime);
        }
        catch (...) {
            throw;
        }
        return function_result(lua_state(), firstreturn + ( handlerpushed ? 0 : 1 ), returncount, returncount, code);
    }

public:
    sol::reference error_handler;

    protected_function() = default;
    protected_function(lua_State* L, int index = -1): reference(L, index), error_handler(get_default_handler()) {
        type_assert(L, index, type::function);
    }
    protected_function(const protected_function&) = default;
    protected_function& operator=(const protected_function&) = default;
    protected_function( protected_function&& ) = default;
    protected_function& operator=( protected_function&& ) = default;

    template<typename... Args>
    function_result operator()(Args&&... args) const {
        return call<>(std::forward<Args>(args)...);
    }

    template<typename... Ret, typename... Args>
    auto operator()(types<Ret...>, Args&&... args) const 
    -> decltype(invoke(types<Ret...>(), types<Ret...>(), 0, std::declval<handler&>())) {
        return call<Ret...>(std::forward<Args>(args)...);
    }

    template<typename... Ret, typename... Args>
    auto call(Args&&... args) const
    -> decltype(invoke(types<Ret...>(), types<Ret...>(), 0, std::declval<handler&>())) {
        handler h(error_handler);
        push();
        int pushcount = stack::push_args(lua_state(), std::forward<Args>(args)...);
        auto tr = types<Ret...>();
        return invoke( tr, tr, pushcount, h );
    }
};

namespace stack {
template<typename... Sigs>
struct pusher<function_sig<Sigs...>> {

    template<typename R, typename... Args, typename Fx, typename = std::result_of_t<Fx(Args...)>>
    static void set_memfx(types<R(Args...)> t, lua_State* L, Fx&& fx) {
        typedef std::decay_t<Unwrapped<Unqualified<Fx>>> raw_fx_t;
        typedef R(* fx_ptr_t)(Args...);
        typedef std::is_convertible<raw_fx_t, fx_ptr_t> is_convertible;
        set_isconvertible_fx(is_convertible(), t, L, std::forward<Fx>(fx));
    }

    template<typename Fx>
    static void set_memfx(types<>, lua_State* L, Fx&& fx) {
        typedef Unwrapped<Unqualified<Fx>> fx_t;
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
        fx_ptr_t fxptr = unwrap(std::forward<Fx>(fx));
        set(L, fxptr);
    }

    template<typename Fx, typename R, typename... Args>
    static void set_isconvertible_fx(std::false_type, types<R(Args...)>, lua_State* L, Fx&& fx) {
        typedef Unwrapped<std::decay_t<Fx>> fx_t;
        std::unique_ptr<base_function> sptr(new functor_function<fx_t>(std::forward<Fx>(fx)));
        set_fx<Fx>(L, std::move(sptr));
    }

    template<typename Fx, typename T>
    static void set_reference_fx(std::true_type, lua_State* L, Fx&& fx, T&& obj) {
        set_fx(std::true_type(), L, std::forward<Fx>(fx), std::forward<T>(obj));
    }

    template<typename Fx, typename T>
    static void set_reference_fx(std::false_type, lua_State* L, Fx&& fx, T&& obj) {
        typedef std::remove_pointer_t<std::decay_t<Fx>> clean_fx;
        std::unique_ptr<base_function> sptr(new member_function<clean_fx, Unqualified<T>>(std::forward<T>(obj), std::forward<Fx>(fx)));
        return set_fx<Fx>(L, std::move(sptr));
    }

    template<typename Fx, typename T>
    static void set_fx(std::true_type, lua_State* L, Fx&& fx, T&& obj) {
        // Layout:
        // idx 1...n: verbatim data of member function pointer
        // idx n + 1: is the object's void pointer
        // We don't need to store the size, because the other side is templated
        // with the same member function pointer type
        typedef std::decay_t<Fx> dFx;
        typedef Unqualified<Fx> uFx;
        dFx memfxptr(std::forward<Fx>(fx));
        auto userptr = sol::detail::get_ptr(obj);
        void* userobjdata = static_cast<void*>(userptr);
        lua_CFunction freefunc = &static_member_function<std::decay_t<decltype(*userptr)>, uFx>::call;

        int upvalues = stack::detail::push_as_upvalues(L, memfxptr);
        upvalues += stack::push(L, userobjdata);
        
        stack::push(L, freefunc, upvalues);
    }

    template<typename Fx>
    static void set_fx(std::false_type, lua_State* L, Fx&& fx) {
        std::decay_t<Fx> target(std::forward<Fx>(fx));
        lua_CFunction freefunc = &static_function<Fx>::call;

        int upvalues = stack::detail::push_as_upvalues(L, target);
        stack::push(L, freefunc, upvalues);
    }

    template<typename Fx>
    static void set_fx(lua_State* L, std::unique_ptr<base_function> luafunc) {
        const auto& metakey = usertype_traits<Unqualified<Fx>>::metatable;
        const char* metatablename = std::addressof(metakey[0]);
        base_function* target = luafunc.release();
        void* userdata = reinterpret_cast<void*>(target);
        lua_CFunction freefunc = &base_function::call;
       
        int metapushed = luaL_newmetatable(L, metatablename);
        if(metapushed == 1) {
            lua_pushstring(L, "__gc");
            stack::push(L, &base_function::gc);
            lua_settable(L, -3);
            lua_pop(L, 1);
        }

        stack::detail::push_userdata<void*>(L, metatablename, userdata);
        stack::push(L, freefunc, 1);
    }

    template<typename... Args>
    static int push(lua_State* L, Args&&... args) {
        // Set will always place one thing (function) on the stack
        set(L, std::forward<Args>(args)...);
        return 1;
    }
};

template<typename T, typename... Args>
struct pusher<sol::detail::function_packer<T, Args...>> {
    template <std::size_t... I, typename FP>
    static int push_func(indices<I...>, lua_State* L, FP&& fp) {
        return stack::push<T>(L, std::get<I>(fp)...);
    }

    template <typename FP>
    static int push(lua_State* L, FP&& fp) {
        return push_func(build_indices<sizeof...(Args)>(), L, std::forward<FP>(fp));
    }
};

template<typename Signature>
struct pusher<std::function<Signature>> {
    static int push(lua_State* L, std::function<Signature> fx) {
        return pusher<function_sig<>>{}.push(L, std::move(fx));
    }
};

template<typename Signature>
struct getter<std::function<Signature>> {
    typedef function_traits<Signature> fx_t;
    typedef typename fx_t::args_type args_t;
    typedef typename tuple_types_t<typename fx_t::return_type> ret_t;

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
