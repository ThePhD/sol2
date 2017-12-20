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

file(GLOB EXAMPLES_SRC *.cpp tutorials/quick_n_dirty/*.cpp)
source_group(examples FILES ${EXAMPLES_SRC})

if (MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /EHsc /std:c++latest")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Wno-unknown-warning -Wno-unknown-warning-option -Wall -Wextra -Wpedantic -pedantic -pedantic-errors -Wno-noexcept-type -ftemplate-depth=1024")
endif()

foreach(example_source_file ${EXAMPLES_SRC})
	get_filename_component(example_name ${example_source_file} NAME_WE)
	file(RELATIVE_PATH example_source_file_relative ${CMAKE_SOURCE_DIR} ${example_source_file})
	get_filename_component(example_output_relative_dir ${example_source_file_relative} DIRECTORY)
	set(example_name "${example_name}")
		
	add_executable(${example_name} ${example_source_file} ${HEADER_SRCS})
	set_target_properties(${example_name} 
		PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${example_output_relative_dir}"
		LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${example_output_relative_dir}"
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${example_output_relative_dir}")
	target_include_directories(${example_name} PRIVATE "${CMAKE_SOURCE_DIR}" "${LUA_INCLUDE_DIR}" "${CMAKE_BINARY_DIR}/Catch/include/")
	target_compile_features(${example_name} PUBLIC ${CXX_FEATURES})
	target_link_libraries(${example_name} ${LUA_LIBRARIES} ${CMAKE_DL_LIBS})
	#if(CMAKE_DL_LIBS)
	#	target_link_libraries(${example_name} )
	#endif()
		
	if (TESTS)
		add_test(NAME ${example_name} COMMAND ${example_name})
	endif()
	install(TARGETS ${example_name} RUNTIME DESTINATION ${example_name})
endforeach()
