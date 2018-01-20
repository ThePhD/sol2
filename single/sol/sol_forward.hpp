// The MIT License (MIT)

// Copyright (c) 2013-2017 Rapptz, ThePhD and contributors

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

// This file was generated with a script.
// Generated 2018-01-20 19:31:24.431906 UTC
// This header was generated with sol v2.19.0 (revision cbd7923)
// https://github.com/ThePhD/sol2

#ifndef SOL_SINGLE_INCLUDE_FORWARD_HPP
#define SOL_SINGLE_INCLUDE_FORWARD_HPP

// beginning of sol/forward.hpp

// beginning of sol/feature_test.hpp

#if (defined(__cplusplus) && __cplusplus == 201703L) || (defined(_MSC_VER) && _MSC_VER > 1900 && ((defined(_HAS_CXX17) && _HAS_CXX17 == 1) || (defined(_MSVC_LANG) && (_MSVC_LANG > 201402L))))
#ifndef SOL_CXX17_FEATURES
#define SOL_CXX17_FEATURES 1
#endif // C++17 features macro
#endif // C++17 features check

#ifdef SOL_CXX17_FEATURES
#if defined(__cpp_noexcept_function_type) || ((defined(_MSC_VER) && _MSC_VER > 1911) && (defined(_MSVC_LANG) && ((_MSVC_LANG >= 201703L) && defined(_WIN64))))
#ifndef SOL_NOEXCEPT_FUNCTION_TYPE
#define SOL_NOEXCEPT_FUNCTION_TYPE 1
#endif // noexcept is part of a function's type
#endif // compiler-specific checks
#endif // C++17 only

#if defined(_WIN32) || defined(_MSC_VER)
#ifndef SOL_CODECVT_SUPPORT
#define SOL_CODECVT_SUPPORT 1
#endif // sol codecvt support
#elif defined(__GNUC__)
#if __GNUC__ >= 5
#ifndef SOL_CODECVT_SUPPORT
#define SOL_CODECVT_SUPPORT 1
#endif // codecvt support
#endif // g++ 5.x.x (MinGW too)
#else
#endif // Windows/VC++ vs. g++ vs Others

#ifdef _MSC_VER
#if defined(_DEBUG) && !defined(NDEBUG)

#ifndef SOL_IN_DEBUG_DETECTED
#define SOL_IN_DEBUG_DETECTED 1
#endif

#endif // VC++ Debug macros

#ifndef _CPPUNWIND
#ifndef SOL_NO_EXCEPTIONS
#define SOL_NO_EXCEPTIONS 1
#endif
#endif // Automatic Exceptions

#ifndef _CPPRTTI
#ifndef SOL_NO_RTTI
#define SOL_NO_RTTI 1
#endif
#endif // Automatic RTTI
#elif defined(__GNUC__) || defined(__clang__)

#if !defined(NDEBUG) && !defined(__OPTIMIZE__)

#ifndef SOL_IN_DEBUG_DETECTED
#define SOL_IN_DEBUG_DETECTED 1
#endif

#endif // Not Debug && g++ optimizer flag

#ifndef __EXCEPTIONS
#ifndef SOL_NO_EXCEPTIONS
#define SOL_NO_EXCEPTIONS 1
#endif
#endif // No Exceptions

#ifndef __GXX_RTTI
#ifndef SOL_NO_RTII
#define SOL_NO_RTTI 1
#endif
#endif // No RTTI

#endif // vc++ || clang++/g++

#if defined(SOL_CHECK_ARGUMENTS)

#if !defined(SOL_SAFE_GETTER)
#define SOL_SAFE_GETTER 1
#endif

#if !defined(SOL_SAFE_USERTYPE)
#define SOL_SAFE_USERTYPE 1
#endif

#if !defined(SOL_SAFE_REFERENCES)
#define SOL_SAFE_REFERENCES 1
#endif

#if !defined(SOL_SAFE_FUNCTION)
#define SOL_SAFE_FUNCTION 1
#endif

#if !defined(SOL_SAFE_FUNCTION_CALLS)
#define SOL_SAFE_FUNCTION_CALLS 1
#endif

#if !defined(SOL_SAFE_PROXIES)
#define SOL_SAFE_PROXIES 1
#endif

#if !defined(SOL_SAFE_NUMERICS)
#define SOL_SAFE_NUMERICS 1
#endif

#endif // Turn on Safety for all if top-level macro is defined

#ifdef SOL_IN_DEBUG_DETECTED

#if !defined(SOL_SAFE_REFERENCES)
#define SOL_SAFE_REFERENCES 1
#endif

#if !defined(SOL_SAFE_USERTYPE)
#define SOL_SAFE_USERTYPE 1
#endif

#if !defined(SOL_SAFE_FUNCTION_CALLS)
#define SOL_SAFE_FUNCTION_CALLS 1
#endif

#endif // Turn on all debug safety features for VC++ / g++ / clang++ and similar

#if defined(__MAC_OS_X_VERSION_MAX_ALLOWED) || defined(__OBJC__) || defined(nil)
#if !defined(SOL_NO_NIL)
#define SOL_NO_NIL 1
#endif
#endif // avoiding nil defines / keywords

// end of sol/feature_test.hpp

namespace sol {

	template <bool b>
	class basic_reference;
	using reference = basic_reference<false>;
	using main_reference = basic_reference<true>;
	class stack_reference;

	struct proxy_base_tag;
	template <typename Super>
	struct proxy_base;
	template <typename Table, typename Key>
	struct proxy;

	template <typename T>
	class usertype;
	template <typename T>
	class simple_usertype;
	template <bool, typename T>
	class basic_table_core;
	template <bool b>
	using table_core = basic_table_core<b, reference>;
	template <bool b>
	using main_table_core = basic_table_core<b, main_reference>;
	template <bool b>
	using stack_table_core = basic_table_core<b, stack_reference>;
	template <typename T>
	using basic_table = basic_table_core<false, T>;
	typedef table_core<false> table;
	typedef table_core<true> global_table;
	typedef main_table_core<false> main_table;
	typedef main_table_core<true> main_global_table;
	typedef stack_table_core<false> stack_table;
	typedef stack_table_core<true> stack_global_table;
	template <typename base_t>
	struct basic_environment;
	using environment = basic_environment<reference>;
	using main_environment = basic_environment<main_reference>;
	using stack_environment = basic_environment<stack_reference>;
	template <typename T, bool>
	class basic_function;
	template <typename T, bool, typename H>
	class basic_protected_function;
	using unsafe_function = basic_function<reference, false>;
	using safe_function = basic_protected_function<reference, false, reference>;
	using main_unsafe_function = basic_function<main_reference, false>;
	using main_safe_function = basic_protected_function<main_reference, false, reference>;
	using stack_unsafe_function = basic_function<stack_reference, false>;
	using stack_safe_function = basic_protected_function<stack_reference, false, reference>;
	using stack_aligned_unsafe_function = basic_function<stack_reference, true>;
	using stack_aligned_safe_function = basic_protected_function<stack_reference, true, reference>;
	using protected_function = safe_function;
	using main_protected_function = main_safe_function;
	using stack_protected_function = stack_safe_function;
	using stack_aligned_protected_function = stack_aligned_safe_function;
#ifdef SOL_SAFE_FUNCTION
	using function = protected_function;
	using main_function = main_protected_function;
	using stack_function = stack_protected_function;
#else
	using function = unsafe_function;
	using main_function = main_unsafe_function;
	using stack_function = stack_unsafe_function;
#endif
	using stack_aligned_function = stack_aligned_unsafe_function;
	using stack_aligned_stack_handler_function = basic_protected_function<stack_reference, true, stack_reference>;

	struct unsafe_function_result;
	struct protected_function_result;
	using safe_function_result = protected_function_result;
#ifdef SOL_SAFE_FUNCTION
	using function_result = safe_function_result;
#else
	using function_result = unsafe_function_result;
#endif

	template <typename base_t>
	class basic_object;
	template <typename base_t>
	class basic_userdata;
	template <typename base_t>
	class basic_lightuserdata;
	template <typename base_t>
	class basic_coroutine;
	template <typename base_t>
	class basic_thread;

	using object = basic_object<reference>;
	using userdata = basic_userdata<reference>;
	using lightuserdata = basic_lightuserdata<reference>;
	using thread = basic_thread<reference>;
	using coroutine = basic_coroutine<reference>;
	using main_object = basic_object<main_reference>;
	using main_userdata = basic_userdata<main_reference>;
	using main_lightuserdata = basic_lightuserdata<main_reference>;
	using main_coroutine = basic_coroutine<main_reference>;
	using stack_object = basic_object<stack_reference>;
	using stack_userdata = basic_userdata<stack_reference>;
	using stack_lightuserdata = basic_lightuserdata<stack_reference>;
	using stack_thread = basic_thread<stack_reference>;
	using stack_coroutine = basic_coroutine<stack_reference>;

	struct stack_proxy_base;
	struct stack_proxy;
	struct variadic_args;
	struct variadic_results;
	struct stack_count;
	struct this_state;
	struct this_main_state;
	struct this_environment;

	template <typename T>
	struct as_table_t;
	template <typename T>
	struct as_container_t;
	template <typename T>
	struct nested;
	template <typename T>
	struct light;
	template <typename T>
	struct user;
	template <typename T>
	struct as_args_t;
	template <typename T>
	struct protect_t;
	template <typename F, typename... Filters>
	struct filter_wrapper;
} // namespace sol

// end of sol/forward.hpp

#endif // SOL_SINGLE_INCLUDE_FORWARD_HPP
