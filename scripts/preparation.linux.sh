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
sudo apt update
sudo apt install build-essential ninja-build libreadline6 libreadline6-dev python3 zsh wget curl openssl cmake

# # CMake Installation
# This is only necessary if we need cmake of a better version than what's available on the system
#CMAKE_VERSION_MAJOR_MINOR=3.10
#CMAKE_VERSION_BUILD=1
#CMAKE_VERSION=$CMAKE_VERSION_MAJOR_MINOR.$CMAKE_VERSION_BUILD
#wget https://cmake.org/files/v$CMAKE_VERSION_MAJOR_MINOR/cmake-$CMAKE_VERSION-Linux-x86_64.sh
#sudo bash cmake-$CMAKE_VERSION-Linux-x86_64.sh --prefix=/usr/local --exclude-subdir --skip-license
#mkdir -p ~/temp
#cd ~/temp
#wget https://cmake.org/files/v$CMAKE_VERSION_MAJOR_MINOR/cmake-$CMAKE_VERSION.tar.gz
#tar -xzvf cmake-$CMAKE_VERSION.tar.gz
#cd cmake-$CMAKE_VERSION/
#./bootstrap
#make -j4
#sudo make install
cd ~
# refresh the shell, then print the CMake version to make 
# sure we've got what we need (need to be 3.6 or better: Ubuntu 16.04 rolls 3.5.1 or somethnig)
#exec bash

# # LLVM and GCC updates
# Grab LLVM or GCC 
# if we need it
# defining both is probably an expotentially terrible idea
if [ "$LLVM_VERSION" ]
then
	# get and use LLVM
	wget http://llvm.org/releases/$LLVM_VERSION/clang+llvm-$LLVM_VERSION-x86_64-linux-gnu-ubuntu-16.04.tar.xz -O $LLVM_ARCHIVE_PATH
	mkdir ~/clang-$LLVM_VERSION
	tar xf $LLVM_ARCHIVE_PATH -C $HOME/clang-$LLVM_VERSION --strip-components 1
	export PATH=~/clang+llvm/bin:$PATH
	export CC=clang
	export CXX=clang++
elif [ "$GCC_VERSION" ]
then
	# get and use GCC version that we desire
	sudo add-apt-repository ppa:ubuntu-toolchain-r/test -y
	sudo apt-get update
	sudo apt-get dist-upgrade
	sudo apt-get install gcc-$GCC_VERSION g++-$GCC_VERSION
	sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-$GCC_VERSION 60 --slave /usr/bin/g++ g++ /usr/bin/g++-$GCC_VERSION
	sudo update-alternatives --config gcc
	export CC=gcc
	export CXX=g++
else
	export CC=gcc
	export CXX=g++	
fi

# show the tool and compiler versions we're using
cmake --version
$CC --version
$CXX --version
