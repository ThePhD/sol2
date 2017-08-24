cmake_minimum_required(VERSION 3.0)

project(sol2)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include_directories(.)

set(LUA_VERSION "5.3" CACHE STRING "The version of Lua needed (5.1, 5.2, 5.3)")
if (LUA_VERSION MATCHES "5.1")
  Find_Package(Lua 5.1 EXACT REQUIRED)
elseif(LUA_VERSION MATCHES "5.2")
  Find_Package(Lua 5.2 EXACT REQUIRED)
elseif(NOT LUA_VERSION OR LUA_VERSION MATCHES "5.3")
  Find_Package(Lua 5.3 EXACT REQUIRED)
endif()

include_directories(${LUA_INCLUDE_DIRS})

file(GLOB EXAMPLES_SRC examples/*.cpp)
source_group(examples FILES ${EXAMPLES_SRC})
foreach(example_source_file ${EXAMPLES_SRC})
message(STATUS "${example_source_file}")
  get_filename_component(example_name ${example_source_file} NAME_WE)
  add_executable(${example_name} ${example_source_file})
  target_link_libraries(${example_name} ${LUA_LIBRARY})
  install(TARGETS ${example_name} RUNTIME DESTINATION bin/examples)
endforeach()

set(TEST_SRC test_state.cpp test_operators.cpp
             test_tables.cpp test_utility.cpp
             test_strings.cpp test_environments.cpp
             test_customizations.cpp test_large_integer.cpp
             test_inheritance.cpp tests.cpp
             test_variadics.cpp test_coroutines.cpp
             test_container_semantics.cpp test_storage.cpp
             test_overflow.cpp test_plain_types.cpp
             test_simple_usertypes.cpp test_gc.cpp
             test_functions.cpp test_usertypes.cpp
             test_containers.cpp test_filters.cpp
)
source_group(Tests FILES ${TEST_SRC})

add_executable(tests ${TEST_SRC})
target_include_directories(tests PRIVATE ./Catch/include/)

find_package(Threads)
target_link_libraries(tests Threads::Threads ${LUA_LIBRARY})
install(TARGETS tests RUNTIME DESTINATION bin)

install(FILES "single/sol/sol.hpp" DESTINATION include)
