# # # # sol3
# The MIT License (MIT)
# 
# Copyright (c) 2013-2020 Rapptz, ThePhD, and contributors
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

# # # # sol3
# # # Required minimum version statement
cmake_minimum_required(VERSION 3.5.0)

# # # project declaration
project(sol2 VERSION 3.2.3 LANGUAGES CXX C)

include(GNUInstallDirs)

# # # Modules
# # Include modules useful to the project, whether locally made in our own cmake DIRECTORY
# # our from the standard cmake libraries
# Add home-rolled modules path to front of module path list
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules" "${CMAKE_MODULE_PATH}")

# Include standard modules
include(CMakeDependentOption)
include(CMakePackageConfigHelpers)


# # # Configuration
# # Cached defines, strings, paths and options
set(SOL2_LUA_VERSION "5.3.5" CACHE STRING "The version of Lua needed. Can be 5.1, 5.2, 5.3, 5.4, LuaJIT, or a more specific 3-part version number for a specifc Lua (e.g., 5.3.4 or luajit-2.0.5)")
set(SOL2_BUILD_LUA TRUE CACHE BOOL "Always build Lua, do not search for it in the system")
set(SOL2_PLATFORM "x64" CACHE STRING "Target platform to compile for when building binaries (x86, x64)")
option(SOL2_CI "Whether or not we are in continguous integration mode" OFF)
option(SOL2_TESTS "Enable build of tests" OFF)
option(SOL2_EXAMPLES "Enable build of examples" OFF)
option(SOL2_INTEROP_EXAMPLES "Enable build of interop examples" OFF)
option(SOL2_DYNAMIC_LOADING_EXAMPLES "Enable build of interop examples" OFF)
option(SOL2_GENERATE_SINGLE "Enable generation and build of single header files" OFF)
option(SOL2_SINGLE "Enable use of prepackaged single header files" OFF)
option(SOL2_DOCS "Enable build of documentation" OFF)
option(SOL2_ENABLE_INSTALL "Enable installation of Sol2" ON)
# Single tests and examples tests will only be turned on if both SINGLE and TESTS are defined
CMAKE_DEPENDENT_OPTION(SOL2_TESTS_SINGLE "Enable build of tests using the premade single headers" ON
	"SOL2_SINGLE;SOL2_TESTS" OFF)
CMAKE_DEPENDENT_OPTION(SOL2_TESTS_SINGLE_GENERATED "Enable build of tests using the generated single headers" ON
	"SOL2_GENERATE_SINGLE;SOL2_TESTS" OFF)
CMAKE_DEPENDENT_OPTION(SOL2_EXAMPLES_SINGLE "Enable build of examples using the generated single headers" OFF
	"SOL2_SINGLE;SOL2_EXAMPLES" OFF)
CMAKE_DEPENDENT_OPTION(SOL2_EXAMPLES_SINGLE_GENERATED "Enable build of examples using the premade single headers" OFF
	"SOL2_GENERATE_SINGLE;SOL2_EXAMPLES" OFF)
CMAKE_DEPENDENT_OPTION(SOL2_INTEROP_EXAMPLES_SINGLE "Enable build of interop examples using the generated single headers" OFF
	"SOL2_SINGLE;SOL2_INTEROP_EXAMPLES" OFF)
CMAKE_DEPENDENT_OPTION(SOL2_INTEROP_EXAMPLES_SINGLE_GENERATED "Enable build of interop examples using the generated single headers" OFF
	"SOL2_GENERATE_SINGLE;SOL2_INTEROP_EXAMPLES" OFF)
CMAKE_DEPENDENT_OPTION(SOL2_DYNAMIC_LOADING_EXAMPLES_SINGLE "Enable build of dynamic loading examples using the generated single headers" OFF
	"SOL2_SINGLE;SOL2_DYNAMIC_LOADING_EXAMPLES" OFF)
CMAKE_DEPENDENT_OPTION(SOL2_DYNAMIC_LOADING_EXAMPLES_SINGLE_GENERATED "Enable build of dynamic loading examples using the generated single headers" OFF
	"SOL2_GENERATE_SINGLE;SOL2_DYNAMIC_LOADING_EXAMPLES" OFF)
CMAKE_DEPENDENT_OPTION(SOL2_TESTS_EXAMPLES "Enable build of examples as tests" ON
	"SOL2_EXAMPLES" OFF)
CMAKE_DEPENDENT_OPTION(SOL2_TESTS_INTEROP_EXAMPLES "Enable build of interop examples as tests" ON
	"SOL2_INTEROP_EXAMPLES" OFF)
CMAKE_DEPENDENT_OPTION(SOL2_TESTS_DYNAMIC_LOADING_EXAMPLES "Enable build of dynamic loading examples as tests" ON
	"SOL2_DYNAMIC_LOADING_EXAMPLES" OFF)
CMAKE_DEPENDENT_OPTION(BUILD_LUA_AS_DLL "Build Lua as a DLL" ON
	"SOL2_BUILD_LUA" OFF)


# # # Platform
# Detect x86 and x64 stuff
if (SOL2_PLATFORM MATCHES "i686" OR SOL2_PLATFORM STREQUAL "x86")
	set(SOL2_IS_X86 TRUE)
elseif (SOL2_PLATFORM MATCHES "ARM64")
	set(IS_ARM64 TRUE)
	set(IS_X64 TRUE)
elseif (SOL2_PLATFORM MATCHES "ARM")
	set(IS_ARM TRUE)
elseif (SOL2_PLATFORM MATCHES "x86_64" OR SOL2_PLATFORM STREQUAL "x64")
	set(IS_X64 TRUE)
else()
	set(IS_X64 TRUE)
endif()

if (PROJECT_SOURCE_DIR STREQUAL ${CMAKE_SOURCE_DIR})
	set(SOL2_IS_TOP_LEVEL TRUE)
	message(STATUS "sol2 is the top-level directory...")
endif()

# # # sol2 Source Groups
# # Sources everyone is going to need
# Header files
file(GLOB SOL2_HEADER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/include/sol*.hpp)
source_group(sol2 FILES ${SOL2_HEADER_SOURCES})

# # # sol2 Library
# # Add a target for sol2's library to be included by external users
add_library(sol2 INTERFACE)
add_library(sol2::sol2 ALIAS sol2)
target_sources(sol2 INTERFACE ${SOL2_SINGLE_HEADER_SOURCES})
set_target_properties(sol2
	PROPERTIES
	EXPORT_NAME sol2::sol2)

target_include_directories(sol2 INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>)

# # Version configurations
configure_package_config_file(
	cmake/sol2-config.cmake.in
	"${CMAKE_CURRENT_BINARY_DIR}/cmake/sol2-config.cmake"
	INSTALL_DESTINATION lib/cmake/sol2
	NO_CHECK_REQUIRED_COMPONENTS_MACRO)

write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/cmake/sol2-config-version.cmake"
	COMPATIBILITY AnyNewerVersion)

export(TARGETS sol2 FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/sol2-targets.cmake")

if(SOL2_ENABLE_INSTALL)
	install(TARGETS sol2
		EXPORT sol2)

	install(EXPORT sol2
		FILE sol2-targets.cmake
		DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/sol2")

	install(DIRECTORY include/sol
		DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

	install(FILES
		"${CMAKE_CURRENT_BINARY_DIR}/cmake/sol2-config.cmake"
		"${CMAKE_CURRENT_BINARY_DIR}/cmake/sol2-config-version.cmake"
		DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/sol2")
endif()

# # # Single header target
# Find Python3 for single header / forward header generation
if (SOL2_GENERATE_SINGLE OR SOL2_SINGLE)
	message(STATUS "sol2 adding single...")
	add_subdirectory(single)
endif()

# # # documentation
# Generates the docs
if (SOL2_DOCS)
	message(STATUS "sol2 adding docs...")
	add_subdirectory(docs)
endif()

if(SOL2_ENABLE_INSTALL)
	# pkg-config support, except on Windows
	if(NOT WIN32 OR NOT CMAKE_HOST_SYSTEM_NAME MATCHES Windows)
		set(PKGCONFIG_INSTALL_DIR
			"${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig"
			CACHE PATH "Path where sol2.pc is installed")

		configure_file("${CMAKE_CURRENT_SOURCE_DIR}/sol2.pc.in" "${CMAKE_CURRENT_BINARY_DIR}/sol2.pc" @ONLY)
		install(FILES "${CMAKE_CURRENT_BINARY_DIR}/sol2.pc" DESTINATION "${PKGCONFIG_INSTALL_DIR}")
	endif()
endif()

if (SOL2_CI)
	message(STATUS "sol2 Contiguous Integration is on")
endif()

if (SOL2_EXAMPLES OR SOL2_TESTS_EXAMPLES OR SOL2_EXAMPLES_SINGLE OR SOL2_INTEROP_EXAMPLES OR SOL2_TESTS_INTEROP_EXAMPLES OR SOL2_INTEROP_EXAMPLES_SINGLE OR SOL2_DYNAMIC_LOADING_EXAMPLES OR SOL2_TESTS_DYNAMIC_LOADING_EXAMPLES OR SOL2_DYNAMIC_LOADING_EXAMPLES_SINGLE)
	set(SOL2_DO_EXAMPLES TRUE)
else()
	set(SOL2_DO_EXAMPLES FALSE)
endif()

if (SOL2_TESTS OR SOL2_TESTS_SINGLE)
	set(SOL2_DO_TESTS TRUE)
else()
	set(SOL2_DO_TESTS FALSE)
endif()

# # # Tests, Examples and other CI suites that come with sol2
if (SOL2_IS_TOP_LEVEL AND (SOL2_DO_TESTS OR SOL2_DO_EXAMPLES))
	# # # General project output locations
	if (SOL2_IS_X86 OR CMAKE_SIZEOF_VOID_P EQUAL 4)
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x86/lib")
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x86/bin")
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x86/bin")
	else()
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x64/lib")
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x64/bin")
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x64/bin")
	endif()

	# # # Libraries
	# Here, we pull in all the necessary libraries for building examples and tests
	# Find threading library
	if (NOT MSVC)
		if (SOL2_IS_X86)
			set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
			set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -m32")
			set(CMAKE_EXECUTABLE_LINKER_FLAGS "${CMAKE_EXECUTABLE_LINKER_FLAGS} -m32")
		endif()
		set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
		set(THREADS_PREFER_PTHREAD_FLAG TRUE)
	else()
		string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_C_FLAGS ${CMAKE_C_FLAGS})
		string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
		if (BUILD_LUA_AS_DLL)
			string(REGEX REPLACE "/MT" "/MD" CMAKE_C_FLAGS ${CMAKE_C_FLAGS})
			string(REGEX REPLACE "/MT" "/MD" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
		else ()
			string(REGEX REPLACE "/MD" "/MT" CMAKE_C_FLAGS ${CMAKE_C_FLAGS})
			string(REGEX REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
		endif()
	endif()
	find_package(Threads REQUIRED)

	string(TOLOWER ${SOL2_LUA_VERSION} NORMALIZED_LUA_VERSION)
	# Find way to get Lua: build if requested, or attempt to build if no matching version is found
	if (SOL2_BUILD_LUA)
		find_package(LuaBuild REQUIRED COMPONENTS ${SOL2_LUA_VERSION})
	elseif (NOT SOL2_LUA_VERSION)
		find_package(LuaBuild REQUIRED)
	else ()
		if (NORMALIZED_LUA_VERSION MATCHES "5.1")
			set(CREATE_LUALIB_TARGET TRUE)
			find_package(Lua 5.1 EXACT REQUIRED)
		elseif(NORMALIZED_LUA_VERSION MATCHES "5.2")
			set(CREATE_LUALIB_TARGET TRUE)
			find_package(Lua 5.2 EXACT REQUIRED)
		elseif(NORMALIZED_LUA_VERSION MATCHES "5.3")
			set(CREATE_LUALIB_TARGET TRUE)
			find_package(Lua 5.3 EXACT REQUIRED)
		elseif(NORMALIZED_LUA_VERSION MATCHES "5.4")
			set(CREATE_LUALIB_TARGET TRUE)
			find_package(Lua 5.4 EXACT REQUIRED)
		elseif(NORMALIZED_LUA_VERSION MATCHES "luajit")
			set(CREATE_LUALIB_TARGET TRUE)
			find_package(LuaJIT REQUIRED)
		else()
			find_package(LuaBuild ${SOL2_LUA_VERSION} REQUIRED)
		endif()
	endif()

	if (CREATE_LUALIB_TARGET AND LUA_FOUND)
		set(lualib lua_imported_lib_${SOL2_LUA_VERSION})
		foreach(lua_search_lib ${LUA_LIBRARIES})
			get_filename_component(lsl_fname ${lua_search_lib} NAME)
			if (lsl_fname MATCHES "lua" OR lsl_fname MATCHES "Lua" OR lsl_fname MATCHES "LUA")
				if (lsl_fname MATCHES "\.so|\.dylib|\.dll")
					set(lualibtype SHARED)
					set(lualiblocation ${lua_search_lib})
				else()
					set(lualibtype STATIC)
					set(lualiblocation ${lua_search_lib})
				endif()
			else()
				set(LUA_SEARCH_DEPENDENCY_LIBS ${LUA_SEARCH_DEPENDENCY_LIBS} "${lua_search_lib}")
			endif()
		endforeach()
		add_library(${lualib} ${lualibtype} IMPORTED)
		set_target_properties(${lualib}
			PROPERTIES 
			INTERFACE_INCLUDE_DIRECTORIES ${LUA_INCLUDE_DIR}
			INTERFACE_LINK_LIBRARIES ${LUA_SEARCH_DEPENDENCY_LIBS}
			IMPORTED_LINK_INTERFACE_LANGUAGES C
			IMPORTED_LOCATION ${lualiblocation})
		if (CMAKE_DL_LIBS)
			set_property(TARGET ${lualib}
				APPEND PROPERTY INTERFACE_LINK_LIBRARIES ${CMAKE_DL_LIBS})
		endif()
		set(LUA_LIBRARIES ${lualib})
	endif()

	if (NOT LUA_FOUND AND NOT LUABUILD_FOUND)
		message(FATAL_ERROR "sol2 Lua \"${SOL2_LUA_VERSION}\" not found and could not be targeted for building")
	endif()
	
	# # Enable test harness for regular, example or single tests
	if (SOL2_DO_TESTS OR (SOL2_TESTS_EXAMPLES AND SOL2_DO_EXAMPLES))
		# enable ctest
		message(STATUS "sol2 testing enabled...")
		enable_testing()
	endif()
	
	# # # Examples
	# # Enable examples to be built against the library
	if (SOL2_DO_EXAMPLES)
		# NOTE: will also add to tests if TESTS is defined
		message(STATUS "sol2 adding examples...")
		add_subdirectory(examples "${CMAKE_BINARY_DIR}/examples")
	endif()
	
	# # # Tests
	# # Add tests here
	if (SOL2_DO_TESTS)
		# add subdir to get going
		message(STATUS "sol2 adding tests...")
		add_subdirectory(tests "${CMAKE_BINARY_DIR}/tests")
	endif()
endif()
