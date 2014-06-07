#ifndef SOL_CONTAINER_HPP
#define SOL_CONTAINER_HPP

#include "userdata.hpp"

namespace sol {

template <typename Tc>
struct container {
    typedef typename std::conditional<std::is_lvalue_reference<Tc>::value, Tc, Decay<Tc>>::type T;
    typedef Decay<decltype(*(std::declval<T>().begin()))> value_type;
    T cont;

    template <typename... Args>
    container (Args&&... args) : cont(std::forward<Args>(args)...){

    }

    operator T& () const {
        return cont;
    }

    void set( std::ptrdiff_t i, const value_type& value ) {
        cont[ i ] = value;
    }

    value_type& get( std::ptrdiff_t i ) {
        return cont[ i ];
    }

    std::size_t size( ) const {
        return cont.size();
    }

};

namespace stack {
template<typename T>
struct pusher<T, typename std::enable_if<has_begin_end<T>::value>::type> {
    template<typename U = T, EnableIf<Not<has_key_value_pair<U>>> = 0>
    static void push(lua_State* L, const T& cont) {
        typedef container<U> container_t;
        // todo: NEED to find a different way of handling this...
        static std::vector<std::shared_ptr<void>> classes{};
        userdata<container_t> classdata(default_constructor,
                                   "__index", &container_t::get,
                                   "__newindex", &container_t::set,
                                   "__len", &container_t::size);
        classes.emplace_back(std::make_shared<userdata<container_t>>(std::move(classdata)));
        auto&& ptr = classes.back();
        auto udata = std::static_pointer_cast<userdata<container_t>>(ptr);
        stack::push(L, *udata);

        container_t* c = static_cast<container_t*>(lua_newuserdata(L, sizeof(container_t)));
        std::allocator<container_t> alloc{};
        alloc.construct(c, cont);

        auto&& meta = userdata_traits<T>::metatable;
        luaL_getmetatable(L, std::addressof(meta[0]));
        lua_setmetatable(L, -2);
    }

    template<typename U = T, EnableIf<has_key_value_pair<U>> = 0>
    static void push(lua_State* L, const T& cont) {
        lua_createtable(L, cont.size(), 0);
        for(auto&& pair : cont) {
            pusher<Unqualified<decltype(pair.first)>>::push(L, pair.first);
            pusher<Unqualified<decltype(pair.second)>>::push(L, pair.second);
            lua_settable(L, -3);
        }
    }
};
} // stack
} // sol
#endif // SOL_CONTAINER_HPP
