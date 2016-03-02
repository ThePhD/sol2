
cmake_minimum_required(VERSION 3.0)

project(sol2)
        
option(SOL2_BUILD_EXAMPLES  "Build the examples provided"   ON)
option(SOL2_BUILD_TESTS     "Build tests"                   ON)

if(SOL2_BUILD_EXAMPLES OR SOL2_BUILD_TESTS)
    if(NOT SOL2_LUA_VERSION)
        message(FATAL_ERROR "Must specify SOL2_LUA_VERSION for testing and examples "
                            "(i.e. -DSOL2_LUA_VERSION=lua51)")
    endif(NOT SOL2_LUA_VERSION)

    string(TOLOWER ${SOL2_LUA_VERSION} SOL2_LUA_VERSION)

    list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)
    string(REGEX MATCH "^lua(jit)?((5)([0-3]))?$" _match ${SOL2_LUA_VERSION})

    if(${CMAKE_MATCH_3} AND ${CMAKE_MATCH_4})
        set(_lua_version ${CMAKE_MATCH_3}.${CMAKE_MATCH_4})
    endif()

    if(SOL2_LUA_VERSION MATCHES "^lua(5[0-3])?$")
        # Lua 
        find_package(Lua ${_lua_version} REQUIRED)

        set(SOL2_LUA_INCLUDE_DIR    ${LUA_INCLUDE_DIR})
        set(SOL2_LUA_LIBRARIES      ${LUA_LIBRARIES})
    elseif(SOL2_LUA_VERSION MATCHES "^luajit(5[0-3])?$")
        # LuaJIT 
        find_package(LuaJIT ${_lua_version} REQUIRED)

        set(SOL2_LUA_INCLUDE_DIR    ${LUAJIT_INCLUDE_DIR})
        set(SOL2_LUA_LIBRARIES      ${LUAJIT_LIBRARIES})
    else()
        message(SEND_ERROR "SOL2_LUA_VERSION specified as '${SOL2_LUA_VERSION}', this is an invalid option "
                           "(expected: lua, lua51, luajit, luajit52, etc)")
    endif()

    # Include the Lua directory as an `isystem` dir 
    include_directories(SYSTEM ${SOL2_LUA_INCLUDE_DIR})
    
    unset(_lua_version)
endif() # build examples or test 

set(CMAKE_CXX_STANDARD 14)

# Set the garget compiling to have the required cxx features 
function(set_sol2_compile_reqs target)
    target_compile_features(${target} PUBLIC cxx_auto_type
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
                                             cxx_variadic_templates
                                        )

    if(NOT MSVC)
        target_compile_options(${target} PUBLIC -Wall -Werror -pedantic)

        if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "CLANG")

        endif()
    else()
        target_compile_options(${target} PUBLIC /W2 /WX )
    endif()
endfunction(set_sol2_compile_reqs)
    
#--Find 3rd party libraries
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# # 
#--source files

# headers
#########
set(hfiles 
        sol/compatibility.hpp
        sol/compatibility/5.0.0.h
        sol/compatibility/5.1.0.h
        sol/compatibility/5.x.x.h
        sol/compatibility/5.x.x.inl
        sol/compatibility/version.hpp
        
        sol/coroutine.hpp
        
        sol/debug.hpp
        sol/demangle.hpp
        sol/deprecate.hpp
        sol/error.hpp
        
        sol/function.hpp
        sol/function_result.hpp
        sol/function_types.hpp
        sol/function_types_allocator.hpp
        sol/function_types_core.hpp
        sol/function_types_member.hpp
        sol/function_types_overload.hpp
        sol/function_types_static.hpp
        sol/function_types_usertype.hpp
        
        sol/object.hpp

        sol/overload.hpp

        sol/proxy.hpp
        sol/proxy_base.hpp

        sol/raii.hpp

        sol/reference.hpp

        sol/resolve.hpp

        sol/stack.hpp
        
        sol/state.hpp
        sol/state_view.hpp
        
        sol/table.hpp
        sol/table_core.hpp
        
        sol/thread.hpp

        sol/traits.hpp
        
        sol/tuple.hpp
        
        sol/types.hpp
        
        sol/usertype.hpp
        sol/usertype_traits.hpp

        sol.hpp
    )

# Examples:

if(SOL2_BUILD_EXAMPLES)

    set(example_srcs    examples/basic.cpp  
                        examples/config.cpp 
                        examples/functions.cpp 
                        examples/tables.cpp 
                        examples/usertype.cpp 
                        examples/variables.cpp
        )

    # build up a list of targets to create the aggregate target `examples`
    set(example_targets )
    foreach(example ${example_srcs})
        get_filename_component(name ${example} NAME_WE)

        add_executable(example-${name} ${example})
        target_link_libraries(example-${name} ${SOL2_LUA_LIBRARIES})
        set_sol2_compile_reqs(example-${name})

        list(APPEND example_targets example-${name})
    endforeach()

    # create simple aggregate target
    add_custom_target(examples)
    add_dependencies(examples ${example_targets})

    unset(example_targets)
else()
    add_custom_target(examples 
                      COMMAND ${CMAKE_COMMAND} -E echo "examples not configured to be built, set -DSOL2_BUILD_EXAMPLES=on")
endif(SOL2_BUILD_EXAMPLES)

if(SOL2_BUILD_TESTS)
    include(CTest)
    enable_testing()

    set(tests tests.cpp)

    # Download and "install" catch-lib
    include(ExternalProject)
    ExternalProject_add(
        catch-lib
        PREFIX vendor
                
        GIT_REPOSITORY "https://github.com/philsquared/Catch.git"
        GIT_TAG tags/v1.3.4
        
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND 
                    ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/include
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/vendor/src/catch-lib/single_include/catch.hpp ${CMAKE_BINARY_DIR}/include/
        TEST_COMMAND "")

    include_directories(SYSTEM ${CMAKE_BINARY_DIR}/include )  

    add_executable(catch ${hfiles} ${tests})
    add_dependencies(catch catch-lib)
    target_link_libraries(catch ${SOL2_LUA_LIBRARIES})
    set_sol2_compile_reqs(catch)

    add_test(NAME test-all
             COMMAND catch
             WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

    add_custom_target(check COMMAND ${CMAKE_COMMAND} -E env CTEST_OUTPUT_ON_FAILURE=1
                        ${CMAKE_CTEST_COMMAND} 
                        DEPENDS catch)
else()
    add_custom_target(check 
                      COMMAND ${CMAKE_COMMAND} -E echo "Tests not configured to be built, set -DSOL2_BUILD_TESTS=on")
endif(SOL2_BUILD_TESTS)

# Install: 
install(FILES ${CMAKE_SOURCE_DIR}/sol.hpp DESTINATION include/)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/sol DESTINATION include/ REGEX ".+\\.(h(pp)?|inl)$")
