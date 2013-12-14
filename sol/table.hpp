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

#ifndef SOL_TABLE_HPP
#define SOL_TABLE_HPP

#include "stack.hpp"
#include "lua_function.hpp"
#include <array>
#include <cstring>

namespace sol {
namespace detail {
template<typename T>
T* get_ptr(T& val) {
    return std::addressof(val);
}

template<typename T>
T* get_ptr(T* val) {
    return val;
}
} // detail

class table : public reference {
    template<typename Table, typename T> struct proxy;
public:
    table() noexcept : reference() {}
    table(lua_State* L, int index = -1) : reference(L, index) {
        type_assert(L, index, type::table);
    }

    template<typename T, typename U>
    T get(U&& key) const {
        push();
        stack::push(state(), std::forward<U>(key));
        lua_gettable(state(), -2);
        type_assert(state(), -1, type_of<T>());
        auto result = stack::pop<T>(state());
        lua_pop(state(), 1);
        return result;
    }

    template<typename T, typename U>
    table& set(T&& key, U&& value) {
        push();
        stack::push(state(), std::forward<T>(key));
        stack::push(state(), std::forward<U>(value));
        lua_settable(state(), -3);
        lua_pop(state(), 1);
        return *this;
    }

    template<typename T, typename TFx>
    table& set_function(T&& key, TFx&& fx) {
        typedef typename std::remove_pointer<Decay<TFx>>::type clean_fx;
        return set_isfunction_fx(std::is_function<clean_fx>(), std::forward<T>(key), std::forward<TFx>(fx));
    }

    template<typename T, typename TFx, typename TObj>
    table& set_function(T&& key, TFx&& fx, TObj&& obj) {
        return set_lvalue_fx(Bool<std::is_lvalue_reference<TObj>::value || std::is_pointer<TObj>::value>(),
            std::forward<T>(key), std::forward<TFx>(fx), std::forward<TObj>(obj));
    }

    template<typename T>
    proxy<table&, T> operator[](T&& key) {
        return proxy<table&, T>(*this, std::forward<T>(key));
    }

    template<typename T>
    proxy<const table&, T> operator[](T&& key) const {
        return proxy<const table&, T>(*this, std::forward<T>(key));
    }

    size_t size() const {
        push();
        return lua_rawlen(state(), -1);
    }
private:
    template<typename Table, typename T>
    struct proxy {
    private:
        Table t;
        T& key;
    public:
        proxy(Table t, T& key) : t(t), key(key) {}

        template<typename U>
        EnableIf<Function<Unqualified<U>>> operator=(U&& other) {
            t.set_function(key, std::forward<U>(other));
        }
    
        template<typename U>
        DisableIf<Function<Unqualified<U>>> operator=(U&& other) {
            t.set(key, std::forward<U>(other));
        }

        template<typename U>
        operator U() const {
            return t.get<U>(key);
        }
    };

    template<typename T, typename TFx>
    table& set_isfunction_fx(std::true_type, T&& key, TFx&& fx) {
        return set_fx(std::false_type(), std::forward<T>(key), std::forward<TFx>(fx));
    }

    template<typename T, typename TFx>
    table& set_isfunction_fx(std::false_type, T&& key, TFx&& fx) {
        typedef Decay<TFx> clean_lambda;
        typedef typename function_traits<decltype(&clean_lambda::operator())>::free_function_pointer_type raw_func_t;
        typedef std::is_convertible<clean_lambda, raw_func_t> isconvertible;
        return set_isconvertible_fx(isconvertible(), std::forward<T>(key), std::forward<TFx>(fx));
    }

    template<typename T, typename TFx>
    table& set_isconvertible_fx(std::true_type, T&& key, TFx&& fx) {
        typedef Decay<TFx> clean_lambda;
        typedef typename function_traits<decltype(&clean_lambda::operator())>::free_function_pointer_type raw_func_t;
        return set_isfunction_fx(std::true_type(), std::forward<T>(key), raw_func_t(std::forward<TFx>(fx)));
    }

    template<typename T, typename TFx>
    table& set_isconvertible_fx(std::false_type, T&& key, TFx&& fx) {
        typedef typename std::remove_pointer<Decay<TFx>>::type clean_fx;
        std::unique_ptr<lua_func> sptr(new lambda_lua_func<clean_fx>(std::forward<TFx>(fx)));
        return set_fx(std::forward<T>(key), std::move(sptr));
    }

    template<typename T, typename TFx, typename TObj>
    table& set_lvalue_fx(std::true_type, T&& key, TFx&& fx, TObj&& obj) {
        return set_fx(std::true_type(), std::forward<T>(key), std::forward<TFx>(fx), std::forward<TObj>(obj));
    }

    template<typename T, typename TFx, typename TObj>
    table& set_lvalue_fx(std::false_type, T&& key, TFx&& fx, TObj&& obj) {
        typedef typename std::remove_pointer<Decay<TFx>>::type clean_fx;
        std::unique_ptr<lua_func> sptr(new explicit_lua_func<clean_fx, TObj>(std::forward<TObj>(obj), std::forward<TFx>(fx)));
        return set_fx(std::forward<T>(key), std::move(sptr));
    }

    template<typename T, typename TFx, typename TObj>
    table& set_fx(std::true_type, T&& key, TFx&& fx, TObj&& obj) {
        std::string fkey(key);

        // Layout: 
        // idx 1...n: verbatim data of member function pointer
        // idx n + 1: is the object's void pointer
        // We don't need to store the size, because the other side is templated
        // with the same member function pointer type
        Decay<TFx> fxptr(std::forward<TFx>(fx));
        void* userobjdata = static_cast<void*>(detail::get_ptr(obj));
        lua_CFunction freefunc = &static_object_lua_func<Decay<TObj>, TFx>::call;
        const char* freefuncname = fkey.c_str();
        const luaL_Reg funcreg[ 2 ] = {
            { freefuncname, freefunc },
            { nullptr, nullptr }
        };


        push();

        int upvalues = stack::push_user(state(), fxptr);
        stack::push(state(), userobjdata);
        luaL_setfuncs(state(), funcreg, upvalues + 1);

        lua_pop(state(), 1);
        return *this;
    }

    template<typename T, typename TFx>
    table& set_fx(std::false_type, T&& key, TFx&& fx) {
        std::string fkey(key);
        Decay<TFx> target(std::forward<TFx>(fx));
        lua_CFunction freefunc = &static_lua_func<TFx>::call;
        const char* freefuncname = fkey.c_str();
        const luaL_Reg funcreg[ 2 ] = {
            { freefuncname, freefunc },
            { nullptr, nullptr }
        };

        push();

        int upvalues = stack::push_user(state(), target);
        luaL_setfuncs(state(), funcreg, upvalues);

        lua_pop(state(), 1);
        return *this;
    }

    template<typename T>
    table& set_fx(T&& key, std::unique_ptr<lua_func> luafunc) {
        std::string fkey(key);
        std::string metakey("sol.stateful.");
        metakey += fkey;
        metakey += ".meta";
        lua_func* target = luafunc.release();
        void* userdata = reinterpret_cast<void*>(target);
        lua_CFunction freefunc = &lua_func::call;
        const char* freefuncname = fkey.c_str();
        const char* metatablename = metakey.c_str();
        const luaL_Reg funcreg[ 2 ] = {
            { freefuncname, freefunc },
            { nullptr, nullptr }
        };

        if (luaL_newmetatable(state(), metatablename) == 1) {
            lua_pushstring(state(), "__gc");
            lua_pushcclosure(state(), &lua_func::gc, 0);
            lua_settable(state(), -3);
        }

        push();
        stack::push_user(state(), userdata, metatablename);
        luaL_setfuncs(state(), funcreg, 1);
        lua_pop(state(), 1);
        return *this;
    }
};
} // sol

#endif // SOL_TABLE_HPP