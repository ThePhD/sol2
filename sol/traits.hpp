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

#ifndef SOL_TRAITS_HPP
#define SOL_TRAITS_HPP

#include "tuple.hpp"
#include <type_traits>
#include <functional>

namespace sol {
template<typename T>
struct identity { typedef T type; };

template<typename... Args>
struct is_tuple : std::false_type{ };

template<typename... Args>
struct is_tuple<std::tuple<Args...>> : std::true_type{ };

template<typename T>
struct unwrap {
    typedef T type;
};

template<typename T>
struct unwrap<std::reference_wrapper<T>> {
    typedef typename std::add_lvalue_reference<T>::type type;
};

template<typename T>
struct remove_member_pointer;

template<typename R, typename T>
struct remove_member_pointer<R T::*> {
    typedef R type;
};

template<typename T, template<typename...> class Templ>
struct is_specialization_of : std::false_type { };
template<typename... T, template<typename...> class Templ>
struct is_specialization_of<Templ<T...>, Templ> : std::true_type { };

template<class T, class...>
struct are_same : std::true_type { };

template<class T, class U, class... Args>
struct are_same<T, U, Args...> : std::integral_constant <bool, std::is_same<T, U>::value && are_same<T, Args...>::value> { };

template<typename T>
using Type = typename T::type;

template<bool B>
using Bool = std::integral_constant<bool, B>;

template<typename T>
using Not = Bool<!T::value>;

template<typename Condition, typename Then, typename Else>
using If = typename std::conditional<Condition::value, Then, Else>::type;

template<typename Condition, typename Then, typename Else>
using TypeIf = typename std::conditional<Condition::value, Type<Then>, Type<Else>>::type;

template<typename... Args>
struct And : Bool<true> {};

template<typename T, typename... Args>
struct And<T, Args...> : If<T, And<Args...>, Bool<false>> {};

template<typename... Args>
struct Or : Bool<true> {};

template<typename T, typename... Args>
struct Or<T, Args...> : If<T, Bool<true>, Or<Args...>> {};

template<typename... Args>
using EnableIf = typename std::enable_if<And<Args...>::value, int>::type;

template<typename... Args>
using DisableIf = typename std::enable_if<Not<And<Args...>>::value, int>::type;

template<typename T>
using Unqualified = typename std::remove_cv<typename std::remove_reference<T>::type>::type;

template<typename T>
using Decay = typename std::decay<T>::type;

template<typename T>
using Unwrap = typename unwrap<T>::type;

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

template <typename Empty, typename... Tn>
using ReturnTypeOr = typename std::conditional<(sizeof...(Tn) < 1), Empty, typename return_type<Tn...>::type>::type;

template <typename... Tn>
using ReturnType = ReturnTypeOr<void, Tn...>;

namespace detail {

template<typename T, bool isclass = std::is_class<Unqualified<T>>::value>
struct is_function_impl : std::is_function<typename std::remove_pointer<T>::type> {};

template<typename T>
struct is_function_impl<T, true> {
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
} // detail

template<class F>
struct has_deducible_signature : detail::check_deducible_signature<F>::type { };

template<typename T>
using has_deducible_signature_t = typename has_deducible_signature<T>::type;

template<typename T>
struct Function : Bool<detail::is_function_impl<T>::value> {};

namespace detail {
template<typename Signature, bool b = has_deducible_signature<Signature>::value>
struct fx_traits;

template<typename Signature>
struct fx_traits<Signature, true> : fx_traits<decltype(&Signature::operator()), false> {

};

template<typename T, typename R, typename... Args>
struct fx_traits<R(T::*)(Args...), false> {
    static const std::size_t arity = sizeof...(Args);
    static const bool is_member_function = true;
    typedef std::tuple<Args...> arg_tuple_type;
    typedef types<Args...> args_type;
    typedef R(T::* function_pointer_type)(Args...);
    typedef typename std::remove_pointer<function_pointer_type>::type function_type;
    typedef R(*free_function_pointer_type)(Args...);
    typedef R return_type;
    typedef typename std::remove_pointer<free_function_pointer_type>::type signature_type;
    template<std::size_t i>
    using arg = typename std::tuple_element<i, arg_tuple_type>::type;
};

template<typename T, typename R, typename... Args>
struct fx_traits<R(T::*)(Args...) const, false> {
    static const std::size_t arity = sizeof...(Args);
    static const bool is_member_function = true;
    typedef std::tuple<Args...> arg_tuple_type;
    typedef types<Args...> args_type;
    typedef R(T::* function_pointer_type)(Args...);
    typedef typename std::remove_pointer<function_pointer_type>::type function_type;
    typedef R(*free_function_pointer_type)(Args...);
    typedef R return_type;
    typedef typename std::remove_pointer<free_function_pointer_type>::type signature_type;
    template<std::size_t i>
    using arg = typename std::tuple_element<i, arg_tuple_type>::type;
};

template<typename R, typename... Args>
struct fx_traits<R(Args...), false> {
    static const std::size_t arity = sizeof...(Args);
    static const bool is_member_function = false;
    typedef std::tuple<Args...> arg_tuple_type;
    typedef types<Args...> args_type;
    typedef R(function_type)(Args...);
    typedef R(*function_pointer_type)(Args...);
    typedef R(*free_function_pointer_type)(Args...);
    typedef R return_type;
    typedef typename std::remove_pointer<free_function_pointer_type>::type signature_type;
    template<std::size_t i>
    using arg = typename std::tuple_element<i, arg_tuple_type>::type;
};

template<typename R, typename... Args>
struct fx_traits<R(*)(Args...), false> {
    static const std::size_t arity = sizeof...(Args);
    static const bool is_member_function = false;
    typedef std::tuple<Args...> arg_tuple_type;
    typedef types<Args...> args_type;
    typedef R(function_type)(Args...);
    typedef R(*function_pointer_type)(Args...);
    typedef R(*free_function_pointer_type)(Args...);
    typedef R return_type;
    typedef typename std::remove_pointer<free_function_pointer_type>::type signature_type;
    template<std::size_t i>
    using arg = typename std::tuple_element<i, arg_tuple_type>::type;
};

} // detail

template<typename Signature>
struct function_traits : detail::fx_traits<Signature> {};

template<typename Signature>
using function_args_t = typename function_traits<Signature>::args_type;

template<typename Signature>
using function_signature_t = typename function_traits<Signature>::signature_type;

template<typename Signature>
using function_return_t = typename function_traits<Signature>::return_type;

namespace detail {
template<typename Signature, bool b = std::is_member_object_pointer<Signature>::value>
struct member_traits : function_traits<Signature> {

};

template<typename Signature>
struct member_traits<Signature, true> {
    typedef typename remove_member_pointer<Signature>::type Arg;
    typedef typename remove_member_pointer<Signature>::type R;
    typedef Signature signature_type;
    static const bool is_member_function = false;
    static const std::size_t arity = 1;
    typedef std::tuple<Arg> arg_tuple_type;
    typedef types<Arg> args_type;
    typedef R return_type;
    typedef R(function_type)(Arg);
    typedef R(*function_pointer_type)(Arg);
    typedef R(*free_function_pointer_type)(Arg);
    template<std::size_t i>
    using arg = typename std::tuple_element<i, arg_tuple_type>::type;
};
} // detail

template<typename Signature>
struct member_traits : detail::member_traits<Signature> {

};

struct has_begin_end_impl {
    template<typename T, typename U = Unqualified<T>,
        typename B = decltype(std::declval<U&>().begin()),
        typename E = decltype(std::declval<U&>().end())>
    static std::true_type test(int);

    template<typename...>
    static std::false_type test(...);
};

template<typename T>
struct has_begin_end : decltype(has_begin_end_impl::test<T>(0)) {};

struct has_key_value_pair_impl {
    template<typename T, typename U = Unqualified<T>,
        typename V = typename U::value_type,
        typename F = decltype(std::declval<V&>().first),
        typename S = decltype(std::declval<V&>().second)>
    static std::true_type test(int);

    template<typename...>
    static std::false_type test(...);
};

template<typename T>
struct has_key_value_pair : decltype(has_key_value_pair_impl::test<T>(0)) {};

template<typename T>
auto unwrapper(T&& item) -> decltype(std::forward<T>(item)) {
    return std::forward<T>(item);
}

template<typename Arg>
Unwrap<Arg> unwrapper(std::reference_wrapper<Arg> arg) {
    return arg.get();
}

} // sol

#endif // SOL_TRAITS_HPP
