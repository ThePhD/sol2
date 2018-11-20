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

#ifndef SOL_USERTYPE_CORE_HPP
#define SOL_USERTYPE_CORE_HPP

#include "wrapper.hpp"
#include "stack.hpp"
#include "types.hpp"
#include "stack_reference.hpp"
#include "usertype_traits.hpp"
#include "inheritance.hpp"
#include "raii.hpp"
#include "deprecate.hpp"
#include "object.hpp"
#include "usertype_container.hpp"

#include <sstream>
#include <type_traits>

namespace sol {
	namespace u_detail {
		constexpr const lua_Integer toplevel_magic = static_cast<lua_Integer>(0xCCC2CCC1);

		constexpr const int environment_index = 1;
		constexpr const int usertype_storage_index = 2;
		constexpr const int usertype_storage_base_index = 3;
		constexpr const int exact_function_index = 4;
		constexpr const int magic_index = 5;

		constexpr const int simple_usertype_storage_index = 2;
		constexpr const int index_function_index = 3;
		constexpr const int new_index_function_index = 4;

		constexpr const int base_walking_failed_index = -32467;
		constexpr const int lookup_failed_index = -42469;

		enum class submetatable {
			// must be sequential
			value,
			reference,
			unique,
			const_reference,
			const_value,
			// must be LAST!
			named
		};

		inline auto make_string_view(string_view s) {
			return s;
		}

		inline auto make_string_view(call_construction) {
			return string_view(to_string(meta_function::call_function));
		}

		inline auto make_string_view(meta_function mf) {
			return string_view(to_string(mf));
		}

		inline auto make_string_view(base_classes_tag) {
			return string_view(detail::base_class_cast_key());
		}

		template <typename Arg>
		inline std::string make_string(Arg&& arg) {
			string_view s = make_string_view(arg);
			return std::string(s.data(), s.size());
		}

		inline int is_indexer(string_view s) {
			if (s == to_string(meta_function::index)) {
				return 1;
			}
			else if (s == to_string(meta_function::new_index)) {
				return 2;
			}
			return 0;
		}

		inline int is_indexer(meta_function mf) {
			if (mf == meta_function::index) {
				return 1;
			}
			else if (mf == meta_function::new_index) {
				return 2;
			}
			return 0;
		}

		inline int is_indexer(call_construction) {
			return 0;
		}
	} // namespace u_detail

	namespace stack { namespace stack_detail {
		template <typename T>
		struct undefined_metatable {
			typedef meta::all<meta::neg<std::is_pointer<T>>, std::is_destructible<T>> is_destructible;
			typedef std::remove_pointer_t<T> P;
			lua_State* L;
			const char* key;

			undefined_metatable(lua_State* l, const char* k)
			: L(l), key(k) {
			}

			void operator()() const {
				if (luaL_newmetatable(L, key) == 1) {
					detail::lua_reg_table l{};
					int index = 0;
					detail::indexed_insert insert_fx(l, index);
					detail::insert_default_registrations<P>(insert_fx, detail::property_always_true);
					if constexpr (!std::is_pointer_v<T>) {
						l[index] = luaL_Reg{ to_string(meta_function::garbage_collect).c_str(), detail::make_destructor<P>() };
					}
					luaL_setfuncs(L, l, 0);

					// __type table
					lua_createtable(L, 0, 2);
					const std::string& name = detail::demangle<T>();
					lua_pushlstring(L, name.c_str(), name.size());
					lua_setfield(L, -2, "name");
					lua_CFunction is_func = &detail::is_check<T>;
					lua_pushcclosure(L, is_func, 0);
					lua_setfield(L, -2, "is");
					lua_setfield(L, -2, to_string(meta_function::type).c_str());
				}
				lua_setmetatable(L, -2);
			}
		};
	}
	} // namespace stack::stack_detail
} // namespace sol

#endif // SOL_USERTYPE_CORE_HPP
