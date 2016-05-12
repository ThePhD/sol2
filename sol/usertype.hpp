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
#include "inheritance.hpp"
#include "raii.hpp"
#include "deprecate.hpp"
#include <vector>
#include <array>
#include <algorithm>
#include <map>

namespace sol {
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

template <>
struct is_constructor<no_construction> : std::true_type {};

template <typename... Args>
using has_constructor = meta::Or<is_constructor<meta::Unqualified<Args>>...>;

template <typename T>
struct is_destructor : std::false_type {};

template <typename Fx>
struct is_destructor<destructor_wrapper<Fx>> : std::true_type {};

template <typename... Args>
using has_destructor = meta::Or<is_destructor<meta::Unqualified<Args>>...>;

enum class stage {
    normalmeta,
    refmeta,
    uniquemeta,
};

template<bool releasemem = false, typename TCont>
inline int push_upvalues(lua_State* L, TCont&& cont) {
    int n = 0;
    for(auto& c : cont) {
        if(releasemem) {
            stack::push<light_userdata_value>(L, c.release());
        }
        else {
            stack::push<light_userdata_value>(L, c.get());
        }
        ++n;
    }
    return n;
}

template<typename T, stage metastage>
inline void push_metatable(lua_State* L, bool needsindexfunction, std::vector<std::unique_ptr<function_detail::base_function>>& funcs, std::vector<luaL_Reg>& functable, std::vector<luaL_Reg>& metafunctable, void* baseclasscheck, void* baseclasscast) {
    static const auto& gcname = meta_function_names[static_cast<int>(meta_function::garbage_collect)];
    luaL_newmetatable(L, &usertype_traits<T>::metatable[0]);
    int metatableindex = lua_gettop(L);
    if (baseclasscheck != nullptr) {
        stack::push(L, light_userdata_value(baseclasscheck));
        lua_setfield(L, metatableindex, &detail::base_class_check_key()[0]);
    }
    if (baseclasscast != nullptr) {
        stack::push(L, light_userdata_value(baseclasscast));
        lua_setfield(L, metatableindex, &detail::base_class_cast_key()[0]);
    }
    if (funcs.size() < 1 && metafunctable.size() < 2) {
        return;
    }
  
    // Functions should be placed on the metatable so that they can be called "statically" if the user wants
    int up = push_upvalues(L, funcs);
    functable.push_back({ nullptr, nullptr });
    luaL_setfuncs(L, functable.data(), up);
    functable.pop_back();

    // Metamethods directly on the metatable itself
    int metaup = push_upvalues(L, funcs);
    switch (metastage) {
        case stage::uniquemeta: {
            if (gcname != metafunctable.back().name) {
                metafunctable.push_back({ "__gc", nullptr });
            }
            luaL_Reg& target = metafunctable.back();
            luaL_Reg old = target;
            target.func = detail::unique_destruct<T>;
            metafunctable.push_back({nullptr, nullptr});
            luaL_setfuncs(L, metafunctable.data(), metaup);
            metafunctable.pop_back();
            target = old;
            break; }
        case stage::refmeta:
            if (gcname == metafunctable.back().name) {
                // We can just "clip" out the __gc function,
                // which we always put as the last entry in the meta function table.
                luaL_Reg& target = metafunctable.back();
                luaL_Reg old = target;
                target = { nullptr, nullptr };
                luaL_setfuncs(L, metafunctable.data(), metaup);
                target = old;
            }
            break;
        case stage::normalmeta:
        default:
            metafunctable.push_back({nullptr, nullptr});
            luaL_setfuncs(L, metafunctable.data(), metaup);
            metafunctable.pop_back();
            break;
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
    up = push_upvalues(L, funcs);
    functable.push_back({nullptr, nullptr});
    luaL_setfuncs(L, functable.data(), up);
    functable.pop_back();
    lua_setfield(L, metatableindex, "__index");
    return;
}

template <typename T, typename Functions>
inline void set_global_deleter(lua_State* L, lua_CFunction cleanup, Functions&& functions) {
    // Automatic deleter table -- stays alive until lua VM dies
    // even if the user calls collectgarbage(), weirdly enough
    lua_createtable(L, 0, 0); // global table that sits at toplevel
    lua_createtable(L, 0, 1); // metatable for the global table
    int up = push_upvalues<true>(L, functions);
    stack::set_field(L, "__gc", c_closure(cleanup, up));
    lua_setmetatable(L, -2);
    // gctable name by default has ♻ part of it
    lua_setglobal(L, &usertype_traits<T>::gc_table[0]);
}
} // usertype_detail

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
    const char* constructfuncname;
    lua_CFunction constructfunc;
    const char* destructfuncname;
    lua_CFunction destructfunc;
    lua_CFunction functiongcfunc;
    bool needsindexfunction;
    void* baseclasscheck;
    void* baseclasscast;

    template<typename... Functions>
    std::unique_ptr<function_detail::base_function> make_function(const std::string&, overload_set<Functions...> func) {
        return std::make_unique<function_detail::usertype_overloaded_function<T, Functions...>>(std::move(func.set));
    }

    template<typename... Functions>
    std::unique_ptr<function_detail::base_function> make_function(const std::string&, constructor_wrapper<Functions...> func) {
        return std::make_unique<function_detail::usertype_constructor_function<T, Functions...>>(std::move(func.set));
    }

    template<typename RSig, typename WSig>
    std::unique_ptr<function_detail::base_function> make_function(const std::string&, member_property<RSig, WSig> func) {
        return std::make_unique<function_detail::usertype_variable_function<T, member_property<RSig, WSig>>>(std::move(func));
    }

    template<typename Fx>
    std::unique_ptr<function_detail::base_function> make_free_function(std::true_type, const std::string&, Fx&& func) {
        typedef std::decay_t<meta::Unqualified<Fx>> function_type;
        return std::make_unique<function_detail::usertype_function<T, function_type>>(func);
    }

    template<typename Fx>
    std::unique_ptr<function_detail::base_function> make_free_function(std::false_type, const std::string&, Fx&& func) {
        typedef std::decay_t<meta::Unqualified<Fx>> function_type;
        return std::make_unique<function_detail::free_function<function_type>>(func);
    }

    template<typename... Args, typename Ret>
    std::unique_ptr<function_detail::base_function> make_function(const std::string& name, Ret(*func)(Args...)) {
        typedef meta::bind_traits<Ret(Args...)> btraits;
        typedef typename btraits::template arg_at<0> Argu;
        typedef meta::Unqualified<std::remove_pointer_t<Argu>> Arg0;
        return make_free_function(std::is_base_of<Arg0, T>(), name, func);
    }

    template<typename Base, typename Ret>
    std::unique_ptr<function_detail::base_function> make_variable_function(std::true_type, const std::string&, Ret Base::* func) {
        static_assert(std::is_base_of<Base, T>::value, "Any registered member variable must be part of the class");
        typedef std::decay_t<decltype(func)> function_type;
        return std::make_unique<function_detail::usertype_variable_function<T, function_type>>(func);
    }

    template<typename Base, typename Ret>
    std::unique_ptr<function_detail::base_function> make_variable_function(std::false_type, const std::string&, Ret Base::* func) {
        static_assert(std::is_base_of<Base, T>::value, "Any registered member function must be part of the class");
        typedef std::decay_t<decltype(func)> function_type;
        return std::make_unique<function_detail::usertype_function<T, function_type>>(func);
    }

    template<typename Base, typename Ret>
    std::unique_ptr<function_detail::base_function> make_function(const std::string& name, Ret Base::* func) {
        typedef std::decay_t<decltype(func)> function_type;
        return make_variable_function(std::is_member_object_pointer<function_type>(), name, func);
    }

    template<typename Fx>
    std::unique_ptr<function_detail::base_function> make_functor_function(std::true_type, const std::string&, Fx&& func) {
        typedef std::decay_t<meta::Unqualified<Fx>> function_type;
        return std::make_unique<function_detail::usertype_function<T, function_type>>(func);
    }

    template<typename Fx>
    std::unique_ptr<function_detail::base_function> make_functor_function(std::false_type, const std::string&, Fx&& func) {
        typedef std::decay_t<meta::Unqualified<Fx>> function_type;
        return std::make_unique<function_detail::functor_function<function_type>>(func);
    }

    template<typename Fx>
    std::unique_ptr<function_detail::base_function> make_function(const std::string& name, Fx&& func) {
        typedef meta::Unqualified<Fx> Fxu;
        typedef meta::bind_traits<Fxu> btraits;
        typedef typename btraits::template arg_at<0> Argu;
        typedef meta::Unqualified<std::remove_pointer_t<Argu>> Arg0;
        return make_functor_function(std::is_base_of<Arg0, T>(), name, std::forward<Fx>(func));
    }

    template<std::size_t N>
    void build_function(std::string, no_construction) {}

    template<std::size_t N, typename... Args>
    void build_function(std::string funcname, constructors<Args...>) {
        functionnames.push_back(std::move(funcname));
        std::string& name = functionnames.back();
        // Insert bubble to keep with compile-time argument count (simpler and cheaper to do)
        functions.push_back(nullptr);
        constructfuncname = name.c_str();
        constructfunc = function_detail::construct<T, Args...>;
        metafunctiontable.push_back({ name.c_str(), constructfunc });
    }

    template<std::size_t N, typename... Args>
    void build_function(std::string funcname, lua_CFunction direct) {
        functionnames.push_back(std::move(funcname));
        std::string& name = functionnames.back();
        
        // Insert bubble to keep with compile-time argument count (simpler and cheaper to do)
        functions.push_back(nullptr);
        
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
                constructfuncname = name.c_str();
                constructfunc = function_detail::usertype_call<N>;
                break;
             default:
                  break;
            }
            return;
        }
        functiontable.push_back({ name.c_str(), direct });
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
        typedef meta::Or<std::is_member_object_pointer<meta::Unqualified<Fx>>, meta::is_specialization_of<member_property, meta::Unqualified<Fx>>> is_variable;
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
                constructfuncname = name.c_str();
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

    template<std::size_t N, typename Fx, typename... Args>
    void build_function_tables(call_construction, Fx&& func, Args&&... args) {
        build_function_tables<N>("__call", std::forward<Fx>(func), std::forward<Args>(args)...);
    }

    template<std::size_t N, typename... Bases, typename... Args>
    void build_function_tables(base_classes_tag, bases<Bases...>, Args&&... args) {
        build_function_tables<N>(std::forward<Args>(args)...);
        if (sizeof...(Bases) < 1)
            return;
#ifndef SOL_NO_EXCEPTIONS
        static_assert(sizeof(void*) <= sizeof(detail::throw_cast), "The size of this data pointer is too small to fit the inheritance checking function: file a bug report.");
        baseclasscast = (void*)&detail::throw_as<T>;
#elif !defined(SOL_NO_RTTI)
        static_assert(sizeof(void*) <= sizeof(detail::inheritance_check_function), "The size of this data pointer is too small to fit the inheritance checking function: file a bug report.");
        static_assert(sizeof(void*) <= sizeof(detail::inheritance_cast_function), "The size of this data pointer is too small to fit the inheritance checking function: file a bug report.");
        baseclasscheck = (void*)&detail::inheritance<T, Bases...>::check;
        baseclasscast = (void*)&detail::inheritance<T, Bases...>::cast;
#else
        static_assert(sizeof(void*) <= sizeof(detail::inheritance_check_function), "The size of this data pointer is too small to fit the inheritance checking function: file a bug report.");
        static_assert(sizeof(void*) <= sizeof(detail::inheritance_cast_function), "The size of this data pointer is too small to fit the inheritance checking function: file a bug report.");
        baseclasscheck = (void*)&detail::inheritance<T, Bases...>::check;
        baseclasscast = (void*)&detail::inheritance<T, Bases...>::cast;
#endif // No Runtime Type Information vs. Throw-Style Inheritance
    }

    template<std::size_t N>
    void build_function_tables() {
        int variableend = 0;
        if (!indexwrapper.empty()) {
            if (indexfunc == nullptr) {
                indexfunc = &function_detail::failure_on_error();
            }
            functions.push_back(std::make_unique<function_detail::usertype_indexing_function>("__index", indexfunc, std::move(indexwrapper)));
            metafunctiontable.push_back({ "__index", function_detail::usertype_call<N> });
            ++variableend;
        }
        if (!newindexwrapper.empty()) {
            if (newindexfunc == nullptr) {
                newindexfunc = &function_detail::failure_on_error();
            }
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
    usertype(usertype_detail::verified_tag, Args&&... args) : indexfunc(nullptr), newindexfunc(nullptr), constructfuncname(""), constructfunc(nullptr),
    destructfuncname(""), destructfunc(nullptr), functiongcfunc(nullptr), needsindexfunction(false), baseclasscheck(nullptr), baseclasscast(nullptr) {
        static_assert((sizeof...(Args) % 2) == 0, "Incorrect argument count to usertype creation: not in pairs. Might be missing name, function/property/variable, comma");
        functionnames.reserve(sizeof...(args)+3);
        functiontable.reserve(sizeof...(args)+3);
        metafunctiontable.reserve(sizeof...(args)+3);

        build_function_tables<0>(std::forward<Args>(args)...);
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
        usertype_detail::push_metatable<T*, usertype_detail::stage::refmeta>(L, needsindexfunction, functions, functiontable, metafunctiontable, baseclasscheck, baseclasscast);
        lua_pop(L, 1);
        usertype_detail::push_metatable<detail::unique_usertype<T>, usertype_detail::stage::uniquemeta>(L, needsindexfunction, functions, functiontable, metafunctiontable, baseclasscheck, baseclasscast);
        lua_pop(L, 1);
        // but leave the regular T table on last
        // so it can be linked to a key for usage with `.new(...)` or `:new(...)`
        usertype_detail::push_metatable<T, usertype_detail::stage::normalmeta>(L, needsindexfunction, functions, functiontable, metafunctiontable, baseclasscheck, baseclasscast);
        // be sure to link the construction function to allow for people to do the whole lua_bind thing
        if (constructfunc != nullptr && constructfuncname != nullptr && std::find(meta_function_names.cbegin(), meta_function_names.cend(), constructfuncname) != meta_function_names.cend()) {
            lua_createtable(L, 0, 0);
		  lua_pushcclosure(L, constructfunc, 0);
            lua_setfield(L, -2, constructfuncname);
            lua_setmetatable(L, -2);
        }
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
