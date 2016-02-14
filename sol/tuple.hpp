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

#ifndef SOL_TUPLE_HPP
#define SOL_TUPLE_HPP

#include <tuple>
#include <cstddef>

namespace sol {
template<typename... Args>
struct types { typedef std::index_sequence_for<Args...> indices; static constexpr std::size_t size() { return sizeof...(Args); } };
 
namespace detail {
template<typename Arg>
struct chop_one : types<> {};

template<typename Arg0, typename Arg1, typename... Args>
struct chop_one<types<Arg0, Arg1, Args...>> : types<Arg1, Args...> {};

template<typename Arg, typename... Args>
struct chop_one<types<Arg, Args...>> : types<Args...> {};

template<typename... Args>
struct tuple_types_ { typedef types<Args...> type; };

template<typename... Args>
struct tuple_types_<std::tuple<Args...>> { typedef types<Args...> type; };

using swallow = std::initializer_list<int>;

} // detail

template<typename... Args>
using tuple_types = typename detail::tuple_types_<Args...>::type;

template<typename Arg>
struct remove_one_type : detail::chop_one<Arg> {};

template<typename... Args>
using constructors = sol::types<Args...>;

const auto default_constructor = constructors<types<>>{};

} // sol

#endif // SOL_TUPLE_HPP
