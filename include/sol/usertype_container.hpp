// sol3

// The MIT License (MIT)

// Copyright (c) 2013-2020 Rapptz, ThePhD and contributors

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

#include <sol/traits.hpp>
#include <sol/stack.hpp>
#include <sol/object.hpp>

namespace sol {

	template <typename T>
	struct usertype_container;

	namespace container_detail {

		template <typename T>
		struct has_clear_test {
		private:
			template <typename C>
			static meta::sfinae_yes_t test(decltype(&C::clear));
			template <typename C>
			static meta::sfinae_no_t test(...);

		public:
			static constexpr bool value = std::is_same_v<decltype(test<T>(0)), meta::sfinae_yes_t>;
		};

		template <typename T>
		struct has_empty_test {
		private:
			template <typename C>
			static meta::sfinae_yes_t test(decltype(&C::empty));
			template <typename C>
			static meta::sfinae_no_t test(...);

		public:
			static constexpr bool value = std::is_same_v<decltype(test<T>(0)), meta::sfinae_yes_t>;
		};

		template <typename T>
		struct has_erase_after_test {
		private:
			template <typename C>
			static meta::sfinae_yes_t test(
			     decltype(std::declval<C>().erase_after(std::declval<std::add_rvalue_reference_t<typename C::const_iterator>>()))*);
			template <typename C>
			static meta::sfinae_no_t test(...);

		public:
			static constexpr bool value = std::is_same_v<decltype(test<T>(0)), meta::sfinae_yes_t>;
		};

		template <typename T, typename = void>
		struct has_find_test {
		private:
			template <typename C>
			static meta::sfinae_yes_t test(decltype(std::declval<C>().find(std::declval<std::add_rvalue_reference_t<typename C::value_type>>()))*);
			template <typename C>
			static meta::sfinae_no_t test(...);

		public:
			static constexpr bool value = std::is_same_v<decltype(test<T>(0)), meta::sfinae_yes_t>;
		};

		template <typename T>
		struct has_find_test<T, std::enable_if_t<meta::is_lookup<T>::value>> {
		private:
			template <typename C>
			static meta::sfinae_yes_t test(decltype(std::declval<C>().find(std::declval<std::add_rvalue_reference_t<typename C::key_type>>()))*);
			template <typename C>
			static meta::sfinae_no_t test(...);

		public:
			static constexpr bool value = std::is_same_v<decltype(test<T>(0)), meta::sfinae_yes_t>;
		};

		template <typename T>
		struct has_erase_test {
		private:
			template <typename C>
			static meta::sfinae_yes_t test(decltype(std::declval<C>().erase(std::declval<typename C::iterator>()))*);
			template <typename C>
			static meta::sfinae_no_t test(...);

		public:
			static constexpr bool value = std::is_same_v<decltype(test<T>(0)), meta::sfinae_yes_t>;
		};

		template <typename T>
		struct has_erase_key_test {
		private:
			template <typename C>
			static meta::sfinae_yes_t test(decltype(std::declval<C>().erase(std::declval<typename C::key_type>()))*);
			template <typename C>
			static meta::sfinae_no_t test(...);

		public:
			static constexpr bool value = std::is_same_v<decltype(test<T>(0)), meta::sfinae_yes_t>;
		};

		template <typename T>
		struct has_traits_find_test {
		private:
			template <typename C>
			static meta::sfinae_yes_t test(decltype(&C::find));
			template <typename C>
			static meta::sfinae_no_t test(...);

		public:
			static constexpr bool value = std::is_same_v<decltype(test<T>(0)), meta::sfinae_yes_t>;
		};

		template <typename T>
		struct has_traits_index_of_test {
		private:
			template <typename C>
			static meta::sfinae_yes_t test(decltype(&C::index_of));
			template <typename C>
			static meta::sfinae_no_t test(...);

		public:
			static constexpr bool value = std::is_same_v<decltype(test<T>(0)), meta::sfinae_yes_t>;
		};

		template <typename T>
		struct has_traits_insert_test {
		private:
			template <typename C>
			static meta::sfinae_yes_t test(decltype(&C::insert));
			template <typename C>
			static meta::sfinae_no_t test(...);

		public:
			static constexpr bool value = std::is_same_v<decltype(test<T>(0)), meta::sfinae_yes_t>;
		};

		template <typename T>
		struct has_traits_erase_test {
		private:
			template <typename C>
			static meta::sfinae_yes_t test(decltype(&C::erase));
			template <typename C>
			static meta::sfinae_no_t test(...);

		public:
			static constexpr bool value = std::is_same_v<decltype(test<T>(0)), meta::sfinae_yes_t>;
		};

		template <typename T>
		struct has_traits_index_set_test {
		private:
			template <typename C>
			static meta::sfinae_yes_t test(decltype(&C::index_set));
			template <typename C>
			static meta::sfinae_no_t test(...);

		public:
			static constexpr bool value = std::is_same_v<decltype(test<T>(0)), meta::sfinae_yes_t>;
		};

		template <typename T>
		struct has_traits_index_get_test {
		private:
			template <typename C>
			static meta::sfinae_yes_t test(decltype(&C::index_get));
			template <typename C>
			static meta::sfinae_no_t test(...);

		public:
			static constexpr bool value = std::is_same_v<decltype(test<T>(0)), meta::sfinae_yes_t>;
		};

		template <typename T>
		struct has_traits_set_test {
		private:
			template <typename C>
			static meta::sfinae_yes_t test(decltype(&C::set));
			template <typename C>
			static meta::sfinae_no_t test(...);

		public:
			static constexpr bool value = std::is_same_v<decltype(test<T>(0)), meta::sfinae_yes_t>;
		};

		template <typename T>
		struct has_traits_get_test {
		private:
			template <typename C>
			static meta::sfinae_yes_t test(decltype(&C::get));
			template <typename C>
			static meta::sfinae_no_t test(...);

		public:
			static constexpr bool value = std::is_same_v<decltype(test<T>(0)), meta::sfinae_yes_t>;
		};

		template <typename T>
		struct has_traits_at_test {
		private:
			template <typename C>
			static meta::sfinae_yes_t test(decltype(&C::at));
			template <typename C>
			static meta::sfinae_no_t test(...);

		public:
			static constexpr bool value = std::is_same_v<decltype(test<T>(0)), meta::sfinae_yes_t>;
		};

		template <typename T>
		struct has_traits_pairs_test {
		private:
			template <typename C>
			static meta::sfinae_yes_t test(decltype(&C::pairs));
			template <typename C>
			static meta::sfinae_no_t test(...);

		public:
			static constexpr bool value = std::is_same_v<decltype(test<T>(0)), meta::sfinae_yes_t>;
		};

		template <typename T>
		struct has_traits_ipairs_test {
		private:
			template <typename C>
			static meta::sfinae_yes_t test(decltype(&C::ipairs));
			template <typename C>
			static meta::sfinae_no_t test(...);

		public:
			static constexpr bool value = std::is_same_v<decltype(test<T>(0)), meta::sfinae_yes_t>;
		};

		template <typename T>
		struct has_traits_next_test {
		private:
			template <typename C>
			static meta::sfinae_yes_t test(decltype(&C::next));
			template <typename C>
			static meta::sfinae_no_t test(...);

		public:
			static constexpr bool value = std::is_same_v<decltype(test<T>(0)), meta::sfinae_yes_t>;
		};

		template <typename T>
		struct has_traits_add_test {
		private:
			template <typename C>
			static meta::sfinae_yes_t test(decltype(&C::add));
			template <typename C>
			static meta::sfinae_no_t test(...);

		public:
			static constexpr bool value = std::is_same_v<decltype(test<T>(0)), meta::sfinae_yes_t>;
		};

		template <typename T>
		struct has_traits_size_test {
		private:
			template <typename C>
			static meta::sfinae_yes_t test(decltype(&C::size));
			template <typename C>
			static meta::sfinae_no_t test(...);

		public:
			static constexpr bool value = std::is_same_v<decltype(test<T>(0)), meta::sfinae_yes_t>;
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
		using has_erase_key = meta::boolean<has_erase_key_test<T>::value>;

		template <typename T>
		using has_erase_after = meta::boolean<has_erase_after_test<T>::value>;

		template <typename T>
		using has_traits_get = meta::boolean<has_traits_get_test<T>::value>;

		template <typename T>
		using has_traits_at = meta::boolean<has_traits_at_test<T>::value>;

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
		using has_traits_next = meta::boolean<has_traits_next_test<T>::value>;

		template <typename T>
		using has_traits_add = meta::boolean<has_traits_add_test<T>::value>;

		template <typename T>
		using has_traits_size = meta::boolean<has_traits_size_test<T>::value>;

		template <typename T>
		using has_traits_clear = has_clear<T>;

		template <typename T>
		using has_traits_empty = has_empty<T>;

		template <typename T>
		using has_traits_find = meta::boolean<has_traits_find_test<T>::value>;

		template <typename T>
		using has_traits_index_of = meta::boolean<has_traits_index_of_test<T>::value>;

		template <typename T>
		using has_traits_insert = meta::boolean<has_traits_insert_test<T>::value>;

		template <typename T>
		using has_traits_erase = meta::boolean<has_traits_erase_test<T>::value>;

		template <typename T>
		struct is_forced_container : is_container<T> { };

		template <typename T>
		struct is_forced_container<as_container_t<T>> : std::true_type { };

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
		struct usertype_container_default {
		private:
			typedef std::remove_pointer_t<meta::unwrap_unqualified_t<X>> T;

		public:
			typedef lua_nil_t iterator;
			typedef lua_nil_t value_type;

			static int at(lua_State* L) {
				return luaL_error(L, "sol: cannot call 'at(index)' on type '%s': it is not recognized as a container", detail::demangle<T>().c_str());
			}

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
				return luaL_error(
				     L, "sol: cannot call 'container[key] = value' on type '%s': it is not recognized as a container", detail::demangle<T>().c_str());
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

			static int index_of(lua_State* L) {
				return luaL_error(L, "sol: cannot call 'index_of' on type '%s': it is not recognized as a container", detail::demangle<T>().c_str());
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

			static int next(lua_State* L) {
				return luaL_error(L, "sol: cannot call 'next' on type '%s': it is not recognized as a container", detail::demangle<T>().c_str());
			}

			static int pairs(lua_State* L) {
				return luaL_error(L, "sol: cannot call '__pairs/pairs' on type '%s': it is not recognized as a container", detail::demangle<T>().c_str());
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
		struct usertype_container_default<X,
		     std::enable_if_t<meta::all<is_forced_container<meta::unqualified_t<X>>, meta::has_value_type<meta::unqualified_t<container_decay_t<X>>>,
		          meta::has_iterator<meta::unqualified_t<container_decay_t<X>>>>::value>> {
		private:
			using T = std::remove_pointer_t<meta::unwrap_unqualified_t<container_decay_t<X>>>;

		private:
			using deferred_uc = usertype_container<X>;
			using is_associative = meta::is_associative<T>;
			using is_lookup = meta::is_lookup<T>;
			using is_ordered = meta::is_ordered<T>;
			using is_matched_lookup = meta::is_matched_lookup<T>;
			using iterator = typename T::iterator;
			using value_type = typename T::value_type;
			typedef meta::conditional_t<is_matched_lookup::value, std::pair<value_type, value_type>,
			     meta::conditional_t<is_associative::value || is_lookup::value, value_type, std::pair<std::ptrdiff_t, value_type>>>
			     KV;
			typedef typename KV::first_type K;
			typedef typename KV::second_type V;
			typedef meta::conditional_t<is_matched_lookup::value, std::ptrdiff_t, K> next_K;
			typedef decltype(*std::declval<iterator&>()) iterator_return;
			typedef meta::conditional_t<is_associative::value || is_matched_lookup::value, std::add_lvalue_reference_t<V>,
			     meta::conditional_t<is_lookup::value, V, iterator_return>>
			     captured_type;
			typedef typename meta::iterator_tag<iterator>::type iterator_category;
			typedef std::is_same<iterator_category, std::input_iterator_tag> is_input_iterator;
			typedef meta::conditional_t<is_input_iterator::value, V, decltype(detail::deref_move_only(std::declval<captured_type>()))> push_type;
			typedef std::is_copy_assignable<V> is_copyable;
			typedef meta::neg<meta::any<std::is_const<V>, std::is_const<std::remove_reference_t<iterator_return>>, meta::neg<is_copyable>>> is_writable;
			typedef meta::unqualified_t<decltype(get_key(is_associative(), std::declval<std::add_lvalue_reference_t<value_type>>()))> key_type;
			typedef meta::all<std::is_integral<K>, meta::neg<meta::any<is_associative, is_lookup>>> is_linear_integral;

			struct iter {
				T& source;
				iterator it;
				std::size_t index;

				iter(T& source_, iterator it_) : source(source_), it(std::move(it_)), index(0) {
				}

				~iter() {
				}
			};

			static auto& get_src(lua_State* L) {
#if SOL_IS_ON(SOL_SAFE_USERTYPE_I_)
				auto p = stack::unqualified_check_get<T*>(L, 1);
				if (!p) {
					luaL_error(L,
					     "sol: 'self' is not of type '%s' (pass 'self' as first argument with ':' or call on proper type)",
					     detail::demangle<T>().c_str());
				}
				if (p.value() == nullptr) {
					luaL_error(
					     L, "sol: 'self' argument is nil (pass 'self' as first argument with ':' or call on a '%s' type)", detail::demangle<T>().c_str());
				}
				return *p.value();
#else
				return stack::unqualified_get<T>(L, 1);
#endif // Safe getting with error
			}

			static detail::error_result at_category(std::input_iterator_tag, lua_State* L, T& self, std::ptrdiff_t pos) {
				pos += deferred_uc::index_adjustment(L, self);
				if (pos < 0) {
					return stack::push(L, lua_nil);
				}
				auto it = deferred_uc::begin(L, self);
				auto e = deferred_uc::end(L, self);
				if (it == e) {
					return stack::push(L, lua_nil);
				}
				while (pos > 0) {
					--pos;
					++it;
					if (it == e) {
						return stack::push(L, lua_nil);
					}
				}
				return get_associative(is_associative(), L, it);
			}

			static detail::error_result at_category(std::random_access_iterator_tag, lua_State* L, T& self, std::ptrdiff_t pos) {
				std::ptrdiff_t len = static_cast<std::ptrdiff_t>(size_start(L, self));
				pos += deferred_uc::index_adjustment(L, self);
				if (pos < 0 || pos >= len) {
					return stack::push(L, lua_nil);
				}
				auto it = std::next(deferred_uc::begin(L, self), pos);
				return get_associative(is_associative(), L, it);
			}

			static detail::error_result at_start(lua_State* L, T& self, std::ptrdiff_t pos) {
				return at_category(iterator_category(), L, self, pos);
			}

			template <typename Iter>
			static detail::error_result get_associative(std::true_type, lua_State* L, Iter& it) {
				decltype(auto) v = *it;
				return stack::stack_detail::push_reference<push_type>(L, detail::deref_move_only(v.second));
			}

			template <typename Iter>
			static detail::error_result get_associative(std::false_type, lua_State* L, Iter& it) {
				return stack::stack_detail::push_reference<push_type>(L, detail::deref_move_only(*it));
			}

			static detail::error_result get_category(std::input_iterator_tag, lua_State* L, T& self, K& key) {
				key = static_cast<K>(key + deferred_uc::index_adjustment(L, self));
				if (key < 0) {
					return stack::push(L, lua_nil);
				}
				auto it = deferred_uc::begin(L, self);
				auto e = deferred_uc::end(L, self);
				if (it == e) {
					return stack::push(L, lua_nil);
				}
				while (key > 0) {
					--key;
					++it;
					if (it == e) {
						return stack::push(L, lua_nil);
					}
				}
				return get_associative(is_associative(), L, it);
			}

			static detail::error_result get_category(std::random_access_iterator_tag, lua_State* L, T& self, K& key) {
				std::ptrdiff_t len = static_cast<std::ptrdiff_t>(size_start(L, self));
				key = static_cast<K>(static_cast<std::ptrdiff_t>(key) + deferred_uc::index_adjustment(L, self));
				if (key < 0 || key >= len) {
					return stack::push(L, lua_nil);
				}
				auto it = std::next(deferred_uc::begin(L, self), key);
				return get_associative(is_associative(), L, it);
			}

			static detail::error_result get_it(std::true_type, lua_State* L, T& self, K& key) {
				return get_category(iterator_category(), L, self, key);
			}

			static detail::error_result get_comparative(std::true_type, lua_State* L, T& self, K& key) {
				auto fx = [&](const value_type& r) -> bool { return key == get_key(is_associative(), r); };
				auto e = deferred_uc::end(L, self);
				auto it = std::find_if(deferred_uc::begin(L, self), e, std::ref(fx));
				if (it == e) {
					return stack::push(L, lua_nil);
				}
				return get_associative(is_associative(), L, it);
			}

			static detail::error_result get_comparative(std::false_type, lua_State*, T&, K&) {
				return detail::error_result("cannot get this key on '%s': no suitable way to increment iterator and compare to key value '%s'",
				     detail::demangle<T>().data(),
				     detail::demangle<K>().data());
			}

			static detail::error_result get_it(std::false_type, lua_State* L, T& self, K& key) {
				return get_comparative(meta::supports_op_equal<K, key_type>(), L, self, key);
			}

			static detail::error_result set_associative(std::true_type, iterator& it, stack_object value) {
				decltype(auto) v = *it;
				v.second = value.as<V>();
				return {};
			}

			static detail::error_result set_associative(std::false_type, iterator& it, stack_object value) {
				decltype(auto) v = *it;
				v = value.as<V>();
				return {};
			}

			static detail::error_result set_writable(std::true_type, lua_State*, T&, iterator& it, stack_object value) {
				return set_associative(is_associative(), it, std::move(value));
			}

			static detail::error_result set_writable(std::false_type, lua_State*, T&, iterator&, stack_object) {
				return detail::error_result(
				     "cannot perform a 'set': '%s's iterator reference is not writable (non-copy-assignable or const)", detail::demangle<T>().data());
			}

			static detail::error_result set_category(std::input_iterator_tag, lua_State* L, T& self, stack_object okey, stack_object value) {
				decltype(auto) key = okey.as<K>();
				key = static_cast<K>(static_cast<std::ptrdiff_t>(key) + deferred_uc::index_adjustment(L, self));
				auto e = deferred_uc::end(L, self);
				auto it = deferred_uc::begin(L, self);
				auto backit = it;
				for (; key > 0 && it != e; --key, ++it) {
					backit = it;
				}
				if (it == e) {
					if (key == 0) {
						return add_copyable(is_copyable(), L, self, std::move(value), meta::has_insert_after<T>::value ? backit : it);
					}
					return detail::error_result("out of bounds (too big) for set on '%s'", detail::demangle<T>().c_str());
				}
				return set_writable(is_writable(), L, self, it, std::move(value));
			}

			static detail::error_result set_category(std::random_access_iterator_tag, lua_State* L, T& self, stack_object okey, stack_object value) {
				decltype(auto) key = okey.as<K>();
				key = static_cast<K>(static_cast<std::ptrdiff_t>(key) + deferred_uc::index_adjustment(L, self));
				if (key < 0) {
					return detail::error_result("sol: out of bounds (too small) for set on '%s'", detail::demangle<T>().c_str());
				}
				std::ptrdiff_t len = static_cast<std::ptrdiff_t>(size_start(L, self));
				if (key == len) {
					return add_copyable(is_copyable(), L, self, std::move(value));
				}
				else if (key >= len) {
					return detail::error_result("sol: out of bounds (too big) for set on '%s'", detail::demangle<T>().c_str());
				}
				auto it = std::next(deferred_uc::begin(L, self), key);
				return set_writable(is_writable(), L, self, it, std::move(value));
			}

			static detail::error_result set_comparative(std::true_type, lua_State* L, T& self, stack_object okey, stack_object value) {
				decltype(auto) key = okey.as<K>();
				if (!is_writable::value) {
					return detail::error_result(
					     "cannot perform a 'set': '%s's iterator reference is not writable (non-copy-assignable or const)", detail::demangle<T>().data());
				}
				auto fx = [&](const value_type& r) -> bool { return key == get_key(is_associative(), r); };
				auto e = deferred_uc::end(L, self);
				auto it = std::find_if(deferred_uc::begin(L, self), e, std::ref(fx));
				if (it == e) {
					return {};
				}
				return set_writable(is_writable(), L, self, it, std::move(value));
			}

			static detail::error_result set_comparative(std::false_type, lua_State*, T&, stack_object, stack_object) {
				return detail::error_result("cannot set this value on '%s': no suitable way to increment iterator or compare to '%s' key",
				     detail::demangle<T>().data(),
				     detail::demangle<K>().data());
			}

			template <typename Iter>
			static detail::error_result set_associative_insert(std::true_type, lua_State*, T& self, Iter& it, K& key, stack_object value) {
				if constexpr (meta::has_insert<T>::value) {
					self.insert(it, value_type(key, value.as<V>()));
					return {};
				}
				else {
					(void)self;
					(void)it;
					(void)key;
					return detail::error_result(
					     "cannot call 'set' on '%s': there is no 'insert' function on this associative type", detail::demangle<T>().c_str());
				}
			}

			template <typename Iter>
			static detail::error_result set_associative_insert(std::false_type, lua_State*, T& self, Iter& it, K& key, stack_object) {
				if constexpr (meta::has_insert<T>::value) {
					self.insert(it, key);
					return {};
				}
				else {
					(void)self;
					(void)it;
					(void)key;
					return detail::error_result(
					     "cannot call 'set' on '%s': there is no 'insert' function on this non-associative type", detail::demangle<T>().c_str());
				}
			}

			static detail::error_result set_associative_find(std::true_type, lua_State* L, T& self, stack_object okey, stack_object value) {
				decltype(auto) key = okey.as<K>();
				auto it = self.find(key);
				if (it == deferred_uc::end(L, self)) {
					return set_associative_insert(is_associative(), L, self, it, key, std::move(value));
				}
				return set_writable(is_writable(), L, self, it, std::move(value));
			}

			static detail::error_result set_associative_find(std::false_type, lua_State* L, T& self, stack_object key, stack_object value) {
				return set_comparative(meta::supports_op_equal<K, key_type>(), L, self, std::move(key), std::move(value));
			}

			static detail::error_result set_it(std::true_type, lua_State* L, T& self, stack_object key, stack_object value) {
				return set_category(iterator_category(), L, self, std::move(key), std::move(value));
			}

			static detail::error_result set_it(std::false_type, lua_State* L, T& self, stack_object key, stack_object value) {
				return set_associative_find(meta::all<has_find<T>, meta::any<is_associative, is_lookup>>(), L, self, std::move(key), std::move(value));
			}

			template <bool idx_of = false>
			static detail::error_result find_has_associative_lookup(std::true_type, lua_State* L, T& self) {
				if constexpr (!is_ordered::value && idx_of) {
					(void)L;
					(void)self;
					return detail::error_result("cannot perform an 'index_of': '%s's is not an ordered container", detail::demangle<T>().data());
				}
				else {
					decltype(auto) key = stack::unqualified_get<K>(L, 2);
					auto it = self.find(key);
					if (it == deferred_uc::end(L, self)) {
						return stack::push(L, lua_nil);
					}
					if constexpr (idx_of) {
						auto dist = std::distance(deferred_uc::begin(L, self), it);
						dist -= deferred_uc::index_adjustment(L, self);
						return stack::push(L, dist);
					}
					else {
						return get_associative(is_associative(), L, it);
					}
				}
			}

			template <bool idx_of = false>
			static detail::error_result find_has_associative_lookup(std::false_type, lua_State* L, T& self) {
				if constexpr (!is_ordered::value && idx_of) {
					(void)L;
					(void)self;
					return detail::error_result("cannot perform an 'index_of': '%s's is not an ordered container", detail::demangle<T>().data());
				}
				else {
					decltype(auto) value = stack::unqualified_get<V>(L, 2);
					auto it = self.find(value);
					if (it == deferred_uc::end(L, self)) {
						return stack::push(L, lua_nil);
					}
					if constexpr (idx_of) {
						auto dist = std::distance(deferred_uc::begin(L, self), it);
						dist -= deferred_uc::index_adjustment(L, self);
						return stack::push(L, dist);
					}
					else {
						return get_associative(is_associative(), L, it);
					}
				}
			}

			template <bool idx_of = false>
			static detail::error_result find_has(std::true_type, lua_State* L, T& self) {
				return find_has_associative_lookup<idx_of>(meta::any<is_lookup, is_associative>(), L, self);
			}

			template <typename Iter>
			static detail::error_result find_associative_lookup(std::true_type, lua_State* L, T&, Iter& it, std::size_t) {
				return get_associative(is_associative(), L, it);
			}

			template <typename Iter>
			static detail::error_result find_associative_lookup(std::false_type, lua_State* L, T& self, Iter&, std::size_t idx) {
				idx = static_cast<std::size_t>(static_cast<std::ptrdiff_t>(idx) - deferred_uc::index_adjustment(L, self));
				return stack::push(L, idx);
			}

			template <bool = false>
			static detail::error_result find_comparative(std::false_type, lua_State*, T&) {
				return detail::error_result("cannot call 'find' on '%s': there is no 'find' function and the value_type is not equality comparable",
				     detail::demangle<T>().c_str());
			}

			template <bool idx_of = false>
			static detail::error_result find_comparative(std::true_type, lua_State* L, T& self) {
				decltype(auto) value = stack::unqualified_get<V>(L, 2);
				auto it = deferred_uc::begin(L, self);
				auto e = deferred_uc::end(L, self);
				std::size_t idx = 0;
				for (;; ++it, ++idx) {
					if (it == e) {
						return stack::push(L, lua_nil);
					}
					if (value == get_value(is_associative(), *it)) {
						break;
					}
				}
				return find_associative_lookup(meta::all<meta::boolean<!idx_of>, meta::any<is_lookup, is_associative>>(), L, self, it, idx);
			}

			template <bool idx_of = false>
			static detail::error_result find_has(std::false_type, lua_State* L, T& self) {
				return find_comparative<idx_of>(meta::supports_op_equal<V>(), L, self);
			}

			template <typename Iter>
			static detail::error_result add_insert_after(std::false_type, lua_State* L, T& self, stack_object value, Iter&) {
				return add_insert_after(std::false_type(), L, self, value);
			}

			static detail::error_result add_insert_after(std::false_type, lua_State*, T&, stack_object) {
				return detail::error_result("cannot call 'add' on type '%s': no suitable insert/push_back C++ functions", detail::demangle<T>().data());
			}

			template <typename Iter>
			static detail::error_result add_insert_after(std::true_type, lua_State*, T& self, stack_object value, Iter& pos) {
				self.insert_after(pos, value.as<V>());
				return {};
			}

			static detail::error_result add_insert_after(std::true_type, lua_State* L, T& self, stack_object value) {
				auto backit = self.before_begin();
				{
					auto e = deferred_uc::end(L, self);
					for (auto it = deferred_uc::begin(L, self); it != e; ++backit, ++it) { }
				}
				return add_insert_after(std::true_type(), L, self, value, backit);
			}

			template <typename Iter>
			static detail::error_result add_insert(std::true_type, lua_State*, T& self, stack_object value, Iter& pos) {
				self.insert(pos, value.as<V>());
				return {};
			}

			static detail::error_result add_insert(std::true_type, lua_State* L, T& self, stack_object value) {
				auto pos = deferred_uc::end(L, self);
				return add_insert(std::true_type(), L, self, value, pos);
			}

			template <typename Iter>
			static detail::error_result add_insert(std::false_type, lua_State* L, T& self, stack_object value, Iter& pos) {
				return add_insert_after(meta::has_insert_after<T>(), L, self, std::move(value), pos);
			}

			static detail::error_result add_insert(std::false_type, lua_State* L, T& self, stack_object value) {
				return add_insert_after(meta::has_insert_after<T>(), L, self, std::move(value));
			}

			template <typename Iter>
			static detail::error_result add_push_back(std::true_type, lua_State*, T& self, stack_object value, Iter&) {
				self.push_back(value.as<V>());
				return {};
			}

			static detail::error_result add_push_back(std::true_type, lua_State*, T& self, stack_object value) {
				self.push_back(value.as<V>());
				return {};
			}

			template <typename Iter>
			static detail::error_result add_push_back(std::false_type, lua_State* L, T& self, stack_object value, Iter& pos) {
				return add_insert(meta::has_insert<T>(), L, self, value, pos);
			}

			static detail::error_result add_push_back(std::false_type, lua_State* L, T& self, stack_object value) {
				return add_insert(meta::has_insert<T>(), L, self, value);
			}

			template <typename Iter>
			static detail::error_result add_associative(std::true_type, lua_State* L, T& self, stack_object key, Iter& pos) {
				if constexpr (meta::has_insert<T>::value) {
					self.insert(pos, value_type(key.as<K>(), stack::unqualified_get<V>(L, 3)));
					return {};
				}
				else {
					(void)L;
					(void)self;
					(void)key;
					(void)pos;
					return detail::error_result(
					     "cannot call 'insert' on '%s': there is no 'insert' function on this associative type", detail::demangle<T>().c_str());
				}
			}

			static detail::error_result add_associative(std::true_type, lua_State* L, T& self, stack_object key) {
				auto pos = deferred_uc::end(L, self);
				return add_associative(std::true_type(), L, self, std::move(key), pos);
			}

			template <typename Iter>
			static detail::error_result add_associative(std::false_type, lua_State* L, T& self, stack_object value, Iter& pos) {
				return add_push_back(meta::has_push_back<T>(), L, self, value, pos);
			}

			static detail::error_result add_associative(std::false_type, lua_State* L, T& self, stack_object value) {
				return add_push_back(meta::has_push_back<T>(), L, self, value);
			}

			template <typename Iter>
			static detail::error_result add_copyable(std::true_type, lua_State* L, T& self, stack_object value, Iter& pos) {
				return add_associative(is_associative(), L, self, std::move(value), pos);
			}

			static detail::error_result add_copyable(std::true_type, lua_State* L, T& self, stack_object value) {
				return add_associative(is_associative(), L, self, value);
			}

			template <typename Iter>
			static detail::error_result add_copyable(std::false_type, lua_State* L, T& self, stack_object value, Iter&) {
				return add_copyable(std::false_type(), L, self, std::move(value));
			}

			static detail::error_result add_copyable(std::false_type, lua_State*, T&, stack_object) {
				return detail::error_result("cannot call 'add' on '%s': value_type is non-copyable", detail::demangle<T>().data());
			}

			static detail::error_result insert_lookup(std::true_type, lua_State* L, T& self, stack_object, stack_object value) {
				// TODO: should we warn or error about someone calling insert on an ordered / lookup container with no associativity?
				return add_copyable(std::true_type(), L, self, std::move(value));
			}

			static detail::error_result insert_lookup(std::false_type, lua_State* L, T& self, stack_object where, stack_object value) {
				auto it = deferred_uc::begin(L, self);
				auto key = where.as<K>();
				key = static_cast<K>(static_cast<std::ptrdiff_t>(key) + deferred_uc::index_adjustment(L, self));
				std::advance(it, key);
				self.insert(it, value.as<V>());
				return {};
			}

			static detail::error_result insert_after_has(std::true_type, lua_State* L, T& self, stack_object where, stack_object value) {
				auto key = where.as<K>();
				auto backit = self.before_begin();
				{
					key = static_cast<K>(static_cast<std::ptrdiff_t>(key) + deferred_uc::index_adjustment(L, self));
					auto e = deferred_uc::end(L, self);
					for (auto it = deferred_uc::begin(L, self); key > 0; ++backit, ++it, --key) {
						if (backit == e) {
							return detail::error_result("sol: out of bounds (too big) for set on '%s'", detail::demangle<T>().c_str());
						}
					}
				}
				self.insert_after(backit, value.as<V>());
				return {};
			}

			static detail::error_result insert_after_has(std::false_type, lua_State*, T&, stack_object, stack_object) {
				return detail::error_result(
				     "cannot call 'insert' on '%s': no suitable or similar functionality detected on this container", detail::demangle<T>().data());
			}

			static detail::error_result insert_has(std::true_type, lua_State* L, T& self, stack_object key, stack_object value) {
				return insert_lookup(meta::any<is_associative, is_lookup>(), L, self, std::move(key), std::move(value));
			}

			static detail::error_result insert_has(std::false_type, lua_State* L, T& self, stack_object where, stack_object value) {
				return insert_after_has(meta::has_insert_after<T>(), L, self, where, value);
			}

			static detail::error_result insert_copyable(std::true_type, lua_State* L, T& self, stack_object key, stack_object value) {
				return insert_has(meta::has_insert<T>(), L, self, std::move(key), std::move(value));
			}

			static detail::error_result insert_copyable(std::false_type, lua_State*, T&, stack_object, stack_object) {
				return detail::error_result("cannot call 'insert' on '%s': value_type is non-copyable", detail::demangle<T>().data());
			}

			static detail::error_result erase_integral(std::true_type, lua_State* L, T& self, K& key) {
				auto it = deferred_uc::begin(L, self);
				key = (static_cast<std::ptrdiff_t>(key) + deferred_uc::index_adjustment(L, self));
				std::advance(it, key);
				self.erase(it);

				return {};
			}

			static detail::error_result erase_integral(std::false_type, lua_State* L, T& self, const K& key) {
				auto fx = [&](const value_type& r) -> bool { return key == r; };
				auto e = deferred_uc::end(L, self);
				auto it = std::find_if(deferred_uc::begin(L, self), e, std::ref(fx));
				if (it == e) {
					return {};
				}
				self.erase(it);

				return {};
			}

			static detail::error_result erase_associative_lookup(std::true_type, lua_State*, T& self, const K& key) {
				self.erase(key);
				return {};
			}

			static detail::error_result erase_associative_lookup(std::false_type, lua_State* L, T& self, K& key) {
				return erase_integral(std::is_integral<K>(), L, self, key);
			}

			static detail::error_result erase_after_has(std::true_type, lua_State* L, T& self, K& key) {
				auto backit = self.before_begin();
				{
					key = static_cast<K>(static_cast<std::ptrdiff_t>(key) + deferred_uc::index_adjustment(L, self));
					auto e = deferred_uc::end(L, self);
					for (auto it = deferred_uc::begin(L, self); key > 0; ++backit, ++it, --key) {
						if (backit == e) {
							return detail::error_result("sol: out of bounds for erase on '%s'", detail::demangle<T>().c_str());
						}
					}
				}
				self.erase_after(backit);
				return {};
			}

			static detail::error_result erase_after_has(std::false_type, lua_State*, T&, const K&) {
				return detail::error_result("sol: cannot call erase on '%s'", detail::demangle<T>().c_str());
			}

			static detail::error_result erase_key_has(std::true_type, lua_State* L, T& self, K& key) {
				return erase_associative_lookup(meta::any<is_associative, is_lookup>(), L, self, key);
			}

			static detail::error_result erase_key_has(std::false_type, lua_State* L, T& self, K& key) {
				return erase_after_has(has_erase_after<T>(), L, self, key);
			}

			static detail::error_result erase_has(std::true_type, lua_State* L, T& self, K& key) {
				return erase_associative_lookup(meta::any<is_associative, is_lookup>(), L, self, key);
			}

			static detail::error_result erase_has(std::false_type, lua_State* L, T& self, K& key) {
				return erase_key_has(has_erase_key<T>(), L, self, key);
			}

			static auto size_has(std::false_type, lua_State* L, T& self) {
				return std::distance(deferred_uc::begin(L, self), deferred_uc::end(L, self));
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
				return deferred_uc::begin(L, self) == deferred_uc::end(L, self);
			}

			static detail::error_result get_associative_find(std::true_type, lua_State* L, T& self, K& key) {
				auto it = self.find(key);
				if (it == deferred_uc::end(L, self)) {
					stack::push(L, lua_nil);
					return {};
				}
				return get_associative(std::true_type(), L, it);
			}

			static detail::error_result get_associative_find(std::false_type, lua_State* L, T& self, K& key) {
				return get_it(is_linear_integral(), L, self, key);
			}

			static detail::error_result get_start(lua_State* L, T& self, K& key) {
				return get_associative_find(std::integral_constant < bool, is_associative::value&& has_find<T>::value > (), L, self, key);
			}

			static detail::error_result set_start(lua_State* L, T& self, stack_object key, stack_object value) {
				return set_it(is_linear_integral(), L, self, std::move(key), std::move(value));
			}

			static std::size_t size_start(lua_State* L, T& self) {
				return static_cast<std::size_t>(size_has(meta::has_size<T>(), L, self));
			}

			static void clear_start(lua_State* L, T& self) {
				clear_has(has_clear<T>(), L, self);
			}

			static bool empty_start(lua_State* L, T& self) {
				return empty_has(has_empty<T>(), L, self);
			}

			static detail::error_result erase_start(lua_State* L, T& self, K& key) {
				return erase_has(has_erase<T>(), L, self, key);
			}

			template <bool ip>
			static int next_associative(std::true_type, lua_State* L) {
				iter& i = stack::unqualified_get<user<iter>>(L, 1);
				auto& source = i.source;
				auto& it = i.it;
				if (it == deferred_uc::end(L, source)) {
					return stack::push(L, lua_nil);
				}
				int p;
				if constexpr (ip) {
					++i.index;
					p = stack::push_reference(L, i.index);
				}
				else {
					p = stack::push_reference(L, it->first);
				}
				p += stack::stack_detail::push_reference<push_type>(L, detail::deref_move_only(it->second));
				std::advance(it, 1);
				return p;
			}

			template <bool>
			static int next_associative(std::false_type, lua_State* L) {
				iter& i = stack::unqualified_get<user<iter>>(L, 1);
				auto& source = i.source;
				auto& it = i.it;
				next_K k = stack::unqualified_get<next_K>(L, 2);
				if (it == deferred_uc::end(L, source)) {
					return stack::push(L, lua_nil);
				}
				int p;
				if constexpr (std::is_integral_v<next_K>) {
					p = stack::push_reference(L, k + 1);
				}
				else {
					p = stack::stack_detail::push_reference(L, k + 1);
				}
				p += stack::stack_detail::push_reference<push_type>(L, detail::deref_move_only(*it));
				std::advance(it, 1);
				return p;
			}

			template <bool ip>
			static int next_iter(lua_State* L) {
				typedef meta::any<is_associative, meta::all<is_lookup, meta::neg<is_matched_lookup>>> is_assoc;
				return next_associative<ip>(is_assoc(), L);
			}

			template <bool ip>
			static int pairs_associative(std::true_type, lua_State* L) {
				auto& src = get_src(L);
				stack::push(L, next_iter<ip>);
				stack::push<user<iter>>(L, src, deferred_uc::begin(L, src));
				stack::push(L, lua_nil);
				return 3;
			}

			template <bool ip>
			static int pairs_associative(std::false_type, lua_State* L) {
				auto& src = get_src(L);
				stack::push(L, next_iter<ip>);
				stack::push<user<iter>>(L, src, deferred_uc::begin(L, src));
				stack::push(L, 0);
				return 3;
			}

		public:
			static int at(lua_State* L) {
				auto& self = get_src(L);
				detail::error_result er;
				{
					std::ptrdiff_t pos = stack::unqualified_get<std::ptrdiff_t>(L, 2);
					er = at_start(L, self, pos);
				}
				return handle_errors(L, er);
			}

			static int get(lua_State* L) {
				auto& self = get_src(L);
				detail::error_result er;
				{
					decltype(auto) key = stack::unqualified_get<K>(L);
					er = get_start(L, self, key);
				}
				return handle_errors(L, er);
			}

			static int index_get(lua_State* L) {
				return get(L);
			}

			static int set(lua_State* L) {
				stack_object value = stack_object(L, raw_index(3));
				if constexpr (is_linear_integral::value) {
					// for non-associative containers,
					// erasure only happens if it is the
					// last index in the container
					auto key = stack::get<K>(L, 2);
					auto self_size = deferred_uc::size(L);
					if (key == static_cast<K>(self_size)) {
						if (type_of(L, 3) == type::lua_nil) {
							return erase(L);
						}
					}
				}
				else {
					if (type_of(L, 3) == type::lua_nil) {
						return erase(L);
					}
				}
				auto& self = get_src(L);
				detail::error_result er = set_start(L, self, stack_object(L, raw_index(2)), std::move(value));
				return handle_errors(L, er);
			}

			static int index_set(lua_State* L) {
				return set(L);
			}

			static int add(lua_State* L) {
				auto& self = get_src(L);
				detail::error_result er = add_copyable(is_copyable(), L, self, stack_object(L, raw_index(2)));
				return handle_errors(L, er);
			}

			static int insert(lua_State* L) {
				auto& self = get_src(L);
				detail::error_result er = insert_copyable(is_copyable(), L, self, stack_object(L, raw_index(2)), stack_object(L, raw_index(3)));
				return handle_errors(L, er);
			}

			static int find(lua_State* L) {
				auto& self = get_src(L);
				detail::error_result er = find_has(has_find<T>(), L, self);
				return handle_errors(L, er);
			}

			static int index_of(lua_State* L) {
				auto& self = get_src(L);
				detail::error_result er = find_has<true>(has_find<T>(), L, self);
				return handle_errors(L, er);
			}

			static iterator begin(lua_State*, T& self) {
				if constexpr (meta::has_begin_end_v<T>) {
					return self.begin();
				}
				else {
					using std::begin;
					return begin(self);
				}
			}

			static iterator end(lua_State*, T& self) {
				if constexpr (meta::has_begin_end_v<T>) {
					return self.end();
				}
				else {
					using std::end;
					return end(self);
				}
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
				detail::error_result er;
				{
					decltype(auto) key = stack::unqualified_get<K>(L, 2);
					er = erase_start(L, self, key);
				}
				return handle_errors(L, er);
			}

			static int empty(lua_State* L) {
				auto& self = get_src(L);
				return stack::push(L, empty_start(L, self));
			}

			static std::ptrdiff_t index_adjustment(lua_State*, T&) {
				return static_cast<std::ptrdiff_t>((SOL_CONTAINER_START_INDEX_I_) == 0 ? 0 : -(SOL_CONTAINER_START_INDEX_I_));
			}

			static int pairs(lua_State* L) {
				typedef meta::any<is_associative, meta::all<is_lookup, meta::neg<is_matched_lookup>>> is_assoc;
				return pairs_associative<false>(is_assoc(), L);
			}

			static int ipairs(lua_State* L) {
				typedef meta::any<is_associative, meta::all<is_lookup, meta::neg<is_matched_lookup>>> is_assoc;
				return pairs_associative<true>(is_assoc(), L);
			}

			static int next(lua_State* L) {
				return stack::push(L, next_iter<false>);
			}
		};

		template <typename X>
		struct usertype_container_default<X, std::enable_if_t<std::is_array<std::remove_pointer_t<meta::unwrap_unqualified_t<X>>>::value>> {
		private:
			typedef std::remove_pointer_t<meta::unwrap_unqualified_t<X>> T;
			typedef usertype_container<X> deferred_uc;

		public:
			typedef std::remove_extent_t<T> value_type;
			typedef value_type* iterator;

		private:
			struct iter {
				T& source;
				iterator it;

				iter(T& source, iterator it) : source(source), it(std::move(it)) {
				}
			};

			static auto& get_src(lua_State* L) {
				auto p = stack::unqualified_check_get<T*>(L, 1);
#if SOL_IS_ON(SOL_SAFE_USERTYPE_I_)
				if (!p) {
					luaL_error(L,
					     "sol: 'self' is not of type '%s' (pass 'self' as first argument with ':' or call on proper type)",
					     detail::demangle<T>().c_str());
				}
				if (p.value() == nullptr) {
					luaL_error(
					     L, "sol: 'self' argument is nil (pass 'self' as first argument with ':' or call on a '%s' type)", detail::demangle<T>().c_str());
				}
#endif // Safe getting with error
				return *p.value();
			}

			static int find(std::true_type, lua_State* L) {
				T& self = get_src(L);
				decltype(auto) value = stack::unqualified_get<value_type>(L, 2);
				std::size_t N = std::extent<T>::value;
				for (std::size_t idx = 0; idx < N; ++idx) {
					using v_t = std::add_const_t<decltype(self[idx])>;
					v_t v = self[idx];
					if (v == value) {
						idx = static_cast<std::size_t>(static_cast<std::ptrdiff_t>(idx) - deferred_uc::index_adjustment(L, self));
						return stack::push(L, idx);
					}
				}
				return stack::push(L, lua_nil);
			}

			static int find(std::false_type, lua_State* L) {
				return luaL_error(L, "sol: cannot call 'find' on '%s': no supported comparison operator for the value type", detail::demangle<T>().c_str());
			}

			static int next_iter(lua_State* L) {
				iter& i = stack::unqualified_get<user<iter>>(L, 1);
				auto& source = i.source;
				auto& it = i.it;
				std::size_t k = stack::unqualified_get<std::size_t>(L, 2);
				if (it == deferred_uc::end(L, source)) {
					return 0;
				}
				int p;
				p = stack::push(L, k + 1);
				p += stack::push_reference(L, detail::deref_move_only(*it));
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

			static int at(lua_State* L) {
				return get(L);
			}

			static int get(lua_State* L) {
				T& self = get_src(L);
				std::ptrdiff_t idx = stack::unqualified_get<std::ptrdiff_t>(L, 2);
				idx += deferred_uc::index_adjustment(L, self);
				if (idx >= static_cast<std::ptrdiff_t>(std::extent<T>::value) || idx < 0) {
					return stack::push(L, lua_nil);
				}
				return stack::push_reference(L, detail::deref_move_only(self[idx]));
			}

			static int index_get(lua_State* L) {
				return get(L);
			}

			static int set(lua_State* L) {
				T& self = get_src(L);
				std::ptrdiff_t idx = stack::unqualified_get<std::ptrdiff_t>(L, 2);
				idx += deferred_uc::index_adjustment(L, self);
				if (idx >= static_cast<std::ptrdiff_t>(std::extent<T>::value)) {
					return luaL_error(L, "sol: index out of bounds (too big) for set on '%s'", detail::demangle<T>().c_str());
				}
				if (idx < 0) {
					return luaL_error(L, "sol: index out of bounds (too small) for set on '%s'", detail::demangle<T>().c_str());
				}
				self[idx] = stack::unqualified_get<value_type>(L, 3);
				return 0;
			}

			static int index_set(lua_State* L) {
				return set(L);
			}

			static int index_of(lua_State* L) {
				return find(L);
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
				stack::push(L, next_iter);
				stack::push<user<iter>>(L, src, deferred_uc::begin(L, src));
				stack::push(L, 0);
				return 3;
			}

			static int ipairs(lua_State* L) {
				return pairs(L);
			}

			static int next(lua_State* L) {
				return stack::push(L, next_iter);
			}

			static std::ptrdiff_t index_adjustment(lua_State*, T&) {
				return (SOL_CONTAINER_START_INDEX_I_) == 0 ? 0 : -(SOL_CONTAINER_START_INDEX_I_);
			}

			static iterator begin(lua_State*, T& self) {
				return std::addressof(self[0]);
			}

			static iterator end(lua_State*, T& self) {
				return std::addressof(self[0]) + std::extent<T>::value;
			}
		};

		template <typename X>
		struct usertype_container_default<usertype_container<X>> : usertype_container_default<X> { };
	} // namespace container_detail

	template <typename T>
	struct usertype_container : container_detail::usertype_container_default<T> { };

} // namespace sol

#endif // SOL_USERTYPE_CONTAINER_HPP
