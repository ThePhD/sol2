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

#ifndef SOL_USERTYPE_CONTAINER_HPP
#define SOL_USERTYPE_CONTAINER_HPP

#include "stack.hpp"
#include "container_traits.hpp"
#include "map.hpp"

namespace sol {

	template <typename X>
	struct usertype_container {
		typedef std::remove_pointer_t<meta::unqualified_t<X>> T;
		typedef container_traits<T> traits;
		typedef container_detail::container_traits_default<T> default_traits;

		static inline int real_index_get_traits(std::true_type, lua_State* L) {
			return traits::index_get(L);
		}

		static inline int real_index_get_traits(std::false_type, lua_State* L) {
			return default_traits::index_get(L);
		}

		static inline int real_index_call(lua_State* L) {
			typedef detail::unordered_map<std::string, lua_CFunction> call_map;
			static const call_map calls{
				{ "at", &at_call },
				{ "get", &real_get_call },
				{ "set", &real_set_call },
				{ "size", &real_length_call },
				{ "add", &real_add_call },
				{ "empty", &real_empty_call },
				{ "insert", &real_insert_call },
				{ "clear", &real_clear_call },
				{ "find", &real_find_call },
				{ "index_of", &real_index_of_call },
				{ "erase", &real_erase_call },
				{ "pairs", &pairs_call },
				{ "next", &next_call },
			};
			auto maybenameview = stack::unqualified_check_get<string_view>(L, 2);
			if (maybenameview) {
				const string_view& nameview = *maybenameview;
#if defined(SOL_UNORDERED_MAP_COMPATIBLE_HASH) && SOL_UNORDERED_MAP_COMPATIBLE_HASH
				auto it = calls.find(nameview, string_view_hash(), std::equal_to<string_view>());
#else
				std::string name(nameview.data(), nameview.size());
				auto it = calls.find(name);
#endif
				if (it != calls.cend()) {
					return stack::push(L, it->second);
				}
			}
			return real_index_get_traits(container_detail::has_traits_index_get<traits>(), L);
		}

		static inline int real_at_traits(std::true_type, lua_State* L) {
			return traits::at(L);
		}

		static inline int real_at_traits(std::false_type, lua_State* L) {
			return default_traits::at(L);
		}

		static inline int real_at_call(lua_State* L) {
			return real_at_traits(container_detail::has_traits_at<traits>(), L);
		}

		static inline int real_get_traits(std::true_type, lua_State* L) {
			return traits::get(L);
		}

		static inline int real_get_traits(std::false_type, lua_State* L) {
			return default_traits::get(L);
		}

		static inline int real_get_call(lua_State* L) {
			return real_get_traits(container_detail::has_traits_get<traits>(), L);
		}

		static inline int real_set_traits(std::true_type, lua_State* L) {
			return traits::set(L);
		}

		static inline int real_set_traits(std::false_type, lua_State* L) {
			return default_traits::set(L);
		}

		static inline int real_set_call(lua_State* L) {
			return real_set_traits(container_detail::has_traits_set<traits>(), L);
		}

		static inline int real_index_set_traits(std::true_type, lua_State* L) {
			return traits::index_set(L);
		}

		static inline int real_index_set_traits(std::false_type, lua_State* L) {
			return default_traits::index_set(L);
		}

		static inline int real_new_index_call(lua_State* L) {
			return real_index_set_traits(container_detail::has_traits_index_set<traits>(), L);
		}

		static inline int real_pairs_traits(std::true_type, lua_State* L) {
			return traits::pairs(L);
		}

		static inline int real_pairs_traits(std::false_type, lua_State* L) {
			return default_traits::pairs(L);
		}

		static inline int real_pairs_call(lua_State* L) {
			return real_pairs_traits(container_detail::has_traits_pairs<traits>(), L);
		}

		static inline int real_ipairs_traits(std::true_type, lua_State* L) {
			return traits::ipairs(L);
		}

		static inline int real_ipairs_traits(std::false_type, lua_State* L) {
			return default_traits::ipairs(L);
		}

		static inline int real_ipairs_call(lua_State* L) {
			return real_ipairs_traits(container_detail::has_traits_ipairs<traits>(), L);
		}

		static inline int real_next_traits(std::true_type, lua_State* L) {
			return traits::next(L);
		}

		static inline int real_next_traits(std::false_type, lua_State* L) {
			return default_traits::next(L);
		}

		static inline int real_next_call(lua_State* L) {
			return real_next_traits(container_detail::has_traits_next<traits>(), L);
		}

		static inline int real_size_traits(std::true_type, lua_State* L) {
			return traits::size(L);
		}

		static inline int real_size_traits(std::false_type, lua_State* L) {
			return default_traits::size(L);
		}

		static inline int real_length_call(lua_State* L) {
			return real_size_traits(container_detail::has_traits_size<traits>(), L);
		}

		static inline int real_add_traits(std::true_type, lua_State* L) {
			return traits::add(L);
		}

		static inline int real_add_traits(std::false_type, lua_State* L) {
			return default_traits::add(L);
		}

		static inline int real_add_call(lua_State* L) {
			return real_add_traits(container_detail::has_traits_add<traits>(), L);
		}

		static inline int real_insert_traits(std::true_type, lua_State* L) {
			return traits::insert(L);
		}

		static inline int real_insert_traits(std::false_type, lua_State* L) {
			return default_traits::insert(L);
		}

		static inline int real_insert_call(lua_State* L) {
			return real_insert_traits(container_detail::has_traits_insert<traits>(), L);
		}

		static inline int real_clear_traits(std::true_type, lua_State* L) {
			return traits::clear(L);
		}

		static inline int real_clear_traits(std::false_type, lua_State* L) {
			return default_traits::clear(L);
		}

		static inline int real_clear_call(lua_State* L) {
			return real_clear_traits(container_detail::has_traits_clear<traits>(), L);
		}

		static inline int real_empty_traits(std::true_type, lua_State* L) {
			return traits::empty(L);
		}

		static inline int real_empty_traits(std::false_type, lua_State* L) {
			return default_traits::empty(L);
		}

		static inline int real_empty_call(lua_State* L) {
			return real_empty_traits(container_detail::has_traits_empty<traits>(), L);
		}

		static inline int real_erase_traits(std::true_type, lua_State* L) {
			return traits::erase(L);
		}

		static inline int real_erase_traits(std::false_type, lua_State* L) {
			return default_traits::erase(L);
		}

		static inline int real_erase_call(lua_State* L) {
			return real_erase_traits(container_detail::has_traits_erase<traits>(), L);
		}

		static inline int real_find_traits(std::true_type, lua_State* L) {
			return traits::find(L);
		}

		static inline int real_find_traits(std::false_type, lua_State* L) {
			return default_traits::find(L);
		}

		static inline int real_find_call(lua_State* L) {
			return real_find_traits(container_detail::has_traits_find<traits>(), L);
		}

		static inline int real_index_of_traits(std::true_type, lua_State* L) {
			return traits::index_of(L);
		}

		static inline int real_index_of_traits(std::false_type, lua_State* L) {
			return default_traits::index_of(L);
		}

		static inline int real_index_of_call(lua_State* L) {
			return real_index_of_traits(container_detail::has_traits_index_of<traits>(), L);
		}

		static inline int add_call(lua_State* L) {
			return detail::typed_static_trampoline<decltype(&real_add_call), (&real_add_call)>(L);
		}

		static inline int erase_call(lua_State* L) {
			return detail::typed_static_trampoline<decltype(&real_erase_call), (&real_erase_call)>(L);
		}

		static inline int insert_call(lua_State* L) {
			return detail::typed_static_trampoline<decltype(&real_insert_call), (&real_insert_call)>(L);
		}

		static inline int clear_call(lua_State* L) {
			return detail::typed_static_trampoline<decltype(&real_clear_call), (&real_clear_call)>(L);
		}

		static inline int empty_call(lua_State* L) {
			return detail::typed_static_trampoline<decltype(&real_empty_call), (&real_empty_call)>(L);
		}

		static inline int find_call(lua_State* L) {
			return detail::typed_static_trampoline<decltype(&real_find_call), (&real_find_call)>(L);
		}

		static inline int index_of_call(lua_State* L) {
			return detail::typed_static_trampoline<decltype(&real_index_of_call), (&real_index_of_call)>(L);
		}

		static inline int length_call(lua_State* L) {
			return detail::typed_static_trampoline<decltype(&real_length_call), (&real_length_call)>(L);
		}

		static inline int pairs_call(lua_State* L) {
			return detail::typed_static_trampoline<decltype(&real_pairs_call), (&real_pairs_call)>(L);
		}

		static inline int ipairs_call(lua_State* L) {
			return detail::typed_static_trampoline<decltype(&real_ipairs_call), (&real_ipairs_call)>(L);
		}

		static inline int next_call(lua_State* L) {
			return detail::typed_static_trampoline<decltype(&real_next_call), (&real_next_call)>(L);
		}

		static inline int at_call(lua_State* L) {
			return detail::typed_static_trampoline<decltype(&real_at_call), (&real_at_call)>(L);
		}

		static inline int get_call(lua_State* L) {
			return detail::typed_static_trampoline<decltype(&real_get_call), (&real_get_call)>(L);
		}

		static inline int set_call(lua_State* L) {
			return detail::typed_static_trampoline<decltype(&real_set_call), (&real_set_call)>(L);
		}

		static inline int index_call(lua_State* L) {
			return detail::typed_static_trampoline<decltype(&real_index_call), (&real_index_call)>(L);
		}

		static inline int new_index_call(lua_State* L) {
			return detail::typed_static_trampoline<decltype(&real_new_index_call), (&real_new_index_call)>(L);
		}
	};

	namespace stack {
		namespace stack_detail {
			template <typename T, bool is_shim = false>
			struct metatable_setup {
				lua_State* L;

				metatable_setup(lua_State* L)
				: L(L) {
				}

				void operator()() {
					typedef usertype_container<std::conditional_t<is_shim,
						as_container_t<std::remove_pointer_t<T>>,
						std::remove_pointer_t<T>>>
						meta_usertype_container;
					static const char* metakey = is_shim ? &usertype_traits<as_container_t<std::remove_pointer_t<T>>>::metatable()[0] : &usertype_traits<T>::metatable()[0];
					static const std::array<luaL_Reg, 20> reg = { { 
						// clang-format off
						{ "__pairs", &meta_usertype_container::pairs_call },
						{ "__ipairs", &meta_usertype_container::ipairs_call },
						{ "__len", &meta_usertype_container::length_call },
						{ "__index", &meta_usertype_container::index_call },
						{ "__newindex", &meta_usertype_container::new_index_call },
						{ "pairs", &meta_usertype_container::pairs_call },
						{ "next", &meta_usertype_container::next_call },
						{ "at", &meta_usertype_container::at_call },
						{ "get", &meta_usertype_container::get_call },
						{ "set", &meta_usertype_container::set_call },
						{ "size", &meta_usertype_container::length_call },
						{ "empty", &meta_usertype_container::empty_call },
						{ "clear", &meta_usertype_container::clear_call },
						{ "insert", &meta_usertype_container::insert_call },
						{ "add", &meta_usertype_container::add_call },
						{ "find", &meta_usertype_container::find_call },
						{ "index_of", &meta_usertype_container::index_of_call },
						{ "erase", &meta_usertype_container::erase_call },
						std::is_pointer<T>::value ? luaL_Reg{ nullptr, nullptr } : luaL_Reg{ "__gc", &detail::usertype_alloc_destruct<T> },
						{ nullptr, nullptr }
						// clang-format on 
					} };

					if (luaL_newmetatable(L, metakey) == 1) {
						luaL_setfuncs(L, reg.data(), 0);
					}
					lua_setmetatable(L, -2);
				}
			};
		} // namespace stack_detail

		template <typename T>
		struct unqualified_pusher<as_container_t<T>> {
			typedef meta::unqualified_t<T> C;

			static int push_lvalue(std::true_type, lua_State* L, const C& cont) {
				stack_detail::metatable_setup<C*, true> fx(L);
				return stack::push<detail::as_pointer_tag<const C>>(L, detail::with_function_tag(), fx, detail::ptr(cont));
			}

			static int push_lvalue(std::false_type, lua_State* L, const C& cont) {
				stack_detail::metatable_setup<C, true> fx(L);
				return stack::push<detail::as_value_tag<C>>(L, detail::with_function_tag(), fx, cont);
			}

			static int push_rvalue(std::true_type, lua_State* L, C&& cont) {
				stack_detail::metatable_setup<C, true> fx(L);
				return stack::push<detail::as_value_tag<C>>(L, detail::with_function_tag(), fx, std::move(cont));
			}

			static int push_rvalue(std::false_type, lua_State* L, const C& cont) {
				return push_lvalue(std::is_lvalue_reference<T>(), L, cont);
			}

			static int push(lua_State* L, const as_container_t<T>& as_cont) {
				return push_lvalue(std::is_lvalue_reference<T>(), L, as_cont.source);
			}

			static int push(lua_State* L, as_container_t<T>&& as_cont) {
				return push_rvalue(meta::all<std::is_rvalue_reference<T>, meta::neg<std::is_lvalue_reference<T>>>(), L, std::forward<T>(as_cont.source));
			}
		};

		template <typename T>
		struct unqualified_pusher<as_container_t<T*>> {
			typedef std::add_pointer_t<meta::unqualified_t<std::remove_pointer_t<T>>> C;

			static int push(lua_State* L, T* cont) {
				stack_detail::metatable_setup<C> fx(L);
				return stack::push<detail::as_pointer_tag<T>>(L, detail::with_function_tag(), fx, cont);
			}
		};

		template <typename T>
		struct unqualified_pusher<T, std::enable_if_t<meta::all<is_container<meta::unqualified_t<T>>, meta::neg<is_lua_reference<meta::unqualified_t<T>>>>::value>> {
			typedef meta::unqualified_t<T> C;

			static int push(lua_State* L, const T& cont) {
				stack_detail::metatable_setup<C> fx(L);
				return stack::push<detail::as_value_tag<T>>(L, detail::with_function_tag(), fx, cont);
			}

			static int push(lua_State* L, T&& cont) {
				stack_detail::metatable_setup<C> fx(L);
				return stack::push<detail::as_value_tag<T>>(L, detail::with_function_tag(), fx, std::move(cont));
			}
		};

		template <typename T>
		struct unqualified_pusher<T*, std::enable_if_t<meta::all<is_container<meta::unqualified_t<T>>, meta::neg<is_lua_reference<meta::unqualified_t<T>>>>::value>> {
			typedef std::add_pointer_t<meta::unqualified_t<std::remove_pointer_t<T>>> C;

			static int push(lua_State* L, T* cont) {
				stack_detail::metatable_setup<C> fx(L);
				return stack::push<detail::as_pointer_tag<T>>(L, detail::with_function_tag(), fx, cont);
			}
		};

		template <typename T, typename C>
		struct unqualified_checker<as_container_t<T>, type::userdata, C> {
			template <typename Handler>
			static bool check(lua_State* L, int index, Handler&& handler, record& tracking) {
				return stack::check<T>(L, index, std::forward<Handler>(handler), tracking);
			}
		};

		template <typename T>
		struct unqualified_getter<as_container_t<T>> {
			static decltype(auto) get(lua_State* L, int index, record& tracking) {
				return stack::unqualified_get<T>(L, index, tracking);
			}
		};

		template <typename T>
		struct unqualified_getter<as_container_t<T>*> {
			static decltype(auto) get(lua_State* L, int index, record& tracking) {
				return stack::unqualified_get<T*>(L, index, tracking);
			}
		};
	} // namespace stack

} // namespace sol

#endif // SOL_USERTYPE_CONTAINER_HPP
