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

namespace sol {
class function : virtual public reference {
private:
    void luacall (std::size_t argcount, std::size_t resultcount) {
        lua_call(state(), static_cast<uint32_t>(argcount), static_cast<uint32_t>(resultcount));
    }

    template<typename... Ret>
    std::tuple<Ret...> invoke(types<Ret...>, std::size_t n) {
        luacall(n, sizeof...(Ret));
        return stack::pop_call(state(), std::make_tuple<Ret...>, types<Ret...>());
    }

    template<typename Ret>
    Ret invoke(types<Ret>, std::size_t n) {
        luacall(n, 1);
        return stack::pop<Ret>(state());
    }

    void invoke(types<void>, std::size_t n) {
        luacall(n, 0);
    }

    void invoke(types<>, std::size_t n) {
        luacall(n, 0);
    }

public:
    function() : reference() {}
    function(lua_State* L, int index = -1): reference(L, index) {
        type_assert(L, index, type::function);
    }

    template<typename... Ret, typename... Args>
    auto call(Args&&... args) -> decltype(invoke(types<Ret...>(), sizeof...(Args))) {
        push();
        stack::push_args(state(), std::forward<Args>(args)...);
        return invoke(types<Ret...>(), sizeof...(Args));
    }
};
} // sol

#endif // SOL_FUNCTION_HPP
