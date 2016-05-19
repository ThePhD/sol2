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

#ifndef SOL_TRAITS_HPP
#define SOL_TRAITS_HPP

#include "tuple.hpp"
#include <type_traits>
#include <memory>
#include <functional>

namespace sol {
template<std::size_t I>
using index_value = std::integral_constant<std::size_t, I>;

namespace meta {
template<typename T>
struct identity { typedef T type; };

template<typename T>
using identity_t = typename identity<T>::type;

template<typename... Args>
struct is_tuple : std::false_type{ };

template<typename... Args>
struct is_tuple<std::tuple<Args...>> : std::true_type{ };

template<typename T>
struct unwrapped {
    typedef T type;
};

template<typename T>
struct unwrapped<std::reference_wrapper<T>> {
    typedef T type;
};

template<typename T>
struct remove_member_pointer;

template<typename R, typename T>
struct remove_member_pointer<R T::*> {
    typedef R type;
};

template<typename R, typename T>
struct remove_member_pointer<R T::* const> {
    typedef R type;
};

template<typename T>
using remove_member_pointer_t = remove_member_pointer<T>;

template<template<typename...> class Templ, typename T>
struct is_specialization_of : std::false_type { };
template<typename... T, template<typename...> class Templ>
struct is_specialization_of<Templ, Templ<T...>> : std::true_type { };

template<class T, class...>
struct are_same : std::true_type { };

template<class T, class U, class... Args>
struct are_same<T, U, Args...> : std::integral_constant <bool, std::is_same<T, U>::value && are_same<T, Args...>::value> { };

template<typename T>
using invoke_t = typename T::type;

template<bool B>
using boolean = std::integral_constant<bool, B>;

template<typename T>
using neg = boolean<!T::value>;

template<typename Condition, typename Then, typename Else>
using condition = std::conditional_t<Condition::value, Then, Else>;

template<typename... Args>
struct all : boolean<true> {};

template<typename T, typename... Args>
struct all<T, Args...> : condition<T, all<Args...>, boolean<false>> {};

template<typename... Args>
struct any : boolean<false> {};

template<typename T, typename... Args>
struct any<T, Args...> : condition<T, boolean<true>, any<Args...>> {};

enum class enable_t {
	_
};

constexpr const auto enabler = enable_t::_;

template<typename... Args>
using enable = std::enable_if_t<all<Args...>::value, enable_t>;

template<typename... Args>
using disable = std::enable_if_t<neg<all<Args...>>::value, enable_t>;

template<typename T>
using unqualified = std::remove_cv<std::remove_reference_t<T>>;

template<typename T>
using unqualified_t = typename unqualified<T>::type;

template<typename T>
using unwrapped_t = typename unwrapped<T>::type;

template <std::size_t N, typename Tuple>
using tuple_element = std::tuple_element<N, unqualified_t<Tuple>>;

template <std::size_t N, typename Tuple>
using tuple_element_t = std::tuple_element_t<N, unqualified_t<Tuple>>;

template<typename V, typename... Vs>
struct find_in_pack_v : boolean<false> { };

template<typename V, typename Vs1, typename... Vs>
struct find_in_pack_v<V, Vs1, Vs...> : any<boolean<(V::value == Vs1::value)>, find_in_pack_v<V, Vs...>> { };

namespace meta_detail {
    template<std::size_t I, typename T, typename... Args>
    struct index_in_pack : std::integral_constant<std::size_t, SIZE_MAX> { };

    template<std::size_t I, typename T, typename T1, typename... Args>
    struct index_in_pack<I, T, T1, Args...> : std::conditional_t<std::is_same<T, T1>::value, std::integral_constant<std::ptrdiff_t, I>, index_in_pack<I + 1, T, Args...>> { };
}

template<typename T, typename... Args>
struct index_in_pack : meta_detail::index_in_pack<0, T, Args...> { };

template<typename T, typename List>
struct index_in : meta_detail::index_in_pack<0, T, List> { };

template<typename T, typename... Args>
struct index_in<T, types<Args...>> : meta_detail::index_in_pack<0, T, Args...> { };

template<std::size_t I, typename... Args>
struct at_in_pack {};

template<std::size_t I, typename... Args>
using at_in_pack_t = typename at_in_pack<I, Args...>::type;

template<std::size_t I, typename Arg, typename... Args>
struct at_in_pack<I, Arg, Args...> : std::conditional<I == 0, Arg, at_in_pack_t<I - 1, Args...>> {};

namespace meta_detail {
    template<std::size_t Limit, std::size_t I, template<typename...> class Pred, typename... Ts>
    struct count_for_pack : std::integral_constant<std::size_t, 0> {};
    template<std::size_t Limit, std::size_t I, template<typename...> class Pred, typename T, typename... Ts>
    struct count_for_pack<Limit, I, Pred, T, Ts...> : std::conditional_t<sizeof...(Ts) == 0 || Limit < 2, 
        std::integral_constant<std::size_t, I + static_cast<std::size_t>(Limit != 0 && Pred<T>::value)>,
        count_for_pack<Limit - 1, I + static_cast<std::size_t>(Pred<T>::value), Pred, Ts...>
    > { };
    template<std::size_t I, template<typename...> class Pred, typename... Ts>
    struct count_2_for_pack : std::integral_constant<std::size_t, 0> {};
    template<std::size_t I, template<typename...> class Pred, typename T, typename U, typename... Ts>
    struct count_2_for_pack<I, Pred, T, U, Ts...> : std::conditional_t<sizeof...(Ts) == 0, 
        std::integral_constant<std::size_t, I + static_cast<std::size_t>(Pred<T>::value)>, 
        count_2_for_pack<I + static_cast<std::size_t>(Pred<T>::value), Pred, Ts...>
    > { };
} // meta_detail

template<template<typename...> class Pred, typename... Ts>
struct count_for_pack : meta_detail::count_for_pack<sizeof...(Ts), 0, Pred, Ts...> { };

template<template<typename...> class Pred, typename List>
struct count_for;

template<template<typename...> class Pred, typename... Args>
struct count_for<Pred, types<Args...>> : count_for_pack<Pred, Args...> {};

template<std::size_t Limit, template<typename...> class Pred, typename... Ts>
struct count_for_to_pack : meta_detail::count_for_pack<Limit, 0, Pred, Ts...> { };

template<template<typename...> class Pred, typename... Ts>
struct count_2_for_pack : meta_detail::count_2_for_pack<0, Pred, Ts...> { };

template<typename... Args>
struct return_type {
    typedef std::tuple<Args...> type;
};

template<typename T>
struct return_type<T> {
    typedef T type;
};

template<>
struct return_type<> {
    typedef void type;
};

template <typename... Args>
using return_type_t = typename return_type<Args...>::type;

namespace meta_detail {
template <typename> struct always_true : std::true_type {};
struct is_invokable_tester {
    template <typename Fun, typename... Args>
    always_true<decltype(std::declval<Fun>()(std::declval<Args>()...))> static test(int);
    template <typename...>
    std::false_type static test(...);
};
} // meta_detail

template <typename T>
struct is_invokable;
template <typename Fun, typename... Args>
struct is_invokable<Fun(Args...)> : decltype(meta_detail::is_invokable_tester::test<Fun, Args...>(0)) {};

namespace meta_detail {

template<typename T, bool isclass = std::is_class<unqualified_t<T>>::value>
struct is_callable : std::is_function<std::remove_pointer_t<T>> {};

template<typename T>
struct is_callable<T, true> {
    using yes = char;
    using no = struct { char s[2]; };

    struct F { void operator()(); };
    struct Derived : T, F {};
    template<typename U, U> struct Check;

    template<typename V>
    static no test(Check<void (F::*)(), &V::operator()>*);

    template<typename>
    static yes test(...);

    static const bool value = sizeof(test<Derived>(0)) == sizeof(yes);
};

template<class F>
struct check_deducible_signature {
    struct nat {};
    template<class G>
    static auto test(int) -> decltype(&G::operator(), void());
    template<class>
    static auto test(...) -> nat;

    using type = std::is_void<decltype(test<F>(0))>;
};
} // meta_detail

template<class F>
struct has_deducible_signature : meta_detail::check_deducible_signature<F>::type { };

template<typename T>
struct is_callable : boolean<meta_detail::is_callable<T>::value> {};

namespace meta_detail {

template <std::size_t I, typename T>
struct void_tuple_element : meta::tuple_element<I, T> {};

template <std::size_t I>
struct void_tuple_element<I, std::tuple<>> { typedef void type; };

template <std::size_t I, typename T>
using void_tuple_element_t = typename void_tuple_element<I, T>::type;

template<typename Signature, bool b = has_deducible_signature<Signature>::value>
struct fx_traits {
    static const bool is_member_function = false;
    static const std::size_t arity = 0;
    typedef types<> args_type;
    typedef std::tuple<> args_tuple_type;
    typedef void object_type;
    typedef void function_pointer_type;
    typedef void function_type;
    typedef void free_function_pointer_type;
    typedef void signature_type;
    typedef void return_type;
    template<std::size_t i>
    using arg_at = void_tuple_element_t<i, args_tuple_type>;
};

template<typename Signature>
struct fx_traits<Signature, true> : fx_traits<decltype(&Signature::operator()), false> {};

template<typename T, typename R, typename... Args>
struct fx_traits<R(T::*)(Args...), false> {
    static const std::size_t arity = sizeof...(Args);
    static const bool is_member_function = true;
    typedef T object_type;
    typedef std::tuple<Args...> args_tuple_type;
    typedef types<Args...> args_type;
    typedef R(T::* function_pointer_type)(Args...);
    typedef std::remove_pointer_t<function_pointer_type> function_type;
    typedef R(*free_function_pointer_type)(Args...);
    typedef R return_type;
    typedef std::remove_pointer_t<free_function_pointer_type> signature_type;
    template<std::size_t i>
    using arg_at = void_tuple_element_t<i, args_tuple_type>;
};

template<typename T, typename R, typename... Args>
struct fx_traits<R(T::*)(Args...) const, false> {
    static const std::size_t arity = sizeof...(Args);
    static const bool is_member_function = true;
    typedef T object_type;
    typedef std::tuple<Args...> args_tuple_type;
    typedef types<Args...> args_type;
    typedef R(T::* function_pointer_type)(Args...);
    typedef std::remove_pointer_t<function_pointer_type> function_type;
    typedef R(*free_function_pointer_type)(Args...);
    typedef R return_type;
    typedef std::remove_pointer_t<free_function_pointer_type> signature_type;
    template<std::size_t i>
    using arg_at = void_tuple_element_t<i, args_tuple_type>;
};

template<typename R, typename... Args>
struct fx_traits<R(Args...), false> {
    static const std::size_t arity = sizeof...(Args);
    static const bool is_member_function = false;
    typedef std::tuple<Args...> args_tuple_type;
    typedef types<Args...> args_type;
    typedef R(function_type)(Args...);
    typedef R(*function_pointer_type)(Args...);
    typedef R(*free_function_pointer_type)(Args...);
    typedef R return_type;
    typedef std::remove_pointer_t<free_function_pointer_type> signature_type;
    template<std::size_t i>
    using arg_at = void_tuple_element_t<i, args_tuple_type>;
};

template<typename R, typename... Args>
struct fx_traits<R(*)(Args...), false> {
    static const std::size_t arity = sizeof...(Args);
    static const bool is_member_function = false;
    typedef std::tuple<Args...> args_tuple_type;
    typedef types<Args...> args_type;
    typedef R(function_type)(Args...);
    typedef R(*function_pointer_type)(Args...);
    typedef R(*free_function_pointer_type)(Args...);
    typedef R return_type;
    typedef std::remove_pointer_t<free_function_pointer_type> signature_type;
    template<std::size_t i>
    using arg_at = void_tuple_element_t<i, args_tuple_type>;
};

template<typename Signature, bool b = std::is_member_object_pointer<Signature>::value>
struct callable_traits : fx_traits<std::decay_t<Signature>> {

};

template<typename R, typename T>
struct callable_traits<R(T::*), true> {
    typedef R Arg;
    typedef T object_type;
    using signature_type = R(T::*);
    static const bool is_member_function = false;
    static const std::size_t arity = 1;
    typedef std::tuple<Arg> args_tuple_type;
    typedef types<Arg> args_type;
    typedef R return_type;
    typedef R(function_type)(Arg);
    typedef R(*function_pointer_type)(Arg);
    typedef R(*free_function_pointer_type)(Arg);
    template<std::size_t i>
    using arg_at = void_tuple_element_t<i, args_tuple_type>;
};
} // meta_detail

template<typename Signature>
struct bind_traits : meta_detail::callable_traits<std::remove_volatile_t<Signature>> {};

template<typename Signature>
using function_args_t = typename bind_traits<Signature>::args_type;

template<typename Signature>
using function_signature_t = typename bind_traits<Signature>::signature_type;

template<typename Signature>
using function_return_t = typename bind_traits<Signature>::return_type;

struct has_begin_end_impl {
    template<typename T, typename U = unqualified_t<T>,
        typename B = decltype(std::declval<U&>().begin()),
        typename E = decltype(std::declval<U&>().end())>
    static std::true_type test(int);

    template<typename...>
    static std::false_type test(...);
};

template<typename T>
struct has_begin_end : decltype(has_begin_end_impl::test<T>(0)) {};

struct has_key_value_pair_impl {
    template<typename T, typename U = unqualified_t<T>,
        typename V = typename U::value_type,
        typename F = decltype(std::declval<V&>().first),
        typename S = decltype(std::declval<V&>().second)>
    static std::true_type test(int);

    template<typename...>
    static std::false_type test(...);
};

template<typename T>
struct has_key_value_pair : decltype(has_key_value_pair_impl::test<T>(0)) {};

template <typename T>
using is_string_constructible = any<std::is_same<unqualified_t<T>, const char*>, std::is_same<unqualified_t<T>, char>, std::is_same<unqualified_t<T>, std::string>, std::is_same<unqualified_t<T>, std::initializer_list<char>>>;

template <typename T>
using is_c_str = any<
    std::is_same<std::decay_t<unqualified_t<T>>, const char*>, 
    std::is_same<std::decay_t<unqualified_t<T>>, char*>, 
    std::is_same<unqualified_t<T>, std::string>
>;

template <typename T>
struct is_move_only : all<
    neg<std::is_reference<T>>, 
    neg<std::is_copy_constructible<unqualified_t<T>>>, 
    std::is_move_constructible<unqualified_t<T>>
> {};

template <typename T>
using is_not_move_only = neg<is_move_only<T>>;

namespace meta_detail {
template <typename T, meta::disable<meta::is_specialization_of<std::tuple, meta::unqualified_t<T>>> = meta::enabler>
decltype(auto) force_tuple(T&& x) {
    return std::forward_as_tuple(std::forward<T>(x));
}

template <typename T, meta::enable<meta::is_specialization_of<std::tuple, meta::unqualified_t<T>>> = meta::enabler>
decltype(auto) force_tuple(T&& x) {
    return std::forward<T>(x);
}
} // meta_detail

template <typename... X>
decltype(auto) tuplefy(X&&... x ) {
    return std::tuple_cat(meta_detail::force_tuple(std::forward<X>(x))...);
}
} // meta
namespace detail {
template <std::size_t I, typename Tuple>
decltype(auto) forward_get( Tuple&& tuple ) {
    return std::forward<meta::tuple_element_t<I, Tuple>>(std::get<I>(tuple));
}

template <std::size_t... I, typename Tuple>
auto forward_tuple_impl( std::index_sequence<I...>, Tuple&& tuple ) -> decltype(std::tuple<decltype(forward_get<I>(tuple))...>(forward_get<I>(tuple)...)) {
    return std::tuple<decltype(forward_get<I>(tuple))...>(std::move(std::get<I>(tuple))...);
}

template <typename Tuple>
auto forward_tuple( Tuple&& tuple ) {
    auto x = forward_tuple_impl(std::make_index_sequence<std::tuple_size<meta::unqualified_t<Tuple>>::value>(), std::forward<Tuple>(tuple));
    return x;
}

template<typename T>
auto unwrap(T&& item) -> decltype(std::forward<T>(item)) {
    return std::forward<T>(item);
}

template<typename T>
T& unwrap(std::reference_wrapper<T> arg) {
    return arg.get();
}

template<typename T>
decltype(auto) deref(T&& item) {
    return std::forward<T>(item);
}

template<typename T>
T& deref(T* item) {
    return *item;
}

template<typename T, typename Dx>
decltype(auto) deref(std::unique_ptr<T, Dx>& item) {
    return *item;
}

template<typename T>
T& deref(std::shared_ptr<T>& item) {
    return *item;
}

template<typename T, typename Dx>
decltype(auto) deref(const std::unique_ptr<T, Dx>& item) {
    return *item;
}

template<typename T>
T& deref(const std::shared_ptr<T>& item) {
    return *item;
}

template<typename T>
inline T* ptr(T& val) {
    return std::addressof(val);
}

template<typename T>
inline T* ptr(std::reference_wrapper<T> val) {
    return std::addressof(val.get());
}

template<typename T>
inline T* ptr(T* val) {
    return val;
}
} // detail
} // sol

#endif // SOL_TRAITS_HPP
