// The MIT License (MIT) 

// Copyright (c) 2013-2016 Rapptz, ThePhD and contributors

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

#ifndef SOL_STACK_PROXY_HPP
#define SOL_STACK_PROXY_HPP

#include "stack.hpp"
#include "proxy_base.hpp"

namespace sol {
struct stack_proxy : public proxy_base<stack_proxy> {
private:
    lua_State* L;
    int index;

public:
    stack_proxy(lua_State* L, int index) : L(L), index(index) {}

    template<typename T>
    decltype(auto) get() const {
        return stack::get<T>(L, stack_index());
    }

    int push () const {
        lua_pushvalue(L, index);
        return 1;
    }

    lua_State* lua_state() const { return L; }
    int stack_index() const { return index; }
};

namespace stack {
template <>
struct getter<stack_proxy> {
    static stack_proxy get(lua_State* L, int index = -1) {
        return stack_proxy(L, index);
    }
};

template <>
struct pusher<stack_proxy> {
    static int push(lua_State*, const stack_proxy& ref) {
        return ref.push();
    }
};
} // stack
} // sol

#endif // SOL_STACK_PROXY_HPP
