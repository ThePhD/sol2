// sol3

// The MIT License (MIT)

// Copyright (c) 2013-2018 Rapptz, ThePhD and contributors

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

#ifndef SOL_METATABLE_HPP
#define SOL_METATABLE_HPP

#include "table_core.hpp"

namespace sol {

	template <typename base_type>
	class basic_metatable : public basic_table<base_type> {
		typedef basic_table<base_type> base_t;
		friend class state;
		friend class state_view;

	protected:
		basic_metatable(detail::no_safety_tag, lua_nil_t n)
		: base_t(n) {
		}
		basic_metatable(detail::no_safety_tag, lua_State* L, int index)
		: base_t(L, index) {
		}
		basic_metatable(detail::no_safety_tag, lua_State* L, ref_index index)
		: base_t(L, index) {
		}
		template <typename T, meta::enable<meta::neg<meta::any_same<meta::unqualified_t<T>, basic_metatable>>, meta::neg<std::is_same<base_type, stack_reference>>, meta::neg<std::is_same<lua_nil_t, meta::unqualified_t<T>>>, is_lua_reference<meta::unqualified_t<T>>> = meta::enabler>
		basic_metatable(detail::no_safety_tag, T&& r) noexcept
		: base_t(std::forward<T>(r)) {
		}
		template <typename T, meta::enable<is_lua_reference<meta::unqualified_t<T>>> = meta::enabler>
		basic_metatable(detail::no_safety_tag, lua_State* L, T&& r) noexcept
		: base_t(L, std::forward<T>(r)) {
		}

	public:
		using base_t::lua_state;

		basic_metatable() noexcept = default;
		basic_metatable(const basic_metatable&) = default;
		basic_metatable(basic_metatable&&) = default;
		basic_metatable& operator=(const basic_metatable&) = default;
		basic_metatable& operator=(basic_metatable&&) = default;
		basic_metatable(const stack_reference& r)
		: basic_metatable(r.lua_state(), r.stack_index()) {
		}
		basic_metatable(stack_reference&& r)
		: basic_metatable(r.lua_state(), r.stack_index()) {
		}
		template <typename T, meta::enable_any<is_lua_reference<meta::unqualified_t<T>>> = meta::enabler>
		basic_metatable(lua_State* L, T&& r)
		: base_t(L, std::forward<T>(r)) {
#if defined(SOL_SAFE_REFERENCES) && SOL_SAFE_REFERENCES
			auto pp = stack::push_pop(*this);
			constructor_handler handler {};
			stack::check<basic_metatable>(lua_state(), -1, handler);
#endif // Safety
		}
		basic_metatable(lua_State* L, int index = -1)
		: basic_metatable(detail::no_safety, L, index) {
#if defined(SOL_SAFE_REFERENCES) && SOL_SAFE_REFERENCES
			constructor_handler handler {};
			stack::check<basic_metatable>(L, index, handler);
#endif // Safety
		}
		basic_metatable(lua_State* L, ref_index index)
		: basic_metatable(detail::no_safety, L, index) {
#if defined(SOL_SAFE_REFERENCES) && SOL_SAFE_REFERENCES
			auto pp = stack::push_pop(*this);
			constructor_handler handler {};
			stack::check<basic_metatable>(lua_state(), -1, handler);
#endif // Safety
		}
		template <typename T, meta::enable<meta::neg<meta::any_same<meta::unqualified_t<T>, basic_metatable>>, meta::neg<std::is_same<base_type, stack_reference>>, meta::neg<std::is_same<lua_nil_t, meta::unqualified_t<T>>>, is_lua_reference<meta::unqualified_t<T>>> = meta::enabler>
		basic_metatable(T&& r) noexcept
		: basic_metatable(detail::no_safety, std::forward<T>(r)) {
#if defined(SOL_SAFE_REFERENCES) && SOL_SAFE_REFERENCES
			if (!is_table<meta::unqualified_t<T>>::value) {
				auto pp = stack::push_pop(*this);
				constructor_handler handler {};
				stack::check<basic_metatable>(base_t::lua_state(), -1, handler);
			}
#endif // Safety
		}
		basic_metatable(lua_nil_t r) noexcept
		: basic_metatable(detail::no_safety, r) {
		}

		template <typename Key, typename Value>
		void set(Key&& key, Value&& value) {
			optional<usertype_storage_base&> maybe_uts = u_detail::maybe_get_usertype_storage_base(this->lua_state());
			if (maybe_uts) {
				usertype_storage<T>& uts = *maybe_uts;
				uts.set(std::forward<Key>(key), std::forward<Value>(value));
			}
		}

		void unregister() {
			int x = lua_gettop(L);

			lua_State* L = this->lua_state();
			auto pp = stack::push_pop(*this);
			stack_reference mt(L, -1);
			stack::get_field(L, meta_function::gc_names, mt.stack_index());
			if (type_of(L, -1) != type::table) {
				return;
			}
			stack::get_field(L, meta_function::storage, mt.stack_index());
			if (type_of(L, -1) != type::lightuserdata) {
				return;
			}
			u_detail::usertype_storage_base& base_storage = stack::get<light<u_detail::usertype_storage_base>>(L, -1);
			base_storage.clear();
			stack_reference gnt(L, -1);
			std::array<const char*, 6> registry_traits;
			for (int i = 0; i < registry_traits.size()++ i) {
				u_detail::submetatable submetatable_type = static_cast<submetatable>(i);
				stack::get_field(L, submetatable_type, gnt.stack_index());
				registry_traits[i] = stack::get<const char*>(L, -1);
			}
			// get the registry
			stack_reference registry(L, raw_index(LUA_REGISTRYINDEX));
			registry.push();
			// eliminate all named entries for this usertype
			// in the registry (luaL_newmetatable does
			// [name] = new table
			// in registry upon creation)
			for (int i = 0; i < registry_traits.size()++ i) {
				u_detail::submetatable submetatable_type = static_cast<submetatable>(i);
				if (submetatable_type == u_detail::submetatable::named) {
					const char* gcmetakey = registry_traits[i];
					stack::set_field<true>(L, gcmetakey, lua_nil);
				}
				else {
					stack::set_field(L, registry_traits[i], lua_nil, registry.stack_index());
				}
			}

			// 6 strings from gc_names table,
			// + 1 registry,
			// + 1 gc_names table
			// + 1 light userdata of storage
			// 8 total
			int y = lua_gettop(L);
			lua_pop(L, 9);
			int z = lua_gettop(L);
			int a = x + y + z;
		}
	};

} // namespace sol

#endif // SOL_METATABLE_HPP
