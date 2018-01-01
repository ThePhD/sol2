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

# # Initial and necessary installations
sudo apt-get -y install ninja-build libreadline6 libreadline6-dev python3 wget curl libcurl3 cmake git

# # LLVM and GCC updates
# Grab LLVM or GCC 
# if we need it
# defining both is probably an expotentially terrible idea
if [ "$LLVM_VERSION" ]
then
	# get and use LLVM
	source /sol2/scripts/preparation.linux.llvm.sh
	export CC=clang
	export CXX=clang++
	wget http://llvm.org/releases/$LLVM_VERSION/clang+llvm-$LLVM_VERSION-x86_64-linux-gnu-ubuntu-16.04.tar.xz -O $LLVM_ARCHIVE_PATH
	mkdir -p $CLANG_PREFIX
	tar xf $LLVM_ARCHIVE_PATH -C $CLANG_PREFIX --strip-components 1
elif [ "$GCC_VERSION" ]
then
	# get and use GCC version that we desire
	sudo apt-get -y install software-properties-common python-software-properties
	sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
	sudo apt-get -y update
	sudo apt-get -y dist-upgrade
	sudo apt-get -y install gcc-$GCC_VERSION g++-$GCC_VERSION
	sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-$GCC_VERSION 60 --slave /usr/bin/g++ g++ /usr/bin/g++-$GCC_VERSION
	sudo update-alternatives --config gcc
	export CC=gcc-$GCC_VERSION
	export CXX=g++-$GCC_VERSION
else
	sudo apt-get -y install build-essential
	export CC=cc
	export CXX=c++
fi
