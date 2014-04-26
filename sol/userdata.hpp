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

#ifndef SOL_USERDATA_HPP
#define SOL_USERDATA_HPP

#include <sol/state.hpp>
#include <sol/lua_function.hpp>
#include <sol/demangle.hpp>
#include <vector>

namespace sol {

template <typename T>
class userdata {
private:
    friend table;
    static const std::string classname;
    static const std::string meta;

    std::string luaname;
    std::vector<std::string> functionnames;
    std::vector<std::unique_ptr<lua_func>> functions;
    std::vector<luaL_Reg> functiontable;
    std::vector<luaL_Reg> metatable;

    struct constructor {
        static int construct(lua_State* L) {
            // First argument is now a table that represent the class to instantiate
            luaL_checktype(L, 1, LUA_TTABLE);

            lua_createtable(L, 0, 0); // Create table to represent instance

            // Set first argument of new to metatable of instance
            lua_pushvalue(L, 1);
            lua_setmetatable(L, -2);

            // Do function lookups in metatable
            lua_pushvalue(L, 1);
            lua_setfield(L, 1, "__index");

            void* userdata = lua_newuserdata(L, sizeof(T));
            T* obj = static_cast<T*>(userdata);
            std::allocator<T> alloc{};
            alloc.construct(obj);

            luaL_getmetatable(L, meta.c_str());
            lua_setmetatable(L, -2);
            lua_setfield(L, -2, "__self");

            return 1;
        }
    };

    template <std::size_t n>
    struct destructor {
        static int destruct(lua_State* L) {
            for (std::size_t i = 0; i < n; ++i) {
                lightuserdata_t luserdata = stack::get<lightuserdata_t>(L, i);
            }
            userdata_t userdata = stack::get<userdata_t>(L, 0);
            T* obj = static_cast<T*>(userdata.value);
            std::allocator<T> alloc{};
            alloc.destroy(obj);

            return 0;
        }
    };

    template <std::size_t i>
    struct class_func {
        static int call(lua_State* L) {
            // Zero-based template parameter, but upvalues start at 1
            void* inheritancedata = stack::get<lightuserdata_t>(L, i + 1);
            if (inheritancedata == nullptr)
                throw sol_error("call from Lua to C++ function has null data");
            lua_func* pfx = static_cast<lua_func*>(inheritancedata);
            lua_func& fx = *pfx;
            int r = fx(L);
            return r;
        }
    };

    template <std::size_t n>
    void build_function_tables() {

    }

    template <std::size_t n, typename... Args, typename Ret, typename... MArgs>
    void build_function_tables(Ret(T::* func)(MArgs...), std::string name, Args&&... args) {
        typedef typename std::decay<decltype(func)>::type fx_t;
        functionnames.push_back(std::move(name));
        functions.emplace_back(std::make_unique<class_lua_func<fx_t, T>>(std::move(func)));
        functiontable.push_back({ functionnames.back().c_str(), &class_func<n>::call });
        build_function_tables<n + 1>(std::forward<Args>(args)...);
    }

public:
    template <typename... Args>
    userdata(Args&&... args) : userdata(classname, std::forward<Args>(args)...) {

    }

    template <typename... Args>
    userdata(std::string name, Args&&... args) : userdata(name, constructors<>(), std::forward<Args>(args)...) {

    }

    template <typename... Args, typename... CArgs>
    userdata(constructors<CArgs...> c, Args&&... args) : userdata(classname, std::move(c), std::forward<Args>(args)...) {

    }

    template <typename... Args, typename... CArgs>
    userdata(std::string name, constructors<CArgs...>, Args&&... args) : luaname(std::move(name)) {
        functionnames.reserve(sizeof...(args));
        functiontable.reserve(sizeof...(args));
        functions.reserve(sizeof...(args));
        metatable.reserve(sizeof...(args));
        build_function_tables<0>(std::forward<Args>(args)...);

        functionnames.push_back("new");
        functiontable.push_back({ functionnames.back().c_str(), &constructor::construct });
        functiontable.push_back({ nullptr, nullptr });

        metatable.push_back({ "__gc", &destructor<sizeof...(Args) / 2>::destruct });
        metatable.push_back({ nullptr, nullptr });
    }

    void register_into(const table& s) {

    }
};

template <typename T>
const std::string userdata<T>::classname = detail::demangle(typeid(T));

template <typename T>
const std::string userdata<T>::meta = std::string("sol.stateful.").append(classname);

}

#endif // SOL_USERDATA_HPP
