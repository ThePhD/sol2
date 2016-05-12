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

#ifndef SOL_FUNCTION_TYPES_USERTYPE_HPP
#define SOL_FUNCTION_TYPES_USERTYPE_HPP

#include "overload.hpp"
#include "function_types_core.hpp"
#include <map>

namespace sol {
namespace function_detail {
template<typename Tp, typename Function>
struct usertype_function_core : public base_function {
    typedef std::remove_pointer_t<Tp> T;
    typedef std::remove_pointer_t<std::decay_t<Function>> function_type;
    typedef functor<T, function_type> fx_t;
    typedef typename fx_t::traits_type traits_type;
    typedef typename fx_t::args_type args_type;
    typedef typename fx_t::return_type return_type;

    fx_t fx;

    template<typename... Args>
    usertype_function_core(Args&&... args): fx(std::forward<Args>(args)...) {}

    template<typename... Ret, typename... Args, std::size_t Start>
    int operator()(types<Ret...> tr, types<Args...> ta, index_value<Start>, lua_State* L) {
        return stack::call_into_lua<1>(tr, ta, L, static_cast<int>(Start), fx);
    }
};

template<typename Tp, typename Function>
struct usertype_function : public usertype_function_core<Tp, Function> {
    typedef usertype_function_core<Tp, Function> base_t;
    typedef std::remove_pointer_t<Tp> T;
    typedef typename base_t::traits_type traits_type;
    typedef typename base_t::args_type args_type;
    typedef typename base_t::return_type return_type;

    template<typename... Args>
    usertype_function(Args&&... args): base_t(std::forward<Args>(args)...) {}

    int prelude(lua_State* L) {
        this->fx.item = detail::ptr(stack::get<T>(L, 1));
        return static_cast<base_t&>(*this)(meta::tuple_types<return_type>(), args_type(), index_value<2>(), L);
    }

    virtual int operator()(lua_State* L) override {
        return prelude(L);
    }
};

template<typename Tp, typename Function>
struct usertype_variable_function : public usertype_function_core<Tp, Function> {
    typedef usertype_function_core<Tp, Function> base_t;
    typedef std::remove_pointer_t<Tp> T;
    typedef typename base_t::fx_t fx_t;
    typedef typename base_t::traits_type traits_type;
    typedef typename base_t::args_type args_type;
    typedef typename base_t::return_type return_type;
    typedef typename fx_t::can_read can_read;
    typedef typename fx_t::can_write can_write;

    template<typename... Args>
    usertype_variable_function(Args&&... args): base_t(std::forward<Args>(args)...) {}

    int set_assignable(std::false_type, lua_State* L) {
        lua_pop(L, 3);
        return luaL_error(L, "sol: cannot write to this type: copy assignment/constructor not available");
    }

    int set_assignable(std::true_type, lua_State* L) {
        return set_writable(can_write(), L);
    }

    int set_writable(std::false_type, lua_State* L) {
        lua_pop(L, 3);
        return luaL_error(L, "sol: cannot write to readonly variable");
    }

    int set_writable(std::true_type, lua_State* L) {
        return static_cast<base_t&>(*this)(meta::tuple_types<void>(), args_type(), index_value<3>(), L);
    }

    int set_variable(std::false_type, lua_State* L) {
        lua_pop(L, 3);
        return luaL_error(L, "sol: cannot write to a const variable");
    }

    int set_variable(std::true_type, lua_State* L) {
        return set_assignable(std::is_assignable<std::add_lvalue_reference_t<return_type>, return_type>(), L);
    }

    int get_variable(std::false_type, lua_State* L) {
        lua_pop(L, 2);
        return luaL_error(L, "sol: cannot read from a readonly property");
    }

    int get_variable(std::true_type, lua_State* L) {
        return static_cast<base_t&>(*this)(meta::tuple_types<return_type>(), types<>(), index_value<2>(), L);
    }

    int prelude(lua_State* L) {
        int argcount = lua_gettop(L);
        this->fx.item = stack::get<T*>(L, 1);
        switch(argcount) {
        case 2:
            return get_variable(can_read(), L);
        case 3:
            return set_variable(meta::Not<std::is_const<return_type>>(), L);
        default:
            return luaL_error(L, "sol: cannot get/set userdata member variable with inappropriate number of arguments");
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
                stack::push<light_userdata_value>(L, target.second);
                stack::push(L, c_closure(usertype_call<0>, 1));
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

struct fail_on_error : base_function {
    int prelude(lua_State* L) {
        const char* accessor = stack::get<const char*>(L, 1 - lua_gettop(L));
        return luaL_error(L, "sol: attempt to index nil value \"%s\" on userdata (bad (mispelled?) key name or does not exist)", accessor);
    }

    virtual int operator()(lua_State* L) override {
        return prelude(L);
    }

    ~fail_on_error() {

    }
};

inline fail_on_error& failure_on_error() {
    static fail_on_error f{};
    return f;
}
} // function_detail
} // sol

#endif // SOL_FUNCTION_TYPES_USERTYPE_HPP
