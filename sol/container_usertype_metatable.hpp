// The MIT License (MIT)

// Copyright (c) 2013-2017 Rapptz, ThePhD and contributors

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

#ifndef SOL_CONTAINER_USERTYPE_METATABLE_HPP
#define SOL_CONTAINER_USERTYPE_METATABLE_HPP

#include "stack.hpp"
#include "container_traits.hpp"
#include <unordered_map>

namespace sol {
	
	template <typename X, typename C = void>
	struct container_usertype_metatable {
		typedef std::remove_pointer_t<meta::unqualified_t<X>> T;
		typedef container_traits<T> traits;
		typedef container_detail::container_traits_default<T> default_traits;


		static int real_index_get_traits(std::true_type, lua_State* L) {
			return traits::index_get(L);
		}

		static int real_index_get_traits(std::false_type, lua_State* L) {
			return default_traits::index_get(L);
		}

		static int real_index_call(lua_State* L) {
			static std::unordered_map<std::string, lua_CFunction> calls{
				{ "get", &real_get_call },
				{ "set", &real_set_call },
				{ "add", &real_add_call },
				{ "insert", &real_insert_call },
				{ "clear", &real_clear_call },
				{ "find", &real_find_call },
				{ "erase", &real_erase_call }
			};
			auto maybename = stack::check_get<std::string>(L, 2);
			if (maybename) {
				const std::string& name = *maybename;
				auto it = calls.find(name);
				if (it != calls.cend()) {
					return stack::push(L, it->second);
				}
			}			
			return real_index_get_traits(container_detail::has_traits_index_get<traits>(), L);
		}

		static int real_get_traits(std::true_type, lua_State* L) {
			return traits::get(L);
		}

		static int real_get_traits(std::false_type, lua_State* L) {
			return default_traits::get(L);
		}

		static int real_get_call(lua_State* L) {
			return real_get_traits(container_detail::has_traits_get<traits>(), L);
		}

		static int real_set_traits(std::true_type, lua_State* L) {
			return traits::set(L);
		}

		static int real_set_traits(std::false_type, lua_State* L) {
			return default_traits::set(L);
		}

		static int real_set_call(lua_State* L) {
			return real_set_traits(container_detail::has_traits_set<traits>(), L);
		}
		
		static int real_index_set_traits(std::true_type, lua_State* L) {
			return traits::index_set(L);
		}

		static int real_index_set_traits(std::false_type, lua_State* L) {
			return default_traits::index_set(L);
		}

		static int real_new_index_call(lua_State* L) {
			return real_index_set_traits(container_detail::has_traits_index_set<traits>(), L);
		}

		static int real_pairs_traits(std::true_type, lua_State* L) {
			return traits::pairs(L);
		}

		static int real_pairs_traits(std::false_type, lua_State* L) {
			return default_traits::pairs(L);
		}

		static int real_pairs_call(lua_State* L) {
			return real_pairs_traits(container_detail::has_traits_pairs<traits>(), L);
		}

		static int real_size_traits(std::true_type, lua_State* L) {
			return traits::size(L);
		}

		static int real_size_traits(std::false_type, lua_State* L) {
			return default_traits::size(L);
		}

		static int real_length_call(lua_State* L) {
			return real_size_traits(container_detail::has_traits_size<traits>(), L);
		}

		static int real_add_traits(std::true_type, lua_State* L) {
			return traits::add(L);
		}

		static int real_add_traits(std::false_type, lua_State* L) {
			return default_traits::add(L);
		}

		static int real_add_call(lua_State* L) {
			return real_add_traits(container_detail::has_traits_add<traits>(), L);
		}

		static int real_insert_traits(std::true_type, lua_State* L) {
			return traits::insert(L);
		}

		static int real_insert_traits(std::false_type, lua_State* L) {
			return default_traits::insert(L);
		}

		static int real_insert_call(lua_State* L) {
			return real_insert_traits(container_detail::has_traits_insert<traits>(), L);
		}

		static int real_clear_traits(std::true_type, lua_State* L) {
			return traits::clear(L);
		}

		static int real_clear_traits(std::false_type, lua_State* L) {
			return default_traits::clear(L);
		}

		static int real_clear_call(lua_State* L) {
			return real_clear_traits(container_detail::has_traits_clear<traits>(), L);
		}

		static int real_erase_traits(std::true_type, lua_State* L) {
			return traits::erase(L);
		}

		static int real_erase_traits(std::false_type, lua_State* L) {
			return default_traits::erase(L);
		}

		static int real_erase_call(lua_State* L) {
			return real_erase_traits(container_detail::has_traits_erase<traits>(), L);
		}

		static int real_find_traits(std::true_type, lua_State* L) {
			return traits::find(L);
		}

		static int real_find_traits(std::false_type, lua_State* L) {
			return default_traits::find(L);
		}

		static int real_find_call(lua_State* L) {
			return real_find_traits(container_detail::has_traits_find<traits>(), L);
		}

		static int add_call(lua_State*L) {
			return detail::typed_static_trampoline<decltype(&real_add_call), (&real_add_call)>(L);
		}

		static int erase_call(lua_State*L) {
			return detail::typed_static_trampoline<decltype(&real_erase_call), (&real_erase_call)>(L);
		}

		static int insert_call(lua_State*L) {
			return detail::typed_static_trampoline<decltype(&real_insert_call), (&real_insert_call)>(L);
		}

		static int clear_call(lua_State*L) {
			return detail::typed_static_trampoline<decltype(&real_clear_call), (&real_clear_call)>(L);
		}

		static int find_call(lua_State*L) {
			return detail::typed_static_trampoline<decltype(&real_find_call), (&real_find_call)>(L);
		}

		static int length_call(lua_State*L) {
			return detail::typed_static_trampoline<decltype(&real_length_call), (&real_length_call)>(L);
		}

		static int pairs_call(lua_State*L) {
			return detail::typed_static_trampoline<decltype(&real_pairs_call), (&real_pairs_call)>(L);
		}

		static int get_call(lua_State*L) {
			return detail::typed_static_trampoline<decltype(&real_get_call), (&real_get_call)>(L);
		}

		static int set_call(lua_State*L) {
			return detail::typed_static_trampoline<decltype(&real_set_call), (&real_set_call)>(L);
		}

		static int index_call(lua_State*L) {
			return detail::typed_static_trampoline<decltype(&real_index_call), (&real_index_call)>(L);
		}

		static int new_index_call(lua_State*L) {
			return detail::typed_static_trampoline<decltype(&real_new_index_call), (&real_new_index_call)>(L);
		}
	};

	namespace stack {
		namespace stack_detail {
			template <typename T>
			struct metatable_setup {
				lua_State* L;

				metatable_setup(lua_State* L) : L(L) {}

				void operator()() {
					typedef container_usertype_metatable<std::remove_pointer_t<T>> meta_cumt;
					static const char* metakey = &usertype_traits<T>::metatable()[0];
					static const std::array<luaL_Reg, 14> reg = { {
						{ "__pairs", &meta_cumt::pairs_call },
						{ "__ipairs", &meta_cumt::pairs_call },
						{ "__len", &meta_cumt::length_call },
						{ "__index", &meta_cumt::index_call },
						{ "__newindex", &meta_cumt::new_index_call },
						{ "get", &meta_cumt::get_call },
						{ "set", &meta_cumt::set_call },
						{ "clear", &meta_cumt::clear_call },
						{ "insert", &meta_cumt::insert_call },
						{ "add", &meta_cumt::add_call },
						{ "find", &meta_cumt::find_call },
						{ "erase", &meta_cumt::erase_call },
						std::is_pointer<T>::value ? luaL_Reg{ nullptr, nullptr } : luaL_Reg{ "__gc", &detail::usertype_alloc_destroy<T> },
						{ nullptr, nullptr }
					} }; 
					
					if (luaL_newmetatable(L, metakey) == 1) {
						stack_reference metatable(L, -1);
						luaL_setfuncs(L, reg.data(), 0);
					}
					lua_setmetatable(L, -2);
				}
			};
		}
		
		template<typename T>
		struct pusher<T, std::enable_if_t<meta::all<is_container<meta::unqualified_t<T>>, meta::neg<meta::any<std::is_base_of<reference, meta::unqualified_t<T>>, std::is_base_of<stack_reference, meta::unqualified_t<T>>>>>::value>> {
			typedef meta::unqualified_t<T> C;

			static int push(lua_State* L, const T& cont) {
				stack_detail::metatable_setup<C> fx(L);
				return pusher<detail::as_value_tag<T>>{}.push_fx(L, fx, cont);
			}

			static int push(lua_State* L, T&& cont) {
				stack_detail::metatable_setup<C> fx(L);
				return pusher<detail::as_value_tag<T>>{}.push_fx(L, fx, std::move(cont));
			}
		};

		template<typename T>
		struct pusher<T*, std::enable_if_t<meta::all<is_container<meta::unqualified_t<T>>, meta::neg<meta::any<std::is_base_of<reference, meta::unqualified_t<T>>, std::is_base_of<stack_reference, meta::unqualified_t<T>>>>>::value>> {
			typedef std::add_pointer_t<meta::unqualified_t<std::remove_pointer_t<T>>> C;

			static int push(lua_State* L, T* cont) {
				stack_detail::metatable_setup<C> fx(L);
				return pusher<detail::as_pointer_tag<T>>{}.push_fx(L, fx, cont);
			}
		};
	} // stack

} // sol

#endif // SOL_CONTAINER_USERTYPE_METATABLE_HPP
