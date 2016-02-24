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
template <bool top_level>
class table_core : public reference {
    friend class state;
    friend class state_view;

    template <typename... Args>
    using is_global = meta::And<meta::Bool<top_level>, meta::is_c_str<Args>...>;

    template<typename... Ret, std::size_t... I, typename Keys>
    auto tuple_get( types<Ret...>, std::index_sequence<I...>, Keys&& keys ) const
    -> decltype(stack::pop<std::tuple<Ret...>>(nullptr)){
        auto pp = stack::push_popper<is_global<decltype(std::get<I>(keys))...>::value>(*this);
        int tableindex = lua_gettop(lua_state());
	   void(detail::swallow{ ( stack::get_field<top_level>(lua_state(), std::get<I>(keys), tableindex), 0)... });
        return stack::pop<std::tuple<Ret...>>( lua_state() );
    }

    template<typename Ret, std::size_t I, typename Keys>
    decltype(auto) tuple_get( types<Ret>, std::index_sequence<I>, Keys&& keys ) const {
        auto pp = stack::push_popper<is_global<decltype(std::get<I>(keys))>::value>(*this);
        stack::get_field<top_level>( lua_state( ), std::get<I>( keys ) );
        return stack::pop<Ret>( lua_state( ) );
    }

    template<typename Pairs, std::size_t... I>
    void tuple_set( std::index_sequence<I...>, Pairs&& pairs ) {
        auto pp = stack::push_popper<is_global<decltype(std::get<I * 2>(pairs))...>::value>(*this);
	   void(detail::swallow{ (stack::set_field<top_level>(lua_state(), std::get<I * 2>(pairs), std::get<I * 2 + 1>(pairs)), 0)... });
    }

    template <bool global, typename T, typename Key>
    decltype(auto) traverse_get_deep( Key&& key ) const {
        stack::get_field<global>( lua_state( ), std::forward<Key>( key ) );
        return stack::get<T>( lua_state( ) );
    }

    template <bool global, typename T, typename Key, typename... Keys>
    decltype(auto) traverse_get_deep( Key&& key, Keys&&... keys ) const {
        stack::get_field<global>( lua_state( ), std::forward<Key>( key ) );
        return traverse_get_deep<false, T>(std::forward<Keys>(keys)...);
    }

    template <bool global, typename Key, typename Value>
    void traverse_set_deep( Key&& key, Value&& value ) const {
        stack::set_field<global>( lua_state( ), std::forward<Key>( key ), std::forward<Value>(value) );
    }

    template <bool global, typename Key, typename... Keys>
    void traverse_set_deep( Key&& key, Keys&&... keys ) const {
        stack::get_field<global>( lua_state( ), std::forward<Key>( key ) );
        traverse_set_deep<false>(std::forward<Keys>(keys)...);
    }

    table_core(lua_State* L, detail::global_tag t) noexcept : reference(L, t) { }

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

    template <typename T, typename... Keys>
    decltype(auto) traverse_get( Keys&&... keys ) const {
        auto pp = stack::push_popper<is_global<Keys...>::value>(*this);
        struct clean { lua_State* L; clean(lua_State* L) : L(L) {} ~clean() { lua_pop(L, static_cast<int>(sizeof...(Keys))); } } c(lua_state());
        return traverse_get_deep<top_level, T>(std::forward<Keys>(keys)...);
    }

    template <typename... Keys>
    table_core& traverse_set( Keys&&... keys ) {
        auto pp = stack::push_popper<is_global<Keys...>::value>(*this);
        traverse_set_deep<top_level>(std::forward<Keys>(keys)...);
        lua_pop(lua_state(), static_cast<int>(sizeof...(Keys)-2));
        return *this;
    }

    template<typename... Args>
    table_core& set( Args&&... args ) {
        tuple_set(std::make_index_sequence<sizeof...(Args) / 2>(), std::forward_as_tuple(std::forward<Args>(args)...));
        return *this;
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

    template<typename Fx, typename Key, meta::EnableIf<meta::is_specialization_of<meta::Unqualified<Fx>, overload_set>> = 0>
    void set_fx( types<>, Key&& key, Fx&& fx ) {
        set(std::forward<Key>(key), std::forward<Fx>(fx));
    }

    template<typename Fx, typename Key, meta::DisableIf<meta::is_specialization_of<meta::Unqualified<Fx>, overload_set>> = 0>
    void set_fx( types<>, Key&& key, Fx&& fx ) {
        typedef meta::Unwrapped<meta::Unqualified<Fx>> fx_t;
        typedef decltype( &fx_t::operator() ) Sig;
        set_fx( types<meta::function_signature_t<Sig>>( ), std::forward<Key>( key ), std::forward<Fx>( fx ) );
    }

    template<typename... Sig, typename... Args, typename Key>
    void set_resolved_function( Key&& key, Args&&... args ) {
        set(std::forward<Key>(key), function_pack<function_sig<Sig...>>(std::forward<Args>(args)...));
    }
};
} // sol

#endif // SOL_TABLE_CORE_HPP
