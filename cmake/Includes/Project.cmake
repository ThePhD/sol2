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

# # # # sol2 - cmake - basic project configuration

include_guard(DIRECTORY)

list(PREPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/Packages")
list(PREPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/Modules")
list(APPEND  CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
list(APPEND  CMAKE_MESSAGE_CONTEXT "${PROJECT_NAME}")

if (PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
	set(sol2-is-top-level-project TRUE)
else()
	set(sol2-is-top-level-project FALSE)
endif()

# # CMake and sol2 Includes
# CMake
include(CheckCXXCompilerFlag)
include(CheckCCompilerFlag)
include(CheckIPOSupported)
include(CMakeDependentOption)
include(CMakePrintHelpers)
include(GNUInstallDirs)
include(FeatureSummary)
include(FetchContent)
include(CTest)
# sol2
include(CheckCompilerDiagnostic)
include(CheckCompilerFlag)
include(FindVersion)

if (sol2-is-top-level-project)
	# # Check environment/prepare generator expressions
	# Basic/Normal flags
	check_compiler_flag(disable-permissive MSVC /permissive- GCC -pedantic)
	check_compiler_flag(utf8-literal-encoding MSVC /execution-charset:utf-8 GCC -fexec-charset=utf-8)
	check_compiler_flag(utf8-source-encoding MSVC /source-charset:utf-8 GCC -finput-charset=utf-8)
	check_compiler_flag(extra-constexpr-depth MSVC /constexpr:depth2147483647 GCC -fconstexpr-depth=2147483647 CLANG -fconstexpr-depth=2147483647)
	check_compiler_flag(extra-constexpr-steps MSVC /constexpr:steps2147483647 GCC -fconstexpr-ops-limit=2147483647 CLANG -fconstexpr-steps=2147483647)
	check_compiler_flag(template-debugging-mode GCC -ftemplate-backtrace-limit=0)
	check_compiler_flag(big-obj MSVC /bigobj)
	# Overall warning flags
	check_compiler_flag(pedantic GCC -pedantic)
	check_compiler_flag(warn-pedantic GCC -Wpedantic)
	check_compiler_flag(warn-all MSVC /W4 GCC -Wall)
	check_compiler_flag(warn-extra GCC -Wextra)
	check_compiler_flag(warn-errors MSVC /WX GCC -Werror)
	# Individual warnings/errors
	check_compiler_diagnostic(unknown-warning)
	check_compiler_diagnostic(unknown-warning-option)
	check_compiler_diagnostic(microsoft-cast)
	check_compiler_diagnostic(noexcept-type)
	check_compiler_diagnostic(unreachable-code MSVC 4702)
	check_compiler_diagnostic(padding-from-alignment MSVC 4324)
endif()