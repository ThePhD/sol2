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

#ifndef SOL_USERTYPE_HPP
#define SOL_USERTYPE_HPP

#include "usertype_core.hpp"
#include "usertype_container_launch.hpp"
#include "usertype_storage.hpp"
#include "usertype_proxy.hpp"
#include "metatable.hpp"

namespace sol {

	template <typename T, typename base_type>
	class basic_usertype : private basic_metatable<base_type> {
	private:
		using base_t = basic_metatable<base_type>;

		template <typename>
		friend class basic_metatable;

		template <bool, typename>
		friend class basic_table_core;

		template <std::size_t... I, typename... Args>
		void tuple_set(std::index_sequence<I...>, std::tuple<Args...>&& args) {
			using args_tuple = std::tuple<Args...>&&;
			optional<u_detail::usertype_storage<T>&> maybe_uts = u_detail::maybe_get_usertype_storage<T>(this->lua_state());
			if constexpr (sizeof...(I) > 0) {
				if (maybe_uts) {
					u_detail::usertype_storage<T>& uts = *maybe_uts;
					(void)detail::swallow{ 0,
						(uts.set(this->lua_state(), std::get<I * 2>(std::forward<args_tuple>(args)), std::get<I * 2 + 1>(std::forward<args_tuple>(args))),
						     0)... };
				}
			}
			else {
				(void)args;
			}
		}

	public:
		using base_t::base_t;

		using base_t::pop;
		using base_t::push;
		using base_t::lua_state;
		using base_t::get;
		using base_t::unregister;

		template <typename Key, typename Value>
		void set(Key&& key, Value&& value) {
			optional<u_detail::usertype_storage<T>&> maybe_uts = u_detail::maybe_get_usertype_storage<T>(this->lua_state());
			if (maybe_uts) {
				u_detail::usertype_storage<T>& uts = *maybe_uts;
				uts.set(this->lua_state(), std::forward<Key>(key), std::forward<Value>(value));
			}
		}

		template <typename Key>
		usertype_proxy<basic_usertype&, std::decay_t<Key>> operator[](Key&& key) {
			return usertype_proxy<basic_usertype&, std::decay_t<Key>>(*this, std::forward<Key>(key));
		}

		template <typename Key>
		usertype_proxy<const basic_usertype&, std::decay_t<Key>> operator[](Key&& key) const {
			return usertype_proxy<const basic_usertype&, std::decay_t<Key>>(*this, std::forward<Key>(key));
		}
	};

} // namespace sol

#endif // SOL_USERTYPE_HPP
