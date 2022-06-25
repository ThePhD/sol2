# # # # sol2
# The MIT License (MIT)
#
# Copyright (c) 2013-2022 Rapptz, ThePhD, and contributors
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

include(FindPackageHandleStandardArgs)
include(FeatureSummary)

find_program(Sphinx_Build_EXECUTABLE NAMES sphinx-build DOC "Path to sphinx-build executable")

if (Sphinx_Build_EXECUTABLE)
  set(Sphinx_Build_FOUND YES)
endif()

if (Sphinx_Build_FOUND)
  execute_process(
    COMMAND ${Sphinx_Build_EXECUTABLE} --version
    OUTPUT_VARIABLE Sphinx_Build_VERSION_OUTPUT
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ENCODING UTF-8)
  if (Sphinx_Build_VERSION_OUTPUT)
    string(REGEX
      MATCH "[^0-9]*([0-9]+)[.]([0-9]+)?[.]?([0-9]+)?[.]?([0-9]+)?.*"
      Sphinx_Build_VERSION_CHECK ${Sphinx_Build_VERSION_OUTPUT})
  endif()
  if (Sphinx_Build_VERSION_CHECK)
    string(JOIN "." Sphinx_Build_VERSION
      ${CMAKE_MATCH_1}
      ${CMAKE_MATCH_2}
      ${CMAKE_MATCH_3}
      ${CMAKE_MATCH_4})
    set(Sphinx_Build_VERSION "${Sphinx_Build_VERSION}" CACHE STRING "sphinx-build version" FORCE)
  endif()
endif()

find_package_handle_standard_args(Sphinx
  REQUIRED_VARS Sphinx_Build_EXECUTABLE
  VERSION_VAR Sphinx_Build_VERSION
  HANDLE_COMPONENTS)

set_package_properties(Sphinx
  PROPERTIES
    DESCRIPTION "Sphinx Documentation Generator"
    URL "https://sphinx-doc.org")

if (Sphinx_Build_FOUND AND NOT TARGET Sphinx::Build)
  add_executable(Sphinx::Build IMPORTED)
  set_property(TARGET Sphinx::Build PROPERTY IMPORTED_LOCATION ${Sphinx_Build_EXECUTABLE})
  set_property(TARGET Sphinx::Build PROPERTY VERSION ${Sphinx_Build_VERSION})
  mark_as_advanced(Sphinx_Build_EXECUTABLE Sphinx_Build_VERSION)
endif()
