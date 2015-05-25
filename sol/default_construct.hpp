// The MIT License (MIT)

// Copyright (c) 2013-2015 Danny Y., Rapptz

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

#ifndef SOL_DEFAULT_CONSTRUCTOR_HPP
#define SOL_DEFAULT_CONSTRUCTOR_HPP

#include <memory>
#include "traits.hpp"

namespace sol {
struct default_construct {
    template<typename T, typename... Args>
    void operator()(T&& obj, Args&&... args) const {
        std::allocator<Unqualified<T>> alloc{};
        alloc.construct(obj, std::forward<Args>(args)...);
    }
};

template <typename T>
struct placement_construct {
    T obj;

    template <typename... Args>
    placement_construct( Args&&... args ) : obj(std::forward<Args>(args)...) {

    }

    template<typename... Args>
    void operator()(Args&&... args) const {
        std::allocator<Unqualified<T>> alloc{};
        alloc.construct(obj, std::forward<Args>(args)...);
    }
};

} // sol

#endif // SOL_DEFAULT_CONSTRUCTOR_HPP
