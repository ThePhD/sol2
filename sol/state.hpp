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
inline int atpanic(lua_State* L) {
    std::string err = lua_tostring(L, -1);
    throw error(err);
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
    std::vector<std::shared_ptr<void>> classes;
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

    template<typename... Args>
    void open_libraries(Args&&... args) {
        static_assert(are_same<lib, Args...>::value, "all types must be libraries");
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
        if (luaL_dofile(L.get(), filename.c_str())) {
            lua_error(L.get());
        }
    }

    template<typename... Args, typename... Keys>
    auto get(Keys&&... keys) const
    -> decltype(global.get(types<Args...>(), std::forward<Keys>(keys)...)) {
       return global.get(types<Args...>(), std::forward<Keys>(keys)...);
    }

    template<typename T, typename U>
    state& set(T&& key, U&& value) {
        global.set(std::forward<T>(key), std::forward<U>(value));
        return *this;
    }

    template<typename T, typename TFx>
    state& set_function(T&& key, TFx&& fx) {
        global.set_function(std::forward<T>(key), std::forward<TFx>(fx));
        return *this;
    }

    template<typename T, typename TFx, typename TObj>
    state& set_function(T&& key, TFx&& fx, TObj&& obj) {
        global.set_function(std::forward<T>(key), std::forward<TFx>(fx), std::forward<TObj>(obj));
        return *this;
    }

    template<typename T>
    state& set_userdata(userdata<T>& user) {
        global.set_userdata(user);
        return *this;
    }

    template<typename Class, typename... CTor, typename... Args>
    state& new_userdata(const std::string& name, Args&&... args) {
        constructors<types<CTor...>> ctor;
        classes.emplace_back(std::make_shared<userdata<Class>>(name, ctor, std::forward<Args>(args)...));
        auto&& ptr = classes.back();
        auto udata = std::static_pointer_cast<userdata<Class>>(ptr);
        global.set_userdata(*udata);
        return *this;
    }

    template<typename T>
    table create_table(T&& key, int narr = 0, int nrec = 0) {
        lua_createtable(L.get(), narr, nrec);
        table result(L.get());
        lua_pop(L.get(), 1);
        global.set(std::forward<T>(key), result);
        return result;
    }

    table create_table(int narr = 0, int nrec = 0) {
        lua_createtable(L.get(), narr, nrec);
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

    template<typename T>
    proxy<table, T> operator[](T&& key) {
        return global[std::forward<T>(key)];
    }

    template<typename T>
    proxy<const table, T> operator[](T&& key) const {
        return global[std::forward<T>(key)];
    }

    lua_State* lua_state() const {
        return L.get();
    }
};
} // sol

#endif // SOL_STATE_HPP
