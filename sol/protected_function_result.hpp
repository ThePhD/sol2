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

#ifndef SOL_PROTECTED_FUNCTION_RESULT_HPP
#define SOL_PROTECTED_FUNCTION_RESULT_HPP

#include "reference.hpp"
#include "tuple.hpp"
#include "stack.hpp"
#include "proxy_base.hpp"

namespace sol {
struct protected_function_result : public proxy_base<protected_function_result> {
private:
    lua_State* L;
    int index;
    int returncount;
    int popcount;
    call_status err;

public:
    protected_function_result() = default;
    protected_function_result(lua_State* L, int index = -1, int returncount = 0, int popcount = 0, call_status error = call_status::ok): L(L), index(index), returncount(returncount), popcount(popcount), err(error) {
        
    }
    protected_function_result(const protected_function_result&) = default;
    protected_function_result& operator=(const protected_function_result&) = default;
    protected_function_result(protected_function_result&& o) : L(o.L), index(o.index), returncount(o.returncount), popcount(o.popcount), err(o.err) {
        // Must be manual, otherwise destructor will screw us
        // return count being 0 is enough to keep things clean
        // but will be thorough
        o.L = nullptr;
        o.index = 0;
        o.returncount = 0;
        o.popcount = 0;
        o.err = call_status::runtime;
    }
    protected_function_result& operator=(protected_function_result&& o) {
        L = o.L;
        index = o.index;
        returncount = o.returncount;
        popcount = o.popcount;
        err = o.err;
        // Must be manual, otherwise destructor will screw us
        // return count being 0 is enough to keep things clean
        // but will be thorough
        o.L = nullptr;
        o.index = 0;
        o.returncount = 0;
        o.popcount = 0;
        o.err = call_status::runtime;
        return *this;
    }

    call_status error() const {
        return err;
    }

    bool valid() const {
        return error() == call_status::ok;
    }

    template<typename T>
    T get() const {
        return stack::get<T>(L, index);
    }

    ~protected_function_result() {
        stack::remove(L, index, popcount);
    }
};

struct protected_result {
private:
    protected_function_result result;

public:
    template <typename... Args>
    protected_result( Args&&... args ) : result(std::forward<Args>(args)...) {
    }

    bool valid() const { 
        return result.valid(); 
    }

    explicit operator bool () const {
        return valid();
    }

    protected_function_result& operator* () {
        return result;
    }
};
} // sol

#endif // SOL_FUNCTION_RESULT_HPP
