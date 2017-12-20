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

# import necessary standard modules
include(ExternalProject)

# Clean up some variables
if (LUA_VERSION MATCHES "^[0-9]+\\.[0-9]+\\.[0-9]+$")
	# probably okay
elseif (LUA_VERSION MATCHES "([0-9]+)\\.([0-9]+)")
	# extend version number with prefix
	if (${CMAKE_MATCH_1} EQUAL 5)
		if (${CMAKE_MATCH_2} EQUAL 1)
			set(LUA_VERSION "5.1.5")
		elseif (${CMAKE_MATCH_2} EQUAL 2)
			set(LUA_VERSION "5.2.4")
		elseif (${CMAKE_MATCH_2} EQUAL 3)
			set(LUA_VERSION "5.3.4")
		endif()
	endif()
endif()

# exact version, coming from CI: pull directly from Lua and use external project to build
# list of known md5 / sha1: must update when there are changes
set(LUA_EXTERNAL_MD5_5.3.4 "53a9c68bcc0eda58bdc2095ad5cdfc63")
set(LUA_EXTERNAL_MD5_5.2.4 "913fdb32207046b273fdb17aad70be13")
set(LUA_EXTERNAL_MD5_5.1.5 "2e115fe26e435e33b0d5c022e4490567")
set(LUA_EXTERNAL_SHA1_5.3.4 "79790cfd40e09ba796b01a571d4d63b52b1cd950")
set(LUA_EXTERNAL_SHA1_5.2.4 "ef15259421197e3d85b7d6e4871b8c26fd82c1cf")
set(LUA_EXTERNAL_SHA1_5.1.5 "b3882111ad02ecc6b972f8c1241647905cb2e3fc")

if (WIN32 AND MSVC)
	set(LUA_EXTERNAL_BUILD_DLL_DEFAULT ON)
else()
	set(LUA_EXTERNAL_BUILD_DLL_DEFAULT OFF)
endif()

# get sha1/md5 hashes to verify integrity of download
set(LUA_EXTERNAL_SHA1 ${LUA_EXTERNAL_SHA1_${LUA_VERSION}})
set(LUA_EXTERNAL_MD5 ${LUA_EXTERNAL_MD5_${LUA_VERSION}})
set(LUA_EXTERNAL_TOPLEVEL "${CMAKE_BINARY_DIR}/vendor/lua-${LUA_VERSION}")
set(LUA_EXTERNAL_INSTALL_DIR "${LUA_EXTERNAL_TOPLEVEL}/install")
set(LUA_INCLUDE_PREFIX "${LUA_EXTERNAL_INSTALL_DIR}" CACHE PATH "Directory that prefixes the Lua include directory")
set(LUA_INCLUDE_DIR "${LUA_INCLUDE_PREFIX}/include" CACHE PATH "Directory with lua include files")
set(LUA_LIB_DIR "${LUA_EXTERNAL_INSTALL_DIR}/lib" CACHE PATH "Directory with lua library files")
set(LUA_BIN_DIR "${LUA_EXTERNAL_INSTALL_DIR}/bin" CACHE PATH "Directory with lua executable and dynamic library files")
option(LUA_BUILD_AS_DLL ${LUA_EXTERNAL_BUILD_DLL_DEFAULT} "Build Lua as a Shared/Dynamic Link Library")

set(LUA_LIBRARY_DESCRIPTION "Name of the either the full built static library, or of the import library used")
set(LUA_DYNAMIC_LIBRARY_DESCRIPTION "Name of the Lua dynamic library to build, if LUA_BUILD_AS_DLL is set")

set(LUA_LIBRARY ${CMAKE_STATIC_LIBRARY_PREFIX}lua${LUA_VERSION}.${CMAKE_STATIC_LIBRARY_PREFIX}
	CACHE STRING 
	${LUA_LIBRARY_DESCRIPTION})
set(LUA_DYNAMIC_LIBRARY ${CMAKE_SHARED_LIBRARY_PREFIX}lua${LUA_VERSION}.${CMAKE_SHARED_LIBRARY_SUFFIX} 
	CACHE STRING 
	${LUA_DYNAMIC_LIBRARY_DESCRIPTION})

# Lua does not out-of-the-box support building 
# a shared library: http://lua-users.org/lists/lua-l/2006-10/msg00098.html
if (LUA_BUILD_AS_DLL)
	set(LUA_EXTERNAL_LIBRARY_TYPE SHARED)
else()
	set(LUA_EXTERNAL_LIBRARY_TYPE STATIC)
endif()

if (MSVC)
	set(LUA_EXTERNAL_OBJECT_FILE_SUFFIX .o)
else()
	set(LUA_EXTERNAL_OBJECT_FILE_SUFFIX .obj)
endif()

if (MSVC OR LUA_BUILD_AS_DLL)
	# in this case, we essentially need to
	# build Lua for our purposes, which is annoying
	# the external project is just a dummy to get and extract the file:
	# after that, we define actual targets...!

	# preconfiguration
	set(LUA_EXTERNAL_LUALIB_COMPILER_OPTIONS -O3 -Wall -Wextra -DLUA_COMPAT_5_2)
	if (LUA_BUILD_AS_DLL)
		 set(LUA_EXTERNAL_LIBLUA_COMPILER_OPTIONS ${LUA_EXTERNAL_LIBLUA_COMPILER_OPTIONS} -fPIC)
	endif()

	# retrieve source files
	if (LUA_VERSION MATCHES "5.1")
		set(LUA_EXTERNAL_LIB_SOURCES lapi.c lcode.c ldebug.c ldo.c ldump.c lfunc.c 
			lgc.c llex.c lmem.c lobject.c lopcodes.c lparser.c lstate.c 
			lstring.c ltable.c ltm.c lundump.c lvm.c lzio.c lauxlib.c 
			lbaselib.c ldblib.c liolib.c lmathlib.c loslib.c ltablib.c 
			lstrlib.c linit.c)
		set(LUA_EXTERNAL_LUA_SOURCES lua.c )
		set(LUA_EXTERNAL_LUAC_SOURCES luac.c print.c )
	elseif (LUA_VERSION MATCHES "5.2")
		set(LUA_EXTERNAL_LIB_SOURCES lapi.c lcode.c lctype.c ldebug.c ldo.c ldump.c 
			lfunc.c lgc.c llex.c lmem.c lobject.c lopcodes.c lparser.c 
			lstate.c lstring.c ltable.c ltm.c lundump.c lvm.c lzio.c
			lauxlib.c lbaselib.c lbitlib.c lcorolib.c ldblib.c 
			liolib.c lmathlib.c loslib.c lstrlib.c ltablib.c linit.c)
		set(LUA_EXTERNAL_LUA_SOURCES lua.c )
		set(LUA_EXTERNAL_LUAC_SOURCES luac.c )
	else()
		set(LUA_EXTERNAL_LIB_SOURCES lapi.c lcode.c lctype.c ldebug.c ldo.c ldump.c 
			lfunc.c lgc.c llex.c lmem.c lobject.c lopcodes.c lparser.c lstate.c 
			lstring.c ltable.c ltm.c lundump.c lvm.c lzio.c lauxlib.c 
			lbaselib.c lbitlib.c lcorolib.c ldblib.c liolib.c lmathlib.c 
			loslib.c lstrlib.c ltablib.c lutf8lib.c loadlib.c linit.c)
		set(LUA_EXTERNAL_LUA_SOURCES lua.c )
		set(LUA_EXTERNAL_LUAC_SOURCES luac.c )
	endif()
	STRING(REGEX REPLACE "\\.c" ${LUA_EXTERNAL_OBJECT_FILE_SUFFIX} LUA_EXTERNAL_LIB_OBJECTS ${LUA_EXTERNAL_LIB_SOURCES})
	STRING(REGEX REPLACE "\\.c" ${LUA_EXTERNAL_OBJECT_FILE_SUFFIX} LUA_EXTERNAL_LUA_OBJECTS ${LUA_EXTERNAL_LUA_SOURCES})
	STRING(REGEX REPLACE "\\.c" ${LUA_EXTERNAL_OBJECT_FILE_SUFFIX} LUA_EXTERNAL_LUAC_OBJECTS ${LUA_EXTERNAL_LUAC_SOURCES})

	# download, just for the sake of download + extract
	ExternalProject_Add(LUA_EXTERNAL
		BUILD_IN_SOURCE TRUE
		BUILD_ALWAYS TRUE
		TLS_VERIFY TRUE
		PREFIX ${LUA_EXTERNAL_TOPLEVEL}
		SOURCE_DIR ${LUA_EXTERNAL_TOPLEVEL}
		DOWNLOAD_DIR ${LUA_EXTERNAL_TOPLEVEL}
		TMP_DIR "${LUA_EXTERNAL_TOPLEVEL}-tmp"
		STAMP_DIR "${LUA_EXTERNAL_TOPLEVEL}-stamp"
		INSTALL_DIR "${LUA_EXTERNAL_INSTALL_DIR}"
		URL https://www.lua.org/ftp/lua-${LUA_VERSION}.tar.gz
		URL_MD5 ${LUA_EXTERNAL_MD5}
		URL_HASH SHA1=${LUA_EXTERNAL_SHA1}
		CONFIGURE_COMMAND ""
		BUILD_COMMAND ""
		INSTALL_COMMAND ""
		TEST_COMMAND ""
		BUILD_BYPRODUCTS ${LUA_EXTERNAL_LIB_SOURCES} ${LUA_EXTERNAL_LUA_SOURCES} ${LUA_EXTERNAL_LUAC_SOURCES})

	# make an actual, buildable target
	# that other parts of the code can depend on
	add_library(lualib_${LUA_VERSION} ${LUA_EXTERNAL_LIBRARY_TYPE} ${LUA_EXTERNAL_LIB_SOURCES})
	set_target_properties(lualib_${LUA_VERSION}
		PROPERTIES
		LANGUAGE C
		LINKER_LANGUAGE C
		INCLUDE_DIRECTORIES src/
		LOCATION ${LUA_EXTERNAL_LIB_FILE}
		SONAME ${LUA_EXTERNAL_DLL_FILE}
		)
	target_compile_options(lualib_${LUA_VERSION}
		PRIVATE ${LUA_EXTERNAL_LUALIB_COMPILER_OPTIONS} )
	add_dependencies(lualib_${LUA_VERSION} LUA_EXTERNAL)

	# we don't really need this section...
	#add_executable(lua_${LUA_VERSION} ${LUA_EXTERNAL_LUA_SOURCES})
	#add_executable(luac_${LUA_VERSION} ${LUA_EXTERNAL_LUAC_SOURCES})
	#add_dependencies(lua_${LUA_VERSION} lualib_${LUA_VERSION})
	#add_dependencies(luac_${LUA_VERSION} lualib_${LUA_VERSION})

	# set externally-visible target indicator
	set(LUA_LIBRARIES lualib_${LUA_VERSION})
else ()
	# we can simply reuse the makefile here
	# so define it as an external project and then just have the proper
	# build/install/test commands
	# note that this only works for shared objects:
	if (MINGW)
		set(LUA_EXTERNAL_MAKE_TARGET mingw)
	elseif(APPLE)
		set(LUA_EXTERNAL_MAKE_TARGET macosx)
	elseif(UNIX AND NOT APPLE)
		set(LUA_EXTERNAL_MAKE_TARGET linux)
	else()
		set(LUA_EXTERNAL_MAKE_TARGET generic)
	endif()
	set(LUA_EXTERNAL_BUILD_COMMAND make ${LUA_EXTERNAL_MAKE_TARGET} LUA_A=${LUA_LIBRARY})
	set(LUA_EXTERNAL_INSTALL_COMMAND make local TO_LIB=${LUA_LIBRARY})
	set(LUA_EXTERNAL_TEST_COMMAND make test LUA_A=${LUA_LIBRARY})
	set(LUA_EXTERNAL_LIB_FILE "${LUA_LIB_DIR}/${LUA_LIBRARY}")
	set(LUA_EXTERNAL_DLL_FILE "${LUA_BIN_DIR}/${LUA_DYNAMIC_LIBRARY}")
	if (LUA_BUILD_AS_DLL)
		set(LUA_EXTERNAL_BYPRODUCTS ${LUA_EXTERNAL_LIB_FILE} ${LUA_EXTERNAL_DLL_FILE})
	else()
		set(LUA_EXTERNAL_BYPRODUCTS ${LUA_EXTERNAL_LIB_FILE})
	endif()
	ExternalProject_Add(LUA_EXTERNAL
		BUILD_IN_SOURCE TRUE
		BUILD_ALWAYS TRUE
		TLS_VERIFY TRUE
		PREFIX ${LUA_EXTERNAL_TOPLEVEL}
		SOURCE_DIR ${LUA_EXTERNAL_TOPLEVEL}
		DOWNLOAD_DIR ${LUA_EXTERNAL_TOPLEVEL}
		TMP_DIR "${LUA_EXTERNAL_TOPLEVEL}-tmp"
		STAMP_DIR "${LUA_EXTERNAL_TOPLEVEL}-stamp"
		INSTALL_DIR "${LUA_EXTERNAL_INSTALL_DIR}"
		URL https://www.lua.org/ftp/lua-${LUA_VERSION}.tar.gz
		URL_MD5 ${LUA_EXTERNAL_MD5}
		URL_HASH SHA1=${LUA_EXTERNAL_SHA1}
		CONFIGURE_COMMAND ""
		BUILD_COMMAND ${LUA_EXTERNAL_BUILD_COMMAND}
		INSTALL_COMMAND ${LUA_EXTERNAL_INSTALL_COMMAND}
		TEST_COMMAND ${LUA_EXTERNAL_TEST_COMMAND}
		BUILD_BYPRODUCTS ${LUA_EXTERNAL_BYPRODUCTS})

	# TODO: SHARED not fully supported yet, stock makefiles do not build
	# dynamic libs on linux out of the box...
	add_library(lualib_${LUA_VERSION} ${LUA_EXTERNAL_LIBRARY_TYPE} IMPORTED)
	set_target_properties(lualib_${LUA_VERSION}
		PROPERTIES 
		LANGUAGE C
		LINKER_LANGUAGE C
		INCLUDE_DIRECTORIES ${LUA_INCLUDE_DIR}
		IMPORTED_LOCATION ${LUA_EXTERNAL_LIB_FILE}
		IMPORTED_SONAME ${LUA_EXTERNAL_DLL_FILE})
	
	# make sure the library we export really does depend on Lua
	add_dependencies(lualib_${LUA_VERSION} LUA_EXTERNAL)

	# set externally-visible target indicator
	set(LUA_LIBRARIES lualib_${LUA_VERSION})		
endif()

set(LUA_FOUND TRUE)

unset(LUA_EXTERNAL_BUILD_DLL_DEFAULT)
unset(LUA_EXTERNAL_MD5)
unset(LUA_EXTERNAL_SHA1)
unset(LUA_EXTERNAL_MD5_5.3.4)
unset(LUA_EXTERNAL_MD5_5.2.4)
unset(LUA_EXTERNAL_MD5_5.1.5)
unset(LUA_EXTERNAL_SHA1_5.3.4)
unset(LUA_EXTERNAL_SHA1_5.2.4)
unset(LUA_EXTERNAL_SHA1_5.1.5)
unset(LUA_EXTERNAL_TOPLEVEL)
unset(LUA_EXTERNAL_INSTALL_DIR)
unset(LUA_EXTERNAL_LIB_FILE)
unset(LUA_EXTERNAL_BUILD_COMMAND)
unset(LUA_EXTERNAL_TEST_COMMAND)
unset(LUA_EXTERNAL_INSTALL_COMMAND)
unset(LUA_EXTERNAL_LIBRARY_TYPE)
unset(LUA_EXTERNAL_OBJECT_FILE_SUFFIX)
unset(LUA_LIBRARY_DESCRIPTION)
unset(LUA_DYNAMIC_LIBRARY_DESCRIPTION)
