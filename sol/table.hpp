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

#ifndef SOL_TABLE_HPP
#define SOL_TABLE_HPP

#include "stack.hpp"
#include "lua_function.hpp"
#include <unordered_map>
#include <memory>

namespace sol {
class table : virtual public reference {
private:
     std::unordered_map<std::string, std::shared_ptr<detail::lua_func>> funcs;
public:
    table() noexcept: reference{} {}
    table(lua_State* L, int index = -1): reference(L, index) {
        type_assert(L, index, type::table);
    }

    template<typename T, typename U>
    T get(U&& key) const {
        push();
        stack::push(state(), std::forward<U>(key));
        lua_gettable(state(), -2);
        type_assert(state(), -1, type_of<T>());
        auto result = stack::pop<T>(state());
        lua_pop(state(), 1);
        return result;
    }

    template<typename T, typename U>
    table& set(T&& key, U&& value) {
        push();
        stack::push(state(), std::forward<T>(key));
        stack::push(state(), std::forward<U>(value));
        lua_settable(state(), -3);
        lua_pop(state(), 1);
        return *this;
    }

    template<typename T, typename TFx>
    table& set_function(T&& key, TFx&& fx) {
        typedef typename std::remove_pointer<typename std::decay<TFx>::type>::type clean_fx;
        const static bool isfunction = std::is_function<clean_fx>::value;
        return set_fx(std::integral_constant<bool, !isfunction>(), std::forward<T>(key), std::forward<TFx>(fx));
    }

    template<typename T, typename TFx, typename TM>
    table& set_function(T&& key, TFx&& fx, TM& mem) {
        typedef typename std::remove_pointer<typename std::decay<TFx>::type>::type clean_fx;
        std::unique_ptr<detail::lua_func> sptr(new detail::explicit_lua_func<clean_fx, TM>(mem, std::forward<TFx>(fx)));
        return set_fx(std::forward<T>(key), std::move(sptr));
    }

    size_t size() const {
        push();
        return lua_rawlen(state(), -1);
    }
private:
    template<typename T, typename TFx>
    table& set_fx(std::true_type, T&& key, TFx&& fx) {
        typedef typename std::remove_pointer<typename std::decay<TFx>::type>::type clean_fx;
        std::unique_ptr<detail::lua_func> sptr(new detail::lambda_lua_func<clean_fx>(std::forward<TFx>(fx)));
        return set_fx(std::forward<T>(key), std::move(sptr));
    }

    template<typename T, typename TFx>
    table& set_fx(std::false_type, T&& key, TFx&& fx) {
        typedef typename std::decay<TFx>::type ptr_fx;
        std::unique_ptr<detail::lua_func> sptr(new detail::explicit_lua_func<ptr_fx>(std::forward<TFx>(fx)));
        return set_fx(std::forward<T>(key), std::move(sptr));
    }

    template<typename T>
    table& set_fx(T&& key, std::unique_ptr<detail::lua_func> funcptr) {
        std::string fkey(key);
        auto hint = funcs.find(fkey);
        if (hint == funcs.end()) {
            std::shared_ptr<detail::lua_func> sptr(funcptr.release());
            hint = funcs.emplace_hint(hint, fkey, std::move(sptr));
        }
        else {
            hint->second.reset(funcptr.release());
        }
        detail::lua_func* target = hint->second.get();
        lua_CFunction freefunc = &detail::lua_cfun;
        lua_pushlightuserdata(state(), static_cast<void*>(target));
        lua_pushcclosure(state(), freefunc, 1);
        lua_setglobal(state(), fkey.c_str());
        return *this;
    }
};
} // sol

#endif // SOL_TABLE_HPP