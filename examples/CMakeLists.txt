# # # # sol2
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

# # # sol2 Examples

if (SOL2_DYNAMIC_LOADING_EXAMPLES OR SOL2_DYNAMIC_LOADING_EXAMPLES_SINGLE)
	# # require_from_dll example
	# just add the subdirectory
	add_subdirectory(require_dll_example)
endif()

if (SOL2_INTEROP_EXAMPLES OR SOL2_INTEROP_EXAMPLES_SINGLE)
	# # interop examples
	add_subdirectory(interop/kaguya)
	add_subdirectory(interop/tolua)
	add_subdirectory(interop/LuaBridge)
	add_subdirectory(interop/luwra)
endif()

# # In-depth customization example
add_subdirectory(customization)

# # single-source compilable examples

file(GLOB sol2.examples.sources source/*.cpp source/tutorials/*.cpp source/tutorials/quick_n_dirty/*.cpp source/docs/*.cpp)
source_group(examples FILES ${sol2.examples.sources})

function (MAKE_EXAMPLE example_source_file example_prefix target_sol)
	
	get_filename_component(example_name ${example_source_file} NAME_WE)
	file(RELATIVE_PATH example_source_file_relative ${CMAKE_SOURCE_DIR} ${example_source_file})
	get_filename_component(example_output_relative_dir ${example_source_file_relative} DIRECTORY)
	file(TO_CMAKE_PATH "${example_output_relative_dir}" example_output_relative_dir_name)
	STRING(REGEX REPLACE "/" "." example_output_relative_dir_name "${example_output_relative_dir}")
	if (example_output_relative_dir_name STREQUAL "")
		set(example_output_name ${example_name})
		set(example_name "${example_prefix}.${example_name}")
	else()
		set(example_output_name ${example_output_relative_dir_name}.${example_name})
		set(example_name "${example_prefix}.${example_output_relative_dir_name}.${example_name}")
	endif()

	add_executable(${example_name} ${example_source_file})
	set_target_properties(${example_name}
		PROPERTIES
		OUTPUT_NAME "${example_output_name}")
	
	if (MSVC)
		target_compile_options(${example_name}
			PRIVATE /std:c++latest /EHsc /W4)
		target_compile_definitions(${example_name}
			PRIVATE UNICODE _UNICODE
			_CRT_SECURE_NO_WARNINGS _CRT_SECURE_NO_DEPRECATE )
	else()
		target_compile_options(${example_name}
			PRIVATE -std=c++1z 
			-ftemplate-backtrace-limit=0
			-Wno-unknown-warning -Wno-unknown-warning-option
			-Wall -Wpedantic -Werror -pedantic -pedantic-errors
			-Wno-noexcept-type)

		if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
			# For another day, when C++ is not so crap
			# and we have time to audit the entire lib
			# for all uses of `detail::swallow`...
			#target_compile_options(${example_name}
			#	PRIVATE -Wcomma)
		endif()
	endif()
	
	target_link_libraries(${example_name}
		PRIVATE Threads::Threads ${target_sol} ${LUA_LIBRARIES} ${CMAKE_DL_LIBS})
	
	if (SOL2_TESTS_EXAMPLES)
		add_test(NAME ${example_output_name} COMMAND ${example_name})
	endif()
	if(SOL2_ENABLE_INSTALL)
		install(TARGETS ${example_name} RUNTIME DESTINATION bin)
	endif()
endfunction(MAKE_EXAMPLE)

if (SOL2_EXAMPLES)
	foreach(example_source_file ${sol2.examples.sources})
		MAKE_EXAMPLE(${example_source_file} "sol2" sol2::sol2)
	endforeach()
endif()

if (SOL2_EXAMPLES_SINGLE)
	foreach(example_source_file ${sol2.examples.sources})
		MAKE_EXAMPLE(${example_source_file} "sol2.single" sol2::sol2::single)
	endforeach()
endif()
