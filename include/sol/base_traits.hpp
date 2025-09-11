// sol2

// The MIT License (MIT)

// Copyright (c) 2013-2022 Rapptz, ThePhD and contributors

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

#ifndef SOL_BASE_TRAITS_HPP
#define SOL_BASE_TRAITS_HPP

#include <type_traits>

namespace sol {
	namespace detail {
		struct unchecked_t { };
		const unchecked_t unchecked = unchecked_t {};
	} // namespace detail

	namespace meta {
		using sfinae_yes_t = std::true_type;
		using sfinae_no_t = std::false_type;

		template <auto C>
		using constant = std::integral_constant<decltype(C), C>;

		template <bool B>
		using boolean = std::bool_constant<B>;

		using std::void_t;

		template <typename T>
		using unqualified = std::remove_cv<std::remove_reference_t<T>>;

		template <typename T>
		using unqualified_t = typename unqualified<T>::type;

		namespace meta_detail {
			template <typename T>
			struct unqualified_non_alias : unqualified<T> { };

			template <template <class...> class Test, class, class... Args>
			struct is_detected : std::false_type { };

			template <template <class...> class Test, class... Args>
			struct is_detected<Test, void_t<Test<Args...>>, Args...> : std::true_type { };
		} // namespace meta_detail

		template <template <class...> class Trait, class... Args>
		using is_detected = typename meta_detail::is_detected<Trait, void, Args...>::type;

		template <template <class...> class Trait, class... Args>
		constexpr inline bool is_detected_v = is_detected<Trait, Args...>::value;

		template <typename _Default, typename _Void, template <typename...> typename _Op, typename... _Args>
		class detector {
		public:
			using value_t = ::std::false_type;
			using type = _Default;
		};

		template <typename _Default, template <typename...> typename _Op, typename... _Args>
		class detector<_Default, void_t<_Op<_Args...>>, _Op, _Args...> {
		public:
			using value_t = ::std::true_type;
			using type = _Op<_Args...>;
		};

		class nonesuch {
		public:
			~nonesuch() = delete;
			nonesuch(nonesuch const&) = delete;
			nonesuch& operator=(nonesuch const&) = delete;
		};

		template <template <typename...> typename _Op, typename... _Args>
		using detected_t = typename detector<nonesuch, void, _Op, _Args...>::type;

		template <typename _Default, template <typename...> typename _Op, typename... _Args>
		using detected_or = detector<_Default, void, _Op, _Args...>;

		template <typename _Default, template <typename...> typename _Op, typename... _Args>
		using detected_or_t = typename detector<_Default, void, _Op, _Args...>::type;

		template <typename _Default, template <typename...> typename _Op, typename... _Args>
		constexpr inline bool detected_or_v = detector<_Default, void, _Op, _Args...>::value;

		template <std::size_t I>
		using index_value = std::integral_constant<std::size_t, I>;

		template <bool>
		struct conditional {
			template <typename T, typename U>
			using type = T;
		};

		template <>
		struct conditional<false> {
			template <typename T, typename U>
			using type = U;
		};

		template <bool B, typename T, typename U>
		using conditional_t = typename conditional<B>::template type<T, U>;

		namespace meta_detail {
			template <typename T, template <typename...> class Templ>
			inline constexpr bool is_specialization_of_v = false;

			template <typename... Ts, template <typename...> class Templ>
			inline constexpr bool is_specialization_of_v<Templ<Ts...>, Templ> = true;

		} // namespace meta_detail

		template <typename T, template <typename...> class Templ>
		inline constexpr bool is_specialization_of_v = meta_detail::is_specialization_of_v<std::remove_cv_t<T>, Templ>;

		template <typename T, template <typename...> class Templ>
		using is_specialization_of = boolean<is_specialization_of_v<T, Templ>>;

		template <typename T>
		struct identity {
			using type = T;
		};

		template <typename T>
		using identity_t = typename identity<T>::type;

		template <typename T>
		using is_builtin_type = boolean<std::is_arithmetic_v<T> || std::is_pointer_v<T> || std::is_array_v<T>>;

		namespace meta_detail {
			template <typename T, typename = void>
			constexpr inline bool has_internal_marker_v = false;

			template <typename T>
			constexpr inline bool has_internal_marker_v<T, void_t<typename T::SOL_INTERNAL_UNSPECIALIZED_MARKER_>> = true;

			template <typename T>
			using has_internal_marker = boolean<has_internal_marker_v<T>>;

		} // namespace meta_detail

	} // namespace meta
} // namespace sol

#endif // SOL_BASE_TRAITS_HPP
