# # # # sol2
# The MIT License (MIT)
# 
# Copyright (c) 2013-2017 Rapptz, ThePhD, and contributors
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
# the Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

# # # Variables
# # Core Paths
string(TOLOWER ${LUA_VERSION} LUA_BUILD_NORMALIZED_LUA_VERSION)
if (LUA_BUILD_NORMALIZED_LUA_VERSION MATCHES "luajit")
	set(LUA_BUILD_LIBNAME ${LUA_VERSION})
elseif (LUAJIT_BUILD)
	set(LUA_BUILD_LIBNAME luajit-${LUA_VERSION})
elseif (LUA_BUILD_NORMALIZED_LUA_VERSION MATCHES "lua")
	set(LUA_BUILD_LIBNAME ${LUA_VERSION})
elseif (LUA_BUILD)
	set(LUA_BUILD_LIBNAME lua-${LUA_VERSION})
else()
	set(LUA_BUILD_LIBNAME lua-${LUA_VERSION})
endif()
set(LUA_BUILD_TOPLEVEL "${CMAKE_BINARY_DIR}/vendor/${LUA_BUILD_LIBNAME}")
set(LUA_BUILD_INSTALL_DIR "${LUA_BUILD_TOPLEVEL}/install")
# # Misc needed variables
set(LUA_BUILD_LIBRARY_DESCRIPTION "The base name of the library to build either the static or the dynamic library")

# Object file suffixes
if (MSVC)
	set(LUA_BUILD_BUILD_DLL_DEFAULT ON)
	set(LUA_BUILD_OBJECT_FILE_SUFFIX .obj)
else()
	set(LUA_BUILD_BUILD_DLL_DEFAULT OFF)
	set(LUA_BUILD_OBJECT_FILE_SUFFIX .o)
endif()


# # # Options
set(LUA_INCLUDE_PREFIX "${LUA_BUILD_INSTALL_DIR}" CACHE PATH "Directory that prefixes the Lua include directory")
set(LUA_INCLUDE_DIR "${LUA_INCLUDE_PREFIX}/include" CACHE PATH "Directory with lua include files")
set(LUA_LIB_DIR "${LUA_BUILD_INSTALL_DIR}/lib" CACHE PATH "Directory with lua library files")
set(LUA_BIN_DIR "${LUA_BUILD_INSTALL_DIR}/bin" CACHE PATH "Directory with lua executable and dynamic library files")
option(BUILD_LUA_AS_DLL ${LUA_BUILD_BUILD_DLL_DEFAULT} "Build Lua or LuaJIT as a Shared/Dynamic Link Library")

STRING(TOLOWER ${LUA_BUILD_LIBNAME} LUA_BUILD_NORMALIZED_LIBNAME)
if (LUA_BUILD_NORMALIZED_LIBNAME MATCHES "luajit")
	set(LUA_LIBRARY luajit
		CACHE STRING 
		${LUA_BUILD_LIBRARY_DESCRIPTION})
else()
	set(LUA_LIBRARY ${LUA_BUILD_LIBNAME}
		CACHE STRING 
		${LUA_BUILD_LIBRARY_DESCRIPTION})
endif()
# # Dependent Variables
# If we're building a DLL, then set the library type to SHARED
if (BUILD_LUA_AS_DLL)
	set(LUA_BUILD_LIBRARY_TYPE SHARED)
else()
	set(LUA_BUILD_LIBRARY_TYPE STATIC)
endif()


# # # Build Lua
# # Select either LuaJIT or Vanilla Lua here, based on what we discover
if (BUILD_LUAJIT OR LUA_BUILD_NORMALIZED_LUA_VERSION MATCHES "luajit")
	include(LuaJITBuild)
else()
	include(LuaVanillaBuild)
endif()

# # # Variable cleanup
unset(LUA_BUILD_LIBNAME)
unset(LUA_BUILD_NORMALIZED_LUA_VERSION)
unset(LUA_BUILD_LIBRARY_DESCRIPTION)
unset(LUA_BUILD_DYNAMIC_LIBRARY_DESCRIPTION)
unset(LUA_BUILD_BUILD_DLL_DEFAULT)
unset(LUA_BUILD_OBJECT_FILE_SUFFIX)
unset(LUA_BUILD_LIBRARY_TYPE)
