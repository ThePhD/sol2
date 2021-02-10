// sol3

// The MIT License (MIT)

// Copyright (c) 2013-2020 Rapptz, ThePhD and contributors

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

#ifndef SOL_OPTIONAL_HPP
#define SOL_OPTIONAL_HPP

#include <sol/forward.hpp>
#include <sol/in_place.hpp>
#include <sol/traits.hpp>
#if SOL_IS_ON(SOL_USE_BOOST_I_)
#include <boost/optional.hpp>

#include <boost/version.hpp>
#if BOOST_VERSION >= 107500 // Since Boost 1.75.0 boost::none is constexpr
#define COULD_BE_CONSTEXPR constexpr
#else
#define COULD_BE_CONSTEXPR const
#endif // BOOST_VERSION

#else
#include <sol/optional_implementation.hpp>
#endif // Boost vs. Better optional

#include <optional>

namespace sol {

#if SOL_IS_ON(SOL_USE_BOOST_I_)
	template <typename T>
	using optional = boost::optional<T>;
	using nullopt_t = boost::none_t;
	COULD_BE_CONSTEXPR nullopt_t nullopt = boost::none;
#endif // Boost vs. Better optional

	namespace meta {
		template <typename T>
		using is_optional = any<is_specialization_of<T, optional>, is_specialization_of<T, std::optional>>;

		template <typename T>
		constexpr inline bool is_optional_v = is_optional<T>::value;
	} // namespace meta

	namespace detail {
		template <typename T>
		struct associated_nullopt {
			inline static constexpr std::nullopt_t value = std::nullopt;
		};

#if SOL_IS_ON(SOL_USE_BOOST_I_)
		template <typename T>
		struct associated_nullopt<boost::optional<T>> {
			inline static COULD_BE_CONSTEXPR boost::none_t value = boost::none;
		};
#endif // Boost nullopt

#if SOL_IS_ON(SOL_USE_BOOST_I_)
		template <typename T>
		inline COULD_BE_CONSTEXPR auto associated_nullopt_v = associated_nullopt<T>::value;
#else
		template <typename T>
		inline constexpr auto associated_nullopt_v = associated_nullopt<T>::value;
#endif // Boost continues to lag behind, to not many people's surprise...
	} // namespace detail
} // namespace sol

#if SOL_IS_ON(SOL_USE_BOOST_I_)
#undef COULD_BE_CONSTEXPR
#endif

#endif // SOL_OPTIONAL_HPP
