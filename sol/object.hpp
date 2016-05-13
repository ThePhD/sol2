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

#ifndef SOL_OBJECT_HPP
#define SOL_OBJECT_HPP

#include "reference.hpp"
#include "userdata.hpp"
#include "stack.hpp"
#include "variadic_args.hpp"

namespace sol {
template <typename base_t>
class basic_object : public base_t {
private:
    template<typename T>
    decltype(auto) as_stack(std::true_type) const {
        return stack::get<T>(base_t::lua_state(), base_t::stack_index());
    }

    template<typename T>
    decltype(auto) as_stack(std::false_type) const {
        base_t::push();
        return stack::pop<T>(base_t::lua_state());
    }

    template<typename T>
    bool is_stack(std::true_type) const {
        return stack::check<T>(base_t::lua_state(), base_t::stack_index(), no_panic);
    }

    template<typename T>
    bool is_stack(std::false_type) const {
        auto pp = stack::push_pop(*this);
        return stack::check<T>(base_t::lua_state(), -1, no_panic);
    }

public:
    basic_object() noexcept = default;
    template <typename T, meta::EnableIf<meta::Not<std::is_same<meta::Unqualified<T>, basic_object>>, std::is_base_of<base_t, meta::Unqualified<T>>> = 0>
    basic_object(T&& r) : base_t(std::forward<T>(r)) {}
    basic_object(nil_t r) : base_t(r) {}
    basic_object(const basic_object&) = default;
    basic_object(basic_object&&) = default;
    basic_object& operator=(const basic_object&) = default;
    basic_object& operator=(basic_object&&) = default;
    basic_object(const stack_reference& r) noexcept : basic_object(r.lua_state(), r.stack_index()) {}
    basic_object(stack_reference&& r) noexcept : basic_object(r.lua_state(), r.stack_index()) {}
    basic_object(lua_State* L, int index = -1) noexcept : base_t(L, index) {}

    template<typename T>
    decltype(auto) as() const {
        return as_stack<T>( std::is_same<base_t, stack_reference>() );
    }

    template<typename T>
    bool is() const {
        if (!base_t::valid())
            return false;
        return is_stack<T>(std::is_same<base_t, stack_reference>());
    }
};

template <typename T>
object make_object(lua_State* L, T&& value) {
    int backpedal = stack::push(L, std::forward<T>(value));
    object r = stack::get<object>(L, -backpedal);
    lua_pop(L, backpedal);
    return r;
}

template <typename T, typename... Args>
object make_object(lua_State* L, Args&&... args) {
    int backpedal = stack::push<T>(L, std::forward<Args>(args)...);
    object r = stack::get<sol::object>(L, -backpedal);
    lua_pop(L, backpedal);
    return r;
}

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
