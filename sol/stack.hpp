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

#ifndef SOL_STACK_HPP
#define SOL_STACK_HPP

#include "error.hpp"
#include "reference.hpp"
#include "tuple.hpp"
#include "traits.hpp"
#include "usertype_traits.hpp"
#include "overload.hpp"
#include <utility>
#include <array>
#include <cstring>
#include <functional>

namespace sol {
namespace stack {
template<typename T, bool global = false, typename = void>
struct field_getter;
template<typename T, bool global = false, typename = void>
struct field_setter;
template<typename T, typename = void>
struct getter;
template<typename T, typename = void>
struct popper;
template<typename T, typename = void>
struct pusher;
template<typename T, type = lua_type_of<T>::value, typename = void>
struct checker;

template<typename T, typename... Args>
inline int push(lua_State* L, T&& t, Args&&... args) {
    return pusher<meta::Unqualified<T>>{}.push(L, std::forward<T>(t), std::forward<Args>(args)...);
}

// overload allows to use a pusher of a specific type, but pass in any kind of args
template<typename T, typename Arg, typename... Args>
inline int push(lua_State* L, Arg&& arg, Args&&... args) {
    return pusher<meta::Unqualified<T>>{}.push(L, std::forward<Arg>(arg), std::forward<Args>(args)...);
}

inline int push_args(lua_State*) {
    // do nothing
    return 0;
}

template<typename T, typename... Args>
inline int push_args(lua_State* L, T&& t, Args&&... args) {
    int pushcount = push(L, std::forward<T>(t));
    void(sol::detail::swallow{(pushcount += sol::stack::push(L, std::forward<Args>(args)), 0)... });
    return pushcount;
}

template<typename T>
inline decltype(auto) get(lua_State* L, int index = -1) {
    return getter<meta::Unqualified<T>>{}.get(L, index);
}

template<typename T>
inline decltype(auto) pop(lua_State* L) {
    return popper<meta::Unqualified<T>>{}.pop(L);
}

template <typename T, typename Handler>
bool check(lua_State* L, int index, Handler&& handler) {
    typedef meta::Unqualified<T> Tu;
    checker<Tu> c;
    // VC++ has a bad warning here: shut it up
    (void)c;
    return c.check(L, index, std::forward<Handler>(handler));
}

template <typename T>
bool check(lua_State* L, int index) {
    auto handler = type_panic;
    return check<T>(L, index, handler);
}

template <bool global = false, typename Key>
void get_field(lua_State* L, Key&& key) {
    field_getter<meta::Unqualified<Key>, global>{}.get(L, std::forward<Key>(key));
}

template <bool global = false, typename Key>
void get_field(lua_State* L, Key&& key, int tableindex) {
    field_getter<meta::Unqualified<Key>, global>{}.get(L, std::forward<Key>(key), tableindex);
}

template <bool global = false, typename Key, typename Value>
void set_field(lua_State* L, Key&& key, Value&& value) {
    field_setter<meta::Unqualified<Key>, global>{}.set(L, std::forward<Key>(key), std::forward<Value>(value));
}

template <bool global = false, typename Key, typename Value>
void set_field(lua_State* L, Key&& key, Value&& value, int tableindex) {
    field_setter<meta::Unqualified<Key>, global>{}.set(L, std::forward<Key>(key), std::forward<Value>(value), tableindex);
}

namespace stack_detail {
const bool default_check_arguments = 
#ifdef SOL_CHECK_ARGUMENTS
true;
#else
false;
#endif

template<bool releasemem = false, typename TCont>
static int push_upvalues(lua_State* L, TCont&& cont) {
    int n = 0;
    for(auto& c : cont) {
        if(releasemem) {
            stack::push<upvalue>(L, c.release());
        }
        else {
            stack::push<upvalue>(L, c.get());
        }
        ++n;
    }
    return n;
}

template <typename T>
struct userdata_pusher {
    template <typename Key, typename... Args>
    static void push (lua_State* L, Key&& metatablekey, Args&&... args) {
        // Basically, we store all user0data like this:
        // If it's a movable/copyable value (no std::ref(x)), then we store the pointer to the new
        // data in the first sizeof(T*) bytes, and then however many bytes it takes to
        // do the actual object. Things that are std::ref or plain T* are stored as 
        // just the sizeof(T*), and nothing else.
        T** pdatum = static_cast<T**>(lua_newuserdata(L, sizeof(T*) + sizeof(T)));
        T** referencepointer = pdatum;
        T*& referencereference = *pdatum;
        T* allocationtarget = reinterpret_cast<T*>(pdatum + 1);
        referencereference = allocationtarget;
        std::allocator<T> alloc{};
        alloc.construct(allocationtarget, std::forward<Args>(args)...);
        luaL_getmetatable(L, &metatablekey[0]);
        lua_setmetatable(L, -2);
    }
};

template <typename T>
struct userdata_pusher<T*> {
    template <typename Key, typename... Args>
    static void push (lua_State* L, Key&& metatablekey, Args&&... args) {
        T** pdatum = static_cast<T**>(lua_newuserdata(L, sizeof(T*)));
        std::allocator<T*> alloc{};
        alloc.construct(pdatum, std::forward<Args>(args)...);
        luaL_getmetatable(L, &metatablekey[0]);
        lua_setmetatable(L, -2);
    }
};

template<typename T, std::size_t... I>
inline int push_tuple(std::index_sequence<I...>, lua_State* L, T&& tuplen) {
    int pushcount = 0;
    void(detail::swallow{(pushcount += sol::stack::push(L, std::get<I>(tuplen)), '\0')... });
    return pushcount;
}

template <typename T, typename Key, typename... Args>
inline int push_confirmed_userdata(lua_State* L, Key&& metatablekey, Args&&... args) {
    userdata_pusher<T>{}.push(L, std::forward<Key>(metatablekey), std::forward<Args>(args)...);
    return 1;
}

template <typename T, typename Key>
inline int push_userdata_pointer(lua_State* L, Key&& metatablekey) {
    return push_confirmed_userdata<T>(L, std::forward<Key>(metatablekey));
}

template <typename T, typename Key, typename Arg, meta::EnableIf<std::is_same<T, meta::Unqualified<Arg>>> = 0>
inline int push_userdata_pointer(lua_State* L, Key&& metatablekey, Arg&& arg) {
    if (arg == nullptr)
        return push(L, nil);
    return push_confirmed_userdata<T>(L, std::forward<Key>(metatablekey), std::forward<Arg>(arg));
}

template <typename T, typename Key, typename Arg, meta::DisableIf<std::is_same<T, meta::Unqualified<Arg>>> = 0>
inline int push_userdata_pointer(lua_State* L, Key&& metatablekey, Arg&& arg) {
    return push_confirmed_userdata<T>(L, std::forward<Key>(metatablekey), std::forward<Arg>(arg));
}

template <typename T, typename Key, typename Arg0, typename Arg1, typename... Args>
inline int push_userdata_pointer(lua_State* L, Key&& metatablekey, Arg0&& arg0, Arg1&& arg1, Args&&... args) {
    return push_confirmed_userdata<T>(L, std::forward<Key>(metatablekey), std::forward<Arg0>(arg0), std::forward<Arg1>(arg1), std::forward<Args>(args)...);
}

template<typename T, typename Key, typename... Args, meta::DisableIf<std::is_pointer<T>> = 0>
inline int push_userdata(lua_State* L, Key&& metatablekey, Args&&... args) {
    return push_confirmed_userdata<T>(L, std::forward<Key>(metatablekey), std::forward<Args>(args)...);
}

template<typename T, typename Key, typename... Args, meta::EnableIf<std::is_pointer<T>> = 0>
inline int push_userdata(lua_State* L, Key&& metatablekey, Args&&... args) {
    return push_userdata_pointer<T>(L, std::forward<Key>(metatablekey), std::forward<Args>(args)...);
}
} // stack_detail

template<typename T, typename>
struct getter {
    static T& get(lua_State* L, int index = -1) {
        return getter<T&>{}.get(L, index);
    }
};

template<typename T>
struct getter<T, std::enable_if_t<std::is_floating_point<T>::value>> {
    static T get(lua_State* L, int index = -1) {
        return static_cast<T>(lua_tonumber(L, index));
    }
};

template<typename T>
struct getter<T, std::enable_if_t<meta::And<std::is_integral<T>, std::is_signed<T>>::value>> {
    static T get(lua_State* L, int index = -1) {
        return static_cast<T>(lua_tointeger(L, index));
    }
};

template<typename T>
struct getter<T, std::enable_if_t<meta::And<std::is_integral<T>, std::is_unsigned<T>>::value>> {
    static T get(lua_State* L, int index = -1) {
        return static_cast<T>(lua_tointeger(L, index));
    }
};

template<typename T>
struct getter<T, std::enable_if_t<std::is_base_of<reference, T>::value>> {
    static T get(lua_State* L, int index = -1) {
        return T(L, index);
    }
};

template<typename T>
struct getter<T*> {
    static T* get(lua_State* L, int index = -1) {
        type t = type_of(L, index);
        if (t == type::nil)
            return nullptr;
        void* udata = lua_touserdata(L, index);
        T** obj = static_cast<T**>(udata);
        return *obj;
    }
};

template<typename T>
struct getter<T&> {
    static T& get(lua_State* L, int index = -1) {
        void* udata = lua_touserdata(L, index);
        T** obj = static_cast<T**>(udata);
        return **obj;
    }
};

template<typename T>
struct getter<std::reference_wrapper<T>> {
    static T& get(lua_State* L, int index = -1) {
        return getter<T&>{}.get(L, index);
    }
};

template<>
struct getter<type> {
    static type get(lua_State *L, int index){
        return static_cast<type>(lua_type(L, index));
    }
};

template<>
struct getter<bool> {
    static bool get(lua_State* L, int index) {
        return lua_toboolean(L, index) != 0;
    }
};

template<>
struct getter<std::string> {
    static std::string get(lua_State* L, int index = -1) {
        std::size_t len;
        auto str = lua_tolstring(L, index, &len);
        return { str, len };
    }
};

template<>
struct getter<const char*> {
    static const char* get(lua_State* L, int index = -1) {
        return lua_tostring(L, index);
    }
};

template<>
struct getter<nil_t> {
    static nil_t get(lua_State* L, int index = -1) {
        if(lua_isnil(L, index) == 0) {
            throw error("not nil");
        }
        return nil_t{ };
    }
};

template<>
struct getter<lua_CFunction> {
    static lua_CFunction get(lua_State* L,  int index = -1) {
        return lua_tocfunction(L, index);
    }
};

template<>
struct getter<c_closure> {
    static c_closure get(lua_State* L,  int index = -1) {
        return c_closure(lua_tocfunction(L, index), -1);
    }
};

template<>
struct getter<userdata> {
    static userdata get(lua_State* L, int index = -1) {
        return{ lua_touserdata(L, index) };
    }
};

template<>
struct getter<light_userdata> {
    static light_userdata get(lua_State* L, int index = -1) {
        return{ lua_touserdata(L, index) };
    }
};

template<>
struct getter<upvalue> {
    static upvalue get(lua_State* L, int index = 1) {
        return{ lua_touserdata(L, lua_upvalueindex(index)) };
    }
};

template<>
struct getter<void*> {
    static void* get(lua_State* L, int index = -1) {
        return lua_touserdata(L, index);
    }
};

template<typename... Args>
struct getter<std::tuple<Args...>> {
    template <std::size_t... I>
    static decltype(auto) apply(std::index_sequence<I...>, lua_State* L, int index = -1) {
        return std::tuple<decltype(stack::get<Args>(L, index + I))...>(stack::get<Args>(L, index + I)...);
    }

    static decltype(auto) get(lua_State* L, int index = -1) {
        return apply(std::index_sequence_for<Args...>(), L, index);
    }
};

template <typename T, type expected, typename>
struct checker {
    template <typename Handler>
    static bool check (lua_State* L, int index, const Handler& handler) {
        const type indextype = type_of(L, index);
        bool success = expected == indextype;
        if (!success) {
            // expected type, actual type
            handler(L, index, expected, indextype);
        }
        return success;
    }
};

template <typename T, typename C>
struct checker<T*, type::userdata, C> {
    template <typename Handler>
    static bool check (lua_State* L, int index, const Handler& handler) {
        const type indextype = type_of(L, index);
        // Allow nil to be transformed to nullptr
        if (indextype == type::nil) {
            return true;
        }
        return checker<T, type::userdata, C>{}.check(L, indextype, index, handler);
    }
};

template <typename T,typename C>
struct checker<T, type::userdata, C> {
    template <typename Handler>
    static bool check (lua_State* L, type indextype, int index, const Handler& handler) {
        if (indextype != type::userdata) {
            handler(L, index, type::userdata, indextype);
            return false;
        }
        if (lua_getmetatable(L, index) == 0) {
             handler(L, index, type::userdata, indextype);
             return false;
        }
        luaL_getmetatable(L, &usertype_traits<T>::metatable[0]);
        const type expectedmetatabletype = get<type>(L);
        if (expectedmetatabletype == type::nil) {
             lua_pop(L, 2);
             handler(L, index, type::userdata, indextype);
             return false;
        }
        bool success = lua_rawequal(L, -1, -2) == 1;
        lua_pop(L, 2);
        return success;
    }

    template <typename Handler>
    static bool check (lua_State* L, int index, const Handler& handler) {
        const type indextype = type_of(L, index);
        return check(L, indextype, index, handler);
    }
};

template <typename T, typename>
struct popper {
    inline decltype(auto) pop(lua_State* L) {
        decltype(auto) r = get<T>(L);
        lua_pop(L, 1);
        return r;
    }
};

template <typename... Args>
struct popper<std::tuple<Args...>> {
    inline decltype(auto) pop(lua_State* L) {
        decltype(auto) r = get<std::tuple<Args...>>(L, lua_gettop(L) - sizeof...(Args) + 1);
        lua_pop(L, static_cast<int>(sizeof...(Args)));
        return r;
    }
};

template<typename T, typename>
struct pusher {
    static int push(lua_State* L, T& t) {
        return stack_detail::push_userdata<T>(L, usertype_traits<T>::metatable, t);
    }

    static int push(lua_State* L, T&& t) {
        return stack_detail::push_userdata<T>(L, usertype_traits<T>::metatable, std::move(t));
    }
};

template<typename T>
struct pusher<T, std::enable_if_t<std::is_floating_point<T>::value>> {
    static int push(lua_State* L, const T& value) {
        lua_pushnumber(L, value);
        return 1;
    }
};

template<typename T>
struct pusher<T, std::enable_if_t<meta::And<std::is_integral<T>, std::is_signed<T>>::value>> {
    static int push(lua_State* L, const T& value) {
        lua_pushinteger(L, static_cast<lua_Integer>(value));
        return 1;
    }
};

template<typename T>
struct pusher<T, std::enable_if_t<meta::And<std::is_integral<T>, std::is_unsigned<T>>::value>> {
    static int push(lua_State* L, const T& value) {
         typedef std::make_signed_t<T> signed_int;
         return stack::push(L, static_cast<signed_int>(value));
    }
};

template<typename T>
struct pusher<T, std::enable_if_t<meta::And<meta::has_begin_end<T>, meta::Not<meta::has_key_value_pair<T>>>::value>> {
    static int push(lua_State* L, const T& cont) {
        lua_createtable(L, static_cast<int>(cont.size()), 0);
        unsigned index = 1;
        for(auto&& i : cont) {
            // push the index
            pusher<unsigned>{}.push(L, index++);
            // push the value
            pusher<meta::Unqualified<decltype(i)>>{}.push(L, i);
            // set the table
            lua_settable(L, -3);
        }
        return 1;
    }
};

template<typename T>
struct pusher<T, std::enable_if_t<meta::And<meta::has_begin_end<T>, meta::has_key_value_pair<T>>::value>> {
    static int push(lua_State* L, const T& cont) {
        lua_createtable(L, static_cast<int>(cont.size()), 0);
        for(auto&& pair : cont) {
            pusher<meta::Unqualified<decltype(pair.first)>>{}.push(L, pair.first);
            pusher<meta::Unqualified<decltype(pair.second)>>{}.push(L, pair.second);
            lua_settable(L, -3);
        }
        return 1;
    }
};

template<typename T>
struct pusher<T, std::enable_if_t<std::is_base_of<reference, T>::value>> {
    static int push(lua_State*, T& ref) {
        return ref.push();
    }
};

template<typename T>
struct pusher<T*> {
    static int push(lua_State* L, T* obj) {
        return stack_detail::push_userdata<T*>(L, usertype_traits<T*>::metatable, obj);
    }
};

template<typename T>
struct pusher<std::reference_wrapper<T>> {
    static int push(lua_State* L, const std::reference_wrapper<T>& t) {
        return stack::push(L, std::addressof(t.get()));
    }
};

template<>
struct pusher<bool> {
    static int push(lua_State* L, bool b) {
        lua_pushboolean(L, b);
        return 1;
    }
};

template<>
struct pusher<nil_t> {
    static int push(lua_State* L, nil_t) {
        lua_pushnil(L);
        return 1;
    }
};

template<>
struct pusher<std::remove_pointer_t<lua_CFunction>> {
    static int push(lua_State* L, lua_CFunction func, int n = 0) {
        lua_pushcclosure(L, func, n);
        return 1;
    }
};

template<>
struct pusher<lua_CFunction> {
    static int push(lua_State* L, lua_CFunction func, int n = 0) {
        lua_pushcclosure(L, func, n);
        return 1;
    }
};

template<>
struct pusher<c_closure> {
    static int push(lua_State* L, c_closure closure) {
        lua_pushcclosure(L, closure.c_function, closure.upvalues);
        return 1;
    }
};

template<>
struct pusher<void*> {
    static int push(lua_State* L, void* userdata) {
        lua_pushlightuserdata(L, userdata);
        return 1;
    }
};

template<>
struct pusher<upvalue> {
    static int push(lua_State* L, upvalue upv) {
        lua_pushlightuserdata(L, upv);
        return 1;
    }
};

template<>
struct pusher<light_userdata> {
    static int push(lua_State* L, light_userdata userdata) {
        lua_pushlightuserdata(L, userdata);
        return 1;
    }
};

template<>
struct pusher<userdata> {
    static int push(lua_State* L, userdata data) {
        void** ud = static_cast<void**>(lua_newuserdata(L, sizeof(void*)));
        *ud = data.value;
        return 1;
    }
};

template<>
struct pusher<const char*> {
    static int push(lua_State* L, const char* str) {
        lua_pushlstring(L, str, std::char_traits<char>::length(str));
        return 1;
    }
};

template<size_t N>
struct pusher<char[N]> {
    static int push(lua_State* L, const char (&str)[N]) {
        lua_pushlstring(L, str, N - 1);
        return 1;
    }
};

template<>
struct pusher<std::string> {
    static int push(lua_State* L, const std::string& str) {
        lua_pushlstring(L, str.c_str(), str.size());
        return 1;
    }
};

template<typename... Args>
struct pusher<std::tuple<Args...>> {
    template <typename Tuple>
    static int push(lua_State* L, Tuple&& tuplen) {
        return stack_detail::push_tuple(std::index_sequence_for<Args...>(), L, std::forward<Tuple>(tuplen));
    }
};

template <typename T, bool, typename>
struct field_getter {
    template <typename Key>
    void get(lua_State* L, Key&& key, int tableindex = -2) {
        push( L, std::forward<Key>( key ) );
        lua_gettable( L, tableindex );
    }
};

template <typename... Args, bool b, typename C>
struct field_getter<std::tuple<Args...>, b, C> {
    template <std::size_t I0, std::size_t... I, typename Key>
    void apply(std::index_sequence<I0, I...>, lua_State* L, Key&& key, int tableindex) {
        get_field<b>(L, std::get<I0>(key), tableindex);
        void(detail::swallow{ (get_field(L, std::get<I>(key)), 0)... });
        reference saved(L, -1);
        lua_pop(L, static_cast<int>(sizeof...(I) + 1));
        saved.push();
    }

    template <typename Key>
    void get(lua_State* L, Key&& key, int tableindex = -2) {
        apply(std::index_sequence_for<Args...>(), L, std::forward<Key>(key), tableindex);
    }
};

template <typename T>
struct field_getter<T, true, std::enable_if_t<meta::is_c_str<T>::value>> {
    template <typename Key>
    void get(lua_State* L, Key&& key, int = -1) {
        lua_getglobal(L, &key[0]);
    }
};

template <typename T>
struct field_getter<T, false, std::enable_if_t<meta::is_c_str<T>::value>> {
    template <typename Key>
    void get(lua_State* L, Key&& key, int tableindex = -1) {
        lua_getfield(L, tableindex, &key[0]);
    }
};

#if SOL_LUA_VERSION >= 503
template <typename T>
struct field_getter<T, false, std::enable_if_t<std::is_integral<T>::value>> {
    template <typename Key>
    void get(lua_State* L, Key&& key, int tableindex = -1) {
        lua_geti(L, tableindex, static_cast<lua_Integer>(key));
    }
};
#endif // Lua 5.3.x

template <typename T, bool, typename>
struct field_setter {
    template <typename Key, typename Value>
    void set(lua_State* L, Key&& key, Value&& value, int tableindex = -3) {
        push(L, std::forward<Key>(key));
        push(L, std::forward<Value>(value));
        lua_settable(L, tableindex);
    }
};

template <typename T>
struct field_setter<T, true, std::enable_if_t<meta::is_c_str<T>::value>> {
    template <typename Key, typename Value>
    void set(lua_State* L, Key&& key, Value&& value, int = -2) {
        push(L, std::forward<Value>(value));
        lua_setglobal(L, &key[0]);
    }
};

template <typename T>
struct field_setter<T, false, std::enable_if_t<meta::is_c_str<T>::value>> {
    template <typename Key, typename Value>
    void set(lua_State* L, Key&& key, Value&& value, int tableindex = -2) {
        push(L, std::forward<Value>(value));
        lua_setfield(L, tableindex, &key[0]);
    }
};

#if SOL_LUA_VERSION >= 503
template <typename T>
struct field_setter<T, false, std::enable_if_t<std::is_integral<T>::value>> {
    template <typename Key, typename Value>
    void set(lua_State* L, Key&& key, Value&& value, int tableindex = -2) {
        push(L, std::forward<Value>(value));
        lua_seti(L, tableindex, static_cast<lua_Integer>(key));
    }
};
#endif // Lua 5.3.x

namespace stack_detail {
template<typename T>
inline int push_as_upvalues(lua_State* L, T& item) {
    typedef std::decay_t<T> TValue;
    const static std::size_t itemsize = sizeof(TValue);
    const static std::size_t voidsize = sizeof(void*);
    const static std::size_t voidsizem1 = voidsize - 1;
    const static std::size_t data_t_count = (sizeof(TValue) + voidsizem1) / voidsize;
    typedef std::array<void*, data_t_count> data_t;

    data_t data{{}};
    std::memcpy(&data[0], std::addressof(item), itemsize);
    int pushcount = 0;
    for(auto&& v : data) {
        pushcount += push(L, upvalue(v));
    }
    return pushcount;
}

template<typename T>
inline std::pair<T, int> get_as_upvalues(lua_State* L, int index = 1) {
    const static std::size_t data_t_count = (sizeof(T)+(sizeof(void*)-1)) / sizeof(void*);
    typedef std::array<void*, data_t_count> data_t;
    data_t voiddata{ {} };
    for(std::size_t i = 0, d = 0; d < sizeof(T); ++i, d += sizeof(void*)) {
        voiddata[i] = get<upvalue>(L, index++);
    }
    return std::pair<T, int>(*reinterpret_cast<T*>(static_cast<void*>(voiddata.data())), index);
}

template <bool b>
struct check_arguments {
    template <std::size_t I0, std::size_t... I, typename Arg0, typename... Args>
    static bool check(types<Arg0, Args...>, std::index_sequence<I0, I...>, lua_State* L, int firstargument) {
        if (!stack::check<Arg0>(L, firstargument + I0))
            return false;
        return check(types<Args...>(), std::index_sequence<I...>(), L, firstargument);
    }

    static bool check(types<>, std::index_sequence<>, lua_State*, int) {
        return true;
    }
};

template <>
struct check_arguments<false> {
    template <std::size_t... I, typename... Args>
    static bool check(types<Args...>, std::index_sequence<I...>, lua_State*, int) {
        return true;
    }
};

template <bool checkargs = default_check_arguments, std::size_t... I, typename R, typename... Args, typename Fx, typename... FxArgs, typename = std::enable_if_t<!std::is_void<R>::value>>
inline R call(types<R>, types<Args...> ta, std::index_sequence<I...> tai, lua_State* L, int start, Fx&& fx, FxArgs&&... args) {
    check_arguments<checkargs>{}.check(ta, tai, L, start);
    return fx(std::forward<FxArgs>(args)..., stack::get<Args>(L, start + I)...);
}

template <bool checkargs = default_check_arguments, std::size_t... I, typename... Args, typename Fx, typename... FxArgs>
inline void call(types<void>, types<Args...> ta, std::index_sequence<I...> tai, lua_State* L, int start, Fx&& fx, FxArgs&&... args) {
    check_arguments<checkargs>{}.check(ta, tai, L, start);
    fx(std::forward<FxArgs>(args)..., stack::get<Args>(L, start + I)...);
}
} // stack_detail

inline void remove( lua_State* L, int index, int count ) {
    if ( count < 1 )
        return;
    int top = lua_gettop( L );
    if ( index == -1 || top == index ) {
        // Slice them right off the top
        lua_pop( L, static_cast<int>(count) );
        return;
    }

    // Remove each item one at a time using stack operations
    // Probably slower, maybe, haven't benchmarked,
    // but necessary
    if ( index < 0 ) {
        index = lua_gettop( L ) + (index + 1);
    }
    int last = index + count;
    for ( int i = index; i < last; ++i ) {
        lua_remove( L, i );
    }
}

template <bool check_args = stack_detail::default_check_arguments, typename R, typename... Args, typename Fx, typename... FxArgs, typename = std::enable_if_t<!std::is_void<R>::value>>
inline R call(types<R> tr, types<Args...> ta, lua_State* L, int start, Fx&& fx, FxArgs&&... args) {
    typedef typename types<Args...>::indices args_indices;
    return stack_detail::call<check_args>(tr, ta, args_indices(), L, start, std::forward<Fx>(fx), std::forward<FxArgs>(args)...);
}

template <bool check_args = stack_detail::default_check_arguments, typename R, typename... Args, typename Fx, typename... FxArgs, typename = std::enable_if_t<!std::is_void<R>::value>>
inline R call(types<R> tr, types<Args...> ta, lua_State* L, Fx&& fx, FxArgs&&... args) {
    return call<check_args>(tr, ta, L, 1, std::forward<Fx>(fx), std::forward<FxArgs>(args)...);
}

template <bool check_args = stack_detail::default_check_arguments, typename... Args, typename Fx, typename... FxArgs>
inline void call(types<void> tr, types<Args...> ta, lua_State* L, int start, Fx&& fx, FxArgs&&... args) {
    typedef typename types<Args...>::indices args_indices;
    stack_detail::call<check_args>(tr, ta, args_indices(), L, start, std::forward<Fx>(fx), std::forward<FxArgs>(args)...);
}

template <bool check_args = stack_detail::default_check_arguments, typename... Args, typename Fx, typename... FxArgs>
inline void call(types<void> tr, types<Args...> ta, lua_State* L, Fx&& fx, FxArgs&&... args) {
    call<check_args>(tr, ta, L, 1, std::forward<Fx>(fx), std::forward<FxArgs>(args)...);
}

template <bool check_args = stack_detail::default_check_arguments, typename R, typename... Args, typename Fx, typename... FxArgs, typename = std::enable_if_t<!std::is_void<R>::value>>
inline R call_from_top(types<R> tr, types<Args...> ta, lua_State* L, Fx&& fx, FxArgs&&... args) {
    return call<check_args>(tr, ta, L, static_cast<int>(lua_gettop(L) - sizeof...(Args)), std::forward<Fx>(fx), std::forward<FxArgs>(args)...);
}

template <bool check_args = stack_detail::default_check_arguments, typename... Args, typename Fx, typename... FxArgs>
inline void call_from_top(types<void> tr, types<Args...> ta, lua_State* L, Fx&& fx, FxArgs&&... args) {
    call<check_args>(tr, ta, L, static_cast<int>(lua_gettop(L) - sizeof...(Args)), std::forward<Fx>(fx), std::forward<FxArgs>(args)...);
}

template<bool check_args = stack_detail::default_check_arguments, typename... Args, typename Fx, typename... FxArgs>
inline int call_into_lua(types<void> tr, types<Args...> ta, Fx&& fx, lua_State* L, int start, FxArgs&&... fxargs) {
    call<check_args>(tr, ta, L, start, fx, std::forward<FxArgs>(fxargs)...);
    int nargs = static_cast<int>(sizeof...(Args));
    lua_pop(L, nargs);
    return 0;
}

template<bool check_args = stack_detail::default_check_arguments, typename Ret0, typename... Ret, typename... Args, typename Fx, typename... FxArgs, typename = std::enable_if_t<meta::Not<std::is_void<Ret0>>::value>>
inline int call_into_lua(types<Ret0, Ret...>, types<Args...> ta, Fx&& fx, lua_State* L, int start, FxArgs&&... fxargs) {
    decltype(auto) r = call<check_args>(types<meta::return_type_t<Ret0, Ret...>>(), ta, L, start, fx, std::forward<FxArgs>(fxargs)...);
    int nargs = static_cast<int>(sizeof...(Args));
    lua_pop(L, nargs);
    return push(L, std::forward<decltype(r)>(r));
}

inline call_syntax get_call_syntax(lua_State* L, const std::string& meta) {
    type metatype = stack::get<type>(L);
    luaL_getmetatable(L, meta.c_str());
    if (lua_compare(L, -1, -2, LUA_OPEQ) == 1) {
        lua_pop(L, 1);
        return call_syntax::colon;
    }
    lua_pop(L, 1);
    return call_syntax::dot;
}

inline void luajit_exception_handler(lua_State* L, int(*handler)(lua_State*, lua_CFunction) = detail::c_trampoline) {
#ifdef SOL_LUAJIT
    lua_pushlightuserdata(L, (void*)handler);
    luaJIT_setmode(L, -1, LUAJIT_MODE_WRAPCFUNC | LUAJIT_MODE_ON);
    lua_pop(L, 1);
#else
    (void)L;
    (void)handler;
#endif
}

inline void luajit_exception_off(lua_State* L) {
#ifdef SOL_LUAJIT
    luaJIT_setmode(L, -1, LUAJIT_MODE_WRAPCFUNC | LUAJIT_MODE_OFF);
#else
    (void)L;
#endif
}
} // stack
} // sol

#endif // SOL_STACK_HPP
