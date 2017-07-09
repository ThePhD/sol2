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

#ifndef SOL_CONTAINER_TRAITS_HPP
#define SOL_CONTAINER_TRAITS_HPP

#include "traits.hpp"
#include "stack.hpp"
#include <unordered_map>

namespace sol {

	namespace container_detail {
		template <typename T>
		struct has_find {
		private:
			typedef std::array<char, 1> one;
			typedef std::array<char, 2> two;

			template <typename C> static one test(decltype(std::declval<C>().find(std::declval<std::add_rvalue_reference_t<typename C::value_type>>()))*);
			template <typename C> static two test(...);

		public:
			static const bool value = sizeof(test<T>(0)) == sizeof(char);
		};

		template <typename T>
		struct has_push_back {
		private:
			typedef std::array<char, 1> one;
			typedef std::array<char, 2> two;

			template <typename C> static one test(decltype(std::declval<C>().push_back(std::declval<std::add_rvalue_reference_t<typename C::value_type>>()))*);
			template <typename C> static two test(...);

		public:
			static const bool value = sizeof(test<T>(0)) == sizeof(char);
		};

		template <typename T>
		struct has_clear {
		private:
			typedef std::array<char, 1> one;
			typedef std::array<char, 2> two;

			template <typename C> static one test(decltype(&C::clear));
			template <typename C> static two test(...);

		public:
			static const bool value = sizeof(test<T>(0)) == sizeof(char);
		};

		template <typename T>
		struct has_insert {
		private:
			typedef std::array<char, 1> one;
			typedef std::array<char, 2> two;

			template <typename C> static one test(decltype(std::declval<C>().insert(std::declval<std::add_rvalue_reference_t<typename C::const_iterator>>(), std::declval<std::add_rvalue_reference_t<typename C::value_type>>()))*);
			template <typename C> static two test(...);

		public:
			static const bool value = sizeof(test<T>(0)) == sizeof(char);
		};

		template <typename X, typename = void>
		struct container_traits_default {
		private:
			typedef std::remove_pointer_t<meta::unwrap_unqualified_t<X>> T;
		public:
			typedef std::false_type is_container;
			typedef std::false_type is_associative;
			typedef lua_nil_t iterator;
			typedef lua_nil_t value_type;

			static lua_nil_t get(lua_State* L, T* self, stack_object key) {
				(void)self;
				(void)key;
				luaL_error(L, "sol: cannot call 'get(key)' on type '%s' because it is not recognized as a container", detail::demangle<X>().c_str());
				return lua_nil;
			}

			static lua_nil_t index_get(lua_State* L, T* self, stack_object key) {
				(void)self;
				(void)key;
				luaL_error(L, "sol: cannot call 'container[key]' on type '%s' because it is not recognized as a container", detail::demangle<X>().c_str());
				return lua_nil;
			}

			static lua_nil_t index_set(lua_State* L, T* self, stack_object key, stack_object value) {
				(void)self;
				(void)key;
				(void)value;
				luaL_error(L, "sol: cannot call 'container[key] = value' on type '%s' because it is not recognized as a container", detail::demangle<X>().c_str());
				return lua_nil;
			}

			static lua_nil_t add(lua_State* L, T* self, stack_object key, stack_object value) {
				(void)self;
				(void)key;
				(void)value;
				luaL_error(L, "sol: cannot call '%s' on type '%s' because it is not recognized as a container", value.valid() ? "add(key, value)" : "add(value)", detail::demangle<X>().c_str());
				return lua_nil;
			}

			static lua_nil_t insert(lua_State* L, T* self, stack_object where, stack_object value) {
				(void)self;
				(void)where;
				(void)value;
				luaL_error(L, "sol: cannot call 'container[key] = value' on type '%s' because it is not recognized as a container", detail::demangle<X>().c_str());
				return lua_nil;
			}

			static lua_nil_t find(lua_State* L, T* self, stack_object key) {
				(void)self;
				(void)key;
				luaL_error(L, "sol: cannot call 'container[key] = value' on type '%s' because it is not recognized as a container", detail::demangle<X>().c_str());
				return lua_nil;
			}

			static lua_nil_t begin(lua_State* L, T* self) {
				(void)self;
				luaL_error(L, "sol: cannot call 'begin' on type '%s' because it is not recognized as a container", detail::demangle<X>().c_str());
				return lua_nil;
			}

			static lua_nil_t end(lua_State* L, T* self) {
				(void)self;
				luaL_error(L, "sol: cannot call 'end' on type '%s' because it is not recognized as a container", detail::demangle<X>().c_str());
				return lua_nil;
			}

			static lua_nil_t size(lua_State* L, T* self) {
				(void)self;
				luaL_error(L, "sol: cannot call 'end' on type '%s' because it is not recognized as a container", detail::demangle<X>().c_str());
				return lua_nil;
			}

			static lua_nil_t clear(lua_State* L, T* self) {
				(void)self;
				luaL_error(L, "sol: cannot call 'clear' on type '%s' because it is not recognized as a container", detail::demangle<X>().c_str());
				return lua_nil;
			}

			static lua_nil_t erase(lua_State* L, T* self, stack_object key) {
				(void)self;
				(void)key;
				luaL_error(L, "sol: cannot call 'erase' on type '%s' because it is not recognized as a container", detail::demangle<X>().c_str());
				return lua_nil;
			}
		};
		template <typename X>
		struct container_traits_default<T, std::enable_if_t<
			meta::all<is_container<meta::unqualified<X>>, meta::has_value_type<meta::unqualified<X>>, meta::has_iterator<meta::unqualified<X>>>::value
			>> {
		private:
			typedef std::remove_pointer_t<meta::unwrap_unqualified_t<X>> T;
			typedef typename T::iterator iterator;
			typedef std::conditional_t<is_associative::value, typename T::value_type, std::pair<std::size_t, typename T::value_type>> KV;
			typedef typename KV::first_type K;
			typedef typename KV::second_type V;
			typedef std::remove_reference_t<decltype(*std::declval<I&>())> iterator_return;
			typedef typename meta::iterator_tag<I>::type iterator_category;
			typedef std::conditional_t<std::is_same<tag_t, std::input_iterator_tag>::value,
				V,
				std::conditional_t<is_associative::value,
				V,
				decltype(*std::declval<I&>())
				>
			> push_type;

			static auto& get_src(lua_State* L) {
#ifdef SOL_SAFE_USERTYPE
				auto p = stack::check_get<T*>(L, 1);
				if (!p || p.value() == nullptr) {
					luaL_error(L, "sol: 'self' argument is not the proper type (pass 'self' as first argument with ':' or call on proper type)");
				}
				return *p.value();
#else
				return stack::get<T>(L, 1);
#endif // Safe getting with error
			}

		public:

			static int delegate_call(lua_State* L) {
				static std::unordered_map<std::string, lua_CFunction> calls{
					{ "add", &real_add_call },
					{ "insert", &real_insert_call },
					{ "clear", &real_clear_call },
					{ "find", &real_find_call },
					{ "get", &real_get_call }
				};
				auto maybename = stack::check_get<std::string>(L, 2);
				if (maybename) {
					auto& name = *maybename;
					auto it = calls.find(name);
					if (it != calls.cend()) {
						return stack::push(L, it->second);
					}
				}
				return stack::push(L, lua_nil);
			}

			static int real_index_call_associative(std::true_type, lua_State* L) {
				auto k = stack::check_get<K>(L, 2);
				if (k) {
					auto& src = get_src(L);
					using std::end;
					auto it = detail::find(src, *k);
					if (it != end(src)) {
						auto& v = *it;
						return stack::stack_detail::push_reference<push_type>(L, v.second);
					}
				}
				else {
					return delegate_call(L);
				}
				return stack::push(L, lua_nil);
			}

			static int real_index_call_associative(std::false_type, lua_State* L) {
				auto& src = get_src(L);
				auto maybek = stack::check_get<K>(L, 2);
				if (maybek) {
					using std::begin;
					auto it = begin(src);
					K k = *maybek;
					if (k > src.size() || k < 1) {
						return stack::push(L, lua_nil);
					}
					--k;
					std::advance(it, k);
					return stack::stack_detail::push_reference<push_type>(L, *it);
				}
				else {
					return delegate_call(L);
				}

				return stack::push(L, lua_nil);
			}

			static int real_index_call(lua_State* L) {
				return real_index_call_associative(is_associative(), L);
			}

			static int real_get_call(lua_State* L) {
				return real_index_call_associative(is_associative(), L);
			}

			static int real_new_index_call_const(std::false_type, std::false_type, lua_State* L) {
				return luaL_error(L, "sol: cannot write to a const value type or an immutable iterator (e.g., std::set)");
			}

			static int real_new_index_call_const(std::false_type, std::true_type, lua_State* L) {
				return luaL_error(L, "sol: cannot write to a const value type or an immutable iterator (e.g., std::set)");
			}

			static int real_new_index_call_fixed(std::true_type, lua_State* L) {
				auto& src = get_src(L);
#ifdef SOL_CHECK_ARGUMENTS
				auto maybek = stack::check_get<K>(L, 2);
				if (!maybek) {
					return luaL_error(L, "sol: improper key of type %s for %s", lua_typename(L, static_cast<int>(type_of(L, 2))), detail::demangle<T>().c_str());
				}
				K& k = *maybek;
#else
				K k = stack::get<K>(L, 2);
#endif
				using std::end;
				auto it = detail::find(src, k);
				if (it != end(src)) {
					auto& v = *it;
					v.second = stack::get<V>(L, 3);
				}
				else {
					src.insert(it, { std::move(k), stack::get<V>(L, 3) });
				}
				return 0;
			}

			static int real_new_index_call_fixed(std::false_type, lua_State* L) {
				auto& src = get_src(L);
#ifdef SOL_CHECK_ARGUMENTS
				auto maybek = stack::check_get<K>(L, 2);
				if (!maybek) {
					return luaL_error(L, "sol: improper key of type %s for %s", lua_typename(L, static_cast<int>(type_of(L, 2))), detail::demangle<T>().c_str());
				}
				K& k = *maybek;
#else
				K k = stack::get<K>(L, 2);
#endif
				using std::end;
				auto it = detail::find(src, k);
				if (it != end(src)) {
					auto& v = *it;
					v.second = stack::get<V>(L, 3);
				}
				else {
					return luaL_error(L, "sol: cannot insert key of type %s to into %s", lua_typename(L, static_cast<int>(type_of(L, 2))), detail::demangle<T>().c_str());
				}
				return 0;
			}

			static int real_new_index_call_const(std::true_type, std::true_type, lua_State* L) {
				return real_new_index_call_fixed(std::integral_constant<bool, detail::has_insert<T>::value>(), L);
			}

			static int real_new_index_call_const(std::true_type, std::false_type, lua_State* L) {
				auto& src = get_src(L);
#ifdef SOL_CHECK_ARGUMENTS
				auto maybek = stack::check_get<K>(L, 2);
				if (!maybek) {
					return luaL_error(L, "sol: improper index of type %s to a %s", lua_typename(L, static_cast<int>(type_of(L, 2))), detail::demangle<T>().c_str());
				}
				K& k = *maybek;
#else
				K k = stack::get<K>(L, 2);
#endif
				using std::begin;
				auto it = begin(src);
#ifdef SOL_CHECK_ARGUMENTS
				if (k < 1) {
					return luaL_error(L, "sol: out of bounds index to a %s", detail::demangle<T>().c_str());
				}
#endif
				--k;
				if (k == src.size()) {
					real_add_call_push(std::integral_constant<bool, detail::has_push_back<T>::value && std::is_copy_constructible<V>::value>(), L, src, 1);
					return 0;
				}
#ifdef SOL_CHECK_ARGUMENTS
				if (k > src.size()) {
					return luaL_error(L, "sol: out of bounds index to a %s", detail::demangle<T>().c_str());
				}
#endif
				std::advance(it, k);
				*it = stack::get<V>(L, 3);
				return 0;
			}

			static int real_new_index_call(lua_State* L) {
				return real_new_index_call_const(meta::neg<meta::any<std::is_const<V>, std::is_const<IR>, meta::neg<std::is_copy_assignable<V>>>>(), meta::all<is_associative, detail::has_insert<T>>(), L);
			}

			static int real_pairs_next_call_assoc(std::true_type, lua_State* L) {
				using std::end;
				iter& i = stack::get<user<iter>>(L, 1);
				auto& source = i.source;
				auto& it = i.it;
				if (it == end(source)) {
					return 0;
				}
				int p;
				p = stack::push_reference(L, it->first);
				p += stack::stack_detail::push_reference<push_type>(L, it->second);
				std::advance(it, 1);
				return p;
			}

			static int real_pairs_call_assoc(std::true_type, lua_State* L) {
				auto& src = get_src(L);
				using std::begin;
				stack::push(L, pairs_next_call);
				stack::push<user<iter>>(L, src, begin(src));
				stack::push(L, 1);
				return 3;
			}

			static int real_pairs_next_call_assoc(std::false_type, lua_State* L) {
				using std::end;
				iter& i = stack::get<user<iter>>(L, 1);
				auto& source = i.source;
				auto& it = i.it;
				K k = stack::get<K>(L, 2);
				if (it == end(source)) {
					return 0;
				}
				int p;
				p = stack::push_reference(L, k + 1);
				p += stack::stack_detail::push_reference<push_type>(L, *it);
				std::advance(it, 1);
				return p;
			}

			static int real_pairs_call_assoc(std::false_type, lua_State* L) {
				auto& src = get_src(L);
				using std::begin;
				stack::push(L, pairs_next_call);
				stack::push<user<iter>>(L, src, begin(src));
				stack::push(L, 0);
				return 3;
			}

			static int real_pairs_next_call(lua_State* L) {
				return real_pairs_next_call_assoc(is_associative(), L);
			}

			static int real_pairs_call(lua_State* L) {
				return real_pairs_call_assoc(is_associative(), L);
			}

			static int real_length_call(lua_State*L) {
				auto& src = get_src(L);
				return stack::push(L, src.size());
			}

			static int real_add_call_insert(std::true_type, lua_State*L, T& src, int boost = 0) {
				using std::end;
				src.insert(end(src), stack::get<V>(L, 2 + boost));
				return 0;
			}

			static int real_add_call_insert(std::false_type, lua_State*L, T&, int = 0) {
				static const std::string& s = detail::demangle<T>();
				return luaL_error(L, "sol: cannot call insert on type %s", s.c_str());
			}

			static int real_add_call_push(std::true_type, lua_State*L, T& src, int boost = 0) {
				src.push_back(stack::get<V>(L, 2 + boost));
				return 0;
			}

			static int real_add_call_push(std::false_type, lua_State*L, T& src, int boost = 0) {
				return real_add_call_insert(std::integral_constant<bool, detail::has_insert<T>::value && std::is_copy_constructible<V>::value>(), L, src, boost);
			}

			static int real_add_call_associative(std::true_type, lua_State* L) {
				return real_insert_call(L);
			}

			static int real_add_call_associative(std::false_type, lua_State* L) {
				auto& src = get_src(L);
				return real_add_call_push(std::integral_constant<bool, detail::has_push_back<T>::value && std::is_copy_constructible<V>::value>(), L, src);
			}

			static int real_add_call_capable(std::true_type, lua_State* L) {
				return real_add_call_associative(is_associative(), L);
			}

			static int real_add_call_capable(std::false_type, lua_State* L) {
				static const std::string& s = detail::demangle<T>();
				return luaL_error(L, "sol: cannot call add on type %s", s.c_str());
			}

			static int real_add_call(lua_State* L) {
				return real_add_call_capable(std::integral_constant<bool, (detail::has_push_back<T>::value || detail::has_insert<T>::value) && std::is_copy_constructible<V>::value>(), L);
			}

			static int real_insert_call_capable(std::false_type, std::false_type, lua_State*L) {
				static const std::string& s = detail::demangle<T>();
				return luaL_error(L, "sol: cannot call insert on type %s", s.c_str());
			}

			static int real_insert_call_capable(std::false_type, std::true_type, lua_State*L) {
				return real_insert_call_capable(std::false_type(), std::false_type(), L);
			}

			static int real_insert_call_capable(std::true_type, std::false_type, lua_State* L) {
				using std::begin;
				auto& src = get_src(L);
				src.insert(std::next(begin(src), stack::get<K>(L, 2)), stack::get<V>(L, 3));
				return 0;
			}

			static int real_insert_call_capable(std::true_type, std::true_type, lua_State* L) {
				return real_new_index_call(L);
			}

			static int real_insert_call(lua_State*L) {
				return real_insert_call_capable(std::integral_constant<bool, detail::has_insert<T>::value && std::is_copy_assignable<V>::value>(), is_associative(), L);
			}

			static int real_clear_call_capable(std::false_type, lua_State* L) {
				static const std::string& s = detail::demangle<T>();
				return luaL_error(L, "sol: cannot call clear on type %s", s.c_str());
			}

			static int real_clear_call_capable(std::true_type, lua_State* L) {
				auto& src = get_src(L);
				src.clear();
				return 0;
			}

			static int real_clear_call(lua_State*L) {
				return real_clear_call_capable(std::integral_constant<bool, detail::has_clear<T>::value>(), L);
			}

			static int real_find_call_capable(std::false_type, std::false_type, lua_State*L) {
				static const std::string& s = detail::demangle<T>();
				return luaL_error(L, "sol: cannot call find on type %s", s.c_str());
			}

			static int real_find_call_capable(std::false_type, std::true_type, lua_State*L) {
				return real_index_call(L);
			}

			static int real_find_call_capable(std::true_type, std::false_type, lua_State* L) {
				auto k = stack::check_get<V>(L, 2);
				if (k) {
					auto& src = get_src(L);
					auto it = src.find(*k);
					if (it != src.end()) {
						auto& v = *it;
						return stack::stack_detail::push_reference<push_type>(L, v);
					}
				}
				return stack::push(L, lua_nil);
			}

			static int real_find_call_capable(std::true_type, std::true_type, lua_State* L) {
				return real_index_call(L);
			}

			static int real_find_call(lua_State*L) {
				return real_find_call_capable(std::integral_constant<bool, detail::has_find<T>::value>(), is_associative(), L);
			}
		};
	} // container_detail

	template <typename T>
	struct container_traits : container_detail::container_traits_default<T> {};

} // sol

#endif // SOL_CONTAINER_TRAITS_HPP
