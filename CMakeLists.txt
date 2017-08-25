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
  target_link_libraries(${example_name} ${LUA_LIBRARIES})
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
target_link_libraries(tests Threads::Threads ${LUA_LIBRARIES})
install(TARGETS tests RUNTIME DESTINATION bin)

set(HEADER_SRCS sol/as_args.hpp
                sol/as_returns.hpp
                sol/bind_traits.hpp
                sol/call.hpp
                sol/compatibility/5.0.0.h
                sol/compatibility/5.1.0.h
                sol/compatibility/5.2.0.h
                sol/compatibility/5.x.x.h
                sol/compatibility/5.x.x.inl
                sol/compatibility/version.hpp
                sol/compatibility.hpp
                sol/container_traits.hpp
                sol/container_usertype_metatable.hpp
                sol/coroutine.hpp
                sol/debug.hpp
                sol/demangle.hpp
                sol/deprecate.hpp
                sol/environment.hpp
                sol/error.hpp
                sol/feature_test.hpp
                sol/filters.hpp
                sol/forward_detail.hpp
                sol/forward.hpp
                sol/function.hpp
                sol/function_result.hpp
                sol/function_types_core.hpp
                sol/function_types.hpp
                sol/function_types_overloaded.hpp
                sol/function_types_stateful.hpp
                sol/function_types_stateless.hpp
                sol/function_types_templated.hpp
                sol/inheritance.hpp
                sol/in_place.hpp
                sol/load_result.hpp
                sol/object_base.hpp
                sol/object.hpp
                sol/optional.hpp
                sol/optional_implementation.hpp
                sol/overload.hpp
                sol/property.hpp
                sol/protected_function.hpp
                sol/protected_function_result.hpp
                sol/protect.hpp
                sol/proxy_base.hpp
                sol/proxy.hpp
                sol/raii.hpp
                sol/reference.hpp
                sol/resolve.hpp
                sol/simple_usertype_metatable.hpp
                sol/stack_check_get.hpp
                sol/stack_check.hpp
                sol/stack_core.hpp
                sol/stack_field.hpp
                sol/stack_get.hpp
                sol/stack_guard.hpp
                sol/stack.hpp
                sol/stack_pop.hpp
                sol/stack_probe.hpp
                sol/stack_proxy.hpp
                sol/stack_push.hpp
                sol/stack_reference.hpp
                sol/state.hpp
                sol/state_view.hpp
                sol/string_shim.hpp
                sol/table_core.hpp
                sol/table.hpp
                sol/table_iterator.hpp
                sol/thread.hpp
                sol/tie.hpp
                sol/traits.hpp
                sol/tuple.hpp
                sol/types.hpp
                sol/unsafe_function.hpp
                sol/userdata.hpp
                sol/usertype_core.hpp
                sol/usertype.hpp
                sol/usertype_metatable.hpp
                sol/usertype_traits.hpp
                sol/variadic_args.hpp
                sol/variadic_results.hpp
                sol/wrapper.hpp)

find_package(PythonInterp 3)
if (PYTHONINTERP_FOUND)
  add_custom_command(OUTPUT sol.hpp COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/single.py --output ${CMAKE_CURRENT_BINARY_DIR}/sol.hpp DEPENDS ${HEADER_SRCS})
  add_custom_target(single_sol DEPENDS sol.hpp)
  install(FILES "${CMAKE_CURRENT_BINARY_DIR}/sol.hpp" DESTINATION include)
  message(STATUS "single_sol can be generated as python 3 has been found.")
else()
  message(STATUS "single_sol cannot be generated as python 3 has not been found.")
endif()
