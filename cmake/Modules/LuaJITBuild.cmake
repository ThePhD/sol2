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

# Latest iterations for specific sub-versions of LuaJIT
set(LUA_JIT_2.0_LATEST_VERSION 2.0.5)
set(LUA_JIT_2.1_LATEST_VERSION 2.1.0-beta3)

# MD5 hashes taken off of LuaJIT's website
# must be updated whenever a new version appears
set(LUA_JIT_MD5_2.1.0-beta3 eae40bc29d06ee5e3078f9444fcea39b)
set(LUA_JIT_MD5_2.1.0-beta3 58d0480e1af0811e7ecee45498d62e2d)
set(LUA_JIT_MD5_2.1.0-beta2 fa14598d0d775a7ffefb138a606e0d7b)
set(LUA_JIT_MD5_2.1.0-beta2 b5d943c0174ca217736e2ddc2d9721c3)
set(LUA_JIT_MD5_2.1.0-beta1 5a5bf71666e77cf6e7a1ae851127b834)
set(LUA_JIT_MD5_2.1.0-beta1 4b5c2c9aef0e7c0b622b09e7c84d566b)
set(LUA_JIT_MD5_2.0.5 48353202cbcacab84ee41a5a70ea0a2c)
set(LUA_JIT_MD5_2.0.5 f7cf52a049d74aee4e624bdc1160b80d)
set(LUA_JIT_MD5_2.0.4 dd9c38307f2223a504cbfb96e477eca0)
set(LUA_JIT_MD5_2.0.4 ed1f0caf3d390171f423f6f1b5c57aac)
set(LUA_JIT_MD5_2.0.3 f14e9104be513913810cd59c8c658dc0)
set(LUA_JIT_MD5_2.0.3 6c0f6958d5e1f67734fb1ff514ec4c84)
set(LUA_JIT_MD5_2.0.2 112dfb82548b03377fbefbba2e0e3a5b)
set(LUA_JIT_MD5_2.0.2 a57c7d1b8eaf46559303dcfd56404045)
set(LUA_JIT_MD5_2.0.1 85e406e8829602988eb1233a82e29f1f)
set(LUA_JIT_MD5_2.0.1 cf4aee0e40b220054ee3cffbe0cd6ed5)
set(LUA_JIT_MD5_2.0.0 97a2b87cc0490784f54b64cfb3b8f5ad)
set(LUA_JIT_MD5_2.0.0 467f4f531f7e08ee252f5030ecada7ed)
set(LUA_JIT_MD5_2.0.0-beta11 824aa2684a11e3cc3abe87350a7b6139)
set(LUA_JIT_MD5_2.0.0-beta11 8629401437048e477c94bd791b0a823a)
set(LUA_JIT_MD5_2.0.0-beta10 ed66689b96f7ad7bfeffe0b4ff2d63d4)
set(LUA_JIT_MD5_2.0.0-beta10 f6bbd472726b761b29438c4a06b5ab3c)
set(LUA_JIT_MD5_2.0.0-beta9 e7e03e67e2550817358bc28b44270c6d)
set(LUA_JIT_MD5_2.0.0-beta9 3f9ca0309f26e789c6c3246c83696f84)
set(LUA_JIT_MD5_2.0.0-beta8 f0748a73ae268d49b1d01f56c4fe3e61)
set(LUA_JIT_MD5_2.0.0-beta8 fb096a90c9e799c8922f32095ef3d93c)
set(LUA_JIT_MD5_2.0.0-beta7 b845dec15dd9eba2fd17d865601a52e5)
set(LUA_JIT_MD5_2.0.0-beta7 e10fc2b19de52d8770cdf24f1791dd77)
set(LUA_JIT_MD5_2.0.0-beta6 bfcbe2a11162cfa84d5a1693b442c8bf)
set(LUA_JIT_MD5_2.0.0-beta6 f64945c5ecaf3ea71a829fdbb5cb196c)
set(LUA_JIT_MD5_2.0.0-beta5 7e0dfa03a140148149a1021d4ffd5c57)
set(LUA_JIT_MD5_2.0.0-beta5 be8087fcb576c30ffbb6368ebc284498)
set(LUA_JIT_MD5_2.0.0-beta4 5c5a9305b3e06765e1dae138e1a95c3a)
set(LUA_JIT_MD5_2.0.0-beta4 d0b241be6207fa7d97b6afc41368e05f)
set(LUA_JIT_MD5_2.0.0-beta3 313b6f164e93e1bbac7bf87abb58d4a1)
set(LUA_JIT_MD5_2.0.0-beta3 bea9c7bcd5084f98830e31956f276ff6)
set(LUA_JIT_MD5_2.0.0-beta2 2ebcc38fa1d9756dc2e341f191701120)
set(LUA_JIT_MD5_2.0.0-beta2 94086f99f647d46a8360adeb11851d66)
set(LUA_JIT_MD5_2.0.0-beta1 9ed7646d03580a1cec4abeb74ca44843)
set(LUA_JIT_MD5_2.0.0-beta1 19d5bac616fa739343c7158d9d99a3f3)
set(LUA_JIT_MD5_1.1.8 ad0e319483fa235e3979537a748631e9)
set(LUA_JIT_MD5_1.1.8 92870c80f504c34c9b7547cd6c5562d0)
set(LUA_JIT_MD5_1.1.7 3aed0795f7c8725d3613269cd56f8e5a)
set(LUA_JIT_MD5_1.1.7 40a8dbc214306bb4b9849fcf026c4ee0)
set(LUA_JIT_MD5_1.1.6 1a1320e09d0cd5b793014556fb7d64c9)
set(LUA_JIT_MD5_1.1.6 350d7b9230637056fbd6158b95e8fa11)
set(LUA_JIT_MD5_1.1.5 b99d244ba4fc1979946ae1025368fc5c)
set(LUA_JIT_MD5_1.1.5 d3ffbae3bfcd5914b02dc00b1118a59d)
set(LUA_JIT_MD5_1.1.4 9fe29cfb8126bc9c4302701c06965f1c)
set(LUA_JIT_MD5_1.1.4 30d318e3287000ecf4c93b29e8783183)
set(LUA_JIT_MD5_1.1.3 f5db1a147ed3d34677ad1ef310c56da7)
set(LUA_JIT_MD5_1.1.3 5949e7bce9d97c37c282e1cbe85aa378)
set(LUA_JIT_MD5_1.1.2 4ae25ce7e3f301d1fcf0b713016edab0)
set(LUA_JIT_MD5_1.1.0 16d880a98a1ff6608ac1039c802233db)
set(LUA_JIT_MD5_1.0.3 d0a63d5394cca549889bd820a05b32d2)

# Clean up some variables
string(TOLOWER ${LUA_VERSION} LUA_JIT_NORMALIZED_LUA_VERSION)
if (LUA_JIT_NORMALIZED_LUA_VERSION MATCHES "([0-9]+\\.[0-9]+\\.[0-9]+(-[A-Za-z0-9_-]+)?)")
	# 3-digit with optional beta1/beta2/beta3 (or whatever): probably okay?
	set(LUA_JIT_VERSION ${CMAKE_MATCH_1})
elseif (LUA_JIT_NORMALIZED_LUA_VERSION MATCHES "([0-9]+\\.[0-9]+)")
	# extend version number with prefix
	if (${CMAKE_MATCH_1} EQUAL 2)
		if (${CMAKE_MATCH_2} EQUAL 0)
			set(LUA_JIT_VERSION ${LUA_JIT_2.0_LATEST_VERSION})
		elseif (${CMAKE_MATCH_2} EQUAL 1)
			set(LUA_JIT_VERSION ${LUA_JIT_2.1_LATEST_VERSION})
		endif()
	endif()
	if (NOT LUA_JIT_VERSION)
		# Just pick a default version and roll with it
		set(LUA_JIT_VERSION ${CMAKE_MATCH_1}.${CMAKE_MATCH_2}.0)			
	endif()
else()
	message(FATAL "Cannot deduce LuaJIT version from ${LUA_VERSION}")
endif()

message(STATUS "Selecting LuaJIT ${LUA_JIT_VERSION}")

# Get hashes for the build
# LuaJIT unfortunately does not give us SHA1 hashes as well
# set(LUA_JIT_SHA1 ${LUA_JIT_SHA1_${LUA_JIT_VERSION}})
set(LUA_JIT_MD5 ${LUA_JIT_MD5_${LUA_JIT_VERSION}})

if (LUA_JIT_MD5)
	set(LUA_JIT_DOWNLOAD_MD5_COMMAND URL_MD5 ${LUA_JIT_MD5})
else ()
	set(LUA_JIT_DOWNLOAD_MD5_COMMAND "")
endif()
if (LUA_JIT_SHA1)
	set(LUA_JIT_DOWNLOAD_SHA1_COMMAND URL_HASH SHA1=${LUA_JIT_SHA1})
else ()
	set(LUA_JIT_DOWNLOAD_SHA1_COMMAND "")
endif()

# # # Do the build
if (MSVC)
	# Visual C++ is predicated off running msvcbuild.bat
	# which requires a Visual Studio Command Prompt

else ()
	# we can simply reuse the makefile here
	# so define it as an external project and then just have the proper
	# build/install/test commands
	
	set(LUA_JIT_BUILD_COMMAND make)
	set(LUA_JIT_INSTALL_COMMAND make install PREFIX=${LUA_LIB_DIR})
	set(LUA_JIT_TEST_COMMAND make test)
	set(LUA_JIT_LIB_FILE "${LUA_LIB_DIR}/${LUA_LIBRARY}")
	set(LUA_JIT_DLL_FILE "${LUA_BIN_DIR}/${LUA_DYNAMIC_LIBRARY}")
	if (BUILD_LUA_AS_DLL)
		set(LUA_JIT_BYPRODUCTS ${LUA_JIT_LIB_FILE} ${LUA_JIT_DLL_FILE})
	else()
		set(LUA_JIT_BYPRODUCTS ${LUA_JIT_LIB_FILE})
	endif()

	ExternalProject_Add(LUA_JIT
		BUILD_IN_SOURCE TRUE
		BUILD_ALWAYS TRUE
		# LuaJIT does not offer a TLS/SSL port
		# TLS_VERIFY TRUE
		PREFIX ${LUA_BUILD_TOPLEVEL}
		SOURCE_DIR ${LUA_BUILD_TOPLEVEL}
		DOWNLOAD_DIR ${LUA_BUILD_TOPLEVEL}
		TMP_DIR "${LUA_BUILD_TOPLEVEL}-tmp"
		STAMP_DIR "${LUA_BUILD_TOPLEVEL}-stamp"
		INSTALL_DIR "${LUA_BUILD_INSTALL_DIR}"
		URL http://luajit.org/download/LuaJIT-${LUA_JIT_VERSION}.tar.gz
		${LUA_JIT_DOWNLOAD_MD5_COMMAND}
		${LUA_JIT_DOWNLOAD_SHA1_COMMAND}
		CONFIGURE_COMMAND ""
		BUILD_COMMAND ${LUA_JIT_BUILD_COMMAND}
		INSTALL_COMMAND ${LUA_JIT_INSTALL_COMMAND}
		TEST_COMMAND ${LUA_JIT_TEST_COMMAND}
		BUILD_BYPRODUCTS ${LUA_JIT_BYPRODUCTS})

	# TODO: SHARED not fully supported yet, stock makefiles do not build
	# dynamic libs on linux out of the box...
	add_library(lualib_${LUA_JIT_VERSION} ${LUA_BUILD_LIBRARY_TYPE} IMPORTED)
	set_target_properties(lualib_${LUA_JIT_VERSION}
		PROPERTIES 
		LANGUAGE C
		LINKER_LANGUAGE C
		INCLUDE_DIRECTORIES ${LUA_INCLUDE_DIR}
		IMPORTED_LOCATION ${LUA_JIT_LIB_FILE}
		IMPORTED_SONAME ${LUA_JIT_DLL_FILE})
	
	# make sure the library we export really does depend on Lua
	add_dependencies(lualib_${LUA_JIT_VERSION} LUA_JIT)

	# set externally-visible target indicator
	set(LUA_LIBRARIES lualib_${LUA_JIT_VERSION})
endif()
