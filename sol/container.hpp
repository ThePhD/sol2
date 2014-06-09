#ifndef SOL_CONTAINER_HPP
#define SOL_CONTAINER_HPP

#include "userdata.hpp"

namespace sol {

template <typename Tc, typename = void>
struct container {
    typedef typename std::conditional<std::is_lvalue_reference<Tc>::value, Tc, Unqualified<Tc>>::type T;
    typedef Unqualified<decltype(*(std::declval<T>().begin()))> value_type;
    T cont;

    template <typename... Args>
    container (Args&&... args) : cont(std::forward<Args>(args)...){

    }

    operator T& () const {
        return cont;
    }

    void set(std::ptrdiff_t i, const value_type& value) {
        cont[ i ] = value;
    }

    value_type& get(std::ptrdiff_t i) {
        return cont[ i ];
    }

    std::size_t size() const {
        return cont.size();
    }

};

template <typename Tc>
struct container<Tc, typename std::enable_if<has_key_value_pair<Unqualified<Tc>>::value>::type> {
    typedef typename std::conditional<std::is_lvalue_reference<Tc>::value, Tc, Decay<Tc>>::type T;
    typedef Unqualified<decltype((*(std::declval<T>().begin())).first)> key_type;
    typedef Unqualified<decltype((*(std::declval<T>().begin())).second)> value_type;
    T cont;

    template <typename... Args>
    container (Args&&... args) : cont(std::forward<Args>(args)...){

    }

    operator T& () const {
        return cont;
    }

    void set(key_type i, const value_type& value) {
        cont[ i ] = value;
    }

    value_type& get(key_type i) {
        return cont.at(i);
    }

    std::size_t size() const {
        return cont.size();
    }
};

namespace stack {
template<typename T>
struct pusher<T, typename std::enable_if<has_begin_end<T>::value>::type> {
    static void push(lua_State* L, const T& cont) {
        typedef container<T> container_t;
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

        auto&& meta = userdata_traits<container_t>::metatable;
        if (luaL_newmetatable(L, std::addressof(meta[0])) == 1) {
            std::string err("metatable not defined for ");
            err += meta;
            throw error(err);
        }
        lua_setmetatable(L, -2);
    }
};

template<typename T>
struct getter<T, typename std::enable_if<has_begin_end<T>::value>::type> {
    static Unqualified<T>& get(lua_State* L, int index = -1) {
        typedef container<Unqualified<T>> container_t;
        container_t& data = stack::get<container_t>(L, index);
        return data.cont;
    }
};
} // stack
} // sol
#endif // SOL_CONTAINER_HPP
