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

#include "tuple.hpp"

#ifndef SOL_FUNCTIONAL_HPP
#define SOL_FUNCTIONAL_HPP

namespace sol {
namespace detail {

template<typename TFuncSignature>
struct function_traits;

template<typename T, typename R, typename... Tn>
struct function_traits<R( T::* )( Tn... )> {
	static const std::size_t arity = sizeof...( Tn );
	static const bool is_member_function = true;
	typedef std::tuple<Tn...> arg_tuple_t;
	typedef types<Tn...> args_t;
	typedef R( T::* func_t )( Tn... );
	typedef R( T::* func_pointer_t )( Tn... );
	typedef R return_t;
	template <std::size_t i>
	using arg_n = std::tuple_element<i, arg_tuple_t>;
};

template<typename T, typename R, typename... Tn>
struct function_traits<R( T::* )( Tn... ) const> {
	static const std::size_t arity = sizeof...( Tn );
	static const bool is_member_function = true;
	typedef std::tuple<Tn...> arg_tuple_t;
	typedef types<Tn...> args_t;
	typedef R( T::* func_t )( Tn... );
	typedef R( T::* func_pointer_t )( Tn... );
	typedef R return_t;
	template <std::size_t i>
	using arg_n = std::tuple_element<i, arg_tuple_t>;
};

template<typename R, typename... Tn>
struct function_traits<R( Tn... )> {
	static const std::size_t arity = sizeof...( Tn );
	static const bool is_member_function = false;
	typedef std::tuple<Tn...> arg_tuple_t;
	typedef types<Tn...> args_t;
	typedef R( func_t )( Tn... );
	typedef R( *func_pointer_t )( Tn... );
	typedef R return_t;
	template <std::size_t i>
	using arg_n = std::tuple_element<i, arg_tuple_t>;
};

template<typename R, typename... Tn>
struct function_traits<R( *)( Tn... )> {
	static const std::size_t arity = sizeof...( Tn );
	static const bool is_member_function = false;
	typedef std::tuple<Tn...> arg_tuple_t;
	typedef types<Tn...> args_t;
	typedef R( func_t )( Tn... );
	typedef R( *func_pointer_t )( Tn... );
	typedef R return_t;
	template <std::size_t i>
	using arg_n = std::tuple_element<i, arg_tuple_t>;
};

using std::get;

template<typename Function, typename Tuple, size_t... Indices>
inline auto call( Function f, const Tuple& t, indices<Indices...> ) -> decltype( f( get<Indices>( t )... ) ) {
	return f( get<Indices>( t )... );
}

} // detail

template<typename Function, typename... Args>
inline auto call( Function f, const std::tuple<Args...>& t ) -> decltype( detail::call( f, t, detail::build_indices<sizeof...( Args )>{} ) ) {
	return call( f, t, detail::build_indices<sizeof...( Args )>{} );
}

} // sol


#endif // SOL_FUNCTIONAL_HPP