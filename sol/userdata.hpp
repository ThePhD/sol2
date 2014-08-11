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

const std::array<std::string, 2> meta_variable_names = {{
    "__index",
    "__newindex"
}};

const std::array<std::string, 19> meta_function_names = {{
    "__index",
    "__newindex",
    "__mode",
    "__call",
    "__metatable",
    "__tostring",
    "__len",
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
    "__gc",
}};

enum class meta_function {
    index,
    new_index,
    mode,
    call,
    metatable,
    to_string,
    length,
    unary_minus,
    addition,
    subtraction,
    multiplication,
    division,
    modulus,
    power_of,
    involution = power_of,
    concatenation,
    equal_to,
    less_than,
    less_than_or_equal_to,
};

template<typename T>
class userdata {
private:
    typedef std::unordered_map<std::string, std::pair<std::unique_ptr<base_function>, bool>> function_map_t;
    function_map_t indexmetafunctions, newindexmetafunctions;
    std::vector<std::string> functionnames;
    std::vector<std::unique_ptr<base_function>> metafunctions;
    std::vector<luaL_Reg> metafunctiontable;
    std::vector<luaL_Reg> ptrmetafunctiontable;
    lua_CFunction cleanup;
    std::string luaname;

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
            if(argcount == sizeof...(CArgs)) {
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

            if(luaL_newmetatable(L, std::addressof(meta[0])) == 1) {
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

    template<std::size_t N>
    void build_cleanup() {
        cleanup = &base_function::userdata<N>::gc;
    }

    template<std::size_t N>
    void build_function_tables(function_map_t*& index, function_map_t*& newindex) {
        int extracount = 0;
        if(!indexmetafunctions.empty()) {
            if(index == nullptr) {
                auto idxptr = detail::make_unique<userdata_indexing_function<void (T::*)(), T>>("__index", nullptr);
                index = &(idxptr->functions);
                functionnames.emplace_back("__index");
                metafunctions.emplace_back(std::move(idxptr));
                std::string& name = functionnames.back();
                metafunctiontable.push_back({ name.c_str(), &base_function::userdata<N>::call });
                ptrmetafunctiontable.push_back({ name.c_str(), &base_function::userdata<N>::ref_call });
                ++extracount;
            }
            auto& idx = *index;
            for(auto&& namedfunc : indexmetafunctions) {
                idx.emplace(std::move(namedfunc.first), std::move(namedfunc.second));
            }
        }
        if(!newindexmetafunctions.empty()) {
            if(newindex == nullptr) {
                auto idxptr = detail::make_unique<userdata_indexing_function<void (T::*)(), T>>("__newindex", nullptr);
                newindex = &(idxptr->functions);
                functionnames.emplace_back("__newindex");
                metafunctions.emplace_back(std::move(idxptr));
                std::string& name = functionnames.back();
                if(extracount > 0) {
                    metafunctiontable.push_back({ name.c_str(), &base_function::userdata<N + 1>::call });
                    ptrmetafunctiontable.push_back({ name.c_str(), &base_function::userdata<N + 1>::ref_call });
                }
                else {
                    metafunctiontable.push_back({ name.c_str(), &base_function::userdata<N>::call });
                    ptrmetafunctiontable.push_back({ name.c_str(), &base_function::userdata<N>::ref_call });
                }
                ++extracount;
            }
            auto& idx = *newindex;
            for(auto&& namedfunc : newindexmetafunctions) {
                idx.emplace(std::move(namedfunc.first), std::move(namedfunc.second));
            }
        }
        switch(extracount) {
        case 2:
            build_cleanup<N + 2>();
            break;
        case 1:
            build_cleanup<N + 1>();
            break;
        case 0:
        default:
            build_cleanup<N + 0>();
            break;
        }
    }

    template<std::size_t N, typename Base, typename Ret>
    bool build_function(std::true_type, function_map_t*&, function_map_t*&, std::string funcname, Ret Base::* func) {
        static_assert(std::is_base_of<Base, T>::value, "Any registered function must be part of the class");
        typedef typename std::decay<decltype(func)>::type function_type;
        indexmetafunctions.emplace(funcname, std::make_pair(detail::make_unique<userdata_variable_function<function_type, T>>(func), false));
        newindexmetafunctions.emplace(funcname, std::make_pair(detail::make_unique<userdata_variable_function<function_type, T>>(func), false));
        return false;
    }

    template<typename Arg, typename... Args, typename Ret>
    std::unique_ptr<base_function> make_function(const std::string&, Ret(*func)(Arg, Args...)) {
        typedef Unqualified<Arg> Argu;
        static_assert(std::is_base_of<Argu, T>::value, "Any non-member-function must have a first argument which is covariant with the desired userdata type.");
        typedef typename std::decay<decltype(func)>::type function_type;
        return detail::make_unique<userdata_function<function_type, T>>(func);
    }

    template<typename Base, typename Ret>
    std::unique_ptr<base_function> make_variable_function(std::true_type, const std::string&, Ret Base::* func) {
        static_assert(std::is_base_of<Base, T>::value, "Any registered function must be part of the class");
        typedef typename std::decay<decltype(func)>::type function_type;
        return detail::make_unique<userdata_variable_function<function_type, T>>(func);
    }

    template<typename Base, typename Ret>
    std::unique_ptr<base_function> make_variable_function(std::false_type, const std::string&, Ret Base::* func) {
        static_assert(std::is_base_of<Base, T>::value, "Any registered function must be part of the class");
        typedef typename std::decay<decltype(func)>::type function_type;
        return detail::make_unique<userdata_function<function_type, T>>(func);
    }

    template<typename Base, typename Ret>
    std::unique_ptr<base_function> make_function(const std::string& name, Ret Base::* func) {
        typedef typename std::decay<decltype(func)>::type function_type;
        return make_variable_function(std::is_member_object_pointer<function_type>(), name, func);
    }

    template<typename Fx>
    std::unique_ptr<base_function> make_function(const std::string&, Fx&& func) {
        typedef Unqualified<Fx> Fxu;
        typedef typename std::tuple_element<0, typename function_traits<Fxu>::arg_tuple_type>::type TArg;
        typedef Unqualified<TArg> TArgu;
        static_assert(std::is_base_of<TArgu, T>::value, "Any non-member-function must have a first argument which is covariant with the desired userdata type.");
        typedef typename std::decay<decltype(func)>::type function_type;
        return detail::make_unique<userdata_function<function_type, T>>(func);
    }

    template<std::size_t N, typename Fx>
    bool build_function(std::false_type, function_map_t*& index, function_map_t*& newindex, std::string funcname, Fx&& func) {
        typedef typename std::decay<Fx>::type function_type;
        auto metamethod = std::find(meta_function_names.begin(), meta_function_names.end(), funcname);
        if(metamethod != meta_function_names.end()) {
            functionnames.push_back(std::move(funcname));
            std::string& name = functionnames.back();
            auto indexmetamethod = std::find(meta_variable_names.begin(), meta_variable_names.end(), name);
            std::unique_ptr<base_function> ptr(nullptr);
            if(indexmetamethod != meta_variable_names.end()) {
                auto idxptr = detail::make_unique<userdata_indexing_function<function_type, T>>(name, func);
                switch(std::distance(indexmetamethod, meta_variable_names.end())) {
                case 0:
                    index = &(idxptr->functions);
                    break;
                case 1:
                    newindex = &(idxptr->functions);
                    break;
                default:
                    break;
                }
                ptr = std::move(idxptr);
            }
            else {
                ptr = make_function(funcname, std::forward<Fx>(func));
            }
            metafunctions.emplace_back(std::move(ptr));
            metafunctiontable.push_back( { name.c_str(), &base_function::userdata<N>::call } );
            ptrmetafunctiontable.push_back( { name.c_str(), &base_function::userdata<N>::ref_call } );
            return true;
        }
        indexmetafunctions.emplace(funcname, std::make_pair(make_function(funcname, std::forward<Fx>(func)), true));
        return false;
    }

    template<std::size_t N, typename Fx, typename... Args>
    void build_function_tables(function_map_t*& index, function_map_t*& newindex, std::string funcname, Fx&& func, Args&&... args) {
        typedef typename std::is_member_object_pointer<Unqualified<Fx>>::type is_variable;
        static const std::size_t V = static_cast<std::size_t>(!is_variable::value);
        if(build_function<N>(is_variable(), index, newindex, std::move(funcname), std::forward<Fx>(func))) {
            build_function_tables<N + V>(index, newindex, std::forward<Args>(args)...);
        }
        else {
            build_function_tables<N>(index, newindex, std::forward<Args>(args)...);
        }
    }

    template<std::size_t N, typename Base, typename Ret, typename... Args>
    void build_function_tables(function_map_t*& index, function_map_t*& newindex, meta_function metafunc, Ret Base::* func, Args&&... args) {
        std::size_t idx = static_cast<std::size_t>(metafunc);
        const std::string& funcname = meta_function_names[idx];
        build_function_tables<N>(index, newindex, funcname, std::move(func), std::forward<Args>(args)...);
    }

public:
    template<typename... Args>
    userdata(Args&&... args): userdata(userdata_traits<T>::name, default_constructor, std::forward<Args>(args)...) {}

    template<typename... Args, typename... CArgs>
    userdata(constructors<CArgs...> c, Args&&... args): userdata(userdata_traits<T>::name, std::move(c), std::forward<Args>(args)...) {}

    template<typename... Args, typename... CArgs>
    userdata(std::string name, constructors<CArgs...>, Args&&... args): luaname(std::move(name)) {
        functionnames.reserve(sizeof...(args) + 2);
        metafunctiontable.reserve(sizeof...(args));
        ptrmetafunctiontable.reserve(sizeof...(args));

        function_map_t* index = nullptr;
        function_map_t* newindex = nullptr;
        build_function_tables<0>(index, newindex, std::forward<Args>(args)...);
        indexmetafunctions.clear();
        newindexmetafunctions.clear();
        functionnames.push_back("new");
        metafunctiontable.push_back({ functionnames.back().c_str(), &constructor<CArgs...>::construct });
        functionnames.push_back("__gc");
        metafunctiontable.push_back({ functionnames.back().c_str(), &destructor::destruct });
        // ptr_functions does not participate in garbage collection/new,
        // as all pointered types are considered
        // to be references. This makes returns of
        // `std::vector<int>&` and `std::vector<int>*` work

        metafunctiontable.push_back({ nullptr, nullptr });
        ptrmetafunctiontable.push_back({ nullptr, nullptr });
    }

    template<typename... Args, typename... CArgs>
    userdata(const char* name, constructors<CArgs...> c, Args&&... args) :
        userdata(std::string(name), std::move(c), std::forward<Args>(args)...) {}

    const std::string& name() const {
        return luaname;
    }

    void push(lua_State* L) {
        // push pointer tables first,
        // but leave the regular T table on last
        // so it can be linked to a type for usage with `.new(...)` or `:new(...)`
        push_metatable(L, userdata_traits<T*>::metatable,
                       metafunctions, ptrmetafunctiontable);
        lua_pop(L, 1);

        push_metatable(L, userdata_traits<T>::metatable,
                       metafunctions, metafunctiontable);
        set_global_deleter(L);
    }
private:

    template<typename Meta, typename MetaFuncs, typename MetaFuncTable>
    static void push_metatable(lua_State* L, Meta&& metakey, MetaFuncs&& metafuncs, MetaFuncTable&& metafunctable) {
        luaL_newmetatable(L, std::addressof(metakey[0]));
        if(metafunctable.size() > 1) {
            // regular functions accessed through __index semantics
            int up = push_upvalues(L, metafuncs);
            luaL_setfuncs(L, metafunctable.data(), up);
        }

    }

    void set_global_deleter(lua_State* L) {
        // Automatic deleter table -- stays alive until lua VM dies
        // even if the user calls collectgarbage()
        lua_createtable(L, 0, 0);
        lua_createtable(L, 0, 1);
        int up = push_upvalues<true>(L, metafunctions);
        lua_pushcclosure(L, cleanup, up);
        lua_setfield(L, -2, "__gc");
        lua_setmetatable(L, -2);
        // gctable name by default has ♻ part of it
        lua_setglobal(L, std::addressof(userdata_traits<T>::gctable[0]));
    }

    template<bool release = false, typename TCont>
    static int push_upvalues(lua_State* L, TCont&& cont) {
        int n = 0;
        for(auto& c : cont) {
            if(release) {
               stack::push<upvalue_t>(L, c.release());
            }
            else {
                stack::push<upvalue_t>(L, c.get());
            }
            ++n;
        }
        return n;
    }
};

namespace stack {
template<typename T>
struct pusher<userdata<T>> {
    static void push(lua_State* L, userdata<T>& user) {
        user.push(L);
    }
};
} // stack
} // sol

#endif // SOL_USERDATA_HPP
