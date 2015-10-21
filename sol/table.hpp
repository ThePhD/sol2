// The MIT License (MIT)

// Copyright (c) 2013-2015 Danny Y., Rapptz

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
#include "usertype.hpp"

namespace sol {
class table : public reference {
    friend class state;
    template<typename T, typename Key>
    stack::get_return<T> single_get(Key&& key) const {
        push();
        stack::push(state(), std::forward<Key>(key));
        lua_gettable(state(), -2);
        stack::get_return<T> result = stack::pop<T>(state());
        pop();
        return result;
    }

    template<typename Keys, typename... Ret, std::size_t... I>
    stack::get_return<Ret...> tuple_get(types<Ret...>, indices<I...>, Keys&& keys) const {
        return stack::get_return<Ret...>(single_get<Ret>(std::get<I>(keys))...);
    }

    template<typename Keys, typename Ret, std::size_t I>
    stack::get_return<Ret> tuple_get(types<Ret>, indices<I>, Keys&& keys) const {
        return single_get<Ret>(std::get<I>(keys));
    }

public:
    table() noexcept : reference() {}
    table(lua_State* L, int index = -1) : reference(L, index) {
        type_assert(L, index, type::table);
    }

    template<typename... Ret, typename... Keys>
    stack::get_return<Ret...> get( Keys&&... keys ) const {
        return tuple_get(types<Ret...>(), build_indices<sizeof...(Ret)>(), std::tie(keys...));
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
    SOL_DEPRECATED table& set_userdata(usertype<T>& user) {
        return set_usertype(user);
    }

    template<typename Key, typename T>
    SOL_DEPRECATED table& set_userdata(Key&& key, usertype<T>& user) {
        return set_usertype(std::forward<Key>(key), user);
    }

    template<typename T>
    table& set_usertype(usertype<T>& user) {
        return set_usertype(usertype_traits<T>::name, user);
    }

    template<typename Key, typename T>
    table& set_usertype(Key&& key, usertype<T>& user) {
        push();
        stack::push(state(), std::forward<Key>(key));
        stack::push(state(), user);
        lua_settable(state(), -3);
        lua_pop(state(), 1);
        return *this;
    }

    template<typename Fx>
    void for_each(Fx&& fx) const {
        push();
        stack::push(state(), nil);
        while (lua_next(this->state(), -2)) {
            sol::object key(state(), -2);
            sol::object value(state(), -1);
            fx(key, value);
            lua_pop(state(), 1);
        }
        pop();
    }

    size_t size() const {
        push();
        size_t result = lua_rawlen(state(), -1);
        pop();
        return result;
    }

    template<typename T>
    proxy<table, T> operator[]( T&& key ) {
        return proxy<table, T>( *this, std::forward<T>( key ) );
    }

    template<typename T>
    proxy<const table, T> operator[]( T&& key ) const {
        return proxy<const table, T>( *this, std::forward<T>( key ) );
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
