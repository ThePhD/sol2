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

#ifndef SOL_FUNCTION_TYPES_CORE_HPP
#define SOL_FUNCTION_TYPES_CORE_HPP

#include "stack.hpp"
#include <memory>

namespace sol {
namespace detail {
struct ref_call_t {} const ref_call = ref_call_t{};
template <typename T>
struct implicit_wrapper {
    T& item;
    implicit_wrapper(T& item) : item(item) {}
    operator T& () {
        return item;
    }
    operator T* () {
        return std::addressof(item);
    }
};

template <typename Sig, typename... Args>
struct function_packer : std::tuple<Args...> { using std::tuple<Args...>::tuple; };

template <typename Sig, typename... Args>
function_packer<Sig, Args...> function_pack( Args&&... args ) { 
    return function_packer<Sig, Args...>(std::forward<Args>(args)...);
}

inline bool check_types(types<>, std::index_sequence<>, lua_State*, int) {
	return true;
}

template <typename Arg, typename... Args, std::size_t I, std::size_t... In>
inline bool check_types(types<Arg, Args...>, std::index_sequence<I, In...>, lua_State* L, int start = 1) {
    if (!stack::check<Arg>(L, start + I, no_panic))
	    return false;
    
    return check_types(types<Args...>(), std::index_sequence<In...>(), L, start);
}

template<typename T, typename Func, typename = void>
struct functor {
    typedef callable_traits<Func> traits_type;
    typedef typename traits_type::args_type args_type;
    typedef typename traits_type::return_type return_type;

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
    typedef callable_traits<Func> traits_type;
    typedef typename traits_type::args_type args_type;
    typedef typename traits_type::return_type return_type;
    T* item;
    Func invocation;

    template<typename... Args>
    functor(Args&&... args): item(nullptr), invocation(std::forward<Args>(args)...) {}

    bool check () const {
         return this->fx.invocation != nullptr;
    }

    template<typename Arg>
    void call(types<return_type>, Arg&& arg) {
        T& member = *item;
        (member.*invocation) = std::forward<Arg>(arg);
    }

    return_type call(types<return_type>) {
         T& member = *item;
         return (member.*invocation);
    }

    template<typename... Args>
    auto operator()(Args&&... args) -> decltype(std::declval<functor>().call(types<return_type>{}, std::forward<Args>(args)...)) {
        return this->call(types<return_type>{}, std::forward<Args>(args)...);
    }
};

template<typename T, typename Func>
struct functor<T, Func, std::enable_if_t<std::is_function<Func>::value || std::is_class<Func>::value>> {
    typedef callable_traits<Func> traits_type;
    typedef remove_one_type<typename traits_type::args_type> args_type;
    typedef typename traits_type::return_type return_type;
    typedef std::tuple_element_t<0, typename traits_type::args_tuple_type> Arg0;
    typedef std::conditional_t<std::is_pointer<Func>::value || std::is_class<Func>::value, Func, std::add_pointer_t<Func>> function_type;
    static_assert(std::is_base_of<Unqualified<std::remove_pointer_t<Arg0>>, T>::value, "Any non-member-function must have a first argument which is covariant with the desired userdata type.");
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
} // detail

struct base_function {
    static int base_call(lua_State* L, void* inheritancedata) {
        if(inheritancedata == nullptr) {
            throw error("call from Lua to C++ function has null data");
        }

        base_function* pfx = static_cast<base_function*>(inheritancedata);
        base_function& fx = *pfx;
        int r = fx(L);
        return r;
    }

    static int base_gc(lua_State*, void* udata) {
        if(udata == nullptr) {
            throw error("call from lua to C++ gc function with null data");
        }

        base_function* ptr = static_cast<base_function*>(udata);
        std::default_delete<base_function> dx{};
        dx(ptr);
        return 0;
    }

    static int call(lua_State* L) {
        void** pinheritancedata = static_cast<void**>(stack::get<upvalue>(L, 1).value);
        return base_call(L, *pinheritancedata);
    }

    static int gc(lua_State* L) {
        void** pudata = static_cast<void**>(stack::get<userdata>(L, 1).value);
        return base_gc(L, *pudata);
    }

    template<std::size_t I>
    struct usertype {
        static int call(lua_State* L) {
            // Zero-based template parameter, but upvalues start at 1
            return base_call(L, stack::get<upvalue>(L, I + 1));
        }

        template <std::size_t limit>
        static void func_gc (std::true_type, lua_State*) {

        }

        template <std::size_t limit>
        static void func_gc (std::false_type, lua_State* L) {
            // Shut up clang tautological error without throwing out std::size_t
            for(std::size_t i = 0; i < limit; ++i) {
                upvalue up = stack::get<upvalue>(L, static_cast<int>(i + 1));
                base_function* obj = static_cast<base_function*>(up.value);
                std::allocator<base_function> alloc{};
                alloc.destroy(obj);
                alloc.deallocate(obj, 1);
            }
        }

        static int gc(lua_State* L) {
            func_gc<I>(Bool<(I < 1)>(), L);
            return 0;
        }
    };

    virtual int operator()(lua_State*) {
        throw error("failure to call specialized wrapped C++ function from Lua");
    }

    virtual ~base_function() {}
};

} // sol

#endif // SOL_FUNCTION_TYPES_CORE_HPP
