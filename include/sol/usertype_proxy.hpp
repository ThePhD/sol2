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

#ifndef SOL_USERTYPE_PROXY_HPP
#define SOL_USERTYPE_PROXY_HPP

#include "traits.hpp"
#include "function.hpp"
#include "protected_function.hpp"
#include "proxy_base.hpp"

namespace sol {
	template <typename Table, typename Key>
	struct usertype_proxy : public proxy_base<usertype_proxy<Table, Key>> {
	public:
		Table tbl;
		Key key;

		template <typename T>
		usertype_proxy(Table table, T&& k)
		: tbl(table), key(std::forward<T>(k)) {
		}

		template <typename T>
		usertype_proxy& set(T&& value) {
			tbl.set(key, std::forward<T>(value));
			return *this;
		}

		template <typename U>
		usertype_proxy& operator=(U&& other) {
			return set(std::forward<U>(other));
		}

		template <typename T>
		usertype_proxy& operator=(std::initializer_list<T> other) {
			return set(std::move(other));
		}

		lua_State* lua_state() const {
			return tbl.lua_state();
		}
	};
} // namespace sol

#endif // SOL_USERTYPE_PROXY_HPP
