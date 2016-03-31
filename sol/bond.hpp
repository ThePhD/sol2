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

#ifndef SOL_BOND_HPP
#define SOL_BOND_HPP

#include "traits.hpp"

namespace sol {

template <typename T>
struct bond_size : std::tuple_size<T> {};

template <typename T>
struct is_bondable : std::integral_constant<bool, (::sol::bond_size<T>::value > 0)> {};

template <typename... Tn>
struct bond_t : public std::tuple<std::add_lvalue_reference_t<Tn>...> {
private:
     typedef std::tuple<std::add_lvalue_reference_t<Tn>...> base_t;

     template <typename T>
     void set( std::false_type, T&& target ) {
          std::get<0>( *this ) = std::forward<T>( target );
     }

     template <typename T>
     void set( std::true_type, T&& target ) {
          typedef bond_size<meta::Unqualified<T>> value_size;
          typedef bond_size<std::tuple<Tn...>> tie_size;
          typedef std::conditional_t<(value_size::value < tie_size::value), value_size, tie_size> indices_size;
          typedef std::make_index_sequence<indices_size::value> indices;
          set( indices(), std::forward<T>( target ) );
     }

     template <std::size_t... I, typename T>
     void set( std::index_sequence<I...>, T&& target ) {
          using std::get;
          (void)detail::swallow{ 0, 
               ( get<I>(*this) = get<I>(target), 0 )...
          , 0 };
     }

public:
     using base_t::base_t;

     template <typename T>
     bond_t& operator= ( T&& value ) {
          typedef is_bondable<meta::Unqualified<T>> bondable;
          set( bondable(), std::forward<T>( value ) );
          return *this;
     }

};

template <typename... Tn>
struct bond_size<::sol::bond_t<Tn...>> : ::std::tuple_size<::std::tuple<Tn...>> { };

template <typename... Tn>
inline bond_t<std::remove_reference_t<Tn>...> bond( Tn&&... argn ) {
     return bond_t<std::remove_reference_t<Tn>...>( std::forward<Tn>( argn )... );
}

} // sol

#endif // SOL_BOND_HPP
