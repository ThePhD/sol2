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
namespace detail {
template<class T, class...>
struct are_same : std::true_type {};

template<class T, class U, class... Args>
struct are_same<T, U, Args...> : std::integral_constant<bool, std::is_same<T,U>{} && are_same<T, Args...>{}> {};

int atpanic(lua_State* L) {
    throw sol_error(lua_tostring(L, -1));
}
} // detail

enum class lib : char {
    base,
    package,
    coroutine,
    string,
    os,
    math,
    table,
    debug,
    bit32,
    io,
    count
};

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
        lua_atpanic(L.get(), detail::atpanic);
    }

    state(const std::string& filename): 
    L(luaL_newstate(), lua_close), 
    reg(L.get(), LUA_REGISTRYINDEX), 
    global(reg.get<table>(LUA_RIDX_GLOBALS)) {
        lua_atpanic(L.get(), detail::atpanic);
        open_file(filename);
    }
    
    template<typename... Args>
    void open_libraries(Args&&... args) {
        static_assert(detail::are_same<lib, Args...>{}, "all types must be libraries");
        if(sizeof...(args) == 0) {
            luaL_openlibs(L.get());
            return;
        }

        lib libraries[1 + sizeof...(args)] = { lib::count, std::forward<Args>(args)... };

        for(auto&& library : libraries) {
            switch(library) {
            case lib::base:
                luaL_requiref(L.get(), "base", luaopen_base, 1);
                break;
            case lib::package:
                luaL_requiref(L.get(), "package", luaopen_package, 1);
                break;
            case lib::coroutine:
                luaL_requiref(L.get(), "coroutine", luaopen_coroutine, 1);
                break;
            case lib::string:
                luaL_requiref(L.get(), "string", luaopen_string, 1);
                break;
            case lib::table:
                luaL_requiref(L.get(), "table", luaopen_table, 1);
                break;
            case lib::math:
                luaL_requiref(L.get(), "math", luaopen_math, 1);
                break;
            case lib::bit32:
                luaL_requiref(L.get(), "bit32", luaopen_bit32, 1);
                break;
            case lib::io:
                luaL_requiref(L.get(), "io", luaopen_io, 1);
                break;
            case lib::os:
                luaL_requiref(L.get(), "os", luaopen_os, 1);
                break;
            case lib::debug:
                luaL_requiref(L.get(), "debug", luaopen_debug, 1);
                break;
            case lib::count:
                break;
            }
        }
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
        global.set(std::forward<T>(key), std::forward<U>(value));
        return *this;
    }

    template<typename T>
    table create_table(T&& key, int narr = 0, int nrec = 0) {
        if(narr == 0 && nrec == 0) {
            lua_newtable(L.get());
        }
        else {
            lua_createtable(L.get(), narr, nrec);
        }

        table result(L.get());
        lua_pop(L.get(), 1);
        global.set(std::forward<T>(key), result);
        return result;
    }

    table create_table(int narr = 0, int nrec = 0) {
        if(narr == 0 && nrec == 0) {
            lua_newtable(L.get());
        }
        else {
            lua_createtable(L.get(), narr, nrec);
        }

        table result(L.get());
        lua_pop(L.get(), 1);
        return result;
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