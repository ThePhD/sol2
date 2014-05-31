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

#include "proxy.hpp"
#include "stack.hpp"
#include "function_types.hpp"
#include "userdata.hpp"

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
    friend class state;
    template<typename T, typename U>
    typename stack::get_return<T>::type single_get(U&& key) const {
        push();
        stack::push(state(), std::forward<U>(key));
        lua_gettable(state(), -2);
        type_assert(state(), -1, type_of<T>());
        auto&& result = stack::pop<T>(state());
        lua_pop(state(), 1);
        return result;
    }

    template<std::size_t I, typename Tup, typename... Ret>
    typename std::tuple_element<I, std::tuple<typename stack::get_return<Ret>::type...>>::type element_get(types<Ret...>, Tup&& key) const {
        typedef typename std::tuple_element<I, std::tuple<Ret...>>::type T;
        return single_get<T>(std::get<I>(key));
    }

    template<typename Tup, typename... Ret, std::size_t... I>
    typename return_type<typename stack::get_return<Ret>::type...>::type tuple_get(types<Ret...> t, indices<I...>, Tup&& tup) const {
       return std::make_tuple(element_get<I>(t, std::forward<Tup>(tup))...);
    }

    template<typename Tup, typename Ret>
    typename stack::get_return<Ret>::type tuple_get(types<Ret> t, indices<0>, Tup&& tup) const {
        return element_get<0>(t, std::forward<Tup>(tup));
    }

    template<typename... Ret, typename... Keys>
    typename return_type<typename stack::get_return<Ret>::type...>::type get(types<Ret...> t, Keys&&... keys) const {
        static_assert(sizeof...(Keys) == sizeof...(Ret), "Must have same number of keys as return values");
        return tuple_get(t, t, std::make_tuple(std::forward<Keys>(keys)...));
    }
public:
    table() noexcept : reference() {}
    table(lua_State* L, int index = -1) : reference(L, index) {
        type_assert(L, index, type::table);
    }

    template<typename... Ret, typename... Keys>
    typename return_type<typename stack::get_return<Ret>::type...>::type get(Keys&&... keys) const {
       return get(types<Ret...>(), std::forward<Keys>(keys)...);
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
    table& set_userdata(userdata<T>& user) {
        auto&& meta = userdata_traits<T>::metatable;
        luaL_newmetatable(state(), meta.c_str());
        for (std::size_t upvalues = 0; upvalues < user.functions.size(); ++upvalues) {
            stack::push(state(), static_cast<void*>(user.functions[ upvalues ].get()));
        }
        luaL_setfuncs(state(), user.functiontable.data(), static_cast<uint32_t>(user.functions.size()));

        lua_pushvalue(state(), -1);
        lua_setfield(state(), -1, "__index");

        lua_setglobal(state(), user.luaname.c_str());

        return *this;
    }

    size_t size() const {
        push();
        return lua_rawlen(state(), -1);
    }

    template<typename T>
    proxy<table, T> operator[](T&& key) {
        return proxy<table, T>(*this, std::forward<T>(key));
    }

    template<typename T>
    proxy<const table, T> operator[](T&& key) const {
        return proxy<const table, T>(*this, std::forward<T>(key));
    }

    void pop(int n = 1) const noexcept {
        lua_pop(state(), n);
    }
private:
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
        std::unique_ptr<base_function> sptr(new functor_function<clean_fx>(std::forward<TFx>(fx)));
        return set_fx(std::forward<T>(key), std::move(sptr));
    }

    template<typename T, typename TFx, typename TObj>
    table& set_lvalue_fx(std::true_type, T&& key, TFx&& fx, TObj&& obj) {
        return set_fx(std::true_type(), std::forward<T>(key), std::forward<TFx>(fx), std::forward<TObj>(obj));
    }

    template<typename T, typename TFx, typename TObj>
    table& set_lvalue_fx(std::false_type, T&& key, TFx&& fx, TObj&& obj) {
        typedef typename std::remove_pointer<Decay<TFx>>::type clean_fx;
        std::unique_ptr<base_function> sptr(new member_function<clean_fx, TObj>(std::forward<TObj>(obj), std::forward<TFx>(fx)));
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
        lua_CFunction freefunc = &static_member_function<Decay<TObj>, TFx>::call;
        const char* freefuncname = fkey.c_str();
        const luaL_Reg funcreg[2] = {
            { freefuncname, freefunc },
            { nullptr, nullptr }
        };


        push();

        int upvalues = stack::push_user(state(), fxptr);
        stack::push(state(), userobjdata);
        luaL_setfuncs(state(), funcreg, upvalues + 1);

        pop();
        return *this;
    }

    template<typename T, typename TFx>
    table& set_fx(std::false_type, T&& key, TFx&& fx) {
        std::string fkey(key);
        Decay<TFx> target(std::forward<TFx>(fx));
        lua_CFunction freefunc = &static_function<TFx>::call;
        const char* freefuncname = fkey.c_str();
        const luaL_Reg funcreg[2] = {
            { freefuncname, freefunc },
            { nullptr, nullptr }
        };

        push();
        int upvalues = stack::push_user(state(), target);
        luaL_setfuncs(state(), funcreg, upvalues);
        pop();

        return *this;
    }

    template<typename T>
    table& set_fx(T&& key, std::unique_ptr<base_function> luafunc) {
        std::string fkey(key);
        std::string metakey("sol.stateful.");
        metakey += fkey;
        metakey += ".meta";
        base_function* target = luafunc.release();
        void* userdata = reinterpret_cast<void*>(target);
        lua_CFunction freefunc = &base_function::call;
        const char* freefuncname = fkey.c_str();
        const char* metatablename = metakey.c_str();
        const luaL_Reg funcreg[2] = {
            { freefuncname, freefunc },
            { nullptr, nullptr }
        };

        if (luaL_newmetatable(state(), metatablename) == 1) {
            lua_pushstring(state(), "__gc");
            lua_pushcclosure(state(), &base_function::gc, 0);
            lua_settable(state(), -3);
        }

        push();
        stack::push_user(state(), userdata, metatablename);
        luaL_setfuncs(state(), funcreg, 1);
        pop();
        return *this;
    }
};
} // sol

#endif // SOL_TABLE_HPP
