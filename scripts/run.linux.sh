#!/usr/bin/env zsh

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

# Things we need
# https://stackoverflow.com/questions/59895/getting-the-source-directory-of-a-bash-script-from-within
get_script_dir () {
	if [ -n "`$SHELL -c 'echo $ZSH_VERSION'`" ]
	then
		SOURCE="${(%):-%x}"
	elif [ -n "`$SHELL -c 'echo $BASH_VERSION'`" ]
	then
		SOURCE="${BASH_SOURCE[0]}"
	else
		SOURCE="${0}"
	fi
     # While $SOURCE is a symlink, resolve it
     while [ -h "$SOURCE" ]; do
          DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
          SOURCE="$( readlink "$SOURCE" )"
          # If $SOURCE was a relative symlink (so no "/" as prefix, need to resolve it relative to the symlink base directory
          [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE"
     done
     DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
     echo "$DIR"
}

scripts_dir="$(get_script_dir)"
sol2_dir="${scripts_dir}/.."

mkdir -p build-sol2
cd build-sol2

# This script runs the actual project
mkdir -p Debug Release

if [ "$LLVM_VERSION" ]
then
	source ${scripts_dir}/preparation.linux.llvm.sh
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

cd Debug
cmake ${sol2_dir} -G Ninja -DCMAKE_BUILD_TYPE=Debug ${build_type_cc} ${build_type_cxx} -DLUA_VERSION="${LUA_VERSION}" -DBUILD_LUA=ON -DBUILD_LUA_AS_DLL=OFF -DTESTS=ON -DEXAMPLES=ON -DSINGLE=ON -DTESTS_EXAMPLES=ON -DTESTS_SINGLE=ON -DCI=ON
cmake --build . --config Debug
ctest -C Debug
cd ${top_level}

cd Release
cmake ${sol2_dir} -G Ninja -DCMAKE_BUILD_TYPE=Release ${build_type_cc} ${build_type_cxx} -DLUA_VERSION="${LUA_VERSION}" -DBUILD_LUA=ON -DBUILD_LUA_AS_DLL=OFF -DTESTS=ON -DEXAMPLES=ON -DSINGLE=ON -DTESTS_EXAMPLES=ON -DTESTS_SINGLE=ON -DCI=ON
cmake --build . --config Release
ctest -C Release
cd ${top_level}
