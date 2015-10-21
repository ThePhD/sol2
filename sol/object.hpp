// The MIT License (MIT)

// Copyright (c) 2013-2015 Danny Y., Rapptz

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

#ifndef SOL_OBJECT_HPP
#define SOL_OBJECT_HPP

#include "reference.hpp"
#include "stack.hpp"
#include "function.hpp"

namespace sol {
class object : public reference {
public:
    object(lua_State* L, int index = -1): reference(L, index) {}
    object() = default;

    template<typename T>
    auto as() const -> decltype(stack::get<T>(state())) const {
        push();
        type actual = stack::get<type>(state());
        // This code is actually present
        // in almost all of the type-getters,
        // and it thus insanely redundant
        // type_assert(state(), -1, type_of<T>(), actual);
        return stack::pop<T>(state());
    }

    template<typename T>
    bool is() const {
        auto expected = type_of<T>();
        auto actual = get_type();
        return (expected == actual) || (expected == type::poly);
    }

    bool valid() const {
        return !this->is<nil_t>();
    }

    operator const char* () const {
        return this->as<const char*>();
    }

    template<typename T, EnableIf<Not<std::is_same<Unqualified<T>, const char*>>, Not<std::is_same<Unqualified<T>, char>>, Not<std::is_same<Unqualified<T>, std::string>>, Not<std::is_same<Unqualified<T>, std::initializer_list<char>>>> = 0>
    operator T () const {
        return this->as<T>();
    }

    template<typename... Ret, typename... Args>
    stack::get_return_or<function_result, Ret...> call( Args&&... args ) {
        return this->as<function>()(types<Ret...>(), std::forward<Args>( args )...);
    }

    template<typename... Args>
    function_result operator()( Args&&... args ) {
        return this->as<function>()(std::forward<Args>( args )...);
    }
};

inline bool operator==(const object& lhs, const nil_t&) {
    return !lhs.valid();
}

inline bool operator==(const nil_t&, const object& rhs) {
    return !rhs.valid();
}

inline bool operator!=(const object& lhs, const nil_t&) {
    return lhs.valid();
}

inline bool operator!=(const nil_t&, const object& rhs) {
    return rhs.valid();
}
} // sol

#endif // SOL_OBJECT_HPP
