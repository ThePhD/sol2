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

#ifndef SOL_PROTECTED_FUNCTION_HPP
#define SOL_PROTECTED_FUNCTION_HPP

#include "reference.hpp"
#include "stack.hpp"
#include "protected_function_result.hpp"
#include <cstdint>

namespace sol {
class protected_function : public reference {
private:
    static reference& handler_storage() {
        static sol::reference h;
        return h;
    }

public:
    static const reference& get_default_handler () {
        return handler_storage();
    }

    static void set_default_handler( reference& ref ) {
        handler_storage() = ref;
    }

private:
    struct handler {
        const reference& target;
        int stackindex;
        handler(const reference& target) : target(target), stackindex(0) {
            if (target.valid()) {
                stackindex = lua_gettop(target.lua_state()) + 1;
                target.push();
            }
        }
        ~handler() {
            if (stackindex > 0) {
                lua_remove(target.lua_state(), stackindex);
            }
        }
    };

    int luacall(std::ptrdiff_t argcount, std::ptrdiff_t resultcount, handler& h) const {
        return lua_pcallk(lua_state(), static_cast<int>(argcount), static_cast<int>(resultcount), h.stackindex, 0, nullptr);
    }

    template<std::size_t... I, typename... Ret>
    auto invoke(types<Ret...>, std::index_sequence<I...>, std::ptrdiff_t n, handler& h) const {
        luacall(n, sizeof...(Ret), h);
        return stack::pop<std::tuple<Ret...>>(lua_state());
    }

    template<std::size_t I, typename Ret>
    Ret invoke(types<Ret>, std::index_sequence<I>, std::ptrdiff_t n, handler& h) const {
        luacall(n, 1, h);
        return stack::pop<Ret>(lua_state());
    }

    template <std::size_t I>
    void invoke(types<void>, std::index_sequence<I>, std::ptrdiff_t n, handler& h) const {
        luacall(n, 0, h);
    }

    protected_function_result invoke(types<>, std::index_sequence<>, std::ptrdiff_t n, handler& h) const {
        bool handlerpushed = error_handler.valid();
        int stacksize = lua_gettop(lua_state());
        int firstreturn = std::max(1, stacksize - static_cast<int>(n) - 1);
        int returncount = 0;
        call_status code = call_status::ok;
#ifndef SOL_NO_EXCEPTIONS
        auto onexcept = [&](const char* error) {
            h.stackindex = 0;
            if (h.target.valid()) {
                h.target.push();
                stack::push(lua_state(), error);
                lua_call(lua_state(), 1, 1);
            }
            else {
                stack::push(lua_state(), error);
            }
        };
        try {
#endif // No Exceptions
            code = static_cast<call_status>(luacall(n, LUA_MULTRET, h));
            int poststacksize = lua_gettop(lua_state());
            returncount = poststacksize - firstreturn;
#ifndef SOL_NO_EXCEPTIONS
        }
        // Handle C++ errors thrown from C++ functions bound inside of lua
        catch (const char* error) {
            onexcept(error);
            firstreturn = lua_gettop(lua_state());
            return protected_function_result(lua_state(), firstreturn, 0, 1, call_status::runtime);
        }
        catch (const std::exception& error) {
            onexcept(error.what());
            firstreturn = lua_gettop(lua_state());
            return protected_function_result(lua_state(), firstreturn, 0, 1, call_status::runtime);
        }
        catch (...) {
            onexcept("caught (...) unknown error during protected_function call");
            firstreturn = lua_gettop(lua_state());
            return protected_function_result(lua_state(), firstreturn, 0, 1, call_status::runtime);
        }
#endif // No Exceptions
        return protected_function_result(lua_state(), firstreturn + ( handlerpushed ? 0 : 1 ), returncount, returncount, code);
    }

public:
    reference error_handler;

    protected_function() = default;
    protected_function(lua_State* L, int index = -1): reference(L, index), error_handler(get_default_handler()) {
        type_assert(L, index, type::function);
    }
    protected_function(const protected_function&) = default;
    protected_function& operator=(const protected_function&) = default;
    protected_function( protected_function&& ) = default;
    protected_function& operator=( protected_function&& ) = default;

    template<typename... Args>
    protected_function_result operator()(Args&&... args) const {
        return call<>(std::forward<Args>(args)...);
    }

    template<typename... Ret, typename... Args>
    decltype(auto) operator()(types<Ret...>, Args&&... args) const {
        return call<Ret...>(std::forward<Args>(args)...);
    }

    template<typename... Ret, typename... Args>
    decltype(auto) call(Args&&... args) const {
        handler h(error_handler);
        push();
        int pushcount = stack::multi_push(lua_state(), std::forward<Args>(args)...);
        return invoke(types<Ret...>(), std::make_index_sequence<sizeof...(Ret)>(), pushcount, h);
    }
};
} // sol

#endif // SOL_FUNCTION_HPP
