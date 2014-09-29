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

#ifndef SOL_FUNCTION_TYPES_HPP
#define SOL_FUNCTION_TYPES_HPP

#include "stack.hpp"
#include <memory>
#include <unordered_map>

namespace sol {
namespace detail {
struct ref_call_t {
    ref_call_t() {}
};

const auto ref_call = ref_call_t{};

template<typename T, typename Func, typename = void>
struct functor {
    typedef member_traits<Func> traits_type;
    typedef typename traits_type::args_type args_type;
    typedef typename traits_type::return_type return_type;

    T* item;
    Func invocation;

    template<typename... FxArgs>
    functor(FxArgs&&... fxargs): item(nullptr), invocation(std::forward<FxArgs>(fxargs)...) {}

    template<typename... Args>
    void call(types<void>, Args&&... args) {
        T& member = *item;
        (member.*invocation)(std::forward<Args>(args)...);
    }

    template<typename Ret, typename... Args>
    Ret call(types<Ret>, Args&&... args) {
        T& member = *item;
        return (member.*invocation)(std::forward<Args>(args)...);
    }

    template<typename... Args>
    auto operator()(Args&&... args) -> decltype(this->call(types<return_type>{}, std::forward<Args>(args)...)) {
        return this->call(types<return_type>{}, std::forward<Args>(args)...);
    }
};

template<typename T, typename Func>
struct functor<T, Func, typename std::enable_if<std::is_member_object_pointer<Func>::value>::type> {
    typedef member_traits<Func> traits_type;
    typedef typename traits_type::args_type args_type;
    typedef typename traits_type::return_type return_type;
    T* item;
    Func invocation;

    template<typename... FxArgs>
    functor(FxArgs&&... fxargs): item(nullptr), invocation(std::forward<FxArgs>(fxargs)...) {}

    template<typename Arg>
    void operator()(Arg&& arg) {
        T& member = *item;
        (member.*invocation) = std::forward<Arg>(arg);
    }

    return_type operator()() {
        T& member = *item;
        return (member.*invocation);
    }
};

template<typename T, typename Func>
struct functor<T, Func, typename std::enable_if<std::is_function<Func>::value || std::is_class<Func>::value>::type> {
    typedef member_traits<Func> traits_type;
    typedef remove_one_type<typename traits_type::args_type> args_type;
    typedef typename traits_type::return_type return_type;
    typedef typename std::conditional<std::is_pointer<Func>::value || std::is_class<Func>::value, Func, typename std::add_pointer<Func>::type>::type function_type;
    T* item;
    function_type invocation;

    template<typename... FxArgs>
    functor(FxArgs&&... fxargs): item(nullptr), invocation(std::forward<FxArgs>(fxargs)...) {}

    template<typename... Args>
    void call(types<void>, Args&&... args) {
        T& member = *item;
        invocation(member, std::forward<Args>(args)...);
    }

    template<typename Ret, typename... Args>
    Ret call(types<Ret>, Args&&... args) {
        T& member = *item;
        return invocation(member, std::forward<Args>(args)...);
    }

    template<typename... Args>
    auto operator()(Args&&... args) -> decltype(this->call(types<return_type>{}, std::forward<Args>(args)...)) {
        return this->call(types<return_type>{}, std::forward<Args>(args)...);
    }
};
} // detail


template<typename Function>
struct static_function {
    typedef typename std::remove_pointer<typename std::decay<Function>::type>::type function_type;
    typedef function_traits<function_type> traits_type;

    template<typename... Args>
    static int typed_call(types<void>, types<Args...> t, function_type* fx, lua_State* L) {
        stack::get_call(L, fx, t);
        std::ptrdiff_t nargs = sizeof...(Args);
        lua_pop(L, nargs);
        return 0;
    }

    template<typename... Args>
    static int typed_call(types<>, types<Args...> t, function_type* fx, lua_State* L) {
        return typed_call(types<void>(), t, fx, L);
    }

    template<typename... Ret, typename... Args>
    static int typed_call(types<Ret...>, types<Args...> t, function_type* fx, lua_State* L) {
        typedef typename return_type<Ret...>::type return_type;
        return_type r = stack::get_call(L, fx, t);
        std::ptrdiff_t nargs = sizeof...(Args);
        lua_pop(L, nargs);
        stack::push(L, detail::return_forward<return_type>{}(r));
        return sizeof...(Ret);
    }

    static int call(lua_State* L) {
        auto udata = stack::detail::get_as_upvalues<function_type*>(L);
        function_type* fx = udata.first;
        int r = typed_call(tuple_types<typename traits_type::return_type>(), typename traits_type::args_type(), fx, L);
        return r;
    }

    int operator()(lua_State* L) {
        return call(L);
    }
};

template<typename T, typename Function>
struct static_member_function {
    typedef typename std::remove_pointer<typename std::decay<Function>::type>::type function_type;
    typedef function_traits<function_type> traits_type;

    template<typename... Args>
    static int typed_call(types<void>, types<Args...>, T& item, function_type& ifx, lua_State* L) {
        auto fx = [&item, &ifx](Args&&... args) -> void { (item.*ifx)(std::forward<Args>(args)...); };
        stack::get_call(L, fx, types<Args...>());
        std::ptrdiff_t nargs = sizeof...(Args);
        lua_pop(L, nargs);
        return 0;
    }

    template<typename... Args>
    static int typed_call(types<>, types<Args...> t, T& item, function_type& ifx, lua_State* L) {
        return typed_call(types<void>(), t, item, ifx, L);
    }

    template<typename... Ret, typename... Args>
    static int typed_call(types<Ret...>, types<Args...>, T& item, function_type& ifx, lua_State* L) {
        typedef typename return_type<Ret...>::type return_type;
        auto fx = [&item, &ifx](Args&&... args) -> return_type { return (item.*ifx)(std::forward<Args>(args)...); };
        return_type r = stack::get_call(L, fx, types<Args...>());
        std::ptrdiff_t nargs = sizeof...(Args);
        lua_pop(L, nargs);
        stack::push(L, detail::return_forward<return_type>{}(r));
        return sizeof...(Ret);
    }

    static int call(lua_State* L) {
        auto memberdata = stack::detail::get_as_upvalues<function_type>(L, 1);
        auto objdata = stack::detail::get_as_upvalues<T*>(L, memberdata.second);
        function_type& memfx = memberdata.first;
        T& obj = *objdata.first;
        int r = typed_call(tuple_types<typename traits_type::return_type>(), typename traits_type::args_type(), obj, memfx, L);
        return r;
    }

    int operator()(lua_State* L) {
        return call(L);
    }
};

struct base_function {
    static int base_call(lua_State* L, void* inheritancedata) {
        if(inheritancedata == nullptr) {
            throw error("call from Lua to C++ function has null data");
        }

        base_function* pfx = static_cast<base_function*>(inheritancedata);
        base_function& fx = *pfx;
        int r = fx(L);
        return r;
    }

    static int ref_base_call(lua_State* L, void* inheritancedata) {
        if(inheritancedata == nullptr) {
            throw error("call from Lua to C++ function has null data");
        }

        base_function* pfx = static_cast<base_function*>(inheritancedata);
        base_function& fx = *pfx;
        int r = fx(L, detail::ref_call);
        return r;
    }

    static int base_gc(lua_State*, void* udata) {
        if(udata == nullptr) {
            throw error("call from lua to C++ gc function with null data");
        }

        base_function* ptr = static_cast<base_function*>(udata);
        std::default_delete<base_function> dx{};
        dx(ptr);
        return 0;
    }

    static int call(lua_State* L) {
        void** pinheritancedata = static_cast<void**>(stack::get<upvalue_t>(L, 1).value);
        return base_call(L, *pinheritancedata);
    }

    static int gc(lua_State* L) {
        void** pudata = static_cast<void**>(stack::get<userdata_t>(L, 1).value);
        return base_gc(L, *pudata);
    }

    template<int I>
    struct userdata {
        static int call(lua_State* L) {
            // Zero-based template parameter, but upvalues start at 1
            return base_call(L, stack::get<upvalue_t>(L, I + 1));
        }

        static int ref_call(lua_State* L) {
            return ref_base_call(L, stack::get<upvalue_t>(L, I + 1));
        }

        static int gc(lua_State* L) {
            for(int i = 0; i < I; ++i) {
                upvalue_t up = stack::get<upvalue_t>(L, i + 1);
                base_function* obj = static_cast<base_function*>(up.value);
                std::allocator<base_function> alloc{};
                alloc.destroy(obj);
                alloc.deallocate(obj, 1);
            }
            return 0;
        }
    };

    virtual int operator()(lua_State*) {
        throw error("failure to call specialized wrapped C++ function from Lua");
    }

    virtual int operator()(lua_State*, detail::ref_call_t) {
        throw error("failure to call reference specialized wrapped C++ function from Lua");
    }

    virtual ~base_function() {}
};

template<typename Function>
struct functor_function : public base_function {
    typedef decltype(&Function::operator()) function_type;
    typedef function_return_t<function_type> return_type;
    typedef function_args_t<function_type> args_type;
    Function fx;

    template<typename... FxArgs>
    functor_function(FxArgs&&... fxargs): fx(std::forward<FxArgs>(fxargs)...) {}

    template<typename... Args>
    int operator()(types<void>, types<Args...> t, lua_State* L) {
        stack::get_call(L, fx, t);
        std::ptrdiff_t nargs = sizeof...(Args);
        lua_pop(L, nargs);
        return 0;
    }

    template<typename... Args>
    int operator()(types<>, types<Args...> t, lua_State* L) {
        return (*this)(types<void>(), t, L);
    }

    template<typename... Ret, typename... Args>
    int operator()(types<Ret...>, types<Args...> t, lua_State* L) {
        return_type r = stack::get_call(L, fx, t);
        std::ptrdiff_t nargs = sizeof...(Args);
        lua_pop(L, nargs);
        stack::push(L, r);
        return sizeof...(Ret);
    }

    virtual int operator()(lua_State* L) override {
        return (*this)(tuple_types<return_type>(), args_type(), L);
    }

    virtual int operator()(lua_State* L, detail::ref_call_t) override {
        return (*this)(tuple_types<return_type>(), args_type(), L);
    }
};

template<typename Function, typename T>
struct member_function : public base_function {
    typedef typename std::remove_pointer<Decay<Function>>::type function_type;
    typedef function_return_t<function_type> return_type;
    typedef function_args_t<function_type> args_type;
    struct functor {
        T member;
        function_type invocation;

        template<typename Tm, typename... FxArgs>
        functor(Tm&& m, FxArgs&&... fxargs): member(std::forward<Tm>(m)), invocation(std::forward<FxArgs>(fxargs)...) {}

        template<typename... Args>
        return_type operator()(Args&&... args) {
           return (member.*invocation)(std::forward<Args>(args)...);
        }
    } fx;

    template<typename Tm, typename... FxArgs>
    member_function(Tm&& m, FxArgs&&... fxargs): fx(std::forward<Tm>(m), std::forward<FxArgs>(fxargs)...) {}

    template<typename... Args>
    int operator()(types<void>, types<Args...> t, lua_State* L) {
        stack::get_call(L, fx, t);
        return 0;
    }

    template<typename... Args>
    int operator()(types<>, types<Args...> t, lua_State* L) {
        return (*this)(types<void>(), t, L);
    }

    template<typename... Ret, typename... Args>
    int operator()(types<Ret...>, types<Args...> t, lua_State* L) {
        return_type r = stack::get_call(L, fx, t);
        std::ptrdiff_t nargs = sizeof...(Args);
        lua_pop(L, nargs);
        stack::push(L, std::move(r));
        return sizeof...(Ret);
    }

    virtual int operator()(lua_State* L) override {
        return (*this)(tuple_types<return_type>(), args_type(), L);
    }

    virtual int operator()(lua_State* L, detail::ref_call_t) override {
        return (*this)(tuple_types<return_type>(), args_type(), L);
    }
};

template<typename Function, typename Tp>
struct userdata_function_core : public base_function {
    typedef typename std::remove_pointer<Tp>::type T;
    typedef typename std::remove_pointer<typename std::decay<Function>::type>::type function_type;
    typedef detail::functor<T, function_type> fx_t;
    typedef typename fx_t::traits_type traits_type;
    typedef typename fx_t::args_type args_type;
    typedef typename fx_t::return_type return_type;

    fx_t fx;

    template<typename... FxArgs>
    userdata_function_core(FxArgs&&... fxargs): fx(std::forward<FxArgs>(fxargs)...) {}

    template<typename Return, typename Raw = Unqualified<Return>>
    typename std::enable_if<std::is_same<T, Raw>::value, void>::type push(lua_State* L, Return&& r) {
        if(detail::get_ptr(r) == fx.item) {
            // push nothing
            // note that pushing nothing with the ':'
            // syntax means we leave the instance of what
            // was pushed onto the stack by lua to do the
            // function call alone,
            // and naturally lua returns that.
            // It's an "easy" way to return *this,
            // without allocating an extra userdata, apparently!
            return;
        }
        stack::push(L, std::forward<Return>(r));
    }

    template<typename Return, typename Raw = Unqualified<Return>>
    typename std::enable_if<!std::is_same<T, Raw>::value, void>::type push(lua_State* L, Return&& r) {
        stack::push(L, std::forward<Return>(r));
    }

    template<typename... Args>
    int operator()(types<void>, types<Args...> t, lua_State* L) {
        static const std::size_t skew = static_cast<std::size_t>(std::is_member_object_pointer<function_type>::value);
        stack::get_call(L, 2 + skew, fx, t);
        std::ptrdiff_t nargs = sizeof...(Args);
        lua_pop(L, nargs);
        return 0;
    }

    template<typename... Ret, typename... Args>
    int operator()(types<Ret...>, types<Args...> t, lua_State* L) {
        return_type r = stack::get_call(L, 2, fx, t);
        std::ptrdiff_t nargs = sizeof...(Args);
        lua_pop(L, nargs);
        push(L, detail::return_forward<return_type>{}(r));
        return sizeof...(Ret);
    }

    template<typename... Args>
    int operator()(types<>, types<Args...> t, lua_State* L) {
        return (*this)(types<void>(), t, L);
    }
};

template<typename Function, typename Tp>
struct userdata_function : public userdata_function_core<Function, Tp> {
    typedef userdata_function_core<Function, Tp> base_t;
    typedef typename std::remove_pointer<Tp>::type T;
    typedef typename base_t::traits_type traits_type;
    typedef typename base_t::args_type args_type;
    typedef typename base_t::return_type return_type;

    template<typename... FxArgs>
    userdata_function(FxArgs&&... fxargs): base_t(std::forward<FxArgs>(fxargs)...) {}

    template<typename Tx>
    int fx_call(lua_State* L) {
        this->fx.item = detail::get_ptr(stack::get<Tx>(L, 1));
        if(this->fx.item == nullptr) {
            throw error("userdata for function call is null: are you using the wrong syntax? (use item:function/variable(...) syntax)");
        }
        return static_cast<base_t&>(*this)(tuple_types<return_type>(), args_type(), L);
    }

    virtual int operator()(lua_State* L) override {
        return fx_call<T>(L);
    }

    virtual int operator()(lua_State* L, detail::ref_call_t) override {
        return fx_call<T*>(L);
    }
};

template<typename Function, typename Tp>
struct userdata_variable_function : public userdata_function_core<Function, Tp> {
    typedef userdata_function_core<Function, Tp> base_t;
    typedef typename std::remove_pointer<Tp>::type T;
    typedef typename base_t::traits_type traits_type;
    typedef typename base_t::args_type args_type;
    typedef typename base_t::return_type return_type;

    template<typename... FxArgs>
    userdata_variable_function(FxArgs&&... fxargs): base_t(std::forward<FxArgs>(fxargs)...) {}

    template<typename Tx>
    int fx_call(lua_State* L) {
        this->fx.item = detail::get_ptr(stack::get<Tx>(L, 1));
        if(this->fx.item == nullptr) {
            throw error("userdata for member variable is null");
        }

        int argcount = lua_gettop(L);
        switch(argcount) {
        case 2:
            return static_cast<base_t&>(*this)(tuple_types<return_type>(), types<>(), L);
        case 3:
            return static_cast<base_t&>(*this)(tuple_types<void>(), args_type(), L);
        default:
            throw error("cannot get/set userdata member variable with inappropriate number of arguments");
        }

    }

    virtual int operator()(lua_State* L) override {
        return fx_call<T>(L);
    }

    virtual int operator()(lua_State* L, detail::ref_call_t) override {
        return fx_call<T*>(L);
    }
};

template<typename Function, typename Tp>
struct userdata_indexing_function : public userdata_function_core<Function, Tp> {
    typedef userdata_function_core<Function, Tp> base_t;
    typedef typename std::remove_pointer<Tp>::type T;
    typedef typename base_t::traits_type traits_type;
    typedef typename base_t::args_type args_type;
    typedef typename base_t::return_type return_type;

    std::string name;
    std::unordered_map<std::string, std::pair<std::unique_ptr<base_function>, bool>> functions;

    template<typename... FxArgs>
    userdata_indexing_function(std::string name, FxArgs&&... fxargs): base_t(std::forward<FxArgs>(fxargs)...), name(std::move(name)) {}

    template<typename Tx>
    int fx_call(lua_State* L) {
        std::string accessor = stack::get<std::string>(L, 1 - lua_gettop(L));
        auto function = functions.find(accessor);
        if(function != functions.end()) {
            if(function->second.second) {
                stack::push<upvalue_t>(L, function->second.first.get());
                if(std::is_same<T*, Tx>::value) {
                    stack::push(L, &base_function::userdata<0>::ref_call, 1);
                }
                else {
                    stack::push(L, &base_function::userdata<0>::call, 1);
                }
                return 1;
            }
            else if(std::is_same<T*, Tx>::value) {
                return (*function->second.first)(L, detail::ref_call);
            }
            else {
                return (*function->second.first)(L);
            }
        }
        if(this->fx.invocation == nullptr) {
            throw error("invalid indexing \"" + accessor + "\" on type: " + name);
        }
        this->fx.item = detail::get_ptr(stack::get<Tx>(L, 1));
        return static_cast<base_t&>(*this)(tuple_types<return_type>(), args_type(), L);
    }

    virtual int operator()(lua_State* L) override {
        return fx_call<T>(L);
    }

    virtual int operator()(lua_State* L, detail::ref_call_t) override {
        return fx_call<T*>(L);
    }
};

} // sol

#endif // SOL_FUNCTION_TYPES_HPP
