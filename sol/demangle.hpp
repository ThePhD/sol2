// The MIT License (MIT)

// Copyright (c) 2013 Danny Y., Rapptz

// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef SOL_DEMANGLE_HPP
#define SOL_DEMANGLE_HPP

#include <string>

#if defined(__GNUC__) || defined(__clang__)
#include <cxxabi.h>
#endif

namespace sol {
namespace detail {
#ifdef _MSC_VER
std::string demangle(const std::type_info& id) {
    const static std::string removals[2] = { "struct ", "class " };
    std::string realname = id.name();
    for(std::size_t r = 0; r < 2; ++r) {
        auto found = realname.find(removals[r]);

        if(found == std::string::npos) {
            continue;
        }

        realname.erase(found, removals[r].size());
    }
    return realname;
}

#elif defined(__GNUC__) || defined(__clang__)
std::string demangle(const std::type_info& id) {
    int status;
    char* unmangled = abi::__cxa_demangle(id.name(), 0, 0, &status);
    std::string realname = unmangled;
    free(unmangled);
    return realname;
}

#else
#error Compiler not supported for demangling
#endif // compilers
} // detail
} // sol

#endif // SOL_DEMANGLE_HPP