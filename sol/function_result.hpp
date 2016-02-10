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

#ifndef SOL_FUNCTION_RESULT_HPP
#define SOL_FUNCTION_RESULT_HPP

#include "reference.hpp"
#include "tuple.hpp"
#include "stack.hpp"
#include "proxy_base.hpp"

namespace sol {
struct function_result : public proxy_base<function_result> {
private:
    lua_State* L;
    int index;
    int returncount;
    int popcount;
    call_error error;

    template <typename T, std::size_t I>
    stack::get_return<T> get(types<T>, indices<I>) const {
        return stack::get<T>(L, index);
    }

    template <typename... Ret, std::size_t... I>
    stack::get_return<Ret...> get(types<Ret...>, indices<I...>) const {
        return stack::get_return<Ret...>(stack::get<Ret>(L, index + I)...);
    }

public:
    function_result() = default;
    function_result(lua_State* L, int index = -1, int returncount = 0, int popcount = 0, call_error error = call_error::ok): L(L), index(index), returncount(returncount), popcount(popcount), error(error) {
        
    }
    function_result(const function_result&) = default;
    function_result& operator=(const function_result&) = default;
    function_result(function_result&& o) : L(o.L), index(o.index), returncount(o.returncount), error(o.error) {
        // Must be manual, otherwise destructor will screw us
        // return count being 0 is enough to keep things clean
        // but will be thorough
        o.L = nullptr;
        o.index = 0;
        o.returncount = 0;
        o.popcount = 0;
        o.error = call_error::runtime;
    }
    function_result& operator=(function_result&& o) {
        L = o.L;
        index = o.index;
        returncount = o.returncount;
        error = o.error;
        // Must be manual, otherwise destructor will screw us
        // return count being 0 is enough to keep things clean
        // but will be thorough
        o.L = nullptr;
        o.index = 0;
        o.returncount = 0;
        o.popcount = 0;
        o.error = call_error::runtime;
        return *this;
    }

    bool valid() const {
        return error == call_error::ok;
    }

    template<typename T>
    T get() const {
        tuple_types<Unqualified<T>> tr;
        return get(tr, tr);
    }

    ~function_result() {
        stack::remove(L, index, popcount);
    }
};
} // sol

#endif // SOL_FUNCTION_RESULT_HPP
