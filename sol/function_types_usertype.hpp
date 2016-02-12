// The MIT License (MIT)

// Copyright (c) 2013-2016 Rapptz and contributors

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

#ifndef SOL_FUNCTION_TYPES_USERTYPE_HPP
#define SOL_FUNCTION_TYPES_USERTYPE_HPP

#include "overload.hpp"
#include "function_types_core.hpp"

namespace sol {
template<typename Function, typename Tp>
struct usertype_function_core : public base_function {
    typedef std::remove_pointer_t<Tp> T;
    typedef std::remove_pointer_t<std::decay_t<Function>> function_type;
    typedef detail::functor<T, function_type> fx_t;
    typedef typename fx_t::traits_type traits_type;
    typedef typename fx_t::args_type args_type;
    typedef typename fx_t::return_type return_type;

    fx_t fx;

    template<typename... Args>
    usertype_function_core(Args&&... args): fx(std::forward<Args>(args)...) {}

    template<typename Return, typename Raw = Unqualified<Return>>
    std::enable_if_t<std::is_same<T, Raw>::value, int> push(lua_State* L, Return&& r) {
        if(ptr(unwrap(r)) == fx.item) {
            // push nothing
            // note that pushing nothing with the ':'
            // syntax means we leave the instance of what
            // was pushed onto the stack by lua to do the
            // function call alone,
            // and naturally lua returns that.
            // It's an "easy" way to return *this,
            // without allocating an extra userdata, apparently!
            return 1;
        }
        return stack::push(L, std::forward<Return>(r));
    }

    template<typename Return, typename Raw = Unqualified<Return>>
    std::enable_if_t<!std::is_same<T, Raw>::value, int> push(lua_State* L, Return&& r) {
        return stack::push(L, std::forward<Return>(r));
    }

    template<typename... Args>
    int operator()(types<void> tr, types<Args...> ta, lua_State* L) {
        //static const std::size_t skew = static_cast<std::size_t>(std::is_member_object_pointer<function_type>::value);
        stack::call(tr, ta, L, 0, fx);
        int nargs = static_cast<int>(sizeof...(Args));
        lua_pop(L, nargs);
        return 0;
    }

    template<typename... Ret, typename... Args>
    int operator()(types<Ret...> tr, types<Args...> ta, lua_State* L) {
        decltype(auto) r = stack::call(tr, ta, L, 0, fx);
        int nargs = static_cast<int>(sizeof...(Args));
        lua_pop(L, nargs);
        int pushcount = push(L, std::forward<decltype(r)>(r));
        return pushcount;
    }
};

template<typename Function, typename Tp>
struct usertype_function : public usertype_function_core<Function, Tp> {
    typedef usertype_function_core<Function, Tp> base_t;
    typedef std::remove_pointer_t<Tp> T;
    typedef typename base_t::traits_type traits_type;
    typedef typename base_t::args_type args_type;
    typedef typename base_t::return_type return_type;

    template<typename... Args>
    usertype_function(Args&&... args): base_t(std::forward<Args>(args)...) {}

    int prelude(lua_State* L) {
        this->fx.item = ptr(stack::get<T>(L, 1));
        if(this->fx.item == nullptr) {
            throw error("userdata for function call is null: are you using the wrong syntax? (use item:function/variable(...) syntax)");
        }
        return static_cast<base_t&>(*this)(tuple_types<return_type>(), args_type(), L);
    }

    virtual int operator()(lua_State* L) override {
        return prelude(L);
    }

    virtual int operator()(lua_State* L, detail::ref_call_t) override {
        return prelude(L);
    }
};

template<typename Function, typename Tp>
struct usertype_variable_function : public usertype_function_core<Function, Tp> {
    typedef usertype_function_core<Function, Tp> base_t;
    typedef std::remove_pointer_t<Tp> T;
    typedef typename base_t::traits_type traits_type;
    typedef typename base_t::args_type args_type;
    typedef typename base_t::return_type return_type;

    template<typename... Args>
    usertype_variable_function(Args&&... args): base_t(std::forward<Args>(args)...) {}

    int prelude(lua_State* L) {
        this->fx.item = ptr(stack::get<T>(L, 1));
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
        return prelude(L);
    }

    virtual int operator()(lua_State* L, detail::ref_call_t) override {
        return prelude(L);
    }
};

template<typename Function, typename Tp>
struct usertype_indexing_function : public usertype_function_core<Function, Tp> {
    typedef usertype_function_core<Function, Tp> base_t;
    typedef std::remove_pointer_t<Tp> T;
    typedef typename base_t::traits_type traits_type;
    typedef typename base_t::args_type args_type;
    typedef typename base_t::return_type return_type;

    std::string name;
    std::unordered_map<std::string, std::pair<std::unique_ptr<base_function>, bool>> functions;

    template<typename... Args>
    usertype_indexing_function(std::string name, Args&&... args): base_t(std::forward<Args>(args)...), name(std::move(name)) {}

    int prelude(lua_State* L) {
        std::string accessor = stack::get<std::string>(L, 1 - lua_gettop(L));
        auto function = functions.find(accessor);
        if(function != functions.end()) {
            if(function->second.second) {
                stack::push<upvalue>(L, function->second.first.get());
                stack::push(L, &base_function::usertype<0>::ref_call, 1);
                return 1;
            }
            return (*function->second.first)(L);
        }
        if (!this->fx.check()) {
            throw error("invalid indexing \"" + accessor + "\" on type: " + name);
        }
        this->fx.item = ptr(stack::get<T>(L, 1));
        return static_cast<base_t&>(*this)(tuple_types<return_type>(), args_type(), L);
    }

    virtual int operator()(lua_State* L) override {
        return prelude(L);
    }

    virtual int operator()(lua_State* L, detail::ref_call_t) override {
        return prelude(L);
    }
};
} // sol

#endif // SOL_FUNCTION_TYPES_USERTYPE_HPP
