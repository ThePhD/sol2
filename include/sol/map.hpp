// sol3

// The MIT License (MIT)

// Copyright (c) 2013-2019 Rapptz, ThePhD and contributors

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

#ifndef SOL_MAP_HPP
#define SOL_MAP_HPP

#include <unordered_map>
#if defined(SOL_USE_BOOST)
#include <boost/unordered_map.hpp>
#endif // SOL_USE_BOOST

namespace sol {
namespace detail {
#if defined(SOL_USE_BOOST)
#if defined(SOL_CXX17_FEATURES)
	template <typename K, typename V, typename H = std::hash<K>, typename E = std::equal_to<>>
	using unordered_map = boost::unordered_map<K, V, H, E>;
#else
	template <typename K, typename V, typename H = boost::hash<K>, typename E = std::equal_to<>>
	using unordered_map = boost::unordered_map<K, V, H, E>;
#endif // C++17 or not, WITH boost
#else
	template <typename K, typename V, typename H = std::hash<K>, typename E = std::equal_to<>>
	using unordered_map = std::unordered_map<K, V, H, E>;
#endif // Boost map target
}

} // namespace sol::detail

#endif // SOL_MAP_HPP
