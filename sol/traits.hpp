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

#ifndef SOL_TRAITS_HPP
#define SOL_TRAITS_HPP

#include <type_traits>

namespace sol {
template<typename T, typename R = void>
using EnableIf = typename std::enable_if<T::value, R>::type;

template<typename T, typename R = void>
using DisableIf = typename std::enable_if<!T::value, R>::type;

template<typename T>
using Unqualified = typename std::remove_reference<typename std::remove_cv<T>::type>::type;

template<typename T>
using Decay = typename std::decay<T>::type;

namespace detail {
// code borrowed from Gears 
// https://github.com/Rapptz/Gears/
template<typename T, bool isclass = std::is_class<Unqualified<T>>::value>
struct is_function_impl : std::is_function<typename std::remove_pointer<T>::type> {};

template<typename T>
struct is_function_impl<T, true> {
    using yes = char;
    using no = struct { char s[2]; };

    struct F { void operator()(); };
    struct Derived : T, F { };
    template<typename U, U> struct Check;

    template<typename V>
    static no test(Check<void (F::*)(), &V::operator()>*);

    template<typename>
    static yes test(...);

    static const bool value = sizeof(test<Derived>(0)) == sizeof(yes);
};
} // detail

template<bool B>
using Bool = std::integral_constant<bool, B>;

template<typename T>
struct Function : Bool<detail::is_function_impl<T>::value> {};

template<typename TFuncSignature>
struct function_traits;

template<typename T, typename R, typename... Args>
struct function_traits<R(T::*)(Args...)> {
	static const std::size_t arity = sizeof...(Args);
	static const bool is_member_function = true;
	typedef std::tuple<Args...> arg_tuple_type;
	typedef types<Args...> args_type;
	typedef R(T::* function_pointer_type)(Args...);
	typedef typename std::remove_pointer<function_pointer_type>::type function_type;
	typedef R(*free_function_pointer_type)(Args...);
	typedef R return_type;
	template<std::size_t i>
	using arg = typename std::tuple_element<i, arg_tuple_type>::type;
};

template<typename T, typename R, typename... Args>
struct function_traits<R(T::*)(Args...) const> {
	static const std::size_t arity = sizeof...(Args);
	static const bool is_member_function = true;
	typedef std::tuple<Args...> arg_tuple_type;
	typedef types<Args...> args_type;
	typedef R(T::* function_pointer_type)(Args...);
	typedef typename std::remove_pointer<function_pointer_type>::type function_type;
	typedef R(*free_function_pointer_type)(Args...);
	typedef R return_type;
	template<std::size_t i>
	using arg = typename std::tuple_element<i, arg_tuple_type>::type;
};

template<typename R, typename... Args>
struct function_traits<R(Args...)> {
	static const std::size_t arity = sizeof...(Args);
	static const bool is_member_function = false;
	typedef std::tuple<Args...> arg_tuple_type;
	typedef types<Args...> args_type;
	typedef R(function_type)(Args...);
	typedef R(*function_pointer_type)(Args...);
	typedef R(*free_function_pointer_type)(Args...);
	typedef R return_type;
	template<std::size_t i>
	using arg = typename std::tuple_element<i, arg_tuple_type>::type;
};

template<typename R, typename... Args>
struct function_traits<R(*)(Args...)> {
	static const std::size_t arity = sizeof...(Args);
	static const bool is_member_function = false;
	typedef std::tuple<Args...> arg_tuple_type;
	typedef types<Args...> args_type;
	typedef R(function_type)(Args...);
	typedef R(*function_pointer_type)(Args...);
	typedef R(*free_function_pointer_type)(Args...);
	typedef R return_type;
	template<std::size_t i>
	using arg = typename std::tuple_element<i, arg_tuple_type>::type;
};
} // sol

#endif // SOL_TRAITS_HPP