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

# # # # sol2
# # # Required minimum version statement
cmake_minimum_required(VERSION 3.5.0)

# # # project declaration
project(sol2 VERSION 2.19.0 LANGUAGES CXX C)

# # # General Project Requirements
# Set general standard requirements here
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Features a C++ compiler must have to be used to compile sol2
# This list is not *complete* as CMake does not support features for
# all of the advanced features utilized.
set(CXX_FEATURES
	cxx_auto_type
	cxx_constexpr
	cxx_decltype
	cxx_decltype_auto
	cxx_default_function_template_args
	cxx_final
	cxx_lambdas
	cxx_noexcept
	cxx_nullptr
	cxx_override
	cxx_range_for
	cxx_return_type_deduction
	cxx_right_angle_brackets
	cxx_static_assert
	cxx_strong_enums
	cxx_variadic_macros
	cxx_variadic_templates)

# # #
if (PLATFORM MATCHES "i686" OR PLATFORM STREQUAL "x86")
	set(IS_X86 TRUE)
elseif (PLATFORM MATCHES "ARM")
	set(IS_ARM TRUE)
else()
	set(IS_X64 TRUE)
endif()

# # # General project flags
if (MSVC)
	add_definitions(/DUNICODE /D_UNICODE /D_SILENCE_CXX17_UNCAUGHT_EXCEPTION_DEPRECATION_WARNING /D_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING /D_CRT_SECURE_NO_WARNINGS /D_CRT_SECURE_NO_DEPRECATE)
	# Warning level, exceptions
	add_compile_options(/W4 /EHsc)
	if (NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
		add_compile_options(/MP)
	endif()
else()
	if (IS_X86)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
	endif()
	add_compile_options(-Wno-unknown-warning -Wno-unknown-warning-option -Wall -Wextra -Wpedantic -pedantic -pedantic-errors)
endif()

if (CI)
	#if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	#	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lc++")
	#	add_compile_options("$<$<STREQUAL:$<TARGET_PROPERTY:LINKER_LANGUAGE>,CXX>:-stdlib=libc++>")
	#	LIST(APPEND CMAKE_LIBRARY_PATH_FLAG "$$ENV{CLANG_PREFIX}/lib")
	#	include_directories("$ENV{CLANG_PREFIX}/include/c++/v1")
	#endif()
endif()

# # # General project output locations
if (PLATFORM MATCHES "x86" OR CMAKE_SIZEOF_VOID_P EQUAL 4)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/x86/lib")
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/x86/bin")
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/x86/bin")
else()
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/x64/lib")
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/x64/bin")
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/x64/bin")
endif()

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
set(LUA_VERSION "5.3.4" CACHE STRING "The version of Lua needed. Can be 5.1, 5.2, 5.3, LuaJIT, or a more specific 3-part version number for a specifc Lua (e.g., 5.3.4 or luajit-2.0.5)")
set(BUILD_LUA TRUE CACHE BOOL "Always build Lua, do not search for it in the system")
set(PLATFORM "x64" CACHE STRING "Target platform to compile for when building binaries (x86, x64)")
option(CI "Enable build of tests" OFF)
option(TESTS "Enable build of tests" OFF)
option(EXAMPLES "Enable build of examples" OFF)
option(INTEROP_EXAMPLES "Enable build of interop examples" OFF)
option(SINGLE "Enable build of single header files" ON)
option(DOCS "Enable build of documentation" OFF)
# Single tests and examples tests will only be turned on if both SINGLE and TESTS are defined
CMAKE_DEPENDENT_OPTION(TESTS_SINGLE "Enable build of tests using the generated single headers" ON
	"SINGLE;TESTS" OFF)
CMAKE_DEPENDENT_OPTION(EXAMPLES_SINGLE "Enable build of examples using the generated single headers" OFF
	"SINGLE;EXAMPLES" OFF)
CMAKE_DEPENDENT_OPTION(INTEROP_EXAMPLES_SINGLE "Enable build of examples using the generated single headers" OFF
	"SINGLE;INTEROP_EXAMPLES" OFF)
CMAKE_DEPENDENT_OPTION(TESTS_EXAMPLES "Enable build of examples as tests" ON
	"EXAMPLES" OFF)
CMAKE_DEPENDENT_OPTION(TESTS_INTEROP_EXAMPLES "Enable build of interop examples as tests" ON
	"EXAMPLES;INTEROP_EXAMPLES" OFF)


# # # sol2 Library
# # Add a target for sol2's library to be included by external users
add_library(sol2 INTERFACE)
target_include_directories(sol2 INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	$<INSTALL_INTERFACE:include/sol>)

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

install(TARGETS sol2 
	EXPORT sol2)

install(EXPORT sol2
	FILE sol2-targets.cmake
	DESTINATION lib/cmake/sol2)

install(DIRECTORY include/sol2
	DESTINATION include)

install(FILES
	"${CMAKE_CURRENT_BINARY_DIR}/cmake/sol2-config.cmake"
	"${CMAKE_CURRENT_BINARY_DIR}/cmake/sol2-config-version.cmake"
	DESTINATION lib/cmake/sol2)

# # # Source Groups
# # Sources everyone is going to need
# Header files
file(GLOB SOL2_HEADER_SOURCES sol*.hpp)
source_group(headers FILES ${SOL2_HEADER_SOURCES})
# single header files
file(GLOB SOL2_SINGLE_HEADER_SOURCES single/sol/sol_forward.hpp single/sol/sol.hpp)
source_group(headers FILES ${SOL2_SINGLE_HEADER_SOURCES})

# # # Single header target
# Find Python3 for single header / forward header generation
find_package(PythonInterp 3)
set(SOL2_SINGLE_HEADER_FOUND FALSE)
set(SOL2_SINGLE_FOUND FALSE)
set(SOL2_DOCS_FOUND FALSE)
if (PYTHONINTERP_FOUND)
	if (SINGLE)
		set(SOL2_SINGLE_FOUND TRUE)
		add_custom_command(OUTPUT include/single/sol/sol.hpp "${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}/include/single/sol/sol_forward.hpp"
			COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_SOURCE_DIR}/single.py" --output "${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}/include/single/sol/sol.hpp"
			DEPENDS ${SOL2_HEADER_SOURCES})
		add_custom_target(sol2_single_header 
			DEPENDS "${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}/include/single/sol/sol.hpp" "${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}/include/single/sol/sol_forward.hpp")
		add_library(sol2_single INTERFACE)
		set_target_properties(sol2_single
			PROPERTIES
			INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}/include/single/sol")
		add_dependencies(sol2_single sol2_single_header)
		install(FILES "${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}/include/single/sol/sol.hpp" "${CMAKE_BINARY_DIR}/include/${CMAKE_CFG_INTDIR}/single/sol/sol_forward.hpp" 
			DESTINATION include/sol/single/sol)
	endif()
	if (DOCS)
		set(SOL2_DOCS_FOUND TRUE)
		add_custom_command(OUTPUT documentation COMMAND "make html" WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/docs)
		add_custom_target(docs DEPENDS documentation)
		install(DIRECTORY "${CMAKE_SOURCE_DIR}/docs/build/html" DESTINATION docs)
	endif()
else()
	if (SINGLE)
		message(STATUS "sol2 single_header cannot be generated as python 3 has not been found.")
	endif()
endif()

if (CI)
	message(STATUS "sol2 Contiguous Integration is on")
endif()

# # # Tests, Examples and other CI suites that come with sol2
if (TESTS OR TESTS_SINGLE OR TESTS_EXAMPLES OR TESTS_INTEROP_EXAMPLES OR EXAMPLES OR EXAMPLES_SINGLE OR INTEROP_EXAMPLES)
	# # # Libraries
	# Here, we pull in all the necessary libraries for building examples and tests
	# Find threading library
	if (NOT MSVC)
		set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
		set(THREADS_PREFER_PTHREAD_FLAG TRUE)
	endif()
	find_package(Threads REQUIRED)

	string(TOLOWER ${LUA_VERSION} NORMALIZED_LUA_VERSION)
	# Find way to get Lua: build if requested, or attempt to build if no matching version is found
	if (BUILD_LUA)
		find_package(LuaBuild REQUIRED COMPONENTS ${LUA_VERSION})
	elseif (NOT LUA_VERSION)
		find_package(LuaBuild REQUIRED)
	else()
		if (NORMALIZED_LUA_VERSION MATCHES "5.1")
			find_package(Lua 5.1 EXACT REQUIRED)
		elseif(NORMALIZED_LUA_VERSION MATCHES "5.2")
			find_package(Lua 5.2 EXACT REQUIRED)
		elseif(NORMALIZED_LUA_VERSION MATCHES "5.3")
			find_package(Lua 5.3 EXACT REQUIRED)
		elseif(NORMALIZED_LUA_VERSION MATCHES "luajit")
			find_package(LuaJIT REQUIRED)
		else()
			find_package(LuaBuild ${LUA_VERSION} REQUIRED)
		endif()
	endif()

	if (NOT LUA_FOUND AND NOT LUABUILD_FOUND)
		message(FATAL_ERROR "sol2 Lua \"${LUA_VERSION}\" not found and could not be targeted for building")
	endif()

	# # Enable test harness for regular, example or single tests
	if (TESTS OR TESTS_SINGLE OR TESTS_EXAMPLES OR TESTS_INTEROP_EXAMPLES)
		# enable ctest
		message(STATUS "sol2 testing enabled")
		enable_testing()
	endif()
	
	# # # Examples
	# # Enable examples to be built against the library
	if (EXAMPLES OR TESTS_EXAMPLES OR EXAMPLES_SINGLE OR INTEROP_EXAMPLES OR INTEROP_EXAMPLES_SINGLE OR TEST_INTEROP_EXAMPLES)
		# NOTE: will also add to tests if TESTS is defined
		message(STATUS "sol2 adding examples...")
		add_subdirectory(examples "${CMAKE_BINARY_DIR}/examples")
	endif()

	# # # Tests
	# # Add tests here
	if (TESTS OR TESTS_SINGLE)
		# add subdir to get going
		message(STATUS "sol2 adding tests...")
		add_subdirectory(tests "${CMAKE_BINARY_DIR}/tests")
	endif()

endif()
