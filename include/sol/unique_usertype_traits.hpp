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

#ifndef SOL_UNIQUE_USERTYPE_TRAITS_HPP
#define SOL_UNIQUE_USERTYPE_TRAITS_HPP

#include "base_traits.hpp"
#include "pointer_like.hpp"

#include <memory>

namespace sol {

	template <typename T>
	struct unique_usertype_traits {
		typedef T type;
		typedef T actual_type;
		template <typename X>
		using rebind_base = void;

		static const bool value = false;

		template <typename U>
		static bool is_null(U&&) {
			return false;
		}

		template <typename U>
		static auto get(U&& value) {
			return std::addressof(detail::deref(value));
		}
	};

	template <typename T>
	struct unique_usertype_traits<std::shared_ptr<T>> {
		typedef T type;
		typedef std::shared_ptr<T> actual_type;
		// rebind is non-void
		// if and only if unique usertype
		// is cast-capable
		template <typename X>
		using rebind_base = std::shared_ptr<X>;

		static const bool value = true;

		static bool is_null(const actual_type& p) {
			return p == nullptr;
		}

		static type* get(const actual_type& p) {
			return p.get();
		}
	};

	template <typename T, typename D>
	struct unique_usertype_traits<std::unique_ptr<T, D>> {
		using type = T;
		using actual_type = std::unique_ptr<T, D>;

		static const bool value = true;

		static bool is_null(const actual_type& p) {
			return p == nullptr;
		}

		static type* get(const actual_type& p) {
			return p.get();
		}
	};

	template <typename T>
	struct is_unique_usertype : std::integral_constant<bool, unique_usertype_traits<T>::value> {};

	template <typename T>
	inline constexpr bool is_unique_usertype_v = is_unique_usertype<T>::value;

	namespace detail {
		template <typename T>
		using is_base_rebindable_test = decltype(T::rebind_base);
	}

	template <typename T>
	using is_base_rebindable = meta::is_detected<detail::is_base_rebindable_test, T>;

	template <typename T>
	inline constexpr bool is_base_rebindable_v = is_base_rebindable<T>::value;

	namespace detail {
		template <typename T, typename>
		struct is_base_rebindable_non_void_sfinae : std::false_type {};

		template <typename T>
		struct is_base_rebindable_non_void_sfinae<T, std::enable_if_t<is_base_rebindable_v<T>>>
		: std::integral_constant<bool, !std::is_void_v<typename T::template rebind_base<void>>> {};
	} // namespace detail

	template <typename T>
	using is_base_rebindable_non_void = meta::is_detected<detail::is_base_rebindable_test, T>;

	template <typename T>
	inline constexpr bool is_base_rebindable_non_void_v = is_base_rebindable_non_void<T>::value;

} // namespace sol

#endif // SOL_UNIQUE_USERTYPE_TRAITS_HPP
