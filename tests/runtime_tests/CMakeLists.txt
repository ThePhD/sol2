# # # # sol3
# The MIT License (MIT)
# 
# Copyright (c) 2013-2021 Rapptz, ThePhD, and contributors
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

# # # # sol3 tests - runtime tests

file(GLOB SOL2_RUNTIME_TEST_SOURCES source/*.cpp)
source_group(test_sources FILES ${SOL2_RUNTIME_TEST_SOURCES})

function(CREATE_TEST test_target_name test_name target_sol)
	set(test_target_name ${test_target_name})
	add_executable(${test_target_name} ${SOL2_RUNTIME_TEST_SOURCES})
	set_target_properties(${test_target_name}
		PROPERTIES
		OUTPUT_NAME ${test_name})
	target_link_libraries(${test_target_name} 
		PUBLIC Threads::Threads ${LUA_LIBRARIES} Catch2::Catch2 ${target_sol} ${CMAKE_DL_LIBS})
	target_compile_definitions(${test_target_name}
		PUBLIC SOL_ALL_SAFETIES_ON=1 SOL_PRINT_ERRORS=1 SOL_ENABLE_INTEROP=1)
	if (MSVC)
		if (NOT CMAKE_COMPILER_ID MATCHES "Clang")
			target_compile_options(${test_target_name} 
				PRIVATE /bigobj /W4 /w44800)
		else()
			target_compile_options(${test_target_name}
				PRIVATE -ftemplate-backtrace-limit=0)
		endif()
	else()
		target_compile_options(${test_target_name} 
			PRIVATE -std=c++1z -pthread
			-Wno-unknown-warning -Wno-unknown-warning-option
			-Wall -Wpedantic -Werror -pedantic -pedantic-errors
			-Wno-noexcept-type)

		if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
			target_compile_options(${test_target_name}
				PRIVATE -Wno-microsoft-cast)
		endif()

		if (IS_X86)
			if(MINGW)
				set_target_properties(${test_target_name}
					PROPERTIES
					LINK_FLAGS -static-libstdc++)
			endif()
		endif()	
	endif()
	if (MSVC)
		target_compile_options(${test_target_name}
			PRIVATE /EHsc /std:c++latest)
		target_compile_definitions(${test_target_name}
			PRIVATE UNICODE _UNICODE 
			_CRT_SECURE_NO_WARNINGS _CRT_SECURE_NO_DEPRECATE)
	else()
		target_compile_options(${test_target_name}
			PRIVATE -std=c++1z -Wno-unknown-warning -Wno-unknown-warning-option 
			-Wall -Wextra -Wpedantic -pedantic -pedantic-errors)
	endif()

	if (SOL2_CI)
		target_compile_definitions(${test_target_name} 
			PRIVATE SOL2_CI)
	endif()
	
	add_test(NAME ${test_name} COMMAND ${test_target_name})
	if(SOL2_ENABLE_INSTALL)
		install(TARGETS ${test_target_name} RUNTIME DESTINATION bin)
	endif()
endfunction(CREATE_TEST)

if (SOL2_TESTS)
	CREATE_TEST(runtime_tests "sol2.runtime_tests" sol2::sol2)
endif()
if (SOL2_TESTS_SINGLE)
	CREATE_TEST(runtime_tests_single "sol2.single.runtime_tests" sol2::sol2::single)
endif()
if (SOL2_TESTS_SINGLE_GENERATED)
	CREATE_TEST(runtime_tests_generated_single "sol2.single.generated.runtime_tests" sol2::sol2::single_generated)
endif()
