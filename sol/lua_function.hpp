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
namespace detail {
template<typename T, std::size_t N>
void get_upvalue_ptr(lua_State* L, T*& data, std::size_t datasize, std::array<void*, N> voiddata, int& upvalue) {
    for (std::size_t i = 0, d = 0; d < datasize; ++i, d += sizeof(void*)) {
        voiddata[i] = lua_touserdata(L, lua_upvalueindex(upvalue++));
    }
    data = reinterpret_cast<T*>(static_cast<void*>(voiddata.data()));
}
template<typename T, std::size_t N>
void get_upvalue_ptr(lua_State* L, T*& data, std::array<void*, N> voiddata, int& upvalue) {
    get_upvalue_ptr(L, data, sizeof(T), voiddata, upvalue);
}
template<typename T>
void get_upvalue_ptr(lua_State* L, T*& data, int& upvalue) {
    const static std::size_t data_t_count = (sizeof(T)+(sizeof(void*)-1)) / sizeof(void*);
    typedef std::array<void*, data_t_count> data_t;
    data_t voiddata{{}};
    return get_upvalue_ptr(L, data, voiddata, upvalue);
}
template<typename T>
void get_upvalue(lua_State* L, T& data, int& upvalue) {
    const static std::size_t data_t_count = (sizeof(T)+(sizeof(void*)-1)) / sizeof(void*);
    typedef std::array<void*, data_t_count> data_t;
    data_t voiddata{{}};
    for (std::size_t i = 0, d = 0; d < sizeof(T); ++i, d += sizeof(void*)) {
        voiddata[i] = lua_touserdata(L, lua_upvalueindex(upvalue++));
    }
    data = *reinterpret_cast<T*>(static_cast<void*>(voiddata.data()));
}
} // detail


template<typename TFx>
struct static_lua_func {
    typedef typename std::remove_pointer<typename std::decay<TFx>::type>::type fx_t;
    typedef function_traits<fx_t> fx_traits;

    template<typename... Args>
    static int typed_call(types<void>, types<Args...> t, fx_t* fx, lua_State* L) {
        stack::pop_call(L, fx, t);
        return 0;
    }

    template<typename... TRn, typename... Args>
    static int typed_call(types<TRn...>, types<Args...> t, fx_t* fx, lua_State* L) {
        auto r = stack::pop_call(L, fx, t);
        stack::push(L, std::move(r));
        return sizeof...(TRn);
    }

    static int call(lua_State* L) {
        int upvalue = 1;
        fx_t* fx;
        detail::get_upvalue(L, fx, upvalue);
        int r = typed_call(tuple_types<typename fx_traits::return_type>(), typename fx_traits::args_type(), fx, L);
        return r;
    }

    int operator()(lua_State* L) {
        return call(L);
    }
};

template<typename T, typename TFx>
struct static_object_lua_func {
    typedef typename std::decay<TFx>::type fx_t;
    typedef function_traits<fx_t> fx_traits;

    template<typename... Args>
    static int typed_call(types<void>, types<Args...>, T& item, fx_t& ifx, lua_State* L) {
        auto fx = [&item, &ifx](Args&&... args) { (item.*ifx)(std::forward<Args>(args)...); };
        stack::pop_call(L, fx, types<Args...>());
        return 0;
    }

    template<typename TR, typename... Args>
    static int typed_call(types<TR>, types<Args...>, T& item, fx_t& ifx, lua_State* L) {
        auto fx = [&item, &ifx](Args&&... args) -> TR { 
           return (item.*ifx)(std::forward<Args>(args)...); 
        };
        auto r = stack::pop_call(L, fx, types<Args...>());
        stack::push(L, std::move(r));
        return 1;
    }

    template<typename... TRn, typename... Args>
    static int typed_call(types<TRn...>, types<Args...>, T& item, fx_t& ifx, lua_State* L) {
        auto fx = [&item, &ifx](Args&&... args) -> std::tuple<TRn...> { return (item.*ifx)(std::forward<Args>(args)...); };
        auto r = stack::pop_call(L, fx, types<Args...>());
        stack::push(L, std::move(r));
        return sizeof...(TRn);
    }

    static int call(lua_State* L) {
        const static std::size_t data_t_count = (sizeof(fx_t)+(sizeof(void*)-1)) / sizeof(void*);
        typedef std::array<void*, data_t_count> data_t;
        int upvalue = 1;
        data_t data = {{}};
        fx_t* fxptr;
        for (std::size_t i = 0, d = 0; d < sizeof(fx_t*); ++i, d += sizeof(void*)) {
            data[i] = lua_touserdata(L, lua_upvalueindex(upvalue++));
        }
        fxptr = reinterpret_cast<fx_t*>(static_cast<void*>(data.data())); 
        fx_t& mem_ptr = *fxptr;
        void* objectdata = lua_touserdata(L, lua_upvalueindex(upvalue++));
        T& obj = *static_cast<T*>(objectdata);
        int r = typed_call(tuple_types<typename fx_traits::return_type>(), typename fx_traits::args_type(), obj, mem_ptr, L);
        return r;
    }

    int operator()(lua_State* L) {
        return call(L);
    }
};

struct lua_func {
    static int call(lua_State* L) {
        void** pinheritancedata = static_cast<void**>(lua_touserdata(L, lua_upvalueindex(1)));
        void* inheritancedata = *pinheritancedata;
        if (inheritancedata == nullptr)
            throw sol_error("call from Lua to C++ function has null data");
        lua_func* pfx = static_cast<lua_func*>(inheritancedata);
        lua_func& fx = *pfx;
        int r = fx(L);
        return r;
    }

    static int gc(lua_State* L) {
        void** puserdata = static_cast<void**>(lua_touserdata(L, 1));
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
struct lambda_lua_func : public lua_func {
    typedef decltype(&TFx::operator()) fx_t;
    typedef function_traits<fx_t> fx_traits;
    TFx fx;

    template<typename... FxArgs>
    lambda_lua_func(FxArgs&&... fxargs): fx(std::forward<FxArgs>(fxargs)...) {}

    virtual int operator()(lua_State* L) override {
        return (*this)(tuple_types<typename fx_traits::return_type>(), typename fx_traits::args_type(), L);
    }

    template<typename... Args>
    int operator()(types<void>, types<Args...> t, lua_State* L) {
        stack::pop_call(L, fx, t);
        return 0;
    }

    template<typename... TRn, typename... Args>
    int operator()(types<TRn...>, types<Args...> t, lua_State* L) {
        auto r = stack::pop_call(L, fx, t);
        stack::push(L, r);
        return sizeof...(TRn);
    }
};

template<typename TFx, typename T = TFx, bool is_member_pointer = std::is_member_function_pointer<TFx>::value>
struct explicit_lua_func : public lua_func {
    typedef typename std::remove_pointer<typename std::decay<TFx>::type>::type fx_t;
    typedef function_traits<fx_t> fx_traits;
    TFx fx;

    template<typename... FxArgs>
    explicit_lua_func(FxArgs&&... fxargs): fx(std::forward<FxArgs>(fxargs)...) {}

    virtual int operator()(lua_State* L) override {
        return (*this)(tuple_types<typename fx_traits::return_type>(), typename fx_traits::args_type(), L);
    }

    template<typename... Args>
    int operator()(types<void>, types<Args...> t, lua_State* L) {
        stack::pop_call(L, fx, t);
        return 0;
    }

    template<typename... TRn, typename... Args>
    int operator()(types<TRn...>, types<Args...> t, lua_State* L) {
        auto r = stack::pop_call(L, fx, t);
        stack::push(L, std::move(r));
        return sizeof...(TRn);
    }
};

template<typename TFx, typename T>
struct explicit_lua_func<TFx, T, true> : public lua_func {
    typedef typename std::remove_pointer<typename std::decay<TFx>::type>::type fx_t;
    typedef function_traits<fx_t> fx_traits;
    struct lambda {
        T member;
        TFx invocation;

        template<typename... FxArgs>
        lambda(T m, FxArgs&&... fxargs): member(std::move(m)), invocation(std::forward<FxArgs>(fxargs)...) {}

        template<typename... Args>
        typename fx_traits::return_type operator()(Args&&... args) {
           return (member.*invocation)(std::forward<Args>(args)...);
        }
    } fx;

    template<typename... FxArgs>
    explicit_lua_func(T m, FxArgs&&... fxargs): fx(std::move(m), std::forward<FxArgs>(fxargs)...) {}

    virtual int operator()(lua_State* L) override {
        return (*this)(tuple_types<typename fx_traits::return_type>(), typename fx_traits::args_type(), L);
    }

    template<typename... Args>
    int operator()(types<void>, types<Args...> t, lua_State* L) {
        stack::pop_call(L, fx, t);
        return 0;
    }

    template<typename... TRn, typename... Args>
    int operator()(types<TRn...>, types<Args...> t, lua_State* L) {
        auto r = stack::pop_call(L, fx, t);
        stack::push(L, r);
        return sizeof...(TRn);
    }
};

} // sol

#endif // SOL_LUA_FUNC_HPP
