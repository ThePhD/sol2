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

# # # # sol3, documentation generation
# # # Required minimum version statement
cmake_minimum_required(VERSION 3.5.0)
	
find_package(Python3 COMPONENTS Interpreter)

if (NOT Python3_FOUND AND NOT Python3_Interpreter_FOUND)
	message(FATAL_ERROR "sol2 documentation cannot be generated as python 3 has not been found: install or set the python 3 interpreter for the docs to find it and be sure to pip install sphinx")
endif()

find_program(sol2_make_executable make make.exe mingw32-make mingw32-make.exe)
if(NOT sol2_make_executable)
	message(FATAL_ERROR "could not find a suitable make executable to build Sphinx documentation")
endif()

add_custom_command(OUTPUT docs_invisible_file_always_generate
	USES_TERMINAL
	COMMAND "${sol2_make_executable}" -C "${CMAKE_CURRENT_SOURCE_DIR}" html "BUILDDIR=${CMAKE_CURRENT_BINARY_DIR}")
add_custom_target(docs 
	DEPENDS docs_invisible_file_always_generate)

if(SOL2_ENABLE_INSTALL)
	install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/docs/html" DESTINATION "${CMAKE_INSTALL_DOCDIR}")
endif()
