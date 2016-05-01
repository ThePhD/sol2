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

#ifndef SOL_STACK_CORE_HPP
#define SOL_STACK_CORE_HPP

#include "types.hpp"
#include "reference.hpp"
#include "stack_reference.hpp"
#include "userdata.hpp"
#include "tuple.hpp"
#include "traits.hpp"
#include "tie.hpp"

namespace sol {
namespace detail {
struct as_reference_tag{};

using special_destruct_func = void(*)(void*);

template <typename T, typename Real>
inline void special_destruct(void* memory) {
    T** pointerpointer = static_cast<T**>(memory);
    special_destruct_func* dx = static_cast<special_destruct_func*>(static_cast<void*>(pointerpointer + 1));
    Real* target = static_cast<Real*>(static_cast<void*>(dx + 1));
    target->~Real();
}

template <typename T>
inline int unique_destruct(lua_State* L) {
    void* memory = lua_touserdata(L, 1);
    T** pointerpointer = static_cast<T**>(memory);
    special_destruct_func& dx = *static_cast<special_destruct_func*>( static_cast<void*>( pointerpointer + 1 ) );
    (dx)(memory);
    return 0;
}
} // detail
namespace stack {

template<typename T, bool global = false, typename = void>
struct field_getter;
template <typename T, bool global = false, typename = void>
struct probe_field_getter;
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
template<typename T, typename = void>
struct check_getter;

struct probe {
    bool success;
    int levels;

    probe(bool s, int l) : success(s), levels(l) {}

    operator bool() const { return success; };
};

namespace stack_detail {
template <typename T>
struct strip {
    typedef T type;
};
template <typename T>
struct strip<std::reference_wrapper<T>> {
    typedef T& type;
};
template <typename T>
struct strip<non_null<T>> {
    typedef T type;
};
template <typename T>
using strip_t = typename strip<T>::type;
const bool default_check_arguments = 
#ifdef SOL_CHECK_ARGUMENTS
true;
#else
false;
#endif
template<typename T>
inline decltype(auto) unchecked_get(lua_State* L, int index = -1) {
    return getter<meta::Unqualified<T>>{}.get(L, index);
}
} // stack_detail

inline bool maybe_indexable(lua_State* L, int index = -1) {
    type t = type_of(L, index);
    return t == type::userdata || t == type::table;
}

template<typename T, typename... Args>
inline int push(lua_State* L, T&& t, Args&&... args) {
    return pusher<meta::Unqualified<T>>{}.push(L, std::forward<T>(t), std::forward<Args>(args)...);
}

// overload allows to use a pusher of a specific type, but pass in any kind of args
template<typename T, typename Arg, typename... Args>
inline int push(lua_State* L, Arg&& arg, Args&&... args) {
    return pusher<meta::Unqualified<T>>{}.push(L, std::forward<Arg>(arg), std::forward<Args>(args)...);
}

template<typename T, typename... Args>
inline int push_reference(lua_State* L, T&& t, Args&&... args) {
    typedef meta::And<
        std::is_lvalue_reference<T>, 
        meta::Not<std::is_const<T>>, 
        meta::Not<is_lua_primitive<T>>
    > use_reference_tag;
    return pusher<std::conditional_t<use_reference_tag::value, detail::as_reference_tag, meta::Unqualified<T>>>{}.push(L, std::forward<T>(t), std::forward<Args>(args)...);
}

inline int multi_push(lua_State*) {
    // do nothing
    return 0;
}

template<typename T, typename... Args>
inline int multi_push(lua_State* L, T&& t, Args&&... args) {
    int pushcount = push(L, std::forward<T>(t));
    void(sol::detail::swallow{(pushcount += sol::stack::push(L, std::forward<Args>(args)), 0)... });
    return pushcount;
}

inline int multi_push_reference(lua_State*) {
    // do nothing
    return 0;
}

template<typename T, typename... Args>
inline int multi_push_reference(lua_State* L, T&& t, Args&&... args) {
    int pushcount = push_reference(L, std::forward<T>(t));
    void(sol::detail::swallow{(pushcount += sol::stack::push_reference(L, std::forward<Args>(args)), 0)... });
    return pushcount;
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
bool check(lua_State* L, int index = -1) {
    auto handler = no_panic;
    return check<T>(L, index, handler);
}

template<typename T, typename Handler>
inline decltype(auto) check_get(lua_State* L, int index, Handler&& handler) {
    return check_getter<meta::Unqualified<T>>{}.get(L, index, std::forward<Handler>(handler));
}

template<typename T>
inline decltype(auto) check_get(lua_State* L, int index = -1) {
    auto handler = no_panic;
    return check_get<T>(L, index, handler);
}

namespace stack_detail {

template <typename T>
inline decltype(auto) tagged_get(types<T>, lua_State* L, int index = -1) {
#ifdef SOL_CHECK_ARGUMENTS
    auto op = check_get<T>(L, index, type_panic);
    typedef typename meta::Unqualified<decltype(op)>::value_type U;
    return static_cast<U>(*op);
#else
    return stack_detail::unchecked_get<T>(L, index);
#endif
}

template <typename T>
inline decltype(auto) tagged_get(types<optional<T>>, lua_State* L, int index = -1) {
    return stack_detail::unchecked_get<optional<T>>(L, index);
}

} // stack_detail

template<typename T>
inline decltype(auto) get(lua_State* L, int index = -1) {
    return stack_detail::tagged_get(types<T>(), L, index);
}

template<typename T>
inline decltype(auto) pop(lua_State* L) {
    return popper<meta::Unqualified<T>>{}.pop(L);
}

template <bool global = false, typename Key>
void get_field(lua_State* L, Key&& key) {
    field_getter<meta::Unqualified<Key>, global>{}.get(L, std::forward<Key>(key));
}

template <bool global = false, typename Key>
void get_field(lua_State* L, Key&& key, int tableindex) {
    field_getter<meta::Unqualified<Key>, global>{}.get(L, std::forward<Key>(key), tableindex);
}

template <bool global = false, typename Key>
probe probe_get_field(lua_State* L, Key&& key) {
    return probe_field_getter<meta::Unqualified<Key>, global>{}.get(L, std::forward<Key>(key));
}

template <bool global = false, typename Key>
probe probe_get_field(lua_State* L, Key&& key, int tableindex) {
    return probe_field_getter<meta::Unqualified<Key>, global>{}.get(L, std::forward<Key>(key), tableindex);
}

template <bool global = false, typename Key, typename Value>
void set_field(lua_State* L, Key&& key, Value&& value) {
    field_setter<meta::Unqualified<Key>, global>{}.set(L, std::forward<Key>(key), std::forward<Value>(value));
}

template <bool global = false, typename Key, typename Value>
void set_field(lua_State* L, Key&& key, Value&& value, int tableindex) {
    field_setter<meta::Unqualified<Key>, global>{}.set(L, std::forward<Key>(key), std::forward<Value>(value), tableindex);
}
} // stack
} // sol

#endif // SOL_STACK_CORE_HPP
