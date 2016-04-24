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

#ifndef SOL_STACK_GET_HPP
#define SOL_STACK_GET_HPP

#include "stack_core.hpp"
#include "usertype_traits.hpp"
#include "inheritance.hpp"
#include "overload.hpp"
#include "error.hpp"
#include <memory>
#include <functional>
#include <utility>

namespace sol {
namespace stack {

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
struct getter<T, std::enable_if_t<std::is_base_of<reference, T>::value || std::is_base_of<stack_reference, T>::value>> {
    static T get(lua_State* L, int index = -1) {
        return T(L, index);
    }
};

template<>
struct getter<userdata_value> {
    static userdata_value get(lua_State* L, int index = -1) {
        return userdata_value( lua_touserdata(L, index) );
    }
};

template<>
struct getter<light_userdata_value> {
    static light_userdata_value get(lua_State* L, int index = -1) {
        return light_userdata_value( lua_touserdata(L, index) );
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
    static nil_t get(lua_State*, int = -1) {
        return nil;
    }
};

template<>
struct pusher<std::nullptr_t> {
    static std::nullptr_t get(lua_State*, int = -1) {
        return nullptr;
    }
};

template<>
struct getter<nullopt_t> {
    static nullopt_t get(lua_State*, int = -1) {
        return nullopt;
    }
};

template<>
struct getter<this_state> {
    static this_state get(lua_State* L, int = -1) {
        return this_state{L};
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
    static c_closure get(lua_State* L, int index = -1) {
        return c_closure(lua_tocfunction(L, index), -1);
    }
};

template<>
struct getter<error> {
    static error get(lua_State* L,  int index = -1) {
        size_t sz = 0;
        const char* err = lua_tolstring(L, index, &sz);
        if (err == nullptr) {
            return error(detail::direct_error, "");
        }
        return error(detail::direct_error, std::string(err, sz));
    }
};

template<>
struct getter<void*> {
    static void* get(lua_State* L, int index = -1) {
        return lua_touserdata(L, index);
    }
};

template<typename T>
struct getter<T*> {
    static T* get_no_nil(lua_State* L, int index = -1) {
        void** pudata = static_cast<void**>(lua_touserdata(L, index));
        void* udata = *pudata;
        return get_no_nil_from(L, udata, index);
    }

    static T* get_no_nil_from(lua_State* L, void* udata, int index = -1) {
#ifndef SOL_NO_EXCEPTIONS
        if (luaL_getmetafield(L, index, &detail::base_class_check_key()[0]) != 0) {
            void* basecastdata = lua_touserdata(L, -1);
            detail::throw_cast basecast = (detail::throw_cast)basecastdata;
            // use the casting function to properly adjust the pointer for the desired T
            udata = detail::catch_cast<T>(udata, basecast);
            lua_pop(L, 1);
        }
#elif !defined(SOL_NO_RTTI)
        if (luaL_getmetafield(L, index, &detail::base_class_cast_key()[0]) != 0) {
            void* basecastdata = lua_touserdata(L, -1);
            detail::inheritance_cast_function ic = (detail::inheritance_cast_function)basecastdata;
            // use the casting function to properly adjust the pointer for the desired T
            udata = ic(udata, typeid(T));
            lua_pop(L, 1);
        }
#else
        // Lol, you motherfucker
        if (luaL_getmetafield(L, index, &detail::base_class_cast_key()[0]) != 0) {
            void* basecastdata = lua_touserdata(L, -1);
            detail::inheritance_cast_function ic = (detail::inheritance_cast_function)basecastdata;
            // use the casting function to properly adjust the pointer for the desired T
            udata = ic(udata, detail::id_for<T>::value);
            lua_pop(L, 1);
        }
#endif // No Runtime Type Information || Exceptions
        T* obj = static_cast<T*>(udata);
        return obj;
    }

    static T* get(lua_State* L, int index = -1) {
        type t = type_of(L, index);
        if (t == type::nil)
            return nullptr;
        return get_no_nil(L, index);
    }
};

template<typename T>
struct getter<non_null<T*>> {
    static T* get(lua_State* L, int index = -1) {
        return getter<T*>::get_no_nil(L, index);
    }
};

template<typename T>
struct getter<T&> {
    static T& get(lua_State* L, int index = -1) {
        return *getter<T*>::get_no_nil(L, index);
    }
};

template<typename T>
struct getter<T, std::enable_if_t<is_unique_usertype<T>::value>> {
    typedef typename unique_usertype_traits<T>::type P;
    typedef typename unique_usertype_traits<T>::actual_type Real;
    
    static Real& get(lua_State* L, int index = -1) {
        P** pref = static_cast<P**>(lua_touserdata(L, index));
        detail::special_destruct_func* fx = static_cast<detail::special_destruct_func*>(static_cast<void*>(pref + 1));
        Real* mem = static_cast<Real*>(static_cast<void*>(fx + 1));
        return *mem;
    }
};

template<typename T>
struct getter<std::reference_wrapper<T>> {
    static T& get(lua_State* L, int index = -1) {
        return getter<T&>{}.get(L, index);
    }
};

template<typename... Args>
struct getter<std::tuple<Args...>> {
    template <std::size_t... I>
    static decltype(auto) apply(std::index_sequence<I...>, lua_State* L, int index = -1) {
        index = index < 0 ? lua_absindex(L, index) - ( sizeof...(I) - 1 ) : index;
        return std::tuple<decltype(stack::get<Args>(L, index + I))...>(stack::get<Args>(L, index + I)...);
    }

    static decltype(auto) get(lua_State* L, int index = -1) {
        return apply(std::make_index_sequence<sizeof...(Args)>(), L, index);
    }
};

template<typename A, typename B>
struct getter<std::pair<A, B>> {
    static decltype(auto) get(lua_State* L, int index = -1) {
        index = index < 0 ? lua_absindex(L, index) - 1 : index;
        return std::pair<decltype(stack::get<A>(L, index)), decltype(stack::get<B>(L, index))>(stack::get<A>(L, index), stack::get<B>(L, index + 1));
    }
};

} // stack
} // sol

#endif // SOL_STACK_GET_HPP
