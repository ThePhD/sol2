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

#ifndef SOL_TUPLE_HPP
#define SOL_TUPLE_HPP

#include <tuple>
#include <cstddef>

namespace sol {
template<typename... Ts>
struct reverse_tuple;

template<>
struct reverse_tuple<std::tuple<>> {
    using type = std::tuple<>;
};

template<typename T, typename... Ts>
struct reverse_tuple<std::tuple<T, Ts...>> {
    using head = std::tuple<T>;
    using tail = typename reverse_tuple<std::tuple<Ts...>>::type;
    using type = decltype(std::tuple_cat(std::declval<tail>(), std::declval<head>()));
};

template<size_t... Ns>
struct indices { typedef indices type; };

template<size_t N, size_t... Ns>
struct build_indices : build_indices<N - 1, N - 1, Ns...> {};

template<size_t... Ns>
struct build_indices<0, Ns...> : indices<Ns...> {};

template<size_t N, size_t... Ns>
struct build_reverse_indices : build_reverse_indices<N - 1, Ns..., N - 1> {};

template<size_t... Ns>
struct build_reverse_indices<0, Ns...> : indices<Ns...> {};

template<typename... Args>
struct types : build_indices<sizeof...(Args)> { typedef types type; };
 
namespace detail {
template<class Acc, class... Args>
struct reversed_ : Acc{};

template<typename... RArgs, typename Arg, typename... Args>
struct reversed_<types<RArgs...>, Arg, Args...> : reversed_<types<Arg, RArgs...>, Args...>{};

template<typename Arg>
struct chop_one : types<> {};

template<typename Arg0, typename Arg1, typename... Args>
struct chop_one<types<Arg0, Arg1, Args...>> : types<Arg1, Args...> {};

template<typename Arg, typename... Args>
struct chop_one<types<Arg, Args...>> : types<Args...> {};
} // detail

template<typename... Args>
struct reversed : detail::reversed_<types<>, Args...>{};

template<typename... Args>
struct tuple_types : types<Args...> {};

template<typename... Args>
struct tuple_types<std::tuple<Args...>> : types<Args...> {};

template<typename Arg>
struct remove_one_type : detail::chop_one<Arg> {};

template<typename... Tn>
struct constructors {};

const auto default_constructor = constructors<types<>>{};

template <typename T>
using ref = std::reference_wrapper<T>;

} // sol

#endif // SOL_TUPLE_HPP
