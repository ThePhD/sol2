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

#include "traits.hpp"
#include "object.hpp"
#include "function.hpp"

namespace sol {
template <typename Table, typename Key>
struct proxy {
private:
	Table& tbl;
	Key& key;

public:

	template <typename T>
	proxy( Table& table, T&& key ) : tbl( table ), key( std::forward<T>( key ) ) {

	}

	template<typename T>
	T get( ) const {
		return tbl.get<T>( key );
	}

	template<typename T>
	proxy& set( T&& item ) {
		tbl.set( key, std::forward<T>( item ) );
		return *this;
	}

	template<typename... Args>
	proxy& set_function( Args&&... args ) {
		tbl.set_function( key, std::forward<Args>( args )... );
		return *this;
	}

	template<typename U>
	EnableIf<Function<Unqualified<U>>> operator=( U&& other ) {
		tbl.set_function( key, std::forward<U>( other ) );
	}

	template<typename U>
	DisableIf<Function<Unqualified<U>>> operator=( U&& other ) {
		tbl.set( key, std::forward<U>( other ) );
	}

	operator nil_t ( ) {
		return get<nil_t>( );
	}

	operator bool( ) {
		return get<bool>( );
	}

	operator std::string( ) {
		return get<std::string>( );
	}

	operator object( ) {
		return get<object>( );
	}

	operator function( ) {
		return get<function>( );
	}

	operator double( ) {
		return get<double>( );
	}

	operator float( ) {
		return get<float>( );
	}

	operator int( ) {
		return get<int>( );
	}

	template<typename... Ret, typename... Args>
	typename multi_return<Ret...>::type call( Args&&... args ) {
		return tbl.get<function>(key)(types<Ret...>(), std::forward<Args>( args )...);
	}
};

template <typename Table, typename Key, typename T>
bool operator== ( T&& left, const proxy<Table, Key>& right ) {
	return right.get<Decay<T>>( ) == left;
}

template <typename Table, typename Key, typename T>
bool operator== ( const proxy<Table, Key>& right, T&& left ) {
	return right.get<Decay<T>>( ) == left;
}

template <typename Table, typename Key, typename T>
bool operator!= ( T&& left, const proxy<Table, Key>& right ) {
	return right.get<Decay<T>>( ) != left;
}

template <typename Table, typename Key, typename T>
bool operator!= ( const proxy<Table, Key>& right, T&& left ) {
	return right.get<Decay<T>>( ) != left;
}

} // sol