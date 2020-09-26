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

# # # # sol3 tests - simple regression tests

file(GLOB test_sources source/*.cpp)
source_group(sources FILES ${test_sources})

function(CREATE_TEST test_target_name test_name target_sol)
	add_executable(${test_target_name} ${test_sources})
	set_target_properties(${test_target_name}
		PROPERTIES
		OUTPUT_NAME ${test_name}
		EXPORT_NAME sol2::${test_name})
	target_link_libraries(${test_target_name} 
		PUBLIC Threads::Threads ${LUA_LIBRARIES} ${target_sol})
	target_compile_definitions(${test_target_name}
		PRIVATE SOL_GET_FUNCTION_POINTER_UNSAFE=1 SOL_ALL_SAFETIES_ON=1)
	target_include_directories(${test_target_name}
		PRIVATE ../../../examples/include)

	if (MSVC)
		if (NOT CMAKE_COMPILER_ID MATCHES "Clang")
			target_compile_options(${test_target_name} 
				PRIVATE /bigobj /W4)
		endif()
	else()
		target_compile_options(${test_target_name} 
			PRIVATE -std=c++1z -pthread
			-Wno-unknown-warning -Wno-unknown-warning-option
			-Wall -Wpedantic -Werror -pedantic -pedantic-errors
			-Wno-noexcept-type)

		if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
			# For another day, when C++ is not so crap
			# and we have time to audit the entire lib
			# for all uses of `detail::swallow`...
			#target_compile_options(${test_target_name}
			#	PRIVATE -Wcomma)		
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

	if (CMAKE_DL_LIBS)
		target_link_libraries(${test_target_name}
			PRIVATE ${CMAKE_DL_LIBS})
	endif()
	
	add_test(NAME ${test_name} COMMAND ${test_target_name})
	install(TARGETS ${test_target_name} RUNTIME DESTINATION bin)
endfunction(CREATE_TEST)

if (SOL2_TESTS)
	CREATE_TEST(config_function_pointers_tests "config_function_pointers_tests" sol2::sol2)
endif()
if (SOL2_TESTS_SINGLE)
	CREATE_TEST(config_function_pointers_tests_single "config_function_pointers_tests.single" sol2::sol2_single)
endif()
if (SOL2_TESTS_SINGLE_GENERATED)
	CREATE_TEST(config_function_pointers_tests_generated_single "config_function_pointers_tests.single.generated" sol2::sol2_single_generated)
endif()
