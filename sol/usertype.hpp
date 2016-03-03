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

#ifndef SOL_USERTYPE_HPP
#define SOL_USERTYPE_HPP

#include "state.hpp"
#include "function_types.hpp"
#include "usertype_traits.hpp"
#include "raii.hpp"
#include "deprecate.hpp"
#include <vector>
#include <array>
#include <algorithm>
#include <map>

namespace sol {
const std::array<std::string, 2> meta_variable_names = { {
    "__index",
    "__newindex",
} };

const std::array<std::string, 21> meta_function_names = { {
    "new",
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
    "__call",
} };

enum class meta_function {
    construct,
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
    garbage_collect,
    call_function,
};

namespace usertype_detail {
struct add_destructor_tag {};
struct check_destructor_tag {};
struct verified_tag {} const verified {};

template <typename T>
struct is_constructor : std::false_type {};

template <typename... Args>
struct is_constructor<constructors<Args...>> : std::true_type {};

template <typename... Args>
struct is_constructor<constructor_wrapper<Args...>> : std::true_type {};

template <typename... Args>
using has_constructor = meta::Or<is_constructor<meta::Unqualified<Args>>...>;

template <typename T>
struct is_destructor : std::false_type {};

template <typename Fx>
struct is_destructor<destructor_wrapper<Fx>> : std::true_type {};

template <typename... Args>
using has_destructor = meta::Or<is_destructor<meta::Unqualified<Args>>...>;

template<typename T, bool refmeta, typename Funcs, typename FuncTable, typename MetaFuncTable>
inline void push_metatable(lua_State* L, bool needsindexfunction, Funcs&& funcs, FuncTable&& functable, MetaFuncTable&& metafunctable) {
    static const auto& gcname = meta_function_names[static_cast<int>(meta_function::garbage_collect)];
    luaL_newmetatable(L, &usertype_traits<T>::metatable[0]);
    int metatableindex = lua_gettop(L);
    if (funcs.size() < 1 && metafunctable.size() < 2) {
        return;
    }
    // Metamethods directly on the metatable itself
    int metaup = stack::stack_detail::push_upvalues(L, funcs);
    if (refmeta && gcname == metafunctable[metafunctable.size()-2].name) {
        // We can just "clip" out the __gc function,
        // which we always put as the last entry in the meta function table.
        luaL_Reg& target = metafunctable[metafunctable.size() - 2];
        luaL_Reg old = target;
        target = { nullptr, nullptr };
        luaL_setfuncs(L, metafunctable.data(), metaup);
        target = old;
    }
    else {
        // Otherwise, just slap it in there.
        luaL_setfuncs(L, metafunctable.data(), metaup);
    }
    if (needsindexfunction) {
        // We don't need to do anything more
        // since we've already bound the __index field using
        // setfuncs above...
        return;
    }
    // Otherwise, we use quick, fast table indexing for methods
    // gives us performance boost in calling them
    lua_createtable(L, 0, static_cast<int>(functable.size()));
    int up = stack::stack_detail::push_upvalues(L, funcs);
    luaL_setfuncs(L, functable.data(), up);
    lua_setfield(L, metatableindex, "__index");
    return;
}

template <typename T, typename Functions>
inline void set_global_deleter(lua_State* L, lua_CFunction cleanup, Functions&& functions) {
    // Automatic deleter table -- stays alive until lua VM dies
    // even if the user calls collectgarbage(), weirdly enough
    lua_createtable(L, 0, 0); // global table that sits at toplevel
    lua_createtable(L, 0, 1); // metatable for the global table
    int up = stack::stack_detail::push_upvalues<true>(L, functions);
    stack::set_field(L, "__gc", c_closure(cleanup, up));
    lua_setmetatable(L, -2);
    // gctable name by default has ♻ part of it
    lua_setglobal(L, &usertype_traits<T>::gc_table[0]);
}
}

template<typename T>
class usertype {
private:
    typedef std::map<std::string, std::pair<bool, function_detail::base_function*>> function_map_t;
    std::vector<std::string> functionnames;
    std::vector<std::unique_ptr<function_detail::base_function>> functions;
    std::vector<luaL_Reg> functiontable;
    std::vector<luaL_Reg> metafunctiontable;
    function_detail::base_function* indexfunc;
    function_detail::base_function* newindexfunc;
    function_map_t indexwrapper, newindexwrapper;
    lua_CFunction constructfunc;
    const char* destructfuncname;
    lua_CFunction destructfunc;
    lua_CFunction functiongcfunc;
    bool needsindexfunction;

    template<typename... Functions>
    std::unique_ptr<function_detail::base_function> make_function(const std::string&, overload_set<Functions...> func) {
        return std::make_unique<function_detail::usertype_overloaded_function<T, Functions...>>(std::move(func));
    }

    template<typename... Functions>
    std::unique_ptr<function_detail::base_function> make_function(const std::string&, constructor_wrapper<Functions...> func) {
        return std::make_unique<function_detail::usertype_constructor_function<T, Functions...>>(std::move(func));
    }

    template<typename Arg, typename... Args, typename Ret>
    std::unique_ptr<function_detail::base_function> make_function(const std::string&, Ret(*func)(Arg, Args...)) {
        typedef meta::Unqualified<std::remove_pointer_t<Arg>> Argu;
        static_assert(std::is_base_of<Argu, T>::value, "Any non-member-function must have a first argument which is covariant with the desired userdata type.");
        typedef std::decay_t<decltype(func)> function_type;
        return std::make_unique<function_detail::usertype_function<function_type, T>>(func);
    }

    template<typename Base, typename Ret>
    std::unique_ptr<function_detail::base_function> make_variable_function(std::true_type, const std::string&, Ret Base::* func) {
        static_assert(std::is_base_of<Base, T>::value, "Any registered function must be part of the class");
        typedef std::decay_t<decltype(func)> function_type;
        return std::make_unique<function_detail::usertype_variable_function<function_type, T>>(func);
    }

    template<typename Base, typename Ret>
    std::unique_ptr<function_detail::base_function> make_variable_function(std::false_type, const std::string&, Ret Base::* func) {
        static_assert(std::is_base_of<Base, T>::value, "Any registered function must be part of the class");
        typedef std::decay_t<decltype(func)> function_type;
        return std::make_unique<function_detail::usertype_function<function_type, T>>(func);
    }

    template<typename Base, typename Ret>
    std::unique_ptr<function_detail::base_function> make_function(const std::string& name, Ret Base::* func) {
        typedef std::decay_t<decltype(func)> function_type;
        return make_variable_function(std::is_member_object_pointer<function_type>(), name, func);
    }

    template<typename Fx>
    std::unique_ptr<function_detail::base_function> make_function(const std::string&, Fx&& func) {
        typedef meta::Unqualified<Fx> Fxu;
        typedef std::tuple_element_t<0, typename meta::function_traits<Fxu>::args_tuple_type> Arg0;
        typedef meta::Unqualified<std::remove_pointer_t<Arg0>> Argu;
        static_assert(std::is_base_of<Argu, T>::value, "Any non-member-function must have a first argument which is covariant with the desired usertype.");
        typedef std::decay_t<Fxu> function_type;
        return std::make_unique<function_detail::usertype_function<function_type, T>>(func);
    }

    template<std::size_t N, typename... Args>
    void build_function(std::string funcname, constructors<Args...>) {
        functionnames.push_back(std::move(funcname));
        std::string& name = functionnames.back();
        // Insert bubble to keep with compile-time argument count (simpler and cheaper to do)
        functions.push_back(nullptr);
        constructfunc = function_detail::construct<T, Args...>;
        metafunctiontable.push_back({ name.c_str(), constructfunc });
    }

    template<std::size_t N>
    void build_function(std::string funcname, destructor_wrapper<void>) {
        auto metamethodfind = std::find(meta_function_names.begin(), meta_function_names.end(), funcname);
        if (metamethodfind == meta_function_names.end())
            throw error("cannot set destructor to anything but the metamethod \"__gc\"");
        meta_function metafunction = static_cast<meta_function>(metamethodfind - meta_function_names.begin());
        if (metafunction != meta_function::garbage_collect)
            throw error("cannot set destructor to anything but the metamethod \"__gc\"");
            
        functionnames.push_back(std::move(funcname));
        std::string& name = functionnames.back();
        destructfunc = function_detail::destruct<T>;
        destructfuncname = name.c_str();
        // Insert bubble to stay with the compile-time count
        functions.push_back(nullptr);    
    }

    template<std::size_t N, typename Fx>
    void build_function(std::string funcname, destructor_wrapper<Fx> dx) {
        auto metamethodfind = std::find(meta_function_names.begin(), meta_function_names.end(), funcname);
        if (metamethodfind == meta_function_names.end())
            throw error("cannot set destructor to anything but the metamethod \"__gc\"");
        meta_function metafunction = static_cast<meta_function>(metamethodfind - meta_function_names.begin());
        if (metafunction != meta_function::garbage_collect)
            throw error("cannot set destructor to anything but the metamethod \"__gc\"");

        functionnames.push_back(std::move(funcname));
        std::string& name = functionnames.back();
        auto baseptr = make_function(name, std::move(dx.fx));
        functions.emplace_back(std::move(baseptr));
        destructfunc = function_detail::usertype_call<N>;
        destructfuncname = name.c_str();
    }

    template<std::size_t N, typename Fx>
    void build_function(std::string funcname, Fx&& func) {
        typedef std::is_member_object_pointer<meta::Unqualified<Fx>> is_variable;
        functionnames.push_back(std::move(funcname));
        std::string& name = functionnames.back();
        auto baseptr = make_function(name, std::forward<Fx>(func));
        functions.emplace_back(std::move(baseptr));
        auto metamethodfind = std::find(meta_function_names.begin(), meta_function_names.end(), name);
        if (metamethodfind != meta_function_names.end()) {
            metafunctiontable.push_back({ name.c_str(), function_detail::usertype_call<N> });
            meta_function metafunction = static_cast<meta_function>(metamethodfind - meta_function_names.begin());
            switch (metafunction) {
            case meta_function::garbage_collect:
                destructfuncname = name.c_str();
                destructfunc = function_detail::usertype_call<N>;
                return;
            case meta_function::index:
                indexfunc = functions.back().get();
                needsindexfunction = true;
                break;
            case meta_function::new_index:
                newindexfunc = functions.back().get();
                break;
            case meta_function::construct:
                constructfunc = function_detail::usertype_call<N>;
                break;
            default:
                break;
            }
            return;
        }
        if (is_variable::value) {
            needsindexfunction = true;
            indexwrapper.insert({ name, { false, functions.back().get() } });
            newindexwrapper.insert({ name, { false, functions.back().get() } });
            return;
        }
        indexwrapper.insert({ name, { true, functions.back().get() } });
        functiontable.push_back({ name.c_str(), function_detail::usertype_call<N> });
    }

    template<std::size_t N, typename Fx, typename... Args>
    void build_function_tables(std::string funcname, Fx&& func, Args&&... args) {
        build_function<N>(std::move(funcname), std::forward<Fx>(func));
        build_function_tables<N + 1>(std::forward<Args>(args)...);
    }

    template<std::size_t N, typename Fx, typename... Args>
    void build_function_tables(meta_function metafunc, Fx&& func, Args&&... args) {
        std::size_t idx = static_cast<std::size_t>(metafunc);
        const std::string& funcname = meta_function_names[idx];
        build_function_tables<N>(funcname, std::forward<Fx>(func), std::forward<Args>(args)...);
    }

    template<std::size_t N>
    void build_function_tables() {
        int variableend = 0;
        if (!indexwrapper.empty()) {
            functions.push_back(std::make_unique<function_detail::usertype_indexing_function>("__index", indexfunc, std::move(indexwrapper)));
            metafunctiontable.push_back({ "__index", function_detail::usertype_call<N> });
            ++variableend;
        }
        if (!newindexwrapper.empty()) {
            functions.push_back(std::make_unique<function_detail::usertype_indexing_function>("__newindex", newindexfunc, std::move(newindexwrapper)));
            metafunctiontable.push_back({ "__newindex", indexwrapper.empty() ? function_detail::usertype_call<N> : function_detail::usertype_call<N + 1> });
            ++variableend;
        }
        if (destructfunc != nullptr) {
            metafunctiontable.push_back({ destructfuncname, destructfunc });
        }
        switch (variableend) {
        case 2:
            functiongcfunc = function_detail::usertype_gc<N + 2>;
            break;
        case 1:
            functiongcfunc = function_detail::usertype_gc<N + 1>;
            break;
        case 0:
            functiongcfunc = function_detail::usertype_gc<N + 0>;
            break;
        }
    }

    template<typename... Args>
    usertype(usertype_detail::verified_tag, Args&&... args) : indexfunc(nullptr), newindexfunc(nullptr), constructfunc(nullptr), destructfunc(nullptr), functiongcfunc(nullptr), needsindexfunction(false) {
        functionnames.reserve(sizeof...(args)+3);
        functiontable.reserve(sizeof...(args)+3);
        metafunctiontable.reserve(sizeof...(args)+3);

        build_function_tables<0>(std::forward<Args>(args)...);
        metafunctiontable.push_back({ nullptr, nullptr });
        functiontable.push_back({ nullptr, nullptr });
    }

    template<typename... Args>
    usertype(usertype_detail::add_destructor_tag, Args&&... args) : usertype(usertype_detail::verified, "__gc", default_destructor, std::forward<Args>(args)...) {}

    template<typename... Args>
    usertype(usertype_detail::check_destructor_tag, Args&&... args) : usertype(meta::If<meta::And<std::is_destructible<T>, meta::Not<usertype_detail::has_destructor<Args...>>>, usertype_detail::add_destructor_tag, usertype_detail::verified_tag>(), std::forward<Args>(args)...) {}

public:

    template<typename... Args>
    usertype(Args&&... args) : usertype(meta::If<meta::And<std::is_default_constructible<T>, meta::Not<usertype_detail::has_constructor<Args...>>>, decltype(default_constructor), usertype_detail::check_destructor_tag>(), std::forward<Args>(args)...) {}

    template<typename... Args, typename... CArgs>
    usertype(constructors<CArgs...> constructorlist, Args&&... args) : usertype(usertype_detail::verified, "new", constructorlist, "__gc", default_destructor, std::forward<Args>(args)...) {
            
    }

    int push(lua_State* L) {
        // push pointer tables first,
        usertype_detail::push_metatable<T*, true>(L, needsindexfunction, functions, functiontable, metafunctiontable);
        lua_pop(L, 1);
        // but leave the regular T table on last
        // so it can be linked to a type for usage with `.new(...)` or `:new(...)`
        usertype_detail::push_metatable<T, false>(L, needsindexfunction, functions, functiontable, metafunctiontable);
        // Make sure to drop a table in the global namespace to properly destroy the pushed functions
        // at some later point in life
        usertype_detail::set_global_deleter<T>(L, functiongcfunc, functions);
        return 1;
    }
};

namespace stack {
template<typename T>
struct pusher<usertype<T>> {
    static int push(lua_State* L, usertype<T>& user) {
        return user.push(L);
    }
};
} // stack
} // sol

#endif // SOL_USERTYPE_HPP
