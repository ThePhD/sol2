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

#ifndef SOL_LUA_FUNC_HPP
#define SOL_LUA_FUNC_HPP

#include "stack.hpp"
#include <memory>

namespace sol {

template<typename TFx>
struct static_lua_func {
    typedef typename std::remove_pointer<typename std::decay<TFx>::type>::type fx_t;
    typedef function_traits<fx_t> fx_traits;
    
    template<typename... Args>
    static int typed_call(types<void>, types<Args...> t, fx_t* fx, lua_State* L) {
        stack::pop_call(L, fx, t);
        return 0;
    }

    template<typename... Args>
    static int typed_call(types<>, types<Args...> t, fx_t* fx, lua_State* L) {
        return typed_call(types<void>(), t, fx, L);
    }

    template<typename... Ret, typename... Args>
    static int typed_call(types<Ret...>, types<Args...> t, fx_t* fx, lua_State* L) {
        typedef typename multi_return<Ret...>::type return_type;
        return_type r = stack::pop_call(L, fx, t);
        stack::push(L, std::move(r));
        return sizeof...(Ret);
    }

    static int call(lua_State* L) {
       auto udata = stack::get_user<fx_t*>(L);
       fx_t* fx = udata.first;
       int r = typed_call(tuple_types<typename fx_traits::return_type>(), typename fx_traits::args_type(), fx, L);
        return r;
    }

    int operator()(lua_State* L) {
        return call(L);
    }
};

template<typename T, typename TFx>
struct static_object_lua_func {
    typedef typename std::remove_pointer<typename std::decay<TFx>::type>::type fx_t;
    typedef function_traits<fx_t> fx_traits;

    template<typename... Args>
    static int typed_call(types<void>, types<Args...>, T& item, fx_t& ifx, lua_State* L) {
        auto fx = [&item, &ifx](Args&&... args) -> void { (item.*ifx)(std::forward<Args>(args)...); };
        stack::pop_call(L, fx, types<Args...>());
        return 0;
    }

    template<typename... Args>
    static int typed_call(types<>, types<Args...> t, T& item, fx_t& ifx, lua_State* L) {
        return typed_call(types<void>(), t, item, ifx, L);
    }

    template<typename... Ret, typename... Args>
    static int typed_call(types<Ret...>, types<Args...>, T& item, fx_t& ifx, lua_State* L) {
        typedef typename multi_return<Ret...>::type return_type;
        auto fx = [&item, &ifx](Args&&... args) -> return_type { return (item.*ifx)(std::forward<Args>(args)...); };
        return_type r = stack::pop_call(L, fx, types<Args...>());
        stack::push(L, std::move(r));
        return sizeof...(Ret);
    }

    static int call(lua_State* L) {
        auto memberdata = stack::get_user<fx_t>(L, 1);
       auto objdata = stack::get_user<T*>(L, memberdata.second);
       fx_t& memfx = memberdata.first;
       T& obj = *objdata.first;
        int r = typed_call(tuple_types<typename fx_traits::return_type>(), typename fx_traits::args_type(), obj, memfx, L);
        return r;
    }

    int operator()(lua_State* L) {
        return call(L);
    }
};

struct lua_func {
    static int call(lua_State* L) {
        void** pinheritancedata = static_cast<void**>(stack::get<lightuserdata_t>(L, 1).value);
        void* inheritancedata = *pinheritancedata;
        if (inheritancedata == nullptr)
            throw sol_error("call from Lua to C++ function has null data");
        lua_func* pfx = static_cast<lua_func*>(inheritancedata);
        lua_func& fx = *pfx;
        int r = fx(L);
        return r;
    }

    static int gc(lua_State* L) {
        void** puserdata = static_cast<void**>(stack::get<userdata_t>(L, 1).value);
        void* userdata = *puserdata;
        lua_func* ptr = static_cast<lua_func*>(userdata);
        std::default_delete<lua_func> dx{};
        dx(ptr);
        return 0;
    }
    
    virtual int operator()(lua_State*) {
        throw sol_error("failure to call specialized wrapped C++ function from Lua");
    }

    virtual ~lua_func() {}
};

template<typename TFx>
struct functor_lua_func : public lua_func {
    typedef decltype(&TFx::operator()) fx_t;
    typedef function_traits<fx_t> fx_traits;
    TFx fx;

    template<typename... FxArgs>
    functor_lua_func(FxArgs&&... fxargs): fx(std::forward<FxArgs>(fxargs)...) {}

    template<typename... Args>
    int operator()(types<void>, types<Args...> t, lua_State* L) {
        stack::pop_call(L, fx, t);
        return 0;
    }

    template<typename... Args>
    int operator()(types<>, types<Args...> t, lua_State* L) {
        return (*this)(types<void>(), t, L);
    }

    template<typename... Ret, typename... Args>
    int operator()(types<Ret...>, types<Args...> t, lua_State* L) {
        typedef typename multi_return<Ret...>::type return_type;
        return_type r = stack::pop_call(L, fx, t);
        stack::push(L, r);
        return sizeof...(Ret);
    }

    virtual int operator()(lua_State* L) override {
        return (*this)(tuple_types<typename fx_traits::return_type>(), typename fx_traits::args_type(), L);
    }
};

template<typename TFx, typename T = TFx, bool is_member_pointer = std::is_member_function_pointer<TFx>::value>
struct function_lua_func : public lua_func {
    typedef typename std::remove_pointer<typename std::decay<TFx>::type>::type fx_t;
    typedef function_traits<fx_t> fx_traits;
    fx_t fx;

    template<typename... FxArgs>
    function_lua_func(FxArgs&&... fxargs): fx(std::forward<FxArgs>(fxargs)...) {}

    template<typename... Args>
    int operator()(types<void>, types<Args...> t, lua_State* L) {
        stack::pop_call(L, fx, t);
        return 0;
    }

    template<typename... Args>
    int operator()(types<>, types<Args...> t, lua_State* L) {
        return (*this)(types<void>(), t, L);
    }

    template<typename... Ret, typename... Args>
    int operator()(types<Ret...>, types<Args...> t, lua_State* L) {
        typedef typename multi_return<Ret...>::type return_type;
        return_type r = stack::pop_call(L, fx, t);
        stack::push(L, std::move(r));
        return sizeof...(Ret);
    }

    virtual int operator()(lua_State* L) override {
        return (*this)(tuple_types<typename fx_traits::return_type>(), typename fx_traits::args_type(), L);
    }
};

template<typename TFx, typename T>
struct function_lua_func<TFx, T, true> : public lua_func {
    typedef typename std::remove_pointer<typename std::decay<TFx>::type>::type fx_t;
    typedef function_traits<fx_t> fx_traits;
    struct functor {
        T member;
        fx_t invocation;

        template<typename... FxArgs>
        functor(T m, FxArgs&&... fxargs): member(std::move(m)), invocation(std::forward<FxArgs>(fxargs)...) {}

        template<typename... Args>
        typename fx_traits::return_type operator()(Args&&... args) {
           return (member.*invocation)(std::forward<Args>(args)...);
        }
    } fx;

    template<typename... FxArgs>
    function_lua_func(T m, FxArgs&&... fxargs): fx(std::move(m), std::forward<FxArgs>(fxargs)...) {}

    template<typename... Args>
    int operator()(types<void>, types<Args...> t, lua_State* L) {
        stack::pop_call(L, fx, t);
        return 0;
    }

    template<typename... Args>
    int operator()(types<>, types<Args...> t, lua_State* L) {
        return (*this)(types<void>(), t, L);
    }

    template<typename... Ret, typename... Args>
    int operator()(types<Ret...>, types<Args...> t, lua_State* L) {
        typedef typename multi_return<Ret...>::type return_type;
        return_type r = stack::pop_call(L, fx, t);
        stack::push(L, std::move(r));
        return sizeof...(Ret);
    }

    virtual int operator()(lua_State* L) override {
        return (*this)(tuple_types<typename fx_traits::return_type>(), typename fx_traits::args_type(), L);
    }
};

template<typename TFx, typename T>
struct class_lua_func : public lua_func {
    typedef typename std::remove_pointer<typename std::decay<TFx>::type>::type fx_t;
    typedef function_traits<fx_t> fx_traits;
    struct functor {
        T* item;
        fx_t invocation;

        template<typename... FxArgs>
        functor(FxArgs&&... fxargs): item(nullptr), invocation(std::forward<FxArgs>(fxargs)...) {}

       void pre_call(lua_State* L) {
            void* userdata = lua_touserdata(L, 0);
            item = static_cast<T*>(userdata);
       }

        template<typename... Args>
        typename fx_traits::return_type operator()(Args&&... args) {
            T& member = *item;
            return (member.*invocation)(std::forward<Args>(args)...);
        }
    } fx;

    template<typename... FxArgs>
    class_lua_func(FxArgs&&... fxargs): fx(std::forward<FxArgs>(fxargs)...) {}

    template<typename... Args>
    int operator()(types<void>, types<Args...> t, lua_State* L) {
        stack::pop_call(L, fx, t);
        return 0;
    }

    template<typename... Args>
    int operator()(types<>, types<Args...> t, lua_State* L) {
        return (*this)(types<void>(), t, L);
    }

    template<typename... Ret, typename... Args>
    int operator()(types<Ret...>, types<Args...> t, lua_State* L) {
        typedef typename multi_return<Ret...>::type return_type;
        return_type r = stack::pop_call(L, fx, t);
        stack::push(L, std::move(r));
        return sizeof...(Ret);
    }

    virtual int operator()(lua_State* L) override {
        fx.pre_call(L);
        return (*this)(tuple_types<typename fx_traits::return_type>(), typename fx_traits::args_type(), L);
    }
};

} // sol

#endif // SOL_LUA_FUNC_HPP
