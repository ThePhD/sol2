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

#ifndef SOL_USERDATA_HPP
#define SOL_USERDATA_HPP

#include "state.hpp"
#include "function_types.hpp"
#include "demangle.hpp"
#include <vector>

namespace sol {
namespace detail {
template<typename T, typename... Args>
inline std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
} // detail

template<typename T>
class userdata {
private:
    friend table;
    static const std::string classname;
    static const std::string meta;

    std::string luaname;
    std::vector<std::string> functionnames;
    std::vector<std::unique_ptr<base_function>> functions;
    std::vector<luaL_Reg> functiontable;

    template<typename... TTypes>
    struct constructor {
        template<typename... Args>
        static void do_constructor(lua_State* L, T* obj, call_syntax syntax, int, types<Args...>) {
            auto fx = [&obj] (Args&&... args) -> void {
                std::allocator<T> alloc{};
                alloc.construct(obj, std::forward<Args>(args)...);
            };
            stack::get_call(L, 1 + static_cast<int>(syntax), fx, types<Args...>());
       }

        static void match_constructor(lua_State*, T*, call_syntax, int) {
            throw sol_error("No matching constructor for the arguments provided");
        }

        template<typename ...CArgs, typename... Args>
        static void match_constructor(lua_State* L, T* obj, call_syntax syntax, int argcount, types<CArgs...> t, Args&&... args) {
            if (argcount == sizeof...(CArgs)) {
                do_constructor(L, obj, syntax, argcount, t);
                return;
            }
            match_constructor(L, obj, syntax, argcount, std::forward<Args>(args)...);
        }

        static int construct(lua_State* L) {
            call_syntax syntax = stack::get_call_syntax(L, meta);
            int argcount = lua_gettop(L);

            void* udata = lua_newuserdata(L, sizeof(T));
            T* obj = static_cast<T*>(udata);
            match_constructor(L, obj, syntax, argcount - static_cast<int>(syntax), typename std::common_type<TTypes>::type()...);

            luaL_getmetatable(L, meta.c_str());
            lua_setmetatable(L, -2);

            return 1;
        }
    };

    template<std::size_t n>
    struct destructor {
        static int destruct(lua_State* L) {
            userdata_t udata = stack::get<userdata_t>(L, 1);
            T* obj = static_cast<T*>(udata.value);
            std::allocator<T> alloc{};
            alloc.destroy(obj);
            return 0;
        }
    };

    template<std::size_t n>
    void build_function_tables() {}

    template<std::size_t n, typename... Args, typename Ret, typename... MArgs>
    void build_function_tables(Ret(T::* func)(MArgs...), std::string name, Args&&... args) {
        typedef typename std::decay<decltype(func)>::type fx_t;
        functionnames.push_back(std::move(name));
        functions.emplace_back(detail::make_unique<userdata_function<fx_t, T>>(std::move(func)));
        functiontable.push_back({ functionnames.back().c_str(), &base_function::userdata<n>::call });
        build_function_tables<n + 1>(std::forward<Args>(args)...);
    }

public:
    template<typename... Args>
    userdata(Args&&... args) : userdata(classname, default_constructor, std::forward<Args>(args)...) {}

    template<typename... Args, typename... CArgs>
    userdata(constructors<CArgs...> c, Args&&... args) : userdata(classname, std::move(c), std::forward<Args>(args)...) {}

    template<typename... Args, typename... CArgs>
    userdata(std::string name, constructors<CArgs...>, Args&&... args) : luaname(std::move(name)) {
        functionnames.reserve(sizeof...(args) + 2);
        functiontable.reserve(sizeof...(args) + 3);
        functions.reserve(sizeof...(args) + 2);
        build_function_tables<0>(std::forward<Args>(args)...);

        functionnames.push_back("new");
        functiontable.push_back({ functionnames.back().c_str(), &constructor<CArgs...>::construct });
        functionnames.push_back("__gc");
        functiontable.push_back({ functionnames.back().c_str(), &destructor<sizeof...(Args) / 2>::destruct });

        functiontable.push_back({ nullptr, nullptr });
    }

    void register_into(const table& s) {}
};

template<typename T>
const std::string userdata<T>::classname = detail::demangle(typeid(T));

template<typename T>
const std::string userdata<T>::meta = std::string("sol.stateful.").append(classname);

}

#endif // SOL_USERDATA_HPP
