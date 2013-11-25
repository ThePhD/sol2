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

#ifndef SOL_STATE_HPP
#define SOL_STATE_HPP

#include "error.hpp"
#include "table.hpp"
#include <memory>

namespace sol {
int atpanic(lua_State* L) {
    throw sol_error(lua_tostring(L, -1));
}

class state {
private:
    std::unique_ptr<lua_State, void(*)(lua_State*)> L;
    table reg;
    table global;
public:
    state(): 
    L(luaL_newstate(), lua_close),  
    reg(L.get(), LUA_REGISTRYINDEX), 
    global(reg.get<table>(LUA_RIDX_GLOBALS)) {
        lua_atpanic(L.get(), atpanic);
    }

    state(const std::string& filename): 
    L(luaL_newstate(), lua_close), 
    reg(L.get(), LUA_REGISTRYINDEX), 
    global(reg.get<table>(LUA_RIDX_GLOBALS)) {
        lua_atpanic(L.get(), atpanic);
        open_file(filename);
    }

    void open_libraries() {
        luaL_openlibs(L.get());
    }

    void script(const std::string& code) {
        if(luaL_dostring(L.get(), code.c_str())) {
            lua_error(L.get());
        }
    }

    void open_file(const std::string& filename) {
        if(luaL_dofile(L.get(), filename.c_str())) {
            lua_error(L.get());
        }
    }

    template<typename T, typename U>
    T get(U&& key) const {
        return global.get<T>(std::forward<U>(key));
    }

    template<typename T, typename U>
    state& set(T&& key, U&& value) {
        return global.set(std::forward<T>(key), std::forward<U>(value));
    }

    table global_table() const {
        return global;
    }

    table registry() const {
        return reg;
    }
};
} // sol

#endif // SOL_STATE_HPP