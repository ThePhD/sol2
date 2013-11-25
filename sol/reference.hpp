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

#ifndef SOL_REFERENCE_HPP
#define SOL_REFERENCE_HPP

#include "types.hpp"

namespace sol {
class reference {
private:
    lua_State* L = nullptr; // non-owning
    int ref = LUA_NOREF;

    int copy() const {
        push();
        return luaL_ref(L, LUA_REGISTRYINDEX);
    }
public:
    reference() noexcept = default;

    reference(lua_State* L, int index): L(L) {
        lua_pushvalue(L, index);
        ref = luaL_ref(L, LUA_REGISTRYINDEX);
    }

    virtual ~reference() {
        luaL_unref(L, LUA_REGISTRYINDEX, ref);
    }


    void push() const noexcept {
        lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
    }

    reference(reference&& o) noexcept {
        L = o.L;
        ref = o.ref;

        o.L = nullptr;
        o.ref = LUA_NOREF;
    }

    reference& operator=(reference&& o) noexcept {
        L = o.L;
        ref = o.ref;

        o.L = nullptr;
        o.ref = LUA_NOREF;

        return *this;
    }

    reference(const reference& o) noexcept {
        L = o.L;
        ref = o.copy();
    }

    reference& operator=(const reference& o) noexcept {
        L = o.L;
        ref = o.copy();
        return *this;
    }

    type get_type() {
        push();
        int result = lua_type(L, -1);
        lua_pop(L, 1);
        return static_cast<type>(result);
    }

    lua_State* state() const noexcept {
        return L;
    }
};
} // sol

#endif // SOL_REFERENCE_HPP