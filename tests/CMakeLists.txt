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

# # # # sol2 tests

file(GLOB SOL2_TEST_SOURCES test*.cpp)
source_group(test_sources FILES ${SOL2_TEST_SOURCES})

if (MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /EHsc /std:c++latest")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Wno-unknown-warning -Wno-unknown-warning-option -Wall -Wextra -Wpedantic -pedantic -pedantic-errors -Wno-noexcept-type -ftemplate-depth=1024")
endif()

file(DOWNLOAD https://github.com/catchorg/Catch2/releases/download/v2.0.1/catch.hpp ${CMAKE_BINARY_DIR}/Catch/include/catch.hpp)

function(CREATE_TEST test_target_name test_name test_includes is_single)
	if (is_single)
		set(header_files ${SOL2_SINGLE_HEADER_SOURCES})
	else()
		set(header_files ${SOL2_HEADER_SOURCES})
	endif()
	add_executable(${test_target_name} ${SOL2_TEST_SOURCES}
		${header_files})
	set_target_properties(${test_target_name}
		PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/tests"
		LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/tests"
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tests")
	target_include_directories(${test_target_name}
		PRIVATE ${test_includes} "${LUA_INCLUDE_DIR}" "${CMAKE_BINARY_DIR}/Catch/include/")
	target_compile_features(${test_target_name}
		PRIVATE ${CXX_FEATURES})
	# if CMAKE_DL_LIBS is empty, then that just resolves to nothingness
	target_link_libraries(${test_target_name}
		Threads::Threads ${LUA_LIBRARIES} ${CMAKE_DL_LIBS})
	if (is_single)
		add_dependencies(${test_target_name} single_header)
	endif()
	add_test(NAME ${test_target_name} 
		COMMAND ${test_target_name})
	install(TARGETS ${test_target_name}
		RUNTIME DESTINATION bin)
endfunction(CREATE_TEST)

CREATE_TEST(tests "tests" "${CMAKE_SOURCE_DIR}" FALSE)
if (TESTS_SINGLE)
	CREATE_TEST(tests_single "tests.single" "${CMAKE_BINARY_DIR}/single/sol" TRUE)
endif()
