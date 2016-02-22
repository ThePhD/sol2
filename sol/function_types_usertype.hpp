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
#include <map>

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

    template<typename... Args, std::size_t Start>
    int operator()(types<void> tr, types<Args...> ta, Index<Start>, lua_State* L) {
        stack::call(tr, ta, L, static_cast<int>(Start), fx);
        int nargs = static_cast<int>(sizeof...(Args));
        lua_pop(L, nargs);
        return 0;
    }

    template<typename... Ret, typename... Args, std::size_t Start>
    int operator()(types<Ret...> tr, types<Args...> ta, Index<Start>, lua_State* L) {
        decltype(auto) r = stack::call(tr, ta, L, static_cast<int>(Start), fx);
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
        return static_cast<base_t&>(*this)(tuple_types<return_type>(), args_type(), Index<2>(), L);
    }

    virtual int operator()(lua_State* L) override {
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
        int argcount = lua_gettop(L);
        this->fx.item = stack::get<T*>(L, 1);
        if(this->fx.item == nullptr) {
            throw error("userdata for member variable is null");
        }
        switch(argcount) {
        case 2:
            return static_cast<base_t&>(*this)(tuple_types<return_type>(), types<>(), Index<2>(), L);
        case 3:
            return static_cast<base_t&>(*this)(tuple_types<void>(), args_type(), Index<3>(), L);
        default:
            throw error("cannot get/set userdata member variable with inappropriate number of arguments");
        }
    }

    virtual int operator()(lua_State* L) override {
        return prelude(L);
    }
};

struct usertype_indexing_function : base_function {
    std::string name;
    base_function* original;
    std::map<std::string, std::pair<bool, base_function*>> functions;

    template<typename... Args>
    usertype_indexing_function(std::string name, base_function* original, Args&&... args): name(std::move(name)), original(original), functions(std::forward<Args>(args)...) {}

    int prelude(lua_State* L) {
        const char* accessor = stack::get<const char*>(L, 1 - lua_gettop(L));
        auto functionpair = functions.find(accessor);
        if (functionpair != functions.end()) {
            std::pair<bool, base_function*>& target = functionpair->second;
            if (target.first) {
                stack::push<upvalue>(L, target.second);
                stack::push(L, c_closure(detail::usertype_call<0>, 1));
                return 1;
            }
            return (*target.second)(L);
       }
       base_function& core = *original;
       return core(L);
    }

    virtual int operator()(lua_State* L) override {
        return prelude(L);
    }
};
} // sol

#endif // SOL_FUNCTION_TYPES_USERTYPE_HPP
