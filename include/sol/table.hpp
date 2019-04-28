// sol3

// The MIT License (MIT)

// Copyright (c) 2013-2019 Rapptz, ThePhD and contributors

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

#include "table_core.hpp"
#include "lua_table.hpp"
#include "usertype.hpp"

namespace sol {
	typedef table_core<false> table;

	template <bool is_global, typename base_type>
	template <typename Class, typename Key>
	usertype<Class> basic_table_core<is_global, base_type>::new_usertype(Key&& key) {
		automagic_enrollments enrollments;
		return this->new_usertype<Class>(std::forward<Key>(key), std::move(enrollments));
	}

	template <bool is_global, typename base_type>
	template <typename Class, typename Key>
	usertype<Class> basic_table_core<is_global, base_type>::new_usertype(Key&& key, automagic_enrollments enrollments) {
		int mt_index = u_detail::register_usertype<Class>(this->lua_state(), std::move(enrollments));
		usertype<Class> mt(this->lua_state(), -mt_index);
		lua_pop(this->lua_state(), 1);
		set(std::forward<Key>(key), mt);
		return mt;
	}

	template <bool is_global, typename base_type>
	template <typename Class, typename Key, typename Arg, typename... Args, typename>
	usertype<Class> basic_table_core<is_global, base_type>::new_usertype(Key&& key, Arg&& arg, Args&&... args) {
		automagic_enrollments enrollments;
		enrollments.default_constructor = !detail::any_is_constructor_v<Arg, Args...>;
		enrollments.destructor = !detail::any_is_destructor_v<Arg, Args...>;
		usertype<Class> ut = this->new_usertype<Class>(std::forward<Key>(key), std::move(enrollments));
		static_assert(sizeof...(Args) % 2 == static_cast<std::size_t>(!detail::any_is_constructor_v<Arg>), 
			"you must pass an even number of arguments to new_usertype after first passing a constructor");
		if constexpr (detail::any_is_constructor_v<Arg>) {
			ut.set(meta_function::construct, std::forward<Arg>(arg));
			ut.tuple_set(std::make_index_sequence<(sizeof...(Args)) / 2>(), std::forward_as_tuple(std::forward<Args>(args)...));
		}
		else {
			ut.tuple_set(std::make_index_sequence<(sizeof...(Args) + 1) / 2>(), std::forward_as_tuple(std::forward<Arg>(arg), std::forward<Args>(args)...));
		}
		return ut;
	}

	template <typename base_type>
	template <typename Key, typename Value>
	void basic_metatable<base_type>::set(Key&& key, Value&& value) {
		this->push();
		lua_State* L = this->lua_state();
		int target = lua_gettop(L);
		optional<u_detail::usertype_storage_base&> maybe_uts = u_detail::maybe_get_usertype_storage_base(L, target);
		lua_pop(L, 1);
		if (maybe_uts) {
			u_detail::usertype_storage_base& uts = *maybe_uts;
			uts.set(L, std::forward<Key>(key), std::forward<Value>(value));
		}
		else {
			base_t::set(std::forward<Key>(key), std::forward<Value>(value));
		}
	}

	namespace stack {
		template <>
		struct unqualified_getter<metatable_key_t> {
			static table get(lua_State* L, int index = -1) {
				if (lua_getmetatable(L, index) == 0) {
					return table(L, ref_index(LUA_REFNIL));
				}
				return table(L, -1);
			}
		};
	} // namespace stack
} // namespace sol

#endif // SOL_TABLE_HPP
