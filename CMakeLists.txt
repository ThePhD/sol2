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
cmake_minimum_required(VERSION 3.5.0)
project(sol2 VERSION 2.19.0 LANGUAGES CXX)


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

# Add home-rolled modules path to front of module path list
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules" "${CMAKE_MODULE_PATH}")

# Include standard modules
include(CMakeDependentOption)
include(CMakePackageConfigHelpers)


# # # Configuration
# # Cached defines, strings, paths and options
set(LUA_VERSION "5.3.4" CACHE STRING "The version of Lua needed. Can be 5.1, 5.2, 5.3, LuaJIT, or a more specific 3-part version number for plain lua (e.g., 5.3.4 or luajit-2.0.5)")
set(BUILD_LUA TRUE CACHE BOOL "Always build Lua, do not search for it in the system")
option(TESTS "Enable build of tests" OFF)
option(EXAMPLES "Enable build of examples" OFF)
option(SINGLE "Enable build of single header files" ON)
# Single tests will only be turned on if both SINGLE and TESTS are defined
CMAKE_DEPENDENT_OPTION(TESTS_SINGLE "Enable build of tests using the generated single headers" ON
	"SINGLE;TESTS" OFF)
option(DOCS "Enable build of documentation" OFF)


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

install( FILES
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

# # # Libraries
# Here, we pull in all the necessary libraries
# Find threading library
find_package(Threads)
# Find way to get Lua: build if requested, or attempt to build if no matching version is found
string(TOLOWER ${LUA_VERSION} LUA_VERSION)
if (BUILD_LUA)
	include(LuaBuild)
elseif (LUA_VERSION MATCHES "5.1")
	find_package(Lua 5.1 EXACT REQUIRED)
elseif(LUA_VERSION MATCHES "5.2")
	find_package(Lua 5.2 EXACT REQUIRED)
elseif(LUA_VERSION MATCHES "5.3")
	find_package(Lua 5.3 EXACT REQUIRED)
elseif(LUA_VERSION MATCHES "luajit")
	find_package(LuaJIT REQUIRED)
else()
	include(LuaBuild)	
endif()

if (NOT LUA_FOUND)
	message(FATAL_ERROR "Lua ${LUA_VERSION} not found and could not be targeted for building.")
endif()

# Find Python3 for single header / forward header generation
find_package(PythonInterp 3)
if (PYTHONINTERP_FOUND)
	if (SINGLE)
		add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/single/sol/sol.hpp" "${CMAKE_BINARY_DIR}/single/sol/sol_forward.hpp"
			COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_SOURCE_DIR}/single.py" --output "${CMAKE_BINARY_DIR}/single/sol/sol.hpp"
			DEPENDS ${HEADER_SRCS})
		add_custom_target(single_header 
			DEPENDS "${CMAKE_BINARY_DIR}/single/sol/sol.hpp" "${CMAKE_BINARY_DIR}/single/sol/sol_forward.hpp")
		set_target_properties(single_header
			PROPERTIES
			INCLUDE_DIRECTORIES "${CMAKE_BINARY_DIR}/single/sol")
		install(FILES "${CMAKE_BINARY_DIR}/single/sol/sol.hpp" "${CMAKE_BINARY_DIR}/single/sol/sol_forward.hpp" 
			DESTINATION include/sol)
	endif()
	if (DOCS)
		add_custom_command(OUTPUT documentation COMMAND "make html" WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/docs)
		add_custom_target(docs DEPENDS documentation)
		install(DIRECTORY "${CMAKE_SOURCE_DIR}/docs/build/html" DESTINATION bin/docs)
	endif()
else()
	if (SINGLE)
		message(STATUS "single_header cannot be generated as python 3 has not been found.")
	endif()
	if (DOCS)
		message(STATUS "documentation cannot be generated as python 3 has not been found.")
	endif()
endif()

# # # Tests
# # Enable test harness for regular or single tests
if (TESTS OR TESTS_SINGLE)
	# enable ctest and add subdir to get going
	enable_testing()
	add_subdirectory(tests "${CMAKE_BINARY_DIR}/tests")
endif()

# # # Examples
# # Enable examples to be built against the library
if (EXAMPLES)
	# NOTE: will also add to tests if TESTS is defined
	add_subdirectory(examples "${CMAKE_BINARY_DIR}/examples")
endif()
