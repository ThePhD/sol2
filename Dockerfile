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

# Start from the ubuntu:xenial image
FROM ubuntu:xenial
# Everything from our current directory (repo toplevel in travis-ci)
# should be copied into our container at the top-level sol2 directory
ADD . /sol2
#VOLUME /sol2
# We want our working directory to be the toplevel
WORKDIR /
# RUN is how you write to the image you've pulled down
# RUN actions are "committed" to the image, and everything will
# start from the base after all run commands are executed
RUN apt update
RUN apt -y install sudo zsh
RUN mkdir -p build-sol2/Debug build-sol2/Release
RUN ls -lah
RUN ./sol2/scripts/preparation.linux.sh
# CMD/ENTRYPOINT is different from RUN
# these are done on a per-instantiation and essentially describe
# the DEFAULT behavior of this container when its started, not what state it
# gets "saved" in...
# it only runs the last CMD/ENTRYPOINT as the default behavior:
# multiple CMDs will not be respected
CMD ["/usr/bin/env", "zsh", "-c", "./sol2/scripts/run.linux.sh"]
