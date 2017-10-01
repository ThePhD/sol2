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

#ifndef SOL_STACK_CORE_HPP
#define SOL_STACK_CORE_HPP

#include "types.hpp"
#include "error_handler.hpp"
#include "reference.hpp"
#include "stack_reference.hpp"
#include "tuple.hpp"
#include "traits.hpp"
#include "tie.hpp"
#include "stack_guard.hpp"
#include "demangle.hpp"
#include "forward_detail.hpp"

#include <vector>
#include <forward_list>
#include <string>
#include <algorithm>

namespace sol {
	namespace detail {
		struct as_reference_tag {};
		template <typename T>
		struct as_pointer_tag {};
		template <typename T>
		struct as_value_tag {};
		template <typename T>
		struct as_table_tag {};

		using unique_destructor = void (*)(void*);

		template <typename T>
		int user_aligned_alloc(lua_State* L, std::size_t offset, void*& mem, T*& obj) {
			const auto alignment = std::alignment_of<T>();
			mem = lua_newuserdata(L, offset + sizeof(T) + alignment - 1);
			auto aligned_userdata = (std::uintptr_t)mem + offset;
			// check the alignment. if it's not correct, adjust the pointer.
			if ((aligned_userdata % alignment) != 0) {
				aligned_userdata += alignment - aligned_userdata % alignment;
			}
			obj = reinterpret_cast<T*>(aligned_userdata);
			return 1;
		}

		template <typename T>
		inline int unique_destruct(lua_State* L) {
			void* memory = lua_touserdata(L, 1);
			T** pointerpointer = static_cast<T**>(memory);
			unique_destructor& dx = *static_cast<unique_destructor*>(static_cast<void*>(pointerpointer + 1));
			(dx)(memory);
			return 0;
		}

		template <typename T>
		inline int user_alloc_destruct(lua_State* L) {
			void* rawdata = lua_touserdata(L, 1);
			auto data = (std::uintptr_t)rawdata;
			if ((data % std::alignment_of<T>()) != 0) {
				data += std::alignment_of<T>() - data % std::alignment_of<T>();
			}
			std::allocator<T> alloc;
			alloc.destroy((T*)data);
			return 0;
		}

		template <typename T>
		inline int usertype_alloc_destruct(lua_State* L) {
			void* rawdata = lua_touserdata(L, 1);
			T** pdata = static_cast<T**>(rawdata);
			T* data = *pdata;
			std::allocator<T> alloc{};
			alloc.destroy(data);
			return 0;
		}

		template <typename T>
		inline int cannot_destruct(lua_State* L) {
			return luaL_error(L, "cannot call the destructor for '%s': it is either hidden (protected/private) or removed with '= delete' and thusly this type is being destroyed without properly destructing, invoking undefined behavior", detail::demangle<T>().data());
		}

		template <typename T, typename Real>
		inline void usertype_unique_alloc_destroy(void* memory) {
			T** pointerpointer = static_cast<T**>(memory);
			unique_destructor* dx = static_cast<unique_destructor*>(static_cast<void*>(pointerpointer + 1));
			Real* target = static_cast<Real*>(static_cast<void*>(dx + 1));
			std::allocator<Real> alloc;
			alloc.destroy(target);
		}

		template <typename T>
		void reserve(T&, std::size_t) {
		}

		template <typename T, typename Al>
		void reserve(std::vector<T, Al>& arr, std::size_t hint) {
			arr.reserve(hint);
		}

		template <typename T, typename Tr, typename Al>
		void reserve(std::basic_string<T, Tr, Al>& arr, std::size_t hint) {
			arr.reserve(hint);
		}
	} // namespace detail

	namespace stack {

		template <typename T>
		struct extensible {};

		template <typename T, bool global = false, bool raw = false, typename = void>
		struct field_getter;
		template <typename T, bool global = false, bool raw = false, typename = void>
		struct probe_field_getter;
		template <typename T, bool global = false, bool raw = false, typename = void>
		struct field_setter;
		template <typename T, typename = void>
		struct getter;
		template <typename T, typename = void>
		struct userdata_getter;
		template <typename T, typename = void>
		struct popper;
		template <typename T, typename = void>
		struct pusher;
		template <typename T, type = lua_type_of<T>::value, typename = void>
		struct checker;
		template <typename T, typename = void>
		struct userdata_checker;
		template <typename T, typename = void>
		struct check_getter;

		struct probe {
			bool success;
			int levels;

			probe(bool s, int l)
			: success(s), levels(l) {
			}

			operator bool() const {
				return success;
			};
		};

		struct record {
			int last;
			int used;

			record()
			: last(), used() {
			}
			void use(int count) {
				last = count;
				used += count;
			}
		};

		namespace stack_detail {
			template <typename T>
			struct strip {
				typedef T type;
			};
			template <typename T>
			struct strip<std::reference_wrapper<T>> {
				typedef T& type;
			};
			template <typename T>
			struct strip<user<T>> {
				typedef T& type;
			};
			template <typename T>
			struct strip<non_null<T>> {
				typedef T type;
			};
			template <typename T>
			using strip_t = typename strip<T>::type;

			template <typename T>
			struct strip_extensible { typedef T type; };

			template <typename T>
			struct strip_extensible<extensible<T>> { typedef T type; };

			template <typename T>
			using strip_extensible_t = typename strip_extensible<T>::type;

			const bool default_check_arguments =
#ifdef SOL_CHECK_ARGUMENTS
				true;
#else
				false;
#endif

			template <typename C>
			static int get_size_hint(const C& c) {
				return static_cast<int>(c.size());
			}

			template <typename V, typename Al>
			static int get_size_hint(const std::forward_list<V, Al>&) {
				// forward_list makes me sad
				return static_cast<int>(32);
			}

			template <typename T>
			inline decltype(auto) unchecked_get(lua_State* L, int index, record& tracking) {
				getter<meta::unqualified_t<T>> g{};
				(void)g;
				return g.get(L, index, tracking);
			}

			template <typename T, typename Arg, typename... Args>
			inline int push_reference(lua_State* L, Arg&& arg, Args&&... args) {
				typedef meta::all<
					std::is_lvalue_reference<T>,
					meta::neg<std::is_const<T>>,
					meta::neg<is_lua_primitive<meta::unqualified_t<T>>>,
					meta::neg<is_unique_usertype<meta::unqualified_t<T>>>>
					use_reference_tag;
				return pusher<std::conditional_t<use_reference_tag::value, detail::as_reference_tag, meta::unqualified_t<T>>>{}.push(L, std::forward<Arg>(arg), std::forward<Args>(args)...);
			}
		} // namespace stack_detail

		inline bool maybe_indexable(lua_State* L, int index = -1) {
			type t = type_of(L, index);
			return t == type::userdata || t == type::table;
		}

		inline int top(lua_State* L) {
			return lua_gettop(L);
		}

		template <typename T, typename... Args>
		inline int push(lua_State* L, T&& t, Args&&... args) {
			return pusher<meta::unqualified_t<T>>{}.push(L, std::forward<T>(t), std::forward<Args>(args)...);
		}

		// overload allows to use a pusher of a specific type, but pass in any kind of args
		template <typename T, typename Arg, typename... Args, typename = std::enable_if_t<!std::is_same<T, Arg>::value>>
		inline int push(lua_State* L, Arg&& arg, Args&&... args) {
			return pusher<meta::unqualified_t<T>>{}.push(L, std::forward<Arg>(arg), std::forward<Args>(args)...);
		}

		template <typename T, typename... Args>
		inline int push_reference(lua_State* L, T&& t, Args&&... args) {
			return stack_detail::push_reference<T>(L, std::forward<T>(t), std::forward<Args>(args)...);
		}

		template <typename T, typename Arg, typename... Args>
		inline int push_reference(lua_State* L, Arg&& arg, Args&&... args) {
			return stack_detail::push_reference<T>(L, std::forward<Arg>(arg), std::forward<Args>(args)...);
		}

		inline int multi_push(lua_State*) {
			// do nothing
			return 0;
		}

		template <typename T, typename... Args>
		inline int multi_push(lua_State* L, T&& t, Args&&... args) {
			int pushcount = push(L, std::forward<T>(t));
			void(detail::swallow{ (pushcount += stack::push(L, std::forward<Args>(args)), 0)... });
			return pushcount;
		}

		inline int multi_push_reference(lua_State*) {
			// do nothing
			return 0;
		}

		template <typename T, typename... Args>
		inline int multi_push_reference(lua_State* L, T&& t, Args&&... args) {
			int pushcount = push_reference(L, std::forward<T>(t));
			void(detail::swallow{ (pushcount += stack::push_reference(L, std::forward<Args>(args)), 0)... });
			return pushcount;
		}

		template <typename T, typename Handler>
		bool check(lua_State* L, int index, Handler&& handler, record& tracking) {
			typedef meta::unqualified_t<T> Tu;
			checker<Tu> c;
			// VC++ has a bad warning here: shut it up
			(void)c;
			return c.check(L, index, std::forward<Handler>(handler), tracking);
		}

		template <typename T, typename Handler>
		bool check(lua_State* L, int index, Handler&& handler) {
			record tracking{};
			return check<T>(L, index, std::forward<Handler>(handler), tracking);
		}

		template <typename T>
		bool check(lua_State* L, int index = -lua_size<meta::unqualified_t<T>>::value) {
			auto handler = no_panic;
			return check<T>(L, index, handler);
		}

		template <typename T, typename Handler>
		inline decltype(auto) check_get(lua_State* L, int index, Handler&& handler, record& tracking) {
			typedef meta::unqualified_t<T> Tu;
			check_getter<Tu> cg{};
			(void)cg;
			return cg.get(L, index, std::forward<Handler>(handler), tracking);
		}

		template <typename T, typename Handler>
		inline decltype(auto) check_get(lua_State* L, int index, Handler&& handler) {
			record tracking{};
			return check_get<T>(L, index, handler, tracking);
		}

		template <typename T>
		inline decltype(auto) check_get(lua_State* L, int index = -lua_size<meta::unqualified_t<T>>::value) {
			auto handler = no_panic;
			return check_get<T>(L, index, handler);
		}

		namespace stack_detail {

#ifdef SOL_CHECK_ARGUMENTS
			template <typename T>
			inline auto tagged_get(types<T>, lua_State* L, int index, record& tracking) -> decltype(stack_detail::unchecked_get<T>(L, index, tracking)) {
				auto op = check_get<T>(L, index, type_panic_c_str, tracking);
				return *std::move(op);
			}
#else
			template <typename T>
			inline decltype(auto) tagged_get(types<T>, lua_State* L, int index, record& tracking) {
				return stack_detail::unchecked_get<T>(L, index, tracking);
			}
#endif

			template <typename T>
			inline decltype(auto) tagged_get(types<optional<T>>, lua_State* L, int index, record& tracking) {
				return stack_detail::unchecked_get<optional<T>>(L, index, tracking);
			}

			template <bool b>
			struct check_types {
				template <typename T, typename... Args, typename Handler>
				static bool check(types<T, Args...>, lua_State* L, int firstargument, Handler&& handler, record& tracking) {
					if (!stack::check<T>(L, firstargument + tracking.used, handler, tracking))
						return false;
					return check(types<Args...>(), L, firstargument, std::forward<Handler>(handler), tracking);
				}

				template <typename Handler>
				static bool check(types<>, lua_State*, int, Handler&&, record&) {
					return true;
				}
			};

			template <>
			struct check_types<false> {
				template <typename... Args, typename Handler>
				static bool check(types<Args...>, lua_State*, int, Handler&&, record&) {
					return true;
				}
			};

		} // namespace stack_detail

		template <bool b, typename... Args, typename Handler>
		bool multi_check(lua_State* L, int index, Handler&& handler, record& tracking) {
			return stack_detail::check_types<b>{}.check(types<meta::unqualified_t<Args>...>(), L, index, std::forward<Handler>(handler), tracking);
		}

		template <bool b, typename... Args, typename Handler>
		bool multi_check(lua_State* L, int index, Handler&& handler) {
			record tracking{};
			return multi_check<b, Args...>(L, index, std::forward<Handler>(handler), tracking);
		}

		template <bool b, typename... Args>
		bool multi_check(lua_State* L, int index) {
			auto handler = no_panic;
			return multi_check<b, Args...>(L, index, handler);
		}

		template <typename... Args, typename Handler>
		bool multi_check(lua_State* L, int index, Handler&& handler, record& tracking) {
			return multi_check<true, Args...>(L, index, std::forward<Handler>(handler), tracking);
		}

		template <typename... Args, typename Handler>
		bool multi_check(lua_State* L, int index, Handler&& handler) {
			return multi_check<true, Args...>(L, index, std::forward<Handler>(handler));
		}

		template <typename... Args>
		bool multi_check(lua_State* L, int index) {
			return multi_check<true, Args...>(L, index);
		}

		template <typename T>
		inline decltype(auto) get(lua_State* L, int index, record& tracking) {
			return stack_detail::tagged_get(types<T>(), L, index, tracking);
		}

		template <typename T>
		inline decltype(auto) get(lua_State* L, int index = -lua_size<meta::unqualified_t<T>>::value) {
			record tracking{};
			return get<T>(L, index, tracking);
		}

		template <typename T>
		inline decltype(auto) pop(lua_State* L) {
			return popper<meta::unqualified_t<T>>{}.pop(L);
		}

		template <bool global = false, bool raw = false, typename Key>
		void get_field(lua_State* L, Key&& key) {
			field_getter<meta::unqualified_t<Key>, global, raw>{}.get(L, std::forward<Key>(key));
		}

		template <bool global = false, bool raw = false, typename Key>
		void get_field(lua_State* L, Key&& key, int tableindex) {
			field_getter<meta::unqualified_t<Key>, global, raw>{}.get(L, std::forward<Key>(key), tableindex);
		}

		template <bool global = false, typename Key>
		void raw_get_field(lua_State* L, Key&& key) {
			get_field<global, true>(L, std::forward<Key>(key));
		}

		template <bool global = false, typename Key>
		void raw_get_field(lua_State* L, Key&& key, int tableindex) {
			get_field<global, true>(L, std::forward<Key>(key), tableindex);
		}

		template <bool global = false, bool raw = false, typename Key>
		probe probe_get_field(lua_State* L, Key&& key) {
			return probe_field_getter<meta::unqualified_t<Key>, global, raw>{}.get(L, std::forward<Key>(key));
		}

		template <bool global = false, bool raw = false, typename Key>
		probe probe_get_field(lua_State* L, Key&& key, int tableindex) {
			return probe_field_getter<meta::unqualified_t<Key>, global, raw>{}.get(L, std::forward<Key>(key), tableindex);
		}

		template <bool global = false, typename Key>
		probe probe_raw_get_field(lua_State* L, Key&& key) {
			return probe_get_field<global, true>(L, std::forward<Key>(key));
		}

		template <bool global = false, typename Key>
		probe probe_raw_get_field(lua_State* L, Key&& key, int tableindex) {
			return probe_get_field<global, true>(L, std::forward<Key>(key), tableindex);
		}

		template <bool global = false, bool raw = false, typename Key, typename Value>
		void set_field(lua_State* L, Key&& key, Value&& value) {
			field_setter<meta::unqualified_t<Key>, global, raw>{}.set(L, std::forward<Key>(key), std::forward<Value>(value));
		}

		template <bool global = false, bool raw = false, typename Key, typename Value>
		void set_field(lua_State* L, Key&& key, Value&& value, int tableindex) {
			field_setter<meta::unqualified_t<Key>, global, raw>{}.set(L, std::forward<Key>(key), std::forward<Value>(value), tableindex);
		}

		template <bool global = false, typename Key, typename Value>
		void raw_set_field(lua_State* L, Key&& key, Value&& value) {
			set_field<global, true>(L, std::forward<Key>(key), std::forward<Value>(value));
		}

		template <bool global = false, typename Key, typename Value>
		void raw_set_field(lua_State* L, Key&& key, Value&& value, int tableindex) {
			set_field<global, true>(L, std::forward<Key>(key), std::forward<Value>(value), tableindex);
		}
	} // namespace stack
} // namespace sol

#endif // SOL_STACK_CORE_HPP
