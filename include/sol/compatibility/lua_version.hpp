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

#ifndef SOL_COMPATIBILITY_VERSION_HPP
#define SOL_COMPATIBILITY_VERSION_HPP

#include <sol/version.hpp>

// clang-format off

#if SOL_IS_ON(SOL_USE_CXX_LUA_I_)
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
#elif SOL_IS_ON(SOL_USE_LUA_HPP_I_)
	#include <lua.hpp>
#else
	extern "C" {
		#include <lua.h>
		#include <lauxlib.h>
		#include <lualib.h>
	}
#endif // C++ Mangling for Lua vs. Not

#if defined(SOL_LUAJIT)
	#if (SOL_LUAJIT != 0)
		#define SOL_USE_LUAJIT_I_ SOL_ON
	#else
		#define SOL_USE_LUAJIT_I_ SOL_OFF
	#endif
#elif defined(LUAJIT_VERSION)
	#define SOL_USE_LUAJIT_I_ SOL_OFF
#else
	#define SOL_USE_LUAJIT_I_ SOL_DEFAULT_OFF
#endif // luajit

#if SOL_IS_ON(SOL_USE_CXX_LUAJIT_I_)
	#include <luajit.h>
#elif SOL_IS_ON(SOL_USE_LUAJIT_I_)
	extern "C" {
		#include <luajit.h>
	}
#endif // C++ LuaJIT ... whatever that means

#if defined(SOL_LUAJIT_VERSION)
	#define SOL_LUAJIT_VERSION_I_ SOL_LUAJIT_VERSION
#elif SOL_IS_ON(SOL_USE_LUAJIT_I_)
	#define SOL_LUAJIT_VERSION_I_ LUAJIT_VERSION_NUM
#else
	#define SOL_LUAJIT_VERSION_I_ 0
#endif

#if defined(MOONJIT_VERSION)
	#define SOL_USE_MOONJIT_I_ SOL_ON
#else
	#define SOL_USE_MOONJIT_I_ SOL_OFF
#endif

#if !defined(SOL_LUA_VERSION)
	#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 502
		#define SOL_LUA_VERSION LUA_VERSION_NUM
	#elif defined(LUA_VERSION_NUM) && LUA_VERSION_NUM == 501
		#define SOL_LUA_VERSION LUA_VERSION_NUM
	#elif !defined(LUA_VERSION_NUM) || !(LUA_VERSION_NUM)
		// Definitely 5.0
		#define SOL_LUA_VERSION 500
	#else
		// ??? Not sure, assume latest?
		#define SOL_LUA_VERSION 504
	#endif // Lua Version 503, 502, 501 || luajit, 500
#endif // SOL_LUA_VERSION

#if defined(SOL_LUA_VERSION)
	#define SOL_LUA_VESION_I_ SOL_LUA_VERSION
#else
	#define SOL_LUA_VESION_I_ 504
#endif

// Exception safety / propagation, according to Lua information
// and user defines. Note this can sometimes change based on version information...
#if defined(SOL_EXCEPTIONS_ALWAYS_UNSAFE)
	#if (SOL_EXCEPTIONS_ALWAYS_UNSAFE != 0)
		#define SOL_PROPAGATE_EXCEPTIONS_I_ SOL_OFF
	#else
		#define SOL_PROPAGATE_EXCEPTIONS_I_ SOL_ON
	#endif
#elif defined(SOL_EXCEPTIONS_SAFE_PROPAGATION)
	#if (SOL_EXCEPTIONS_SAFE_PROPAGATION != 0)
		#define SOL_PROPAGATE_EXCEPTIONS_I_ SOL_ON
	#else
		#define SOL_PROPAGATE_EXCEPTIONS_I_ SOL_OFF
	#endif
#elif SOL_LUAJIT_VERSION_I_ >= 20100
	// LuaJIT 2.1.0-beta3 and better have exception support locked in for all platforms (mostly)
	#define SOL_PROPAGATE_EXCEPTIONS_I_ SOL_DEFAULT_ON
#elif SOL_LUAJIT_VERSION_I_ >= 20000
	// LuaJIT 2.0.x have exception support only on x64 builds
	#if SOL_IS_ON(SOL_PLATFORM_X64_I_)
		#define SOL_PROPAGATE_EXCEPTIONS_I_ SOL_DEFAULT_ON
	#else
		#define SOL_PROPAGATE_EXCEPTIONS_I_ SOL_OFF
	#endif
#else
	// otherwise, there is no exception safety for
	// shoving exceptions through Lua and errors should
	// always be serialized
	#define SOL_PROPAGATE_EXCEPTIONS_I_ SOL_DEFAULT_OFF
#endif // LuaJIT beta 02.01.00 have better exception handling on all platforms since beta3

#if defined(SOL_LUAJIT_USE_EXCEPTION_TRAMPOLINE)
	#if (SOL_LUAJIT_USE_EXCEPTION_TRAMPOLINE != 0)
		#define SOL_USE_LUAJIT_EXCEPTION_TRAMPOLINE_I_ SOL_ON
	#else
		#define SOL_USE_LUAJIT_EXCEPTION_TRAMPOLINE_I_ SOL_OFF
	#endif
#else
	#if SOL_IS_OFF(SOL_PROPAGATE_EXCEPTIONS_I_) && SOL_IS_ON(SOL_USE_LUAJIT_I_)
		#define SOL_USE_LUAJIT_EXCEPTION_TRAMPOLINE_I_ SOL_ON
	#else
		#define SOL_USE_LUAJIT_EXCEPTION_TRAMPOLINE_I_ SOL_DEFAULT_OFF
	#endif
#endif

#if defined(SOL_LUAL_STREAM_HAS_CLOSE_FUNCTION)
	#if (SOL_LUAL_STREAM_HAS_CLOSE_FUNCTION != 0)
		#define SOL_LUAL_STREAM_USE_CLOSE_FUNCTION_I_ SOL_ON
	#else
		#define SOL_LUAL_STREAM_USE_CLOSE_FUNCTION_I_ SOL_OFF
	#endif
#else
	#if SOL_IS_OFF(SOL_USE_LUAJIT_I_) && (SOL_LUA_VERSION > 501)
		#define SOL_LUAL_STREAM_USE_CLOSE_FUNCTION_I_ SOL_ON
	#else
		#define SOL_LUAL_STREAM_USE_CLOSE_FUNCTION_I_ SOL_DEFAULT_OFF
	#endif
#endif

// clang-format on

#endif // SOL_COMPATIBILITY_VERSION_HPP
