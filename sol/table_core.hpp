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

#ifndef SOL_TABLE_CORE_HPP
#define SOL_TABLE_CORE_HPP

#include "proxy.hpp"
#include "stack.hpp"
#include "function_types.hpp"
#include "usertype.hpp"

namespace sol {
namespace detail {
struct global_overload_tag { } const global_overload;
} // detail

template <bool top_level>
class table_core : public reference {
    friend class state;
    friend class state_view;

    template<typename T, typename Key, EnableIf<Bool<top_level>, is_c_str<Key>> = 0>
    decltype(auto) single_get( Key&& key ) const {
        lua_getglobal(lua_state( ), &key[0]);
        return stack::pop<T>(lua_state());
    }

    template<typename T, typename Key, EnableIf<Not<Bool<top_level>>, is_c_str<Key>> = 0>
    decltype(auto) single_get( Key&& key ) const {
        auto pp = stack::push_popper(*this);
        lua_getfield( lua_state( ), -1, &key[0] );
        return stack::pop<T>( lua_state( ) );
    }

    template<typename T, typename Key, EnableIf<Not<is_c_str<Key>>> = 0>
    decltype(auto) single_get( Key&& key ) const {
        auto pp = stack::push_popper(*this);
        stack::push( lua_state( ), std::forward<Key>( key ) );
        lua_gettable( lua_state( ), -2 );
        return stack::pop<T>( lua_state( ) );
    }

    template<typename Key, typename Value, EnableIf<Bool<top_level>, is_c_str<Key>> = 0>
    void single_set( Key&& key, Value&& value ) {
        stack::push( lua_state( ), std::forward<Value>( value ) );
        lua_setglobal( lua_state( ), &key[0] );
    }

    template<typename Key, typename Value, EnableIf<Not<Bool<top_level>>, is_c_str<Key>> = 0>
    void single_set(Key&& key, Value&& value) {
        push();
        stack::push(lua_state(), std::forward<Value>(value));
        lua_setfield(lua_state(), -2, &key[0]);
        pop();
    }

    template<typename Key, typename Value, EnableIf<Not<is_c_str<Key>>> = 0>
    void single_set(Key&& key, Value&& value) {
        push();
        stack::push(lua_state(), std::forward<Key>(key));
        stack::push(lua_state(), std::forward<Value>(value));
        lua_settable(lua_state(), -3);
        pop();
    }

    template<typename Keys, typename... Ret, std::size_t... I>
    std::tuple<decltype(stack::get<Ret>(nullptr, 0))...> tuple_get( types<Ret...>, std::index_sequence<I...>, Keys&& keys ) const {
        typedef std::tuple<decltype(single_get<Ret>(0))...> tup;
        return tup( single_get<Ret>( std::get<I>( keys ) )... );
    }

    template<typename Keys, typename Ret, std::size_t I>
    decltype(auto) tuple_get( types<Ret>, std::index_sequence<I>, Keys&& keys ) const {
        return single_get<Ret>( std::get<I>( keys ) );
    }

    template<typename Pairs, std::size_t... I>
    void tuple_set( std::index_sequence<I...>, Pairs&& pairs ) {
        using swallow = int[];
        swallow{ 0, ( single_set(std::get<I * 2>(pairs), std::get<I * 2 + 1>(pairs)) , 0)..., 0 };
    }

#if SOL_LUA_VERSION < 502
    table_core( detail::global_overload_tag, const table_core<false>& reg ) noexcept : reference( reg.lua_state(), LUA_GLOBALSINDEX ) { }
#else
    table_core( detail::global_overload_tag, const table& reg ) noexcept : reference( reg.get<table>( LUA_RIDX_GLOBALS ) ) { }
#endif
public:
    table_core( ) noexcept : reference( ) { }
    table_core( const table_core<true>& global ) : reference( global ) { }
    table_core( lua_State* L, int index = -1 ) : reference( L, index ) {
        type_assert( L, index, type::table );
    }

    template<typename... Ret, typename... Keys>
    decltype(auto) get( Keys&&... keys ) const {
        return tuple_get( types<Ret...>( ), std::index_sequence_for<Ret...>( ), std::forward_as_tuple(std::forward<Keys>(keys)...));
    }

    template<typename... Args>
    table_core& set( Args&&... args ) {
        tuple_set(std::make_index_sequence<sizeof...(Args) / 2>(), std::forward_as_tuple(std::forward<Args>(args)...));
        return *this;
    }

    template<typename T>
    SOL_DEPRECATED table_core& set_userdata( usertype<T>& user ) {
        return set_usertype( user );
    }

    template<typename Key, typename T>
    SOL_DEPRECATED table_core& set_userdata( Key&& key, usertype<T>& user ) {
        return set_usertype(std::forward<Key>(key), user);
    }

    template<typename T>
    table_core& set_usertype( usertype<T>& user ) {
        return set_usertype(usertype_traits<T>::name, user);
    }

    template<typename Key, typename T>
    table_core& set_usertype( Key&& key, usertype<T>& user ) {
        return set(std::forward<Key>(key), user);
    }

    template<typename Fx>
    void for_each( Fx&& fx ) const {
        auto pp = stack::push_popper( *this );
        stack::push( lua_state( ), nil );
        while ( lua_next( this->lua_state( ), -2 ) ) {
            sol::object key( lua_state( ), -2 );
            sol::object value( lua_state( ), -1 );
            fx( key, value );
            lua_pop( lua_state( ), 1 );
        }
    }

    size_t size( ) const {
        auto pp = stack::push_popper( *this );
        return lua_rawlen(lua_state(), -1);
    }

    template<typename T>
    proxy<table_core&, T> operator[]( T&& key ) & {
        return proxy<table_core&, T>( *this, std::forward<T>( key ) );
    }

    template<typename T>
    proxy<const table_core&, T> operator[]( T&& key ) const & {
        return proxy<const table_core&, T>( *this, std::forward<T>( key ) );
    }

    template<typename T>
    proxy<table_core, T> operator[]( T&& key ) && {
        return proxy<table_core, T>( *this, std::forward<T>( key ) );
    }

    void pop( int n = 1 ) const noexcept {
        lua_pop( lua_state( ), n );
    }

    template<typename... Args, typename R, typename Key>
    table_core& set_function( Key&& key, R fun_ptr( Args... ) ) {
        set_resolved_function( std::forward<Key>( key ), fun_ptr );
        return *this;
    }

    template<typename Sig, typename Key>
    table_core& set_function( Key&& key, Sig* fun_ptr ) {
        set_resolved_function( std::forward<Key>( key ), fun_ptr );
        return *this;
    }

    template<typename... Args, typename R, typename C, typename T, typename Key>
    table_core& set_function( Key&& key, R( C::*mem_ptr )( Args... ), T&& obj ) {
        set_resolved_function( std::forward<Key>( key ), mem_ptr, std::forward<T>( obj ) );
        return *this;
    }

    template<typename Sig, typename C, typename T, typename Key>
    table_core& set_function( Key&& key, Sig C::* mem_ptr, T&& obj ) {
        set_resolved_function( std::forward<Key>( key ), mem_ptr, std::forward<T>( obj ) );
        return *this;
    }

    template<typename... Sig, typename Fx, typename Key>
    table_core& set_function( Key&& key, Fx&& fx ) {
        set_fx( types<Sig...>( ), std::forward<Key>( key ), std::forward<Fx>( fx ) );
        return *this;
    }

private:
    template<typename R, typename... Args, typename Fx, typename Key, typename = std::result_of_t<Fx( Args... )>>
    void set_fx( types<R( Args... )>, Key&& key, Fx&& fx ) {
        set_resolved_function<R( Args... )>( std::forward<Key>( key ), std::forward<Fx>( fx ) );
    }

    template<typename Fx, typename Key, EnableIf<is_specialization_of<Unqualified<Fx>, overload_set>> = 0>
    void set_fx( types<>, Key&& key, Fx&& fx ) {
        set(std::forward<Key>(key), std::forward<Fx>(fx));
    }

    template<typename Fx, typename Key, DisableIf<is_specialization_of<Unqualified<Fx>, overload_set>> = 0>
    void set_fx( types<>, Key&& key, Fx&& fx ) {
        typedef Unwrapped<Unqualified<Fx>> fx_t;
        typedef decltype( &fx_t::operator() ) Sig;
        set_fx( types<function_signature_t<Sig>>( ), std::forward<Key>( key ), std::forward<Fx>( fx ) );
    }

    template<typename... Sig, typename... Args, typename Key>
    void set_resolved_function( Key&& key, Args&&... args ) {
        set(std::forward<Key>(key), detail::function_pack<function_sig<Sig...>>(std::forward<Args>(args)...));
    }
};
} // sol

#endif // SOL_TABLE_CORE_HPP
