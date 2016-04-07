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

#ifndef SOL_FUNCTION_TYPES_CORE_HPP
#define SOL_FUNCTION_TYPES_CORE_HPP

#include "stack.hpp"
#include <memory>

namespace sol {
namespace function_detail {
template <typename T>
struct implicit_wrapper {
    T& item;
    implicit_wrapper(T* item) : item(*item) {}
    implicit_wrapper(T& item) : item(item) {}
    operator T& () {
        return item;
    }
    operator T* () {
        return std::addressof(item);
    }
};

inline decltype(auto) cleanup_key() {
    const auto& name = u8"sol.ƒ.♲.🗑.(/¯◡ ‿ ◡)/¯ ~ ┻━┻ (ﾉ◕ヮ◕)ﾉ*:･ﾟ✧";
    return name;
}

template<typename T, typename Func, typename = void>
struct functor {
    typedef meta::bind_traits<Func> traits_type;
    typedef typename traits_type::args_type args_type;
    typedef typename traits_type::return_type return_type;
    static const std::size_t arity = traits_type::arity;

    T* item;
    Func invocation;

    template<typename... Args>
    functor(Args&&... args): item(nullptr), invocation(std::forward<Args>(args)...) {}

    bool check () const {
         return invocation != nullptr;
    }

    template<typename... Args>
    void call(types<void>, Args&&... args) {
        T& member = *item;
        (member.*invocation)(std::forward<Args>(args)...);
    }

    template<typename Ret, typename... Args>
    Ret call(types<Ret>, Args&&... args) {
        T& member = *item;
        return (member.*invocation)(std::forward<Args>(args)...);
    }

    template<typename... Args>
    decltype(auto) operator()(Args&&... args) {
        return this->call(types<return_type>{}, std::forward<Args>(args)...);
    }
};

template<typename T, typename Func>
struct functor<T, Func, std::enable_if_t<std::is_member_object_pointer<Func>::value>> {
    typedef meta::bind_traits<Func> traits_type;
    typedef typename traits_type::args_type args_type;
    typedef typename traits_type::return_type return_type;
    static const std::size_t arity = traits_type::arity;
    T* item;
    Func invocation;

    template<typename... Args>
    functor(Args&&... args): item(nullptr), invocation(std::forward<Args>(args)...) {}

    bool check () const {
         return this->fx.invocation != nullptr;
    }

    template<typename Arg>
    void call(Arg&& arg) {
        T& member = *item;
        (member.*invocation) = std::forward<Arg>(arg);
    }

    return_type& call() {
         T& member = *item;
         return (member.*invocation);
    }

    template<typename... Args>
    decltype(auto) operator()(Args&&... args) {
        return this->call(std::forward<Args>(args)...);
    }
};

template<typename T, typename Func>
struct functor<T, Func, std::enable_if_t<std::is_function<Func>::value || std::is_class<Func>::value>> {
    typedef meta::bind_traits<Func> traits_type;
    typedef meta::pop_front_type_t<typename traits_type::args_type> args_type;
    typedef typename traits_type::return_type return_type;
    static const std::size_t arity = traits_type::arity;
    typedef meta::tuple_element_t<0, typename traits_type::args_tuple_type> Arg0;
    typedef std::conditional_t<std::is_pointer<Func>::value || std::is_class<Func>::value, Func, std::add_pointer_t<Func>> function_type;
    static_assert(std::is_base_of<meta::Unqualified<std::remove_pointer_t<Arg0>>, T>::value, "Any non-member-function must have a first argument which is covariant with the desired userdata type.");
    T* item;
    function_type invocation;

private:
    bool check(std::false_type) const {
        return true;
    }

    bool check(std::true_type) const {
        return this->invocation != nullptr;
    }

public:

    template<typename... Args>
    functor(Args&&... args): item(nullptr), invocation(std::forward<Args>(args)...) {}

    bool check () const {
         return this->check(std::is_function<Func>());
    }

    template<typename... Args>
    void call(types<void>, Args&&... args) {
        T& member = *item;
        invocation(implicit_wrapper<T>(member), std::forward<Args>(args)...);
    }

    template<typename Ret, typename... Args>
    Ret call(types<Ret>, Args&&... args) {
        T& member = *item;
        return invocation(implicit_wrapper<T>(member), std::forward<Args>(args)...);
    }

    template<typename... Args>
    auto operator()(Args&&... args) -> decltype(std::declval<functor>().call(types<return_type>{}, std::forward<Args>(args)...)) {
        return this->call(types<return_type>(), std::forward<Args>(args)...);
    }
};

struct base_function {
    virtual int operator()(lua_State* L) {
        return luaL_error(L, "sol: failure to call specialized wrapped C++ function from Lua");
    }

    virtual ~base_function() {}
};

static int base_call(lua_State* L, void* inheritancedata) {
    if (inheritancedata == nullptr) {
        return luaL_error(L, "sol: call from Lua to C++ function has null data");
    }

    base_function* pfx = static_cast<base_function*>(inheritancedata);
    base_function& fx = *pfx;
    return detail::trampoline(L, fx);
}

static int base_gc(lua_State* L, void* udata) {
    if (udata == nullptr) {
        return luaL_error(L, "sol: call from lua to C++ gc function with null data");
    }

    base_function* ptr = static_cast<base_function*>(udata);
    std::default_delete<base_function> dx{};
    dx(ptr);
    return 0;
}

template <std::size_t limit>
static void func_gc(std::true_type, lua_State*) {

}

template <std::size_t limit>
static void func_gc(std::false_type, lua_State* L) {
    for (std::size_t i = 0; i < limit; ++i) {
        void* value = stack::get<light_userdata_value>(L, up_value_index(static_cast<int>(i + 1)));
        if (value == nullptr)
            continue;
        base_function* obj = static_cast<base_function*>(value);
        std::allocator<base_function> alloc{};
        alloc.destroy(obj);
        alloc.deallocate(obj, 1);
    }
}

inline int call(lua_State* L) {
    void* ludata = stack::get<light_userdata_value>(L, up_value_index(1));
    void** pinheritancedata = static_cast<void**>(ludata);
    return base_call(L, *pinheritancedata);
}

inline int gc(lua_State* L) {
    void* udata = stack::get<userdata_value>(L, 1);
    void** pudata = static_cast<void**>(udata);
    return base_gc(L, *pudata);
}

template<std::size_t I>
inline int usertype_call(lua_State* L) {
    // Zero-based template parameter, but upvalues start at 1
    return base_call(L, stack::get<light_userdata_value>(L, up_value_index(static_cast<int>(I + 1))));
}

template<std::size_t I>
inline int usertype_gc(lua_State* L) {
    func_gc<I>(meta::Bool<(I < 1)>(), L);
    return 0;
}

inline void free_function_cleanup(lua_State* L) {
    const static char* metatablename = &cleanup_key()[0];
    int metapushed = luaL_newmetatable(L, metatablename);
    if (metapushed == 1) {
        stack::set_field(L, "__gc", function_detail::gc);
    }
}
} // function_detail
} // sol

#endif // SOL_FUNCTION_TYPES_CORE_HPP
