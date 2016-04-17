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

#ifndef SOL_VARIADIC_ARGS_HPP
#define SOL_VARIADIC_ARGS_HPP

#include "stack.hpp"
#include "stack_proxy.hpp"

namespace sol {
struct variadic_args {
private:
    lua_State* L;
    int index;
    int stacktop;

public:
    variadic_args() = default;
    variadic_args(lua_State* L, int index = -1): L(L), index(lua_absindex(L, index)), stacktop(lua_gettop(L)) {}
    variadic_args(const variadic_args&) = default;
    variadic_args& operator=(const variadic_args&) = default;
    variadic_args(variadic_args&& o) : L(o.L), index(o.index), stacktop(o.stacktop) {
        // Must be manual, otherwise destructor will screw us
        // return count being 0 is enough to keep things clean
        // but will be thorough
        o.L = nullptr;
        o.index = 0;
        o.stacktop = 0;
    }
    variadic_args& operator=(variadic_args&& o) {
        L = o.L;
        index = o.index;
        stacktop = o.stacktop;
        // Must be manual, otherwise destructor will screw us
        // return count being 0 is enough to keep things clean
        // but will be thorough
        o.L = nullptr;
        o.index = 0;
        o.stacktop = 0;
        return *this;
    }

    int push () const {
        int pushcount = 0;
        for (int i = index; i <= stacktop; ++i) {
            lua_pushvalue(L, i);
            pushcount += 1;
        }
        return pushcount;
    }

    template<typename T>
    decltype(auto) get(int start = 0) const {
        return stack::get<T>(L, index + start);
    }

    stack_proxy operator[](int start) const {
        return stack_proxy(L, index + start);
    }

    lua_State* lua_state() const { return L; };
    int stack_index() const { return index; };
    int leftover_count() const { return stacktop - (index - 1); }
    int top() const { return stacktop; }
};

namespace stack {
template <>
struct getter<variadic_args> {
    static variadic_args get(lua_State* L, int index = -1) {
        return variadic_args(L, index);
    }
};

template <>
struct pusher<variadic_args> {
    static int push(lua_State*, const variadic_args& ref) {
        return ref.push();
    }
};
} // stack
} // sol

#endif // SOL_VARIADIC_ARGS_HPP
