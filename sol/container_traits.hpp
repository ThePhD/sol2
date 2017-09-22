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

	template <typename T>
	struct container_traits;

	template <typename T>
	struct as_container_t {
		T source;

		as_container_t(T value)
		: source(std::move(value)) {
		}

		operator std::add_rvalue_reference_t<T>() {
			return std::move(source);
		}

		operator std::add_lvalue_reference_t<std::add_const_t<T>>() const {
			return source;
		}
	};

	template <typename T>
	struct as_container_t<T&> {
		std::reference_wrapper<T> source;

		as_container_t(T& value)
		: source(value) {
		}

		operator T&() {
			return source;
		}
	};

	template <typename T>
	auto as_container(T&& value) {
		return as_container_t<T>(std::forward<T>(value));
	}

	namespace container_detail {

		template <typename T>
		struct has_clear_test {
		private:
			typedef std::array<char, 1> one;
			typedef std::array<char, 2> two;

			template <typename C>
			static one test(decltype(&C::clear));
			template <typename C>
			static two test(...);

		public:
			static const bool value = sizeof(test<T>(0)) == sizeof(char);
		};

		template <typename T>
		struct has_empty_test {
		private:
			typedef std::array<char, 1> one;
			typedef std::array<char, 2> two;

			template <typename C>
			static one test(decltype(&C::empty));
			template <typename C>
			static two test(...);

		public:
			static const bool value = sizeof(test<T>(0)) == sizeof(char);
		};

		template <typename T>
		struct has_erase_after_test {
		private:
			typedef std::array<char, 1> one;
			typedef std::array<char, 2> two;

			template <typename C>
			static one test(decltype(std::declval<C>().erase_after(std::declval<std::add_rvalue_reference_t<typename C::const_iterator>>()))*);
			template <typename C>
			static two test(...);

		public:
			static const bool value = sizeof(test<T>(0)) == sizeof(char);
		};

		template <typename T, typename = void>
		struct has_find_test {
		private:
			typedef std::array<char, 1> one;
			typedef std::array<char, 2> two;

			template <typename C>
			static one test(decltype(std::declval<C>().find(std::declval<std::add_rvalue_reference_t<typename C::value_type>>()))*);
			template <typename C>
			static two test(...);

		public:
			static const bool value = sizeof(test<T>(0)) == sizeof(char);
		};

		template <typename T>
		struct has_find_test<T, std::enable_if_t<meta::is_lookup<T>::value>> {
		private:
			typedef std::array<char, 1> one;
			typedef std::array<char, 2> two;

			template <typename C>
			static one test(decltype(std::declval<C>().find(std::declval<std::add_rvalue_reference_t<typename C::key_type>>()))*);
			template <typename C>
			static two test(...);

		public:
			static const bool value = sizeof(test<T>(0)) == sizeof(char);
		};

		template <typename T>
		struct has_erase_test {
		private:
			typedef std::array<char, 1> one;
			typedef std::array<char, 2> two;

			template <typename C>
			static one test(decltype(std::declval<C>().erase(std::declval<typename C::iterator>()))*);
			template <typename C>
			static two test(...);

		public:
			static const bool value = sizeof(test<T>(0)) == sizeof(char);
		};

		template <typename T>
		struct has_traits_find_test {
		private:
			typedef std::array<char, 1> one;
			typedef std::array<char, 2> two;

			template <typename C>
			static one test(decltype(&C::find));
			template <typename C>
			static two test(...);

		public:
			static const bool value = sizeof(test<T>(0)) == sizeof(char);
		};

		template <typename T>
		struct has_traits_insert_test {
		private:
			typedef std::array<char, 1> one;
			typedef std::array<char, 2> two;

			template <typename C>
			static one test(decltype(&C::insert));
			template <typename C>
			static two test(...);

		public:
			static const bool value = sizeof(test<T>(0)) == sizeof(char);
		};

		template <typename T>
		struct has_traits_erase_test {
		private:
			typedef std::array<char, 1> one;
			typedef std::array<char, 2> two;

			template <typename C>
			static one test(decltype(&C::erase));
			template <typename C>
			static two test(...);

		public:
			static const bool value = sizeof(test<T>(0)) == sizeof(char);
		};

		template <typename T>
		struct has_traits_index_set_test {
		private:
			typedef std::array<char, 1> one;
			typedef std::array<char, 2> two;

			template <typename C>
			static one test(decltype(&C::index_set));
			template <typename C>
			static two test(...);

		public:
			static const bool value = sizeof(test<T>(0)) == sizeof(char);
		};

		template <typename T>
		struct has_traits_index_get_test {
		private:
			typedef std::array<char, 1> one;
			typedef std::array<char, 2> two;

			template <typename C>
			static one test(decltype(&C::index_get));
			template <typename C>
			static two test(...);

		public:
			static const bool value = sizeof(test<T>(0)) == sizeof(char);
		};

		template <typename T>
		struct has_traits_set_test {
		private:
			typedef std::array<char, 1> one;
			typedef std::array<char, 2> two;

			template <typename C>
			static one test(decltype(&C::set));
			template <typename C>
			static two test(...);

		public:
			static const bool value = sizeof(test<T>(0)) == sizeof(char);
		};

		template <typename T>
		struct has_traits_get_test {
		private:
			typedef std::array<char, 1> one;
			typedef std::array<char, 2> two;

			template <typename C>
			static one test(decltype(&C::get));
			template <typename C>
			static two test(...);

		public:
			static const bool value = sizeof(test<T>(0)) == sizeof(char);
		};

		template <typename T>
		struct has_traits_pairs_test {
		private:
			typedef std::array<char, 1> one;
			typedef std::array<char, 2> two;

			template <typename C>
			static one test(decltype(&C::pairs));
			template <typename C>
			static two test(...);

		public:
			static const bool value = sizeof(test<T>(0)) == sizeof(char);
		};

		template <typename T>
		struct has_traits_ipairs_test {
		private:
			typedef std::array<char, 1> one;
			typedef std::array<char, 2> two;

			template <typename C>
			static one test(decltype(&C::ipairs));
			template <typename C>
			static two test(...);

		public:
			static const bool value = sizeof(test<T>(0)) == sizeof(char);
		};

		template <typename T>
		struct has_traits_add_test {
		private:
			typedef std::array<char, 1> one;
			typedef std::array<char, 2> two;

			template <typename C>
			static one test(decltype(&C::add));
			template <typename C>
			static two test(...);

		public:
			static const bool value = sizeof(test<T>(0)) == sizeof(char);
		};

		template <typename T>
		using has_clear = meta::boolean<has_clear_test<T>::value>;

		template <typename T>
		using has_empty = meta::boolean<has_empty_test<T>::value>;

		template <typename T>
		using has_find = meta::boolean<has_find_test<T>::value>;

		template <typename T>
		using has_erase = meta::boolean<has_erase_test<T>::value>;

		template <typename T>
		using has_erase_after = meta::boolean<has_erase_after_test<T>::value>;

		template <typename T>
		using has_traits_get = meta::boolean<has_traits_get_test<T>::value>;

		template <typename T>
		using has_traits_set = meta::boolean<has_traits_set_test<T>::value>;

		template <typename T>
		using has_traits_index_get = meta::boolean<has_traits_index_get_test<T>::value>;

		template <typename T>
		using has_traits_index_set = meta::boolean<has_traits_index_set_test<T>::value>;

		template <typename T>
		using has_traits_pairs = meta::boolean<has_traits_pairs_test<T>::value>;

		template <typename T>
		using has_traits_ipairs = meta::boolean<has_traits_ipairs_test<T>::value>;

		template <typename T>
		using has_traits_add = meta::boolean<has_traits_add_test<T>::value>;

		template <typename T>
		using has_traits_size = meta::has_size<T>;

		template <typename T>
		using has_traits_clear = has_clear<T>;

		template <typename T>
		using has_traits_empty = has_empty<T>;

		template <typename T>
		using has_traits_find = meta::boolean<has_traits_find_test<T>::value>;

		template <typename T>
		using has_traits_insert = meta::boolean<has_traits_insert_test<T>::value>;

		template <typename T>
		using has_traits_erase = meta::boolean<has_traits_erase_test<T>::value>;

		template <typename T>
		struct is_forced_container : is_container<T> {};

		template <typename T>
		struct is_forced_container<as_container_t<T>> : std::true_type {};

		template <typename T>
		struct container_decay {
			typedef T type;
		};

		template <typename T>
		struct container_decay<as_container_t<T>> {
			typedef T type;
		};

		template <typename T>
		using container_decay_t = typename container_decay<meta::unqualified_t<T>>::type;

		template <typename T>
		decltype(auto) get_key(std::false_type, T&& t) {
			return std::forward<T>(t);
		}

		template <typename T>
		decltype(auto) get_key(std::true_type, T&& t) {
			return t.first;
		}

		template <typename T>
		decltype(auto) get_value(std::false_type, T&& t) {
			return std::forward<T>(t);
		}

		template <typename T>
		decltype(auto) get_value(std::true_type, T&& t) {
			return t.second;
		}

		template <typename X, typename = void>
		struct container_traits_default {
		private:
			typedef std::remove_pointer_t<meta::unwrap_unqualified_t<X>> T;

		public:
			typedef lua_nil_t iterator;
			typedef lua_nil_t value_type;

			static int get(lua_State* L) {
				return luaL_error(L, "sol: cannot call 'get(key)' on type '%s': it is not recognized as a container", detail::demangle<T>().c_str());
			}

			static int index_get(lua_State* L) {
				return luaL_error(L, "sol: cannot call 'container[key]' on type '%s': it is not recognized as a container", detail::demangle<T>().c_str());
			}

			static int set(lua_State* L) {
				return luaL_error(L, "sol: cannot call 'set(key, value)' on type '%s': it is not recognized as a container", detail::demangle<T>().c_str());
			}

			static int index_set(lua_State* L) {
				return luaL_error(L, "sol: cannot call 'container[key] = value' on type '%s': it is not recognized as a container", detail::demangle<T>().c_str());
			}

			static int add(lua_State* L) {
				return luaL_error(L, "sol: cannot call 'add' on type '%s': it is not recognized as a container", detail::demangle<T>().c_str());
			}

			static int insert(lua_State* L) {
				return luaL_error(L, "sol: cannot call 'insert' on type '%s': it is not recognized as a container", detail::demangle<T>().c_str());
			}

			static int find(lua_State* L) {
				return luaL_error(L, "sol: cannot call 'find' on type '%s': it is not recognized as a container", detail::demangle<T>().c_str());
			}

			static int size(lua_State* L) {
				return luaL_error(L, "sol: cannot call 'end' on type '%s': it is not recognized as a container", detail::demangle<T>().c_str());
			}

			static int clear(lua_State* L) {
				return luaL_error(L, "sol: cannot call 'clear' on type '%s': it is not recognized as a container", detail::demangle<T>().c_str());
			}

			static int empty(lua_State* L) {
				return luaL_error(L, "sol: cannot call 'empty' on type '%s': it is not recognized as a container", detail::demangle<T>().c_str());
			}

			static int erase(lua_State* L) {
				return luaL_error(L, "sol: cannot call 'erase' on type '%s': it is not recognized as a container", detail::demangle<T>().c_str());
			}

			static int pairs(lua_State* L) {
				return luaL_error(L, "sol: cannot call '__pairs' on type '%s': it is not recognized as a container", detail::demangle<T>().c_str());
			}

			static int ipairs(lua_State* L) {
				return luaL_error(L, "sol: cannot call '__ipairs' on type '%s': it is not recognized as a container", detail::demangle<T>().c_str());
			}

			static iterator begin(lua_State* L, T&) {
				luaL_error(L, "sol: cannot call 'being' on type '%s': it is not recognized as a container", detail::demangle<T>().c_str());
				return lua_nil;
			}

			static iterator end(lua_State* L, T&) {
				luaL_error(L, "sol: cannot call 'end' on type '%s': it is not recognized as a container", detail::demangle<T>().c_str());
				return lua_nil;
			}
		};

		template <typename X>
		struct container_traits_default<X, std::enable_if_t<meta::all<is_forced_container<meta::unqualified_t<X>>, meta::has_value_type<meta::unqualified_t<container_decay_t<X>>>, meta::has_iterator<meta::unqualified_t<container_decay_t<X>>>>::value>> {
		private:
			typedef std::remove_pointer_t<meta::unwrap_unqualified_t<container_decay_t<X>>> T;

		private:
			typedef container_traits<X> deferred_traits;
			typedef meta::is_associative<T> is_associative;
			typedef meta::is_lookup<T> is_lookup;
			typedef typename T::iterator iterator;
			typedef typename T::value_type value_type;
			typedef std::conditional_t<is_associative::value,
				value_type,
				std::conditional_t<is_lookup::value, std::pair<value_type, value_type>, std::pair<std::ptrdiff_t, value_type>>>
				KV;
			typedef typename KV::first_type K;
			typedef typename KV::second_type V;
			typedef decltype(*std::declval<iterator&>()) iterator_return;
			typedef typename meta::iterator_tag<iterator>::type iterator_category;
			typedef std::is_same<iterator_category, std::input_iterator_tag> is_input_iterator;
			typedef std::conditional_t<is_input_iterator::value,
				V,
				decltype(detail::deref(std::declval<std::conditional_t<is_associative::value, std::add_lvalue_reference_t<V>, iterator_return>>()))>
				push_type;
			typedef std::is_copy_assignable<V> is_copyable;
			typedef meta::neg<meta::any<
				std::is_const<V>, std::is_const<std::remove_reference_t<iterator_return>>, meta::neg<is_copyable>>>
				is_writable;
			typedef meta::unqualified_t<decltype(get_key(is_associative(), std::declval<std::add_lvalue_reference_t<value_type>>()))> key_type;
			typedef meta::all<std::is_integral<K>, meta::neg<meta::any<is_associative, is_lookup>>> is_linear_integral;

			struct iter {
				T& source;
				iterator it;
				std::size_t i;

				iter(T& source, iterator it)
				: source(source), it(std::move(it)), i(0) {
				}
			};

			static auto& get_src(lua_State* L) {
				typedef std::remove_pointer_t<meta::unwrap_unqualified_t<X>> Tu;
#ifdef SOL_SAFE_USERTYPE
				auto p = stack::check_get<Tu*>(L, 1);
				if (!p) {
					luaL_error(L, "sol: 'self' is not of type '%s' (pass 'self' as first argument with ':' or call on proper type)", detail::demangle<T>().c_str());
				}
				if (p.value() == nullptr) {
					luaL_error(L, "sol: 'self' argument is nil (pass 'self' as first argument with ':' or call on a '%s' type)", detail::demangle<T>().c_str());
				}
				return *p.value();
#else
				return stack::get<Tu>(L, 1);
#endif // Safe getting with error
			}

			static int get_associative(std::true_type, lua_State* L, iterator& it) {
				auto& v = *it;
				return stack::stack_detail::push_reference<push_type>(L, detail::deref(v.second));
			}

			static int get_associative(std::false_type, lua_State* L, iterator& it) {
				return stack::stack_detail::push_reference<push_type>(L, detail::deref(*it));
			}

			static int get_category(std::input_iterator_tag, lua_State* L, T& self, K& key) {
				if (key < 1) {
					return stack::push(L, lua_nil);
				}
				auto it = begin(L, self);
				auto e = end(L, self);
				if (it == e) {
					return stack::push(L, lua_nil);
				}
				while (key > 1) {
					--key;
					++it;
					if (it == e) {
						return stack::push(L, lua_nil);
					}
				}
				return get_associative(is_associative(), L, it);
			}

			static int get_category(std::random_access_iterator_tag, lua_State* L, T& self, K& key) {
				std::ptrdiff_t len = static_cast<std::ptrdiff_t>(size_start(L, self));
				if (key < 1 || key > len) {
					return stack::push(L, lua_nil);
				}
				--key;
				auto it = std::next(begin(L, self), key);
				return get_associative(is_associative(), L, it);
			}

			static int get_it(std::true_type, lua_State* L, T& self, K& key) {
				return get_category(iterator_category(), L, self, key);
			}

			static int get_comparative(std::true_type, lua_State* L, T& self, K& key) {
				auto fx = [&](const value_type& r) -> bool {
					return key == get_key(is_associative(), r);
				};
				auto e = end(L, self);
				auto it = std::find_if(begin(L, self), e, std::ref(fx));
				if (it == e) {
					return stack::push(L, lua_nil);
				}
				return get_associative(is_associative(), L, it);
			}

			static int get_comparative(std::false_type, lua_State* L, T&, K&) {
				return luaL_error(L, "cannot get this key on '%s': no suitable way to increment iterator and compare to key value '%s'", detail::demangle<T>().data(), detail::demangle<K>().data());
			}

			static int get_it(std::false_type, lua_State* L, T& self, K& key) {
				return get_comparative(meta::supports_op_equal<K, key_type>(), L, self, key);
			}

			static void set_associative(std::true_type, iterator& it, stack_object value) {
				auto& v = *it;
				v.second = value.as<V>();
			}

			static void set_associative(std::false_type, iterator& it, stack_object value) {
				auto& v = *it;
				v = value.as<V>();
			}

			static void set_writable(std::true_type, lua_State*, T&, iterator& it, stack_object value) {
				set_associative(is_associative(), it, std::move(value));
			}

			static void set_writable(std::false_type, lua_State* L, T&, iterator&, stack_object) {
				luaL_error(L, "cannot perform a 'set': '%s's iterator reference is not writable (non-copy-assignable or const)", detail::demangle<T>().data());
			}

			static void set_category(std::input_iterator_tag, lua_State* L, T& self, stack_object okey, stack_object value) {
				decltype(auto) key = okey.as<K>();
				auto e = end(L, self);
				auto it = begin(L, self);
				auto backit = it;
				for (; key > 1 && it != e; --key, ++it) {
					backit = it;
				}
				if (it == e) {
					if (key == 1) {
						add_copyable(is_copyable(), L, self, std::move(value), meta::has_insert_after<T>::value ? backit : it);
						return;
					}
					luaL_error(L, "out of bounds (too big) for set on '%s'", detail::demangle<T>().c_str());
					return;
				}
				set_writable(is_writable(), L, self, it, std::move(value));
			}

			static void set_category(std::random_access_iterator_tag, lua_State* L, T& self, stack_object okey, stack_object value) {
				decltype(auto) key = okey.as<K>();
				if (key < 1) {
					luaL_error(L, "sol: out of bounds (too small) for set on '%s'", detail::demangle<T>().c_str());
					return;
				}
				--key;
				std::ptrdiff_t len = static_cast<std::ptrdiff_t>(size_start(L, self));
				if (key == len) {
					add_copyable(is_copyable(), L, self, std::move(value));
					return;
				}
				else if (key > len) {
					luaL_error(L, "sol: out of bounds (too big) for set on '%s'", detail::demangle<T>().c_str());
					return;
				}
				auto it = std::next(begin(L, self), key);
				set_writable(is_writable(), L, self, it, std::move(value));
			}

			static void set_comparative(std::true_type, lua_State* L, T& self, stack_object okey, stack_object value) {
				decltype(auto) key = okey.as<K>();
				if (!is_writable::value) {
					luaL_error(L, "cannot perform a 'set': '%s's iterator reference is not writable (non-copy-assignable or const)", detail::demangle<T>().data());
					;
					return;
				}
				auto fx = [&](const value_type& r) -> bool {
					return key == get_key(is_associative(), r);
				};
				auto e = end(L, self);
				auto it = std::find_if(begin(L, self), e, std::ref(fx));
				if (it == e) {
					return;
				}
				set_writable(is_writable(), L, self, it, std::move(value));
			}

			static void set_comparative(std::false_type, lua_State* L, T&, stack_object, stack_object) {
				luaL_error(L, "cannot set this value on '%s': no suitable way to increment iterator or compare to '%s' key", detail::demangle<T>().data(), detail::demangle<K>().data());
			}

			static void set_associative_insert(std::true_type, lua_State*, T& self, iterator& it, K& key, stack_object value) {
				self.insert(it, value_type(key, value.as<V>()));
			}

			static void set_associative_insert(std::false_type, lua_State*, T& self, iterator& it, K& key, stack_object) {
				self.insert(it, key);
			}

			static void set_associative_find(std::true_type, lua_State* L, T& self, stack_object okey, stack_object value) {
				decltype(auto) key = okey.as<K>();
				auto it = self.find(key);
				if (it == end(L, self)) {
					set_associative_insert(is_associative(), L, self, it, key, std::move(value));
					return;
				}
				set_writable(is_writable(), L, self, it, std::move(value));
			}

			static void set_associative_find(std::false_type, lua_State* L, T& self, stack_object key, stack_object value) {
				set_comparative(meta::supports_op_equal<K, key_type>(), L, self, std::move(key), std::move(value));
			}

			static void set_it(std::true_type, lua_State* L, T& self, stack_object key, stack_object value) {
				set_category(iterator_category(), L, self, std::move(key), std::move(value));
			}

			static void set_it(std::false_type, lua_State* L, T& self, stack_object key, stack_object value) {
				set_associative_find(meta::all<has_find<T>, meta::any<is_associative, is_lookup>>(), L, self, std::move(key), std::move(value));
			}

			static int find_has_associative_lookup(std::true_type, lua_State* L, T& self) {
				decltype(auto) key = stack::get<K>(L, 2);
				auto it = self.find(key);
				if (it == end(L, self)) {
					return stack::push(L, lua_nil);
				}
				return get_associative(is_associative(), L, it);
			}

			static int find_has_associative_lookup(std::false_type, lua_State* L, T& self) {
				decltype(auto) value = stack::get<V>(L, 2);
				auto it = self.find(value);
				if (it == end(L, self)) {
					return stack::push(L, lua_nil);
				}
				return get_associative(is_associative(), L, it);
			}

			static int find_has(std::true_type, lua_State* L, T& self) {
				return find_has_associative_lookup(meta::any<is_lookup, is_associative>(), L, self);
			}

			static int find_associative_lookup(std::true_type, lua_State* L, iterator& it, std::size_t) {
				return get_associative(is_associative(), L, it);
			}

			static int find_associative_lookup(std::false_type, lua_State* L, iterator&, std::size_t index) {
				return stack::push(L, index);
			}

			static int find_comparative(std::false_type, lua_State* L, T&) {
				return luaL_error(L, "cannot call 'find' on '%s': there is no 'find' function and the value_type is not equality comparable", detail::demangle<T>().c_str());
			}

			static int find_comparative(std::true_type, lua_State* L, T& self) {
				decltype(auto) value = stack::get<V>(L, 2);
				auto it = begin(L, self);
				auto e = end(L, self);
				std::size_t index = 1;
				for (;; ++it, ++index) {
					if (it == e) {
						return stack::push(L, lua_nil);
					}
					if (value == get_value(is_associative(), *it)) {
						break;
					}
				}
				return find_associative_lookup(meta::any<is_lookup, is_associative>(), L, it, index);
			}

			static int find_has(std::false_type, lua_State* L, T& self) {
				return find_comparative(meta::supports_op_equal<V>(), L, self);
			}

			static void add_insert_after(std::false_type, lua_State* L, T& self, stack_object value, iterator&) {
				add_insert_after(std::false_type(), L, self, value);
			}

			static void add_insert_after(std::false_type, lua_State* L, T&, stack_object) {
				luaL_error(L, "cannot call 'add' on type '%s': no suitable insert/push_back C++ functions", detail::demangle<T>().data());
			}

			static void add_insert_after(std::true_type, lua_State*, T& self, stack_object value, iterator& at) {
				self.insert_after(at, value.as<V>());
			}

			static void add_insert_after(std::true_type, lua_State* L, T& self, stack_object value) {
				auto backit = self.before_begin();
				{
					auto e = end(L, self);
					for (auto it = begin(L, self); it != e; ++backit, ++it) {
					}
				}
				return add_insert_after(std::true_type(), L, self, value, backit);
			}

			static void add_insert(std::true_type, lua_State*, T& self, stack_object value, iterator& at) {
				self.insert(at, value.as<V>());
			}

			static void add_insert(std::true_type, lua_State* L, T& self, stack_object value) {
				auto at = end(L, self);
				add_insert(std::true_type(), L, self, value, at);
			}

			static void add_insert(std::false_type, lua_State* L, T& self, stack_object value, iterator& at) {
				return add_insert_after(meta::has_insert_after<T>(), L, self, std::move(value), at);
			}

			static void add_insert(std::false_type, lua_State* L, T& self, stack_object value) {
				return add_insert_after(meta::has_insert_after<T>(), L, self, std::move(value));
			}

			static void add_push_back(std::true_type, lua_State*, T& self, stack_object value, iterator&) {
				self.push_back(value.as<V>());
			}

			static void add_push_back(std::true_type, lua_State*, T& self, stack_object value) {
				self.push_back(value.as<V>());
			}

			static void add_push_back(std::false_type, lua_State* L, T& self, stack_object value, iterator& at) {
				add_insert(meta::has_insert<T>(), L, self, value, at);
			}

			static void add_push_back(std::false_type, lua_State* L, T& self, stack_object value) {
				add_insert(meta::has_insert<T>(), L, self, value);
			}

			static void add_associative(std::true_type, lua_State* L, T& self, stack_object key, iterator& at) {
				self.insert(at, value_type(key.as<K>(), stack::get<V>(L, 3)));
			}

			static void add_associative(std::true_type, lua_State* L, T& self, stack_object key) {
				auto at = end(L, self);
				add_associative(std::true_type(), L, self, std::move(key), at);
			}

			static void add_associative(std::false_type, lua_State* L, T& self, stack_object value, iterator& at) {
				add_push_back(meta::has_push_back<T>(), L, self, value, at);
			}

			static void add_associative(std::false_type, lua_State* L, T& self, stack_object value) {
				add_push_back(meta::has_push_back<T>(), L, self, value);
			}

			static void add_copyable(std::true_type, lua_State* L, T& self, stack_object value, iterator& at) {
				add_associative(is_associative(), L, self, std::move(value), at);
			}

			static void add_copyable(std::true_type, lua_State* L, T& self, stack_object value) {
				add_associative(is_associative(), L, self, value);
			}

			static void add_copyable(std::false_type, lua_State* L, T& self, stack_object value, iterator&) {
				add_copyable(std::false_type(), L, self, std::move(value));
			}

			static void add_copyable(std::false_type, lua_State* L, T&, stack_object) {
				luaL_error(L, "cannot call 'add' on '%s': value_type is non-copyable", detail::demangle<T>().data());
			}

			static void insert_lookup(std::true_type, lua_State* L, T& self, stack_object, stack_object value) {
				// TODO: should we warn or error about someone calling insert on an ordered / lookup container with no associativity?
				add_copyable(std::true_type(), L, self, std::move(value));
			}

			static void insert_lookup(std::false_type, lua_State* L, T& self, stack_object where, stack_object value) {
				auto it = begin(L, self);
				auto key = where.as<K>();
				--key;
				std::advance(it, key);
				self.insert(it, value.as<V>());
			}

			static void insert_after_has(std::true_type, lua_State* L, T& self, stack_object where, stack_object value) {
				auto key = where.as<K>();
				auto backit = self.before_begin();
				{
					--key;
					auto e = end(L, self);
					for (auto it = begin(L, self); key > 0; ++backit, ++it, --key) {
						if (backit == e) {
							luaL_error(L, "sol: out of bounds (too big) for set on '%s'", detail::demangle<T>().c_str());
							return;
						}
					}
				}
				self.insert_after(backit, value.as<V>());
			}

			static void insert_after_has(std::false_type, lua_State* L, T&, stack_object, stack_object) {
				luaL_error(L, "cannot call 'insert' on '%s': no suitable or similar functionality detected on this container", detail::demangle<T>().data());
			}

			static void insert_has(std::true_type, lua_State* L, T& self, stack_object key, stack_object value) {
				insert_lookup(meta::all<is_associative, is_lookup>(), L, self, std::move(key), std::move(value));
			}

			static void insert_has(std::false_type, lua_State* L, T& self, stack_object where, stack_object value) {
				insert_after_has(meta::has_insert_after<T>(), L, self, where, value);
			}

			static void insert_copyable(std::true_type, lua_State* L, T& self, stack_object key, stack_object value) {
				insert_has(meta::has_insert<T>(), L, self, std::move(key), std::move(value));
			}

			static void insert_copyable(std::false_type, lua_State* L, T&, stack_object, stack_object) {
				luaL_error(L, "cannot call 'insert' on '%s': value_type is non-copyable", detail::demangle<T>().data());
			}

			static void erase_integral(std::true_type, lua_State* L, T& self, K& key) {
				auto it = begin(L, self);
				--key;
				std::advance(it, key);
				self.erase(it);
			}

			static void erase_integral(std::false_type, lua_State* L, T& self, const K& key) {
				auto fx = [&](const value_type& r) -> bool {
					return key == r;
				};
				auto e = end(L, self);
				auto it = std::find_if(begin(L, self), e, std::ref(fx));
				if (it == e) {
					return;
				}
				self.erase(it);
			}

			static void erase_associative_lookup(std::true_type, lua_State*, T& self, const K& key) {
				self.erase(key);
			}

			static void erase_associative_lookup(std::false_type, lua_State* L, T& self, K& key) {
				erase_integral(std::is_integral<K>(), L, self, key);
			}

			static void erase_after_has(std::true_type, lua_State* L, T& self, K& key) {
				auto backit = self.before_begin();
				{
					--key;
					auto e = end(L, self);
					for (auto it = begin(L, self); key > 0; ++backit, ++it, --key) {
						if (backit == e) {
							luaL_error(L, "sol: out of bounds for erase on '%s'", detail::demangle<T>().c_str());
							return;
						}
					}
				}
				self.erase_after(backit);
			}

			static void erase_after_has(std::false_type, lua_State* L, T&, const K&) {
				luaL_error(L, "sol: cannot call erase on '%s'", detail::demangle<T>().c_str());
			}

			static void erase_has(std::true_type, lua_State* L, T& self, K& key) {
				erase_associative_lookup(meta::any<is_associative, is_lookup>(), L, self, key);
			}

			static void erase_has(std::false_type, lua_State* L, T& self, K& key) {
				erase_after_has(has_erase_after<T>(), L, self, key);
			}

			static auto size_has(std::false_type, lua_State* L, T& self) {
				return std::distance(deferred_traits::begin(L, self), deferred_traits::end(L, self));
			}

			static auto size_has(std::true_type, lua_State*, T& self) {
				return self.size();
			}

			static void clear_has(std::true_type, lua_State*, T& self) {
				self.clear();
			}

			static void clear_has(std::false_type, lua_State* L, T&) {
				luaL_error(L, "sol: cannot call clear on '%s'", detail::demangle<T>().c_str());
			}

			static bool empty_has(std::true_type, lua_State*, T& self) {
				return self.empty();
			}

			static bool empty_has(std::false_type, lua_State* L, T& self) {
				return deferred_traits::begin(L, self) == deferred_traits::end(L, self);
			}

			static int get_start(lua_State* L, T& self, K& key) {
				return get_it(is_linear_integral(), L, self, key);
			}

			static void set_start(lua_State* L, T& self, stack_object key, stack_object value) {
				set_it(is_linear_integral(), L, self, std::move(key), std::move(value));
			}

			static std::size_t size_start(lua_State* L, T& self) {
				return size_has(meta::has_size<T>(), L, self);
			}

			static void clear_start(lua_State* L, T& self) {
				clear_has(has_clear<T>(), L, self);
			}

			static bool empty_start(lua_State* L, T& self) {
				return empty_has(has_empty<T>(), L, self);
			}

			static void erase_start(lua_State* L, T& self, K& key) {
				erase_has(has_erase<T>(), L, self, key);
			}

			template <bool ip>
			static int next_associative(std::true_type, lua_State* L) {
				iter& i = stack::get<user<iter>>(L, 1);
				auto& source = i.source;
				auto& it = i.it;
				if (it == deferred_traits::end(L, source)) {
					return 0;
				}
				int p;
				if (ip) {
					p = stack::push_reference(L, it->first);
				}
				else {
					++i.i;
					p = stack::push_reference(L, i.i);
				}
				p += stack::stack_detail::push_reference<push_type>(L, detail::deref(it->second));
				std::advance(it, 1);
				return p;
			}

			template <bool ip>
			static int pairs_associative(std::true_type, lua_State* L) {
				auto& src = get_src(L);
				stack::push(L, next<ip>);
				stack::push<user<iter>>(L, src, deferred_traits::begin(L, src));
				stack::push(L, lua_nil);
				return 3;
			}

			template <bool>
			static int next_associative(std::false_type, lua_State* L) {
				iter& i = stack::get<user<iter>>(L, 1);
				auto& source = i.source;
				auto& it = i.it;
				K k = stack::get<K>(L, 2);
				if (it == deferred_traits::end(L, source)) {
					return 0;
				}
				int p;
				p = stack::push_reference(L, k + 1);
				p += stack::stack_detail::push_reference<push_type>(L, detail::deref(*it));
				std::advance(it, 1);
				return p;
			}

			template <bool ip>
			static int pairs_associative(std::false_type, lua_State* L) {
				auto& src = get_src(L);
				stack::push(L, next<ip>);
				stack::push<user<iter>>(L, src, deferred_traits::begin(L, src));
				stack::push(L, 0);
				return 3;
			}

			template <bool ip>
			static int next(lua_State* L) {
				return next_associative<ip>(is_associative(), L);
			}

		public:
			static int get(lua_State* L) {
				auto& self = get_src(L);
				decltype(auto) key = stack::get<K>(L);
				return get_start(L, self, key);
			}

			static int index_get(lua_State* L) {
				return get(L);
			}

			static int set(lua_State* L) {
				stack_object value = stack_object(L, raw_index(3));
				if (type_of(L, 3) == type::lua_nil) {
					return erase(L);
				}
				auto& self = get_src(L);
				set_start(L, self, stack_object(L, raw_index(2)), std::move(value));
				return 0;
			}

			static int index_set(lua_State* L) {
				return set(L);
			}

			static int add(lua_State* L) {
				auto& self = get_src(L);
				add_copyable(is_copyable(), L, self, stack_object(L, raw_index(2)));
				return 0;
			}

			static int insert(lua_State* L) {
				auto& self = get_src(L);
				insert_copyable(is_copyable(), L, self, stack_object(L, raw_index(2)), stack_object(L, raw_index(3)));
				return 0;
			}

			static int find(lua_State* L) {
				auto& self = get_src(L);
				return find_has(has_find<T>(), L, self);
			}

			static iterator begin(lua_State*, T& self) {
				using std::begin;
				return begin(self);
			}

			static iterator end(lua_State*, T& self) {
				using std::end;
				return end(self);
			}

			static int size(lua_State* L) {
				auto& self = get_src(L);
				std::size_t r = size_start(L, self);
				return stack::push(L, r);
			}

			static int clear(lua_State* L) {
				auto& self = get_src(L);
				clear_start(L, self);
				return 0;
			}

			static int erase(lua_State* L) {
				auto& self = get_src(L);
				decltype(auto) key = stack::get<K>(L, 2);
				erase_start(L, self, key);
				return 0;
			}

			static int empty(lua_State* L) {
				auto& self = get_src(L);
				return stack::push(L, empty_start(L, self));
			}

			static int pairs(lua_State* L) {
				return pairs_associative<false>(is_associative(), L);
			}

			static int ipairs(lua_State* L) {
				return pairs_associative<true>(is_associative(), L);
			}
		};

		template <typename X>
		struct container_traits_default<X, std::enable_if_t<std::is_array<std::remove_pointer_t<meta::unwrap_unqualified_t<X>>>::value>> {
		private:
			typedef std::remove_pointer_t<meta::unwrap_unqualified_t<X>> T;
			typedef container_traits<X> deferred_traits;

		public:
			typedef std::remove_extent_t<T> value_type;
			typedef value_type* iterator;

		private:
			struct iter {
				T& source;
				iterator it;

				iter(T& source, iterator it)
				: source(source), it(std::move(it)) {
				}
			};

			static auto& get_src(lua_State* L) {
				auto p = stack::check_get<T*>(L, 1);
#ifdef SOL_SAFE_USERTYPE
				if (!p || p.value() == nullptr) {
					luaL_error(L, "sol: 'self' argument is nil or not of type '%s' (pass 'self' as first argument with ':' or call on proper type)", detail::demangle<T>().c_str());
				}
#endif // Safe getting with error
				return *p.value();
			}

			static int find(std::true_type, lua_State* L) {
				T& self = get_src(L);
				decltype(auto) value = stack::get<value_type>(L, 2);
				std::size_t N = std::extent<T>::value;
				for (std::size_t idx = 0; idx < N; ++idx) {
					const auto& v = self[idx];
					if (v == value) {
						return stack::push(L, idx + 1);
					}
				}
				return stack::push(L, lua_nil);
			}

			static int find(std::false_type, lua_State* L) {
				return luaL_error(L, "sol: cannot call 'find' on '%s': no supported comparison operator for the value type", detail::demangle<T>().c_str());
			}

			static int next(lua_State* L) {
				iter& i = stack::get<user<iter>>(L, 1);
				auto& source = i.source;
				auto& it = i.it;
				std::size_t k = stack::get<std::size_t>(L, 2);
				if (it == deferred_traits::end(L, source)) {
					return 0;
				}
				int p;
				p = stack::push_reference(L, k + 1);
				p += stack::push_reference(L, detail::deref(*it));
				std::advance(it, 1);
				return p;
			}

		public:
			static int clear(lua_State* L) {
				return luaL_error(L, "sol: cannot call 'clear' on type '%s': cannot remove all items from a fixed array", detail::demangle<T>().c_str());
			}

			static int erase(lua_State* L) {
				return luaL_error(L, "sol: cannot call 'erase' on type '%s': cannot remove an item from fixed arrays", detail::demangle<T>().c_str());
			}

			static int add(lua_State* L) {
				return luaL_error(L, "sol: cannot call 'add' on type '%s': cannot add to fixed arrays", detail::demangle<T>().c_str());
			}

			static int insert(lua_State* L) {
				return luaL_error(L, "sol: cannot call 'insert' on type '%s': cannot insert new entries into fixed arrays", detail::demangle<T>().c_str());
			}

			static int get(lua_State* L) {
				T& self = get_src(L);
				std::ptrdiff_t idx = stack::get<std::ptrdiff_t>(L, 2);
				if (idx > static_cast<std::ptrdiff_t>(std::extent<T>::value) || idx < 1) {
					return stack::push(L, lua_nil);
				}
				--idx;
				return stack::push_reference(L, detail::deref(self[idx]));
			}

			static int index_get(lua_State* L) {
				return get(L);
			}

			static int set(lua_State* L) {
				T& self = get_src(L);
				std::ptrdiff_t idx = stack::get<std::ptrdiff_t>(L, 2);
				if (idx > static_cast<std::ptrdiff_t>(std::extent<T>::value)) {
					return luaL_error(L, "sol: index out of bounds (too big) for set on '%s'", detail::demangle<T>().c_str());
				}
				if (idx < 1) {
					return luaL_error(L, "sol: index out of bounds (too small) for set on '%s'", detail::demangle<T>().c_str());
				}
				--idx;
				self[idx] = stack::get<value_type>(L, 3);
				return 0;
			}

			static int index_set(lua_State* L) {
				return set(L);
			}

			static int find(lua_State* L) {
				return find(meta::supports_op_equal<value_type, value_type>(), L);
			}

			static int size(lua_State* L) {
				return stack::push(L, std::extent<T>::value);
			}

			static int empty(lua_State* L) {
				return stack::push(L, std::extent<T>::value > 0);
			}

			static int pairs(lua_State* L) {
				auto& src = get_src(L);
				stack::push(L, next);
				stack::push<user<iter>>(L, src, deferred_traits::begin(L, src));
				stack::push(L, 0);
				return 3;
			}

			static int ipairs(lua_State* L) {
				return pairs(L);
			}

			static iterator begin(lua_State*, T& self) {
				return std::addressof(self[0]);
			}

			static iterator end(lua_State*, T& self) {
				return std::addressof(self[0]) + std::extent<T>::value;
			}
		};

		template <typename X>
		struct container_traits_default<container_traits<X>> : container_traits_default<X> {};
	} // namespace container_detail

	template <typename T>
	struct container_traits : container_detail::container_traits_default<T> {};

} // namespace sol

#endif // SOL_CONTAINER_TRAITS_HPP
