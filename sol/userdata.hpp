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

#include "state.hpp"
#include "function_types.hpp"
#include "userdata_traits.hpp"
#include "default_construct.hpp"
#include <vector>
#include <array>
#include <algorithm>

namespace sol {
namespace detail {
template<typename T, typename... Args>
inline std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
} // detail

template<typename T>
class userdata {
private:
    const static std::array<std::string, 19> metafunctionnames;
    std::string luaname;
    std::vector<std::string> functionnames;
    std::vector<std::unique_ptr<base_function>> funcs;
    std::vector<std::unique_ptr<base_function>> ptrfuncs;
    std::vector<std::unique_ptr<base_function>> metafuncs;
    std::vector<std::unique_ptr<base_function>> ptrmetafuncs;
    std::vector<luaL_Reg> functiontable;
    std::vector<luaL_Reg> ptrfunctiontable;
    std::vector<luaL_Reg> metafunctiontable;
    std::vector<luaL_Reg> ptrmetafunctiontable;
    lua_CFunction cleanup;

    template<typename... TTypes>
    struct constructor {
        template<typename... Args>
        static void do_constructor(lua_State* L, T* obj, call_syntax syntax, int, types<Args...>) {
            default_construct fx{};
            stack::get_call(L, 1 + static_cast<int>(syntax), fx, types<Args...>(), obj);
        }

        static void match_constructor(lua_State*, T*, call_syntax, int) {
            throw error("No matching constructor for the arguments provided");
        }

        template<typename ...CArgs, typename... Args>
        static void match_constructor(lua_State* L, T* obj, call_syntax syntax, int argcount, types<CArgs...> t, Args&&... args) {
            if (argcount == sizeof...(CArgs)) {
                do_constructor(L, obj, syntax, argcount, t);
                return;
            }
            match_constructor(L, obj, syntax, argcount, std::forward<Args>(args)...);
        }

        static int construct(lua_State* L) {
            auto&& meta = userdata_traits<T>::metatable;
            call_syntax syntax = stack::get_call_syntax(L, meta);
            int argcount = lua_gettop(L);

            void* udata = lua_newuserdata(L, sizeof(T));
            T* obj = static_cast<T*>(udata);
            match_constructor(L, obj, syntax, argcount - static_cast<int>(syntax), typename identity<TTypes>::type()...);


            if (luaL_newmetatable(L, std::addressof(meta[0])) == 1) {
                lua_pop(L, 1);
                std::string err = "Unable to get userdata metatable for ";
                err += meta;
                throw error(err);
            }

            lua_setmetatable(L, -2);

            return 1;
        }
    };

    struct destructor {
        static int destruct(lua_State* L) {
            userdata_t udata = stack::get<userdata_t>(L, 1);
            T* obj = static_cast<T*>(udata.value);
            std::allocator<T> alloc{};
            alloc.destroy(obj);
            return 0;
        }
    };

    template <bool release = false, typename TCont>
    static int push_upvalues (lua_State* L, TCont&& cont) {
        int n = 0;
        for (auto& c : cont) {
            if (release)
               stack::push<upvalue_t>(L, c.release());
            else
                stack::push<upvalue_t>(L, c.get());
            ++n;
        }
        return n;
    }

    template <typename Meta, typename Funcs, typename FuncTable, typename MetaFuncs, typename MetaFuncTable>
    static void push_metatable(lua_State* L, Meta&& meta, Funcs&& funcs, FuncTable&& functable, MetaFuncs&& metafuncs, MetaFuncTable&& metafunctable) {
        luaL_newmetatable(L, std::addressof(meta[0]));
        if (functable.size() > 1) {
            // regular functions accessed through __index semantics
            int up = push_upvalues(L, funcs);
            luaL_setfuncs(L, functable.data(), up);
        }
        if (metafunctable.size() > 1) {
            // meta functions
            int up = push_upvalues(L, metafuncs);
            luaL_setfuncs(L, metafunctable.data(), up);
        }
        lua_pushvalue(L, -1);
        lua_setfield(L, -1, "__index");
    }

    template<std::size_t N, std::size_t M>
    void build_function_tables() {}

    template<std::size_t N, std::size_t M, typename... Args, typename TBase, typename Ret>
    void build_function_tables(std::string funcname, Ret TBase::* func, Args&&... args) {
        static_assert(std::is_base_of<TBase, T>::value, "Any registered function must be part of the class");
        typedef typename std::decay<decltype(func)>::type function_type;
        functionnames.push_back(std::move(funcname));
        std::string& name = functionnames.back();
        auto metamethod = std::find(metafunctionnames.begin(), metafunctionnames.end(), name);
        if (metamethod != metafunctionnames.end()) {
            metafuncs.emplace_back(detail::make_unique<userdata_function<function_type, T>>(std::move(func)));
            ptrmetafuncs.emplace_back(detail::make_unique<userdata_function<function_type, typename std::add_pointer<T>::type>>(std::move(func)));
            metafunctiontable.push_back({ name.c_str(), &base_function::userdata<N>::call });
            ptrmetafunctiontable.push_back({ name.c_str(), &base_function::userdata<N>::call });
            build_function_tables<N + 1, M>(std::forward<Args>(args)...);
        }
        else {
            funcs.emplace_back(detail::make_unique<userdata_function<function_type, T>>(std::move(func)));
            ptrfuncs.emplace_back(detail::make_unique<userdata_function<function_type, typename std::add_pointer<T>::type>>(std::move(func)));
            functiontable.push_back({ name.c_str(), &base_function::userdata<M>::call });
            ptrfunctiontable.push_back({ name.c_str(), &base_function::userdata<M>::call });
            build_function_tables<N, M + 1>(std::forward<Args>(args)...);
        }
    }

public:
    template<typename... Args>
    userdata(Args&&... args): userdata(userdata_traits<T>::name, default_constructor, std::forward<Args>(args)...) {}

    template<typename... Args, typename... CArgs>
    userdata(constructors<CArgs...> c, Args&&... args): userdata(userdata_traits<T>::name, std::move(c), std::forward<Args>(args)...) {}

    template<typename... Args, typename... CArgs>
    userdata(std::string name, constructors<CArgs...>, Args&&... args): luaname(std::move(name)) {
        functionnames.reserve(sizeof...(args) + 2);

        functiontable.reserve(sizeof...(args));
        ptrfunctiontable.reserve(sizeof...(args));
        metafunctiontable.reserve(sizeof...(args));
        ptrmetafunctiontable.reserve(sizeof...(args));

        funcs.reserve(sizeof...(args));
        ptrfuncs.reserve(sizeof...(args));
        metafuncs.reserve(sizeof...(args));
        ptrmetafuncs.reserve(sizeof...(args));

        cleanup = &base_function::userdata_gc<sizeof...(Args)>::gc;

        build_function_tables<0, 0>(std::forward<Args>(args)...);
        functionnames.push_back("new");
        functiontable.push_back({ functionnames.back().c_str(), &constructor<CArgs...>::construct });
        functionnames.push_back("__gc");
        metafunctiontable.push_back({ functionnames.back().c_str(), &destructor::destruct });
        // ptr_functions does not participate in garbage collection/new,
        // as all pointered types are considered
        // to be references. This makes returns of
        // `std::vector<int>&` and `std::vector<int>*` work

        functiontable.push_back({ nullptr, nullptr });
        metafunctiontable.push_back({ nullptr, nullptr });
        ptrfunctiontable.push_back({ nullptr, nullptr });
        ptrmetafunctiontable.push_back({ nullptr, nullptr });
    }

    template<typename... Args, typename... CArgs>
    userdata(const char* name, constructors<CArgs...> c, Args&&... args) :
        userdata(std::string(name), std::move(c), std::forward<Args>(args)...) {}

    const std::string& name () const {
        return luaname;
    }

    void push (lua_State* L) {
        // push pointer tables first,
        // but leave the regular T table on last so it can be linked to a type for usage with `.new(...)`
        push_metatable(L, userdata_traits<T*>::metatable,
                       ptrfuncs, ptrfunctiontable,
                       ptrmetafuncs, ptrmetafunctiontable);
        push_metatable(L, userdata_traits<T>::metatable,
                       funcs, functiontable,
                       metafuncs, metafunctiontable);
        // Automatic deleter table -- stays alive until lua VM dies
        // even if the user calls collectgarbage()
        lua_createtable(L, 0, 0);
        lua_createtable(L, 0, 1);
        int up = push_upvalues<true>(L, funcs);
        up += push_upvalues<true>(L, ptrfuncs);
        up += push_upvalues<true>(L, metafuncs);
        up += push_upvalues<true>(L, ptrmetafuncs);
        lua_pushcclosure(L, cleanup, up);
        lua_setfield(L, -2, "__gc");
        lua_setmetatable(L, -2);
        // gctable name by default has ♻ part of it
        lua_setglobal(L, std::addressof(userdata_traits<T>::gctable[0]));
    }
};

template <typename T>
const std::array<std::string, 19> userdata<T>::metafunctionnames = {
    "__index",
    "__newindex",
    "__mode",
    "__call",
    "__metatable",
    "__tostring",
    "__len",
    "__gc",
    "__unm",
    "__add",
    "__sub",
    "__mul",
    "__div",
    "__mod",
    "__pow",
    "__concat",
    "__eq",
    "__lt",
    "__le",
};

namespace stack {
template <typename T>
struct pusher<userdata<T>> {
    static void push (lua_State* L, userdata<T>& user) {
        user.push(L);
    }
};
} // stack

} // sol

#endif // SOL_USERDATA_HPP
