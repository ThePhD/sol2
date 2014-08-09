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

    template<typename T>
    table& set_userdata(userdata<T>& user) {
        return set_userdata(user.name(), user);
    }

    template<typename Key, typename T>
    table& set_userdata(Key&& key, userdata<T>& user) {
        push();
        stack::push(state(), std::forward<Key>(key));
        stack::push(state(), user);
        lua_settable(state(), -3);
        lua_pop(state(), 1);
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

    template<typename... Args, typename R, typename Key>
    table& set_function(Key&& key, R fun_ptr(Args...)){
        set_resolved_function(std::forward<Key>(key), fun_ptr);
        return *this;
    }

    template<typename Sig, typename Key>
    table& set_function(Key&& key, Sig* fun_ptr){
        set_resolved_function(std::forward<Key>(key), fun_ptr);
        return *this;
    }

    template<typename... Args, typename R, typename C, typename T, typename Key>
    table& set_function(Key&& key, R (C::*mem_ptr)(Args...), T&& obj) {
        set_resolved_function(std::forward<Key>(key), mem_ptr, std::forward<T>(obj));
        return *this;
    }

    template<typename Sig, typename C, typename T, typename Key>
    table& set_function(Key&& key, Sig C::* mem_ptr, T&& obj) {
        set_resolved_function(std::forward<Key>(key), mem_ptr, std::forward<T>(obj));
        return *this;
    }

    template<typename... Sig, typename Fx, typename Key>
    table& set_function(Key&& key, Fx&& fx) {
        set_fx(types<Sig...>(), std::forward<Key>(key), std::forward<Fx>(fx));
        return *this;
    }

private:
    template<typename R, typename... Args, typename Fx, typename Key, typename = typename std::result_of<Fx(Args...)>::type>
    void set_fx(types<R(Args...)>, Key&& key, Fx&& fx) {
        set_resolved_function<R(Args...)>(std::forward<Key>(key), std::forward<Fx>(fx));
    }

    template<typename... Args, typename Fx, typename Key, typename R = typename std::result_of<Fx(Args...)>::type>
    void set_fx(types<Args...>, Key&& key, Fx&& fx){
        set_fx(types<R(Args...)>(), std::forward<Key>(key), std::forward<Fx>(fx));
    }

    template<typename Fx, typename Key>
    void set_fx(types<>, Key&& key, Fx&& fx) {
        typedef Unqualified<Unwrap<Fx>> fx_t;
        typedef decltype(&fx_t::operator()) Sig;
        set_fx(types<function_signature_t<Sig>>(), std::forward<Key>(key), std::forward<Fx>(fx));
    }

    template<typename... Sig, typename... Args, typename Key>
    void set_resolved_function(Key&& key, Args&&... args) {
        std::string fkey(std::forward<Key>(key));
        push();
        int tabletarget = lua_gettop(state());
        stack::push<function_sig_t<Sig...>>(state(), std::forward<Args>(args)...);
        lua_setfield(state(), tabletarget, fkey.c_str());
        pop();
    }
};
} // sol

#endif // SOL_TABLE_HPP
