#!/usr/bin/env bash

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

# This script runs the actual project

if [ "$LLVM_VERSION" ]
then
	source /sol2/scripts/preparation.linux.llvm.sh
	export CC=clang
	export CXX=clang++
elif [ "$GCC_VERSION" ]
then
	export CC=gcc-$GCC_VERSION
	export CXX=g++-$GCC_VERSION
else
	export CC=cc
	export CXX=c++
fi
export build_type_cxx=-DCMAKE_CXX_COMPILER\=${CXX}
export build_type_cc=-DCMAKE_C_COMPILER\=${CC}

# show the tool and compiler versions we're using
cmake --version
$CC --version
$CXX --version
echo build_type_cc : "${build_type_cc}"
echo build_type_cxx: "${build_type_cxx}"

top_level=${PWD}

cd /build-sol2/Debug
cmake ../../sol2 -G Ninja -DCMAKE_BUILD_TYPE=Debug ${build_type_cc} ${build_type_cxx} -DLUA_VERSION="${LUA_VERSION}" -DBUILD_LUA=ON -DBUILD_LUA_AS_DLL=OFF -DTESTS=ON -DEXAMPLES=ON -DSINGLE=ON -DTESTS_EXAMPLES=ON -DTESTS_SINGLE=ON -DCI=ON
cmake --build . --config Debug
ctest -C Debug
cd ${top_level}

cd /build-sol2/Release
cmake ../../sol2 -G Ninja -DCMAKE_BUILD_TYPE=Release ${build_type_cc} ${build_type_cxx} -DLUA_VERSION="${LUA_VERSION}" -DBUILD_LUA=ON -DBUILD_LUA_AS_DLL=OFF -DTESTS=ON -DEXAMPLES=ON -DSINGLE=ON -DTESTS_EXAMPLES=ON -DTESTS_SINGLE=ON -DCI=ON
cmake --build . --config Release
ctest -C Release
cd ${top_level}
