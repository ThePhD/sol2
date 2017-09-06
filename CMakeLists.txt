cmake_minimum_required(VERSION 3.0)

project(sol2)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include_directories(.)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules" "${CMAKE_MODULE_PATH}")

set(LUA_VERSION "5.3" CACHE STRING "The version of Lua needed (5.1, 5.2, 5.3, LuaJIT)")
string(TOLOWER ${LUA_VERSION} LUA_VERSION)
if (LUA_VERSION MATCHES "5.1")
  find_package(Lua 5.1 EXACT REQUIRED)
elseif(LUA_VERSION MATCHES "5.2")
  find_package(Lua 5.2 EXACT REQUIRED)
elseif(LUA_VERSION MATCHES "5.3")
  find_package(Lua 5.3 EXACT REQUIRED)
elseif(LUA_VERSION MATCHES "luajit")
  find_package(LuaJIT REQUIRED)
else()
  message(FATAL_ERROR "${LUA_VERSION} is not a supported version for lua.")
endif()

option(TESTS "Enable build of tests" ON)
option(EXAMPLES "Enable build of examples" ON)

include_directories(${LUA_INCLUDE_DIR})

# Features a C++ compiler must have to be used to compile sol2
# This list is not *complete* as CMake does not support features for
# all of the advanced features utilized.
set(CXX_FEATURES  cxx_auto_type
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

file(GLOB HEADER_SRCS sol/*.hpp)
set(HEADER_SRCS sol.hpp ${HEADER_SRCS})
source_group(headers FILES ${HEADER_SRCS})

if (NOT MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-warning -Wno-unknown-warning-option -Wall -Wextra -Wpedantic -pedantic -pedantic-errors -Wno-noexcept-type -ftemplate-depth=1024")
endif()

if (EXAMPLES)
  file(GLOB EXAMPLES_SRC examples/*.cpp)
  source_group(examples FILES ${EXAMPLES_SRC})
  foreach(example_source_file ${EXAMPLES_SRC})
    get_filename_component(example_name ${example_source_file} NAME_WE)
    set(example_name "example-${example_name}")
    add_executable(${example_name} ${example_source_file} ${HEADER_SRCS})
    target_link_libraries(${example_name} ${LUA_LIBRARIES})
    target_compile_features(${example_name} PUBLIC ${CXX_FEATURES})
    install(TARGETS ${example_name} RUNTIME DESTINATION bin/examples)
  endforeach()
endif()

if (TESTS)
  file(GLOB TEST_SRC tests/test*.cpp)
  source_group(tests FILES ${TEST_SRC})

  add_executable(tests ${TEST_SRC} ${HEADER_SRCS})
  target_include_directories(tests PRIVATE ./Catch/include/)

  find_package(Threads)
  target_link_libraries(tests Threads::Threads ${LUA_LIBRARIES})
  install(TARGETS tests RUNTIME DESTINATION bin)
endif()

find_package(PythonInterp 3)
if (PYTHONINTERP_FOUND)
  add_custom_command(OUTPUT single/sol/sol.hpp COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/single.py --output ${CMAKE_CURRENT_SOURCE_DIR}/single/sol/sol.hpp DEPENDS ${HEADER_SRCS})
  add_custom_target(single_sol DEPENDS single/sol/sol.hpp)
  install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/single/sol/sol.hpp" DESTINATION include/sol)
  message(STATUS "single_sol can be generated as python 3 has been found.")
else()
  message(STATUS "single_sol cannot be generated as python 3 has not been found.")
endif()
