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

#ifndef SOL_OBJECT_HPP
#define SOL_OBJECT_HPP

#include "table.hpp"
#include "function.hpp"

namespace sol {
class object : public table, public function {
public:
    object(lua_State* L, int index = -1): reference(L, index) {}
    object() = default;

    template<typename T>
    T as() {
        push();
        type_assert(state(), -1, type_of<T>());
        return stack::get<T>(state());
    }

    template<typename T>
    bool is() {
        push();
        auto expected = type_of<T>();
        auto actual = lua_type(state(), -1);
        return (static_cast<int>(expected) == actual) || (expected == type::poly);
    }
};
} // sol

#endif // SOL_OBJECT_HPP