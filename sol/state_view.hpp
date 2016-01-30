// The MIT License (MIT)

// Copyright (c) 2013-2016 Rapptz and contributors

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

#ifndef SOL_STATE_VIEW_HPP
#define SOL_STATE_VIEW_HPP

#include "error.hpp"
#include "table.hpp"
#include <memory>

namespace sol {
namespace detail {
inline int atpanic(lua_State* L) {
    const char* message = lua_tostring(L, -1);
    std::string err = message ? message : "An unexpected error occurred and forced the lua state to call atpanic";
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

class state_view {
private:
    lua_State* L;
    table reg;
    global_table globals;
public:
    state_view(lua_State* L):
    L(L),
    reg(L, LUA_REGISTRYINDEX),
    globals(detail::global_overload, reg) {

    }

    lua_State* lua_state() const {
        return L;
    }

    template<typename... Args>
    void open_libraries(Args&&... args) {
        static_assert(are_same<lib, Args...>::value, "all types must be libraries");
        if(sizeof...(args) == 0) {
            luaL_openlibs(L);
            return;
        }

        lib libraries[1 + sizeof...(args)] = { lib::count, std::forward<Args>(args)... };

        for(auto&& library : libraries) {
            switch(library) {
#if SOL_LUA_VERSION <= 501 && defined(SOL_LUAJIT)
            case lib::coroutine:
#endif // luajit opens coroutine base stuff
            case lib::base:
                luaL_requiref(L, "base", luaopen_base, 1);
                lua_pop(L, 1);
                break;
            case lib::package:
                luaL_requiref(L, "package", luaopen_package, 1);
                lua_pop(L, 1);
                break;
#if SOL_LUA_VERSION > 501
            case lib::coroutine:
                luaL_requiref(L, "coroutine", luaopen_coroutine, 1);
                lua_pop(L, 1);
                break;
#endif // Lua 5.2+ only
            case lib::string:
                luaL_requiref(L, "string", luaopen_string, 1);
                lua_pop(L, 1);
                break;
            case lib::table:
                luaL_requiref(L, "table", luaopen_table, 1);
                lua_pop(L, 1);
                break;
            case lib::math:
                luaL_requiref(L, "math", luaopen_math, 1);
                lua_pop(L, 1);
                break;
            case lib::bit32:
#if SOL_LUA_VERSION > 510
                luaL_requiref(L, "bit32", luaopen_bit32, 1);
                lua_pop(L, 1);
#else
#endif // Lua 5.2+ only
                break;
            case lib::io:
                luaL_requiref(L, "io", luaopen_io, 1);
                lua_pop(L, 1);
                break;
            case lib::os:
                luaL_requiref(L, "os", luaopen_os, 1);
                lua_pop(L, 1);
                break;
            case lib::debug:
                luaL_requiref(L, "debug", luaopen_debug, 1);
                lua_pop(L, 1);
                break;
            case lib::count:
                break;
            }
        }
    }

    void script(const std::string& code) {
        if(luaL_dostring(L, code.c_str())) {
            lua_error(L);
        }
    }

    void open_file(const std::string& filename) {
        if(luaL_dofile(L, filename.c_str())) {
            lua_error(L);
        }
    }

    template<typename... Args, typename... Keys>
    auto get(Keys&&... keys) const
    -> decltype(globals.get<Args...>(std::forward<Keys>(keys)...)) {
        return globals.get<Args...>(std::forward<Keys>(keys)...);
    }

    template<typename T, typename U>
    state_view& set(T&& key, U&& value) {
        globals.set(std::forward<T>(key), std::forward<U>(value));
        return *this;
    }

    template<typename T>
    SOL_DEPRECATED table& set_userdata(usertype<T>& user) {
        return set_usertype(user);
    }

    template<typename Key, typename T>
    SOL_DEPRECATED table& set_userdata(Key&& key, usertype<T>& user) {
        return set_usertype(std::forward<Key>(key), user);
    }

    template<typename Class, typename... CTor, typename... Args>
    SOL_DEPRECATED state_view& new_userdata(const std::string& name, Args&&... args) {
        return new_usertype<Class>(name, std::forward<Args>(args)...);
    }

    template<typename Class, typename... CArgs, typename... Args>
    SOL_DEPRECATED state_view& new_userdata(const std::string& name, constructors<CArgs...> ctor, Args&&... args) {
        return new_usertype(name, std::move(ctor), std::forward<Args>(args)...);
    }

    template<typename T>
    state_view& set_usertype(usertype<T>& user) {
        return set_usertype(usertype_traits<T>::name, user);
    }

    template<typename Key, typename T>
    state_view& set_usertype(Key&& key, usertype<T>& user) {
        globals.set_usertype(std::forward<Key>(key), user);
        return *this;
    }

    template<typename Class, typename... CTor, typename... Args>
    state_view& new_usertype(const std::string& name, Args&&... args) {
        constructors<types<CTor...>> ctor{};
        return new_usertype<Class>(name, ctor, std::forward<Args>(args)...);
    }

    template<typename Class, typename... CArgs, typename... Args>
    state_view& new_usertype(const std::string& name, constructors<CArgs...> ctor, Args&&... args) {
        usertype<Class> utype(ctor, std::forward<Args>(args)...);
        set_usertype(name, utype);
        return *this;
    }

    template <typename Fx>
    void for_each(Fx&& fx) {
        globals.for_each(std::forward<Fx>(fx));
    }

    template<typename T>
    table create_table(T&& key, int narr = 0, int nrec = 0) {
        lua_createtable(L, narr, nrec);
        table result(L);
        lua_pop(L, 1);
        globals.set(std::forward<T>(key), result);
        return result;
    }

    template <typename... Tn>
    table create_table(int narr = 0, int nrec = sizeof...(Tn), Tn&&... argn) {
        lua_createtable(L, narr, nrec);
        table result(L);
	   result.set(std::forward<Tn>(argn)...);
	   lua_pop(L, 1);
        return result;
    }

    global_table global() const {
        return globals;
    }

    table registry() const {
        return reg;
    }

    void set_panic(lua_CFunction panic){
        lua_atpanic(L, panic);
    }

    template<typename T>
    proxy<global_table&, T> operator[](T&& key) {
        return globals[std::forward<T>(key)];
    }

    template<typename T>
    proxy<const global_table, T> operator[](T&& key) const {
        return globals[std::forward<T>(key)];
    }

    template<typename... Args, typename R, typename Key>
    state_view& set_function(Key&& key, R fun_ptr(Args...)){
        globals.set_function(std::forward<Key>(key), fun_ptr);
        return *this;
    }

    template<typename Sig, typename Key>
    state_view& set_function(Key&& key, Sig* fun_ptr){
        globals.set_function(std::forward<Key>(key), fun_ptr);
        return *this;
    }

    template<typename... Args, typename R, typename C, typename T, typename Key>
    state_view& set_function(Key&& key, R (C::*mem_ptr)(Args...), T&& obj) {
        globals.set_function(std::forward<Key>(key), mem_ptr, std::forward<T>(obj));
        return *this;
    }

    template<typename Sig, typename C, typename T, typename Key>
    state_view& set_function(Key&& key, Sig C::* mem_ptr, T&& obj) {
        globals.set_function(std::forward<Key>(key), mem_ptr, std::forward<T>(obj));
        return *this;
    }

    template<typename... Sig, typename Fx, typename Key>
    state_view& set_function(Key&& key, Fx&& fx) {
        globals.set_function<Sig...>(std::forward<Key>(key), std::forward<Fx>(fx));
        return *this;
    }
};
} // sol

#endif // SOL_STATE_VIEW_HPP
