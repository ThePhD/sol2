# # # # sol2
# The MIT License (MIT)
# 
# Copyright (c) 2013-2022 Rapptz, ThePhD, and contributors
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

# Include guard
if(_lua_vanilla_build_included)
	return()
endif(_lua_vanilla_build_included)
set(_lua_vanilla_build_included true)

# import necessary standard modules
include(ExternalProject)
# import other modules
include(Common/Core)

# Latest versions for specific sub-versions of Lua
set(LUA_VANILLA_5.1_LATEST_VERSION 5.1.5)
set(LUA_VANILLA_5.2_LATEST_VERSION 5.2.4)
set(LUA_VANILLA_5.3_LATEST_VERSION 5.3.6)
set(LUA_VANILLA_5.4_LATEST_VERSION 5.4.4)

# Clean up some variables
if (LUA_VERSION MATCHES "^([0-9]+)\\.([0-9]+)\\.([0-9]+)$")
	# probably okay!
	set(LUA_VANILLA_VERSION ${CMAKE_MATCH_1}.${CMAKE_MATCH_2}.${CMAKE_MATCH_3})
elseif (LUA_VERSION MATCHES "([0-9]+)\\.([0-9]+)")
	# extend version number with prefix
	if (${CMAKE_MATCH_1} EQUAL 5)
		if (${CMAKE_MATCH_2} EQUAL 1)
			set(LUA_VANILLA_VERSION ${LUA_VANILLA_5.1_LATEST_VERSION})
		elseif (${CMAKE_MATCH_2} EQUAL 2)
			set(LUA_VANILLA_VERSION ${LUA_VANILLA_5.2_LATEST_VERSION})
		elseif (${CMAKE_MATCH_2} EQUAL 3)
			set(LUA_VANILLA_VERSION ${LUA_VANILLA_5.3_LATEST_VERSION})
		elseif (${CMAKE_MATCH_2} EQUAL 4)
			set(LUA_VANILLA_VERSION ${LUA_VANILLA_5.4_LATEST_VERSION})
		else()			
			# default to whatever the first two
			# numbers happen to be, plus build 0
			set(LUA_VANILLA_VERSION ${CMAKE_MATCH_1}.${CMAKE_MATCH_2}.0)
		endif()
	endif()
	if(NOT LUA_VANILLA_VERSION)			
		# default to whatever the first two
		# numbers happen to be, plus build 0
		set(LUA_VANILLA_VERSION ${CMAKE_MATCH_1}.${CMAKE_MATCH_2}.0)
	endif()
else()
	message(FATAL_ERROR "Cannot deduce the proper Lua version from ${LUA_VERSION}")
endif()

if (BUILD_LUA_AS_CXX)
	set(LUA_VANILLA_LANGUAGE CXX)
	set(LUA_VANILLA_LANGUAGE_STANDARD 98)
else()
	set(LUA_VANILLA_LANGUAGE C)
	set(LUA_VANILLA_LANGUAGE_STANDARD 99)
endif()

FIND_PACKAGE_MESSAGE(LUABUILD
	"Selecting PUC-RIO Lua ${LUA_VANILLA_VERSION} from '${LUA_VERSION}' and building a ${LUA_BUILD_LIBRARY_TYPE} library with ${LUA_VANILLA_LANGUAGE} linkage..."
	"[${LUA_VANILLA_VERSION}][${LUA_VERSION}][${LUA_BUILD_LIBRARY_TYPE}][${LUA_VANILLA_LANGUAGE}]")

# # # Makefile and self-build configurations

# # Potential compiler variables
if (MSVC)
	if (BUILD_LUA_AS_DLL)
		set(LUA_VANILLA_DLL_DEFINE LUA_BUILD_AS_DLL)
	else()
		set(LUA_VANILLA_DLL_DEFINE "")
	endif()
else()
	set(LUA_VANILLA_LUALIB_COMPILER_OPTIONS "")
	set(LUA_VANILLA_DLL_DEFINE "")
endif()

# # Source files for natural build, if we have to go that far
# retrieve source files
set(LUA_VANILLA_DOWNLOAD_URL https://www.lua.org/ftp/lua-${LUA_VANILLA_VERSION}.tar.gz)
if (LUA_VANILLA_VERSION MATCHES "^5\\.1")
	set(LUA_VANILLA_LIB_SOURCES lapi.c lcode.c ldebug.c ldo.c ldump.c lfunc.c 
		lgc.c llex.c lmem.c lobject.c lopcodes.c lparser.c lstate.c 
		lstring.c ltable.c ltm.c lundump.c lvm.c lzio.c lauxlib.c 
		lbaselib.c ldblib.c liolib.c lmathlib.c loslib.c ltablib.c 
		lstrlib.c loadlib.c linit.c)
	set(LUA_VANILLA_LUA_SOURCES lua.c )
	if (LUA_BUILD_LUA_COMPILER)
		set(LUA_VANILLA_LUAC_SOURCES luac.c print.c)
	endif()
	set(LUA_VANILLA_GENERATE_LUA_HPP true)
elseif (LUA_VANILLA_VERSION MATCHES "^5\\.2")
	set(LUA_VANILLA_LIB_SOURCES lapi.c lbitlib.c lcode.c lctype.c ldebug.c ldo.c ldump.c 
		lfunc.c lgc.c llex.c lmem.c lobject.c lopcodes.c lparser.c 
		lstate.c lstring.c ltable.c ltm.c lundump.c lvm.c lzio.c
		lauxlib.c lbaselib.c lcorolib.c ldblib.c liolib.c 
		lmathlib.c loslib.c lstrlib.c ltablib.c loadlib.c linit.c)
	set(LUA_VANILLA_LUA_SOURCES lua.c)
	if (LUA_BUILD_LUA_COMPILER)
		set(LUA_VANILLA_LUAC_SOURCES luac.c)
	endif()
	set(LUA_VANILLA_GENERATE_LUA_HPP false)
elseif (LUA_VANILLA_VERSION MATCHES "^5\\.3")
	set(LUA_VANILLA_LIB_SOURCES lapi.c lcode.c lctype.c ldebug.c ldo.c ldump.c 
		lfunc.c lgc.c llex.c lmem.c lobject.c lopcodes.c lparser.c lstate.c 
		lstring.c ltable.c ltm.c lundump.c lvm.c lzio.c lauxlib.c 
		lbaselib.c lbitlib.c lcorolib.c ldblib.c liolib.c lmathlib.c 
		loslib.c lstrlib.c ltablib.c lutf8lib.c loadlib.c linit.c)
	set(LUA_VANILLA_LUA_SOURCES lua.c )
	if (LUA_BUILD_LUA_COMPILER)
		set(LUA_VANILLA_LUAC_SOURCES luac.c)
	endif()
	set(LUA_VANILLA_GENERATE_LUA_HPP false)
elseif (LUA_VANILLA_VERSION MATCHES "^5\\.4")
	if (LUA_VANILLA_VERSION MATCHES "work" OR LUA_VANILLA_VERSION MATCHES "alpha"  OR LUA_VANILLA_VERSION MATCHES "beta")
		set(LUA_VANILLA_DOWNLOAD_URL https://www.lua.org/work/lua-${LUA_VANILLA_VERSION}.tar.gz)
	endif()
	set(LUA_VANILLA_LIB_SOURCES lapi.c lauxlib.c lbaselib.c lcode.c lcorolib.c 
		lctype.c ldblib.c ldebug.c ldo.c ldump.c lfunc.c lgc.c linit.c liolib.c
		llex.c lmathlib.c lmem.c loadlib.c lobject.c lopcodes.c loslib.c
		lparser.c lstate.c lstring.c lstrlib.c ltable.c ltablib.c ltm.c lundump.c
		lutf8lib.c lvm.c lzio.c)
	set(LUA_VANILLA_LUA_SOURCES lua.c)
	if (LUA_BUILD_LUA_COMPILER)
		set(LUA_VANILLA_LUAC_SOURCES luac.c)
	endif()
	set(LUA_VANILLA_GENERATE_LUA_HPP false)
else()
	MESSAGE(WARNING "Using Lua 5.4.4 file list for ${LUA_VERSION} version")
	set(LUA_VANILLA_LIB_SOURCES lapi.c lauxlib.c lbaselib.c lcode.c lcorolib.c 
		lctype.c ldblib.c ldebug.c ldo.c ldump.c lfunc.c lgc.c linit.c liolib.c
		llex.c lmathlib.c lmem.c loadlib.c lobject.c lopcodes.c loslib.c
		lparser.c lstate.c lstring.c lstrlib.c ltable.c ltablib.c ltm.c lundump.c
		lutf8lib.c lvm.c lzio.c)
	set(LUA_VANILLA_LUA_SOURCES lua.c)
	if (LUA_BUILD_LUA_COMPILER)
		set(LUA_VANILLA_LUAC_SOURCES luac.c)
	endif()
	set(LUA_VANILLA_GENERATE_LUA_HPP false)
endif()

# download, just for the sake of download + extract
# or pull from local folder
if (LUA_LOCAL_DIR)
	set(LUA_VANILLA_SOURCE_DIR ${LUA_BUILD_TOPLEVEL}-src)
	list(TRANSFORM LUA_VANILLA_LIB_SOURCES PREPEND "${LUA_VANILLA_SOURCE_DIR}/src/")
	list(TRANSFORM LUA_VANILLA_LUA_SOURCES PREPEND "${LUA_VANILLA_SOURCE_DIR}/src/")
	file(COPY "${LUA_LOCAL_DIR}/src"
		DESTINATION "${LUA_VANILLA_SOURCE_DIR}/")
	file(COPY "${LUA_LOCAL_DIR}/include"
		DESTINATION "${LUA_VANILLA_SOURCE_DIR}/")
	add_custom_target(LUA_VANILLA
		DEPENDS "${LUA_VANILLA_LIB_SOURCES}" "${LUA_VANILLA_LUA_SOURCES}")
	set(LUA_VANILLA_INCLUDE_DIRS ${LUA_VANILLA_INCLUDE_DIRS} "${LUA_VANILLA_SOURCE_DIR}/include")
else()
	include(FetchContent)
	FetchContent_Declare(
		lua-vanilla
		URL ${LUA_VANILLA_DOWNLOAD_URL})
	FetchContent_GetProperties(lua-vanilla)
	if ( NOT lua-vanilla_POPULATED)
		# Fetch the content using previously declared details
		FetchContent_Populate(lua-vanilla)
		# do not add_subdirectory / build: we are JUST using ti as a download step!
	endif()
	list(TRANSFORM LUA_VANILLA_LIB_SOURCES PREPEND "${lua-vanilla_SOURCE_DIR}/src/")
	list(TRANSFORM LUA_VANILLA_LUA_SOURCES PREPEND "${lua-vanilla_SOURCE_DIR}/src/")
	# make a quick lua.hpp for targets that don't have it
	if (LUA_VANILLA_GENERATE_LUA_HPP)
		set(LUA_VANILLA_LUA_HPP_CONTENT "// lua.hpp
// Lua header files for C++
// <<extern \"C\">> not supplied automatically because Lua also compiles as C++
extern \"C\" {
#include \"lua.h\"
#include \"lualib.h\"
#include \"lauxlib.h\"
}
")
		set(LUA_VANILLA_SOURCE_LUA_HPP "${lua-vanilla_SOURCE_DIR}/include/lua.hpp")
		file(CONFIGURE OUTPUT "${LUA_VANILLA_SOURCE_LUA_HPP}" CONTENT "${LUA_VANILLA_LUA_HPP_CONTENT}" @ONLY)
		file(TO_NATIVE_PATH "${LUA_VANILLA_SOURCE_LUA_HPP}" LUA_VANILLA_SOURCE_LUA_HPP)
	endif()

	set(LUA_VANILLA_INCLUDE_DIRS ${lua-vanilla_SOURCE_DIR}/src)
endif()

# # Target names
set(liblua "liblua-${LUA_VANILLA_VERSION}")
set(luainterpreter "lua-${LUA_VANILLA_VERSION}")
set(luacompiler "luac-${LUA_VANILLA_VERSION}")

# Lua does not out-of-the-box support building 
# a shared library: http://lua-users.org/lists/lua-l/2006-10/msg00098.html
# in this case, we essentially need to
# build Lua for our purposes, which is annoying
# the external project is just a dummy to get and extract the file:
# after that, we define actual targets...!

# make an actual, buildable target
# that other parts of the code can depend on
add_library(${liblua} ${LUA_BUILD_LIBRARY_TYPE} ${LUA_VANILLA_LIB_SOURCES})
add_library(Lua::Lua ALIAS ${liblua})
set_target_properties(${liblua}
	PROPERTIES
	LANGUAGE ${LUA_VANILLA_LANGUAGE}
	LINKER_LANGUAGE ${LUA_VANILLA_LANGUAGE}
	${LUA_VANILLA_LANGUAGE}_STANDARD ${LUA_VANILLA_LANGUAGE_STANDARD}
	${LUA_VANILLA_LANGUAGE}_EXTENSIONS TRUE
	POSITION_INDEPENDENT_CODE TRUE
	OUTPUT_NAME ${LUA_BUILD_LIBNAME}
	RUNTIME_OUTPUT_NAME ${LUA_BUILD_LIBNAME}
	LIBRARY_OUTPUT_NAME ${LUA_BUILD_LIBNAME}
	ARCHIVE_OUTPUT_NAME ${LUA_BUILD_LIBNAME})
set_source_files_properties(${LUA_VANILLA_LIB_SOURCES}
	PROPERTIES LANGUAGE ${LUA_VANILLA_LANGUAGE})
target_include_directories(${liblua}
	PUBLIC "${LUA_VANILLA_INCLUDE_DIRS}")
target_compile_definitions(${liblua}
	PUBLIC
		LUA_COMPAT_ALL
		${LUA_VANILLA_DLL_DEFINE})
if (MSVC)
	target_compile_options(${liblua}
		PRIVATE /W1)
else()
	target_compile_options(${liblua}
		PRIVATE -w)
endif()
if (WIN32)
else()
	target_compile_definitions(${liblua}
		PRIVATE LUA_USE_LINUX)
endif()
target_compile_options(${liblua}
	PRIVATE ${LUA_VANILLA_LUALIB_COMPILER_OPTIONS})
if (LUA_LOCAL_DIR)
	add_dependencies(${liblua} LUA_VANILLA)
endif()
target_link_libraries(${liblua} PRIVATE ${CMAKE_DL_LIBS})
if (UNIX)
	target_link_libraries(${liblua} PRIVATE m)
endif()

# we don't really need this section...
# Lua Interpreter
if (LUA_BUILD_LUA_INTERPRETER)
	add_executable(${luainterpreter} ${LUA_VANILLA_LUA_SOURCES})
	set_target_properties(${luainterpreter}
		PROPERTIES
		LANGUAGE ${LUA_VANILLA_LANGUAGE}
		LINKER_LANGUAGE ${LUA_VANILLA_LANGUAGE}
		${LUA_VANILLA_LANGUAGE}_STANDARD ${LUA_VANILLA_LANGUAGE_STANDARD}
		${LUA_VANILLA_LANGUAGE}_EXTENSIONS TRUE
		OUTPUT_NAME lua-${LUA_VANILLA_VERSION})
	set_source_files_properties(${LUA_VANILLA_LUA_SOURCES}
		PROPERTIES LANGUAGE ${LUA_VANILLA_LANGUAGE})
	target_include_directories(${luainterpreter}
		PRIVATE "${LUA_VANILLA_INCLUDE_DIRS}")
	target_compile_definitions(${luainterpreter}
		PUBLIC LUA_COMPAT_ALL ${LUA_VANILLA_DLL_DEFINE}
		PRIVATE LUA_COMPAT_ALL ${LUA_VANILLA_DLL_DEFINE})
	if (MSVC)
		target_compile_options(${luainterpreter}
			PRIVATE /W1)
	else()
		target_compile_options(${luainterpreter}
			PRIVATE -w)
	endif()
	if (WIN32)
		#target_compile_definitions(${luainterpreter} 
		#	PRIVATE LUA_USE_WINDOWS)
	else()
		target_compile_definitions(${luainterpreter} 
			PRIVATE LUA_USE_LINUX)
	endif()
	target_link_libraries(${luainterpreter} PRIVATE ${liblua} ${CMAKE_DL_LIBS})
	if (UNIX)
		target_link_libraries(${luainterpreter} PRIVATE m readline)
	endif()
endif()

# LuaC Compiler
if (LUA_BUILD_LUA_COMPILER)
	add_executable(${luainterpreter} ${LUA_VANILLA_LUA_SOURCES})
	set_target_properties(${luacompiler}
		PROPERTIES
		LANGUAGE ${LUA_VANILLA_LANGUAGE}
		LINKER_LANGUAGE ${LUA_VANILLA_LANGUAGE}
		OUTPUT_NAME luac-${LUA_VANILLA_VERSION})
	set_source_files_properties(${LUA_VANILLA_LIB_SOURCES}
		PROPERTIES LANGUAGE ${LUA_VANILLA_LANGUAGE})
	target_include_directories(${luacompiler}
		PRIVATE "${LUA_VANILLA_INCLUDE_DIRS}")
	target_compile_definitions(${luacompiler}
		PUBLIC LUA_COMPAT_ALL ${LUA_VANILLA_DLL_DEFINE}
		PRIVATE LUA_COMPAT_ALL ${LUA_VANILLA_DLL_DEFINE})
	if (MSVC)
		target_compile_options(${luacompiler}
			PRIVATE /W1)
	else()
		target_compile_options(${luacompiler}
			PRIVATE -w)
	endif()
	if (WIN32)
		#target_compile_definitions(${luacompiler} 
		#	PRIVATE LUA_USE_WINDOWS)
	else()
		target_compile_definitions(${luacompiler} 
			PRIVATE LUA_USE_LINUX)
	endif()
	target_link_libraries(${luacompiler} PRIVATE ${liblua} ${CMAKE_DL_LIBS})
	if (UNIX)
		# TODO: make readline optional?
		target_link_libraries(${luacompiler} PRIVATE m readline)
	endif()
endif()

# set externally-visible target indicator
set(LUA_LIBRARIES ${liblua})
set(LUA_INTERPRETER ${luainterpreter})
if (LUA_BUILD_LUA_COMPILER)
	set(LUA_COMPILER ${luacompiler})
endif()
set(LUA_INCLUDE_DIRS ${LUA_VANILLA_INCLUDE_DIRS})
