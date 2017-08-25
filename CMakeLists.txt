cmake_minimum_required(VERSION 3.0)

project(sol2)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include_directories(.)

set(LUA_VERSION "5.3" CACHE STRING "The version of Lua needed (5.1, 5.2, 5.3)")
if (LUA_VERSION MATCHES "5.1")
  find_package(Lua 5.1 EXACT REQUIRED)
elseif(LUA_VERSION MATCHES "5.2")
  find_package(Lua 5.2 EXACT REQUIRED)
elseif(LUA_VERSION MATCHES "5.3")
  find_package(Lua 5.3 EXACT REQUIRED)
elseif(LUA_VERSION MATCHES "LuaJIT")
  find_package(LuaJIT REQUIRED)
else()
  message(FATAL_ERROR "${LUA_VERSION} is not a supported version for lua.")
endif()

include_directories(${LUA_INCLUDE_DIR})

if (NOT MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-warning -Wno-unknown-warning-option -Wall -Wextra -Wpedantic -pedantic -pedantic-errors -Wno-noexcept-type -ftemplate-depth=1024")
endif()

set(EXAMPLES_SRC examples/any_return.cpp
                 examples/basic.cpp
                 examples/calling_lua_functions.cpp
                 examples/config.cpp
                 examples/containers_as_table.cpp
                 examples/containers.cpp
                 examples/container_usertype_as_container.cpp
                 examples/coroutine.cpp
                 examples/customization.cpp
                 examples/dynamic_object.cpp
                 examples/environments.cpp
                 examples/environment_snooping.cpp
                 examples/environments_on_functions.cpp
                 examples/functions.cpp
                 examples/multi_results.cpp
                 examples/namespacing.cpp
                 examples/optional_with_iteration.cpp
                 examples/overloading.cpp
                 examples/overloading_with_fallback.cpp
                 examples/protected_functions.cpp
                 examples/require.cpp
                 examples/runtime_additions.cpp
                 examples/script_error_handling.cpp
                 examples/self_call.cpp
                 examples/self_from_lua.cpp
                 examples/stack_aligned_function.cpp
                 examples/static_variables.cpp
                 examples/tables.cpp
                 examples/usertype_advanced.cpp
                 examples/usertype_automatic_operators.cpp
                 examples/usertype_bitfields.cpp
                 examples/usertype_call_from_c++.cpp
                 examples/usertype.cpp
                 examples/usertype_dynamic_getter_setter.cpp
                 examples/usertype_initializers.cpp
                 examples/usertype_simple.cpp
                 examples/usertype_special_functions.cpp
                 examples/usertype_var.cpp
                 examples/variables.cpp
                 examples/variadic_args.cpp)
source_group(EXAMPLES FILES ${EXAMPLES_SRC})
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
source_group(TESTS FILES ${TEST_SRC})

add_executable(tests ${TEST_SRC})
target_include_directories(tests PRIVATE ./Catch/include/)

find_package(Threads)
target_link_libraries(tests Threads::Threads ${LUA_LIBRARY})
install(TARGETS tests RUNTIME DESTINATION bin)

install(FILES "single/sol/sol.hpp" DESTINATION include)
