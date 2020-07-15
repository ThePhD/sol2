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

#ifndef SOL_VERSION_HPP
#define SOL_VERSION_HPP

#include "feature_test.hpp"
#include <sol/config.hpp>

#define SOL_ON(X) ((X + X) != 0)
#define SOL_OFF(X) ((X + X) == 0)

// clang-format off

#if defined(SOL_IN_DEBUG_DETECTED) && (SOL_IN_DEBUG_DETECTED != 0)
	#define SOL_IN_DEBUG_DETECTED_ 1
#else
	#define SOL_IN_DEBUG_DETECTED_ 0
#endif // We are in a debug mode of some sort

#if defined(SOL_NO_THREAD_LOCAL) && (SOL_NO_THREAD_LOCAL != 0)
	#define SOL_USE_THREAD_LOCAL_ 0
#else
	#define SOL_USE_THREAD_LOCAL_ 1
#endif // thread_local keyword is bjorked on some platforms

#if defined(SOL_USERTYPE_TYPE_BINDING_INFO) && (SOL_USERTYPE_TYPE_BINDING_INFO != 0)
	#define SOL_USERTYPE_TYPE_BINDING_INFO_ 1
#else
	#define SOL_USERTYPE_TYPE_BINDING_INFO_ 0
#endif // We should generate a my_type.__type table with lots of class information for usertypes

#if defined(SOL_AUTOMAGICAL_TYPES_BY_DEFAULT) && SOL_AUTOMAGICAL_TYPES_BY_DEFAULT != 0
	#define SOL_AUTOMAGICAL_TYPES_BY_DEFAULT_ 1
#else
	#define SOL_AUTOMAGICAL_TYPES_BY_DEFAULT_ 1
#endif // make is_automagical on/off by default

#if defined(SOL_STD_VARIANT) && SOL_STD_VARIANT != 0
	#define SOL_STD_VARIANT_ 1
#else
	#define SOL_STD_VARIANT_ 0
#endif // make is_automagical on/off by default

#if defined(SOL_ID_SIZE) && SOL_ID_SIZE > 0
	#define SOL_ID_SIZE_ SOL_ID_SIZE
#else
	#define SOL_ID_SIZE_ 512
#endif

#if defined(LUA_IDSIZE) && LUA_IDSIZE > 0
	#define SOL_FILE_ID_SIZE_ LUA_IDSIZE
#elif defined(SOL_ID_SIZE) && SOL_ID_SIZE > 0
	#define SOL_FILE_ID_SIZE_ SOL_FILE_ID_SIZE
#else
	#define SOL_FILE_ID_SIZE_ 2048
#endif


// clang-format on

#endif // SOL_VERSION_HPP
