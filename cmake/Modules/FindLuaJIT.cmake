#.rst:
# FindLua
# -------
#
#
#
# Locate Lua library This module defines
#
# ::
#
#   LUAJIT_FOUND          - if false, do not try to link to Lua
#   LUAJIT_LIBRARIES      - both lua and lualib
#   LUAJIT_INCLUDE_DIR    - where to find lua.h
#   LUAJIT_VERSION_STRING - the version of Lua found
#   LUAJIT_VERSION_MAJOR  - the major version of Lua
#   LUAJIT_VERSION_MINOR  - the minor version of Lua
#   LUAJIT_VERSION_PATCH  - the patch version of Lua
#
#
#
# Note that the expected include convention is
#
# ::
#
#   #include "lua.h"
#
# and not
#
# ::
#
#   #include <lua/lua.h>
#
# This is because, the lua location is not standardized and may exist in
# locations other than lua/

#=============================================================================
# Copyright 2007-2009 Kitware, Inc.
# Copyright 2013 Rolf Eike Beer <eike@sf-mail.de>
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

unset(_lua_include_subdirs)
unset(_lua_library_names)

# this is a function only to have all the variables inside go away automatically
function(set_lua_version_vars)
    set(LUA_VERSIONS5 5.3 5.2 5.1 5.0)
    set(LUAJIT_VERSIONS 2.0)

    if (LuaJIT_FIND_VERSION_EXACT)
        if (LuaJIT_FIND_VERSION_COUNT GREATER 1)
            set(luajit_append_versions ${LuaJIT_FIND_VERSION_MAJOR}.${LuaJIT_FIND_VERSION_MINOR})
        endif ()
    elseif (LuaJIT_FIND_VERSION)  
        # once there is a different major version supported this should become a loop
        if (NOT LuaJIT_FIND_VERSION_MAJOR GREATER 5)
            if (LuaJIT_FIND_VERSION_COUNT EQUAL 1) 
                set(luajit_append_versions ${LUA_VERSIONS5})
            else ()
                foreach (subver IN LISTS LUA_VERSIONS5)
                    if (NOT subver VERSION_LESS ${LuaJIT_FIND_VERSION})
                        list(APPEND luajit_append_versions ${subver})
                    endif ()
                endforeach ()
            endif ()
        endif ()
    else ()
        # once there is a different major version supported this should become a loop
        set(luajit_append_versions ${LUA_VERSIONS5})
    endif ()

    foreach (ver IN LISTS luajit_append_versions LUAJIT_VERSIONS)
        string(REGEX MATCH "^([0-9]+)\\.([0-9]+)$" _ver "${ver}")
        list(APPEND _luajit_include_subdirs
             include/luajit${CMAKE_MATCH_1}${CMAKE_MATCH_2}
             include/luajit${CMAKE_MATCH_1}.${CMAKE_MATCH_2}
             include/luajit-${CMAKE_MATCH_1}.${CMAKE_MATCH_2}
        )

        list(APPEND _luajit_library_names
             luajit${CMAKE_MATCH_1}${CMAKE_MATCH_2}
             luajit${CMAKE_MATCH_1}.${CMAKE_MATCH_2}
             luajit.${CMAKE_MATCH_1}.${CMAKE_MATCH_2}
             luajit-${CMAKE_MATCH_1}.${CMAKE_MATCH_2}
        )
    endforeach ()

    foreach (ver IN LISTS LUA_VERSIONS5)
        string(REGEX MATCH "^([0-9]+)\\.([0-9]+)$" _ver "${ver}")
        list(APPEND _luajit_library_names
             luajit${CMAKE_MATCH_1}${CMAKE_MATCH_2}
             luajit${CMAKE_MATCH_1}.${CMAKE_MATCH_2}
             luajit.${CMAKE_MATCH_1}.${CMAKE_MATCH_2}
             luajit-${CMAKE_MATCH_1}.${CMAKE_MATCH_2}
        )
    endforeach ()

    set(_luajit_include_subdirs "${_luajit_include_subdirs}" PARENT_SCOPE)
    set(_luajit_library_names "${_luajit_library_names}" PARENT_SCOPE)
endfunction(set_lua_version_vars)

set_lua_version_vars()

find_path(LUAJIT_INCLUDE_DIR luajit.h
  HINTS
    ENV LUAJIT_DIR
  PATH_SUFFIXES ${_luajit_include_subdirs} include/luajit include
  PATHS
  ${LUAJIT_DIR}
  ~/Library/Frameworks
  /Library/Frameworks
  /sw # Fink
  /opt/local # DarwinPorts
  /opt/csw # Blastwave
  /opt
)
unset(_luajit_include_subdirs)

find_library(LUAJIT_LIBRARY
  NAMES ${_luajit_library_names} luajit
  HINTS
    ENV LUAJIT_DIR
  PATH_SUFFIXES lib
  PATHS
  ${LUAJIT_DIR}
  ~/Library/Frameworks
  /Library/Frameworks
  /sw
  /opt/local
  /opt/csw
  /opt
)
unset(_luajit_library_names)

if (LUAJIT_LIBRARY)
    # include the math library for Unix
    if (UNIX AND NOT APPLE AND NOT BEOS)
        find_library(LUAJIT_MATH_LIBRARY m)
        set(LUAJIT_LIBRARIES "${LUAJIT_LIBRARY};${LUAJIT_MATH_LIBRARY}")
    # For Windows and Mac, don't need to explicitly include the math library
    else ()
        set(LUAJIT_LIBRARIES "${LUAJIT_LIBRARY}")
    endif ()
endif ()

# This macro is taken word-for-word from the FindLua module and placed into a macro to keep the semantics the same
macro(get_lua_versions include_dir)
    # At least 5.[012] have different ways to express the version
    # so all of them need to be tested. Lua 5.2 defines LUA_VERSION
    # and LUA_RELEASE as joined by the C preprocessor, so avoid those.
    file(STRINGS "${${include_dir}}/lua.h" lua_version_strings
         REGEX "^#define[ \t]+LUA_(RELEASE[ \t]+\"Lua [0-9]|VERSION([ \t]+\"Lua [0-9]|_[MR])).*")

    string(REGEX REPLACE ".*;#define[ \t]+LUA_VERSION_MAJOR[ \t]+\"([0-9])\"[ \t]*;.*" "\\1" LUAJIT_LUA_VERSION_MAJOR ";${lua_version_strings};")
    if (LUAJIT_LUA_VERSION_MAJOR MATCHES "^[0-9]+$")
        string(REGEX REPLACE ".*;#define[ \t]+LUA_VERSION_MINOR[ \t]+\"([0-9])\"[ \t]*;.*" "\\1" LUAJIT_LUA_VERSION_MINOR ";${lua_version_strings};")
        string(REGEX REPLACE ".*;#define[ \t]+LUA_VERSION_RELEASE[ \t]+\"([0-9])\"[ \t]*;.*" "\\1" LUAJIT_LUA_VERSION_PATCH ";${lua_version_strings};")
        set(LUA_VERSION_STRING "${LUAJIT_LUA_VERSION_MAJOR}.${LUAJIT_LUA_VERSION_MINOR}.${LUAJIT_LUA_VERSION_PATCH}")
    else ()
        string(REGEX REPLACE ".*;#define[ \t]+LUA_RELEASE[ \t]+\"Lua ([0-9.]+)\"[ \t]*;.*" "\\1" LUAJIT_LUA_VERSION_STRING ";${lua_version_strings};")
        if (NOT LUA_VERSION_STRING MATCHES "^[0-9.]+$")
            string(REGEX REPLACE ".*;#define[ \t]+LUA_VERSION[ \t]+\"Lua ([0-9.]+)\"[ \t]*;.*" "\\1" LUAJIT_LUA_VERSION_STRING ";${lua_version_strings};")
        endif ()
        string(REGEX REPLACE "^([0-9]+)\\.[0-9.]*$" "\\1" LUAJIT_LUA_VERSION_MAJOR "${LUAJIT_LUA_VERSION_STRING}")
        string(REGEX REPLACE "^[0-9]+\\.([0-9]+)[0-9.]*$" "\\1" LUAJIT_LUA_VERSION_MINOR "${LUAJIT_LUA_VERSION_STRING}")
        string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]).*" "\\1" LUAJIT_LUA_VERSION_PATCH "${LUAJIT_LUA_VERSION_STRING}")
    endif ()

    unset(lua_version_strings)
endmacro(get_lua_versions)

if (LUAJIT_INCLUDE_DIR AND EXISTS "${LUAJIT_INCLUDE_DIR}/luajit.h")
    # LuaJIT defines two preprocessor macros: 
    #   LUA_VERSION     -> version string with lua version in it
    #   LUA_VERSION_NUM -> numeric representation, i.e. 20003 for 2.0.3 
    # This just parses the LUAJIT_VERSION macro and extracts the version. 
    file(STRINGS "${LUAJIT_INCLUDE_DIR}/luajit.h" version_strings
         REGEX "^#define[ \t]+LUAJIT_VERSION?[ \t]+(\"LuaJIT [0-9\\.]+(-(beta|alpha)[0-9]*)?\").*")
    

    string(REGEX REPLACE ".*;#define[ \t]+LUAJIT_VERSION[ \t]+\"LuaJIT ([0-9\\.]+(-(beta|alpha)[0-9]*)?)\"[ \t]*;.*" "\\1" LUAJIT_VERSION_STRING_SHORT ";${version_strings};")
    string(REGEX REPLACE ".*;([0-9]+\\.[0-9]+\\.[0-9]+(-(beta|alpha)[0-9]*)?);.*" "\\1" luajit_version_num ";${LUAJIT_VERSION_STRING_SHORT};")

    string(REGEX REPLACE "^([0-9]+)\\.[0-9]+\\.(-(beta|alpha)[0-9]*)?$" "\\1" LUAJIT_VERSION_MAJOR "${luajit_version_num}")
    string(REGEX REPLACE "^[0-9]+\\.([0-9]+)\\.[0-9](-(beta|alpha)[0-9]*)?$" "\\1" LUAJIT_VERSION_MINOR "${luajit_version_num}")
    string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+(-(beta|alpha)[0-9]*)?)$" "\\1" LUAJIT_VERSION_PATCH "${luajit_version_num}")
    
    get_lua_versions(LUAJIT_INCLUDE_DIR)

    set(LUAJIT_VERSION_STRING "${LUAJIT_LUA_VERSION_STRING} (${LUAJIT_VERSION_STRING_SHORT})")

    message("Found LuaJIT ${LUAJIT_VERSION_STRING}")
endif()

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LUAJIT_FOUND to TRUE if
# all listed variables are TRUE
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LuaJIT
                                  REQUIRED_VARS LUAJIT_LIBRARIES LUAJIT_INCLUDE_DIR
                                  VERSION_VAR LUAJIT_VERSION_STRING)

mark_as_advanced(LUAJIT_INCLUDE_DIR LUAJIT_LIBRARY LUAJIT_MATH_LIBRARY)

