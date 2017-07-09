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

#if (defined(__cplusplus) && __cplusplus == 201703L) || (defined(_MSC_VER) && _MSC_VER > 1900 && (defined(_HAS_CXX17) && _HAS_CXX17 == 1) || (_MSVC_LANG > 201402))
#ifndef SOL_CXX17_FEATURES
#define SOL_CXX17_FEATURES 1
#endif // C++17 features macro
#endif // C++17 features check

#if defined(__cpp_noexcept_function_type)
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
// not without checking the library versions explicitly (and we're not gonna do that, so fuck you)
#endif // Windows/VC++ vs. g++ vs Others

#ifdef _MSC_VER
#ifdef _DEBUG
#ifndef NDEBUG
#ifndef SOL_CHECK_ARGUMENTS
// Do not define by default: let user turn it on
//#define SOL_CHECK_ARGUMENTS
#endif // Check Arguments
#ifndef SOL_SAFE_USERTYPE
#define SOL_SAFE_USERTYPE
#endif // Safe Usertypes
#endif // NDEBUG
#endif // Debug

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

#ifndef NDEBUG
#ifndef __OPTIMIZE__
#ifndef SOL_CHECK_ARGUMENTS
// Do not define by default: let user choose
//#define SOL_CHECK_ARGUMENTS
// But do check userdata by default:
#endif // Check Arguments
#ifndef SOL_SAFE_USERTYPE
#define SOL_SAFE_USERTYPE
#endif // Safe Usertypes
#endif // g++ optimizer flag
#endif // Not Debug

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

#ifndef SOL_SAFE_USERTYPE
#ifdef SOL_CHECK_ARGUMENTS
#define SOL_SAFE_USERTYPE
#endif // Turn on Safety for all
#endif // Safe Usertypes

#endif // SOL_FEATURE_TEST_HPP
