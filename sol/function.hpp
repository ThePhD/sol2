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

#ifndef SOL_FUNCTION_HPP
#define SOL_FUNCTION_HPP

#include "reference.hpp"
#include "tuple.hpp"
#include "stack.hpp"
#include <cstdint>
#include <functional>

namespace sol {
class function : public reference {
private:
    void luacall (std::size_t argcount, std::size_t resultcount) const {
        lua_call(state(), static_cast<uint32_t>(argcount), static_cast<uint32_t>(resultcount));
    }

    template<typename... Ret>
    std::tuple<Ret...> invoke(types<Ret...>, std::size_t n) const {
        luacall(n, sizeof...(Ret));
        return stack::pop_reverse_call(state(), std::make_tuple<Ret...>, types<Ret...>());
    }

    template<typename Ret>
    Ret invoke(types<Ret>, std::size_t n) const {
        luacall(n, 1);
        return stack::pop<Ret>(state());
    }

    void invoke(types<void>, std::size_t n) const {
        luacall(n, 0);
    }

    void invoke(types<>, std::size_t n) const {
        luacall(n, 0);
    }

public:
    function() : reference() {}
    function(lua_State* L, int index = -1): reference(L, index) {
        type_assert(L, index, type::function);
    }
    function(const function&) = default;
    function& operator=(const function&) = default;

    template<typename... Args>
    void operator()(Args&&... args) const {
        call<>(std::forward<Args>(args)...);
    }

    template<typename... Ret, typename... Args>
    typename return_type<Ret...>::type operator()(types<Ret...>, Args&&... args) const {
        return call<Ret...>(std::forward<Args>(args)...);
    }

    template<typename... Ret, typename... Args>
    typename return_type<Ret...>::type call(Args&&... args) const {
        push();
        stack::push_args(state(), std::forward<Args>(args)...);
        return invoke(types<Ret...>(), sizeof...(Args));
    }
};

namespace stack {
namespace detail {
template <typename Signature, typename... FxArgs, typename... Ret>
inline std::function<Signature> get_std_func(types<FxArgs...>, types<Ret...>, lua_State* L, int index = -1) {
    typedef typename function_traits<Signature>::return_type return_t;
    sol::function f(L, index);
    auto fx = [ f, L, index ] (FxArgs&&... args) -> return_t {
        return f(types<Ret...>(), std::forward<FxArgs>(args)...);
    };
    return std::move(fx);
}

template <typename Signature, typename... FxArgs>
inline std::function<Signature> get_std_func(types<FxArgs...>, types<void>, lua_State* L, int index = -1) {
    sol::function f(L, index);
    auto fx = [ f, L, index ] (FxArgs&&... args) -> void {
        f(std::forward<FxArgs>(args)...);
    };
    return std::move(fx);
}

template <typename Signature, typename... FxArgs>
inline std::function<Signature> get_std_func(types<FxArgs...> t, types<>, lua_State* L, int index = -1) {
    return get_std_func<Signature>(std::move(t), types<void>(), L, index);
}

template <typename Signature>
inline std::function<Signature> get(types<std::function<Signature>>, lua_State* L, int index = -1) {
    typedef typename function_traits<Signature> fx_t;
    typedef typename fx_t::args_type args_t;
    typedef typename tuple_types<typename fx_t::return_type>::type ret_t;
    return get_std_func<Signature>(args_t(), ret_t(), L, index);
}
} // detail
} // stack
} // sol

#endif // SOL_FUNCTION_HPP
