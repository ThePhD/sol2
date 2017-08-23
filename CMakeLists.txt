cmake_minimum_required(VERSION 3.0)

project(sol2)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include_directories(.)

add_executable(containers examples/containers.cpp)
target_link_libraries(containers lua)
add_executable(tables examples/tables.cpp)
target_link_libraries(tables lua)
add_executable(environments_on_functions examples/environments_on_functions.cpp)
target_link_libraries(environments_on_functions lua)
add_executable(variables examples/variables.cpp)
target_link_libraries(variables lua)
add_executable(usertype_bitfields examples/usertype_bitfields.cpp)
target_link_libraries(usertype_bitfields lua)
add_executable(protected_functions examples/protected_functions.cpp)
target_link_libraries(protected_functions lua)
add_executable(usertype_dynamic_getter_setter examples/usertype_dynamic_getter_setter.cpp)
target_link_libraries(usertype_dynamic_getter_setter lua)
add_executable(environment_snooping examples/environment_snooping.cpp)
target_link_libraries(environment_snooping lua)
add_executable(container_usertype_as_container examples/container_usertype_as_container.cpp)
target_link_libraries(container_usertype_as_container lua)
add_executable(multi_results examples/multi_results.cpp)
target_link_libraries(multi_results lua)
add_executable(self_call examples/self_call.cpp)
target_link_libraries(self_call lua)
add_executable(any_return examples/any_return.cpp)
target_link_libraries(any_return lua)
add_executable(usertype_advanced examples/usertype_advanced.cpp)
target_link_libraries(usertype_advanced lua)
add_executable(usertype_initializers examples/usertype_initializers.cpp)
target_link_libraries(usertype_initializers lua)
add_executable(namespacing examples/namespacing.cpp)
target_link_libraries(namespacing lua)
add_executable(overloading_with_fallback examples/overloading_with_fallback.cpp)
target_link_libraries(overloading_with_fallback lua)
add_executable(self_from_lua examples/self_from_lua.cpp)
target_link_libraries(self_from_lua lua)
add_executable(usertype_simple examples/usertype_simple.cpp)
target_link_libraries(usertype_simple lua)
add_executable(runtime_additions examples/runtime_additions.cpp)
target_link_libraries(runtime_additions lua)
add_executable(usertype_call_from_c++ examples/usertype_call_from_c++.cpp)
target_link_libraries(usertype_call_from_c++ lua)
add_executable(static_variables examples/static_variables.cpp)
target_link_libraries(static_variables lua)
add_executable(script_error_handling examples/script_error_handling.cpp)
target_link_libraries(script_error_handling lua)
add_executable(variadic_args examples/variadic_args.cpp)
target_link_libraries(variadic_args lua)
add_executable(coroutine examples/coroutine.cpp)
target_link_libraries(coroutine lua)
add_executable(calling_lua_functions examples/calling_lua_functions.cpp)
target_link_libraries(calling_lua_functions lua)
add_executable(customization examples/customization.cpp)
target_link_libraries(customization lua)
add_executable(usertype_var examples/usertype_var.cpp)
target_link_libraries(usertype_var lua)
add_executable(environments examples/environments.cpp)
target_link_libraries(environments lua)
add_executable(basic examples/basic.cpp)
target_link_libraries(basic lua)
add_executable(require examples/require.cpp)
target_link_libraries(require lua)
add_executable(usertype examples/usertype.cpp)
target_link_libraries(usertype lua)
add_executable(overloading examples/overloading.cpp)
target_link_libraries(overloading lua)
add_executable(usertype_automatic_operators examples/usertype_automatic_operators.cpp)
target_link_libraries(usertype_automatic_operators lua)
add_executable(containers_as_table examples/containers_as_table.cpp)
target_link_libraries(containers_as_table lua)
add_executable(usertype_special_functions examples/usertype_special_functions.cpp)
target_link_libraries(usertype_special_functions lua)
add_executable(stack_aligned_function examples/stack_aligned_function.cpp)
target_link_libraries(stack_aligned_function lua)
add_executable(optional_with_iteration examples/optional_with_iteration.cpp)
target_link_libraries(optional_with_iteration lua)
add_executable(config examples/config.cpp)
target_link_libraries(config lua)
add_executable(functions examples/functions.cpp)
target_link_libraries(functions lua)
install(TARGETS 
        containers
        tables
        environments_on_functions
        variables
        usertype_bitfields
        protected_functions
        usertype_dynamic_getter_setter
        environment_snooping
        container_usertype_as_container
        multi_results
        self_call
        any_return
        usertype_advanced
        usertype_initializers
        namespacing
        overloading_with_fallback
        self_from_lua
        usertype_simple
        runtime_additions
        usertype_call_from_c++
        static_variables
        script_error_handling
        variadic_args
        coroutine
        calling_lua_functions
        customization
        usertype_var
        environments
        basic
        require
        usertype
        overloading
        usertype_automatic_operators
        containers_as_table
        usertype_special_functions
        stack_aligned_function
        optional_with_iteration
        config
        functions
        RUNTIME DESTINATION bin/examples)

add_executable(tests 
		test_state.cpp
		test_operators.cpp
		test_tables.cpp
		test_utility.cpp
		test_strings.cpp
		test_environments.cpp
		test_customizations.cpp
		test_large_integer.cpp
		test_inheritance.cpp
		tests.cpp
		test_variadics.cpp
		test_coroutines.cpp
		test_container_semantics.cpp
		test_storage.cpp
		test_overflow.cpp
		test_plain_types.cpp
		test_simple_usertypes.cpp
		test_gc.cpp
		test_functions.cpp
		test_usertypes.cpp
         	test_containers.cpp
        	test_filters.cpp)
install(TARGETS tests RUNTIME DESTINATION bin)
target_include_directories(tests PRIVATE ./Catch/include/)
find_package(Threads)
target_link_libraries(tests lua Threads::Threads)

install(FILES "single/sol/sol.hpp" DESTINATION include)
