// sol2 

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

#ifndef SOL_FEATURE_TEST_HPP
#define SOL_FEATURE_TEST_HPP

#if (defined(__cplusplus) && __cplusplus == 201703L) || (defined(_MSC_VER) && _MSC_VER > 1900 && ((defined(_HAS_CXX17) && _HAS_CXX17 == 1) || (defined(_MSVC_LANG) && _MSVC_LANG > 201402L)))
#ifndef SOL_CXX17_FEATURES
#define SOL_CXX17_FEATURES 1
#endif // C++17 features macro
#endif // C++17 features check

#if defined(__cpp_noexcept_function_type) || ((defined(_MSC_VER) && _MSC_VER > 1911) && ((defined(_HAS_CXX17) && _HAS_CXX17 == 1) || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)))
#ifndef SOL_NOEXCEPT_FUNCTION_TYPE
#define SOL_NOEXCEPT_FUNCTION_TYPE 1
#endif // noexcept is part of a function's type
#endif

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
// Clang sucks and doesn't really utilize codecvt support,
// not without checking the library versions explicitly (and we're not gonna do that)
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
// turn things on automatically

// Checks low-level getter function
// (and thusly, affects nearly entire framework)
#if !defined(SOL_SAFE_GETTER)
#define SOL_SAFE_GETTER 1
#endif

// Checks access on usertype functions
// local my_obj = my_type.new()
// my_obj.my_member_function()
// -- bad syntax and crash
#if !defined(SOL_SAFE_USERTYPE)
#define SOL_SAFE_USERTYPE 1
#endif

// Checks sol::reference derived boundaries
// sol::function ref(L, 1);
// sol::userdata sref(L, 2);
#if !defined(SOL_SAFE_REFERENCES)
#define SOL_SAFE_REFERENCES 1
#endif

// Changes all typedefs of sol::function to point to the 
// protected_function version, instead of unsafe_function
#if !defined(SOL_SAFE_FUNCTION)
#define SOL_SAFE_FUNCTION 1
#endif

// Checks function parameters and
// returns upon call into/from Lua
// local a = 1
// local b = "woof"
// my_c_function(a, b)
#if !defined(SOL_SAFE_FUNCTION_CALLS)
#define SOL_SAFE_FUNCTION_CALLS 1
#endif

// Checks conversions
// int v = lua["bark"];
// int v2 = my_sol_function();
#if !defined(SOL_SAFE_PROXIES)
#define SOL_SAFE_PROXIES 1
#endif

// Check overflowing number conversions
// for things like 64 bit integers that don't fit in a typical lua_Number
// for Lua 5.1 and 5.2
#if !defined(SOL_SAFE_NUMERICS)
#define SOL_SAFE_NUMERICS 1
#endif

#endif // Turn on Safety for all if top-level macro is defined

#ifdef SOL_IN_DEBUG_DETECTED

#if !defined(SOL_SAFE_REFERENCES)
// Ensure that references are forcefully type-checked upon construction
#define SOL_SAFE_REFERENCES 1
#endif

#if !defined(SOL_SAFE_USERTYPE)
// Usertypes should be safe no matter what
#define SOL_SAFE_USERTYPE 1
#endif

#if !defined(SOL_SAFE_FUNCTION_CALLS)
// Function calls from Lua should be automatically safe in debug mode
#define SOL_SAFE_FUNCTION_CALLS 1
#endif

#endif // Turn on all debug safety features for VC++ / g++ / clang++ and similar

#if defined(__MAC_OS_X_VERSION_MAX_ALLOWED) || defined(__OBJC__) || defined(nil)
#if !defined(SOL_NO_NIL)
#define SOL_NO_NIL 1
#endif
#endif // avoiding nil defines / keywords

#endif // SOL_FEATURE_TEST_HPP
