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

#ifndef SOL_STACK_PUSH_HPP
#define SOL_STACK_PUSH_HPP

#include "stack_core.hpp"
#include "raii.hpp"
#include "optional.hpp"
#include "usertype_traits.hpp"
#include "filters.hpp"
#include "unicode.hpp"

#include <memory>
#include <type_traits>
#include <cassert>
#include <limits>
#include <cmath>
#if defined(SOL_CXX17_FEATURES) && SOL_CXX17_FEATURES
#include <string_view>
#include <optional>
#if defined(SOL_STD_VARIANT) && SOL_STD_VARIANT
#include <variant>
#endif // Can use variant
#endif // C++17

namespace sol {
	namespace stack {
		namespace stack_detail {
			template <typename T>
			inline bool integer_value_fits(const T& value) {
				if constexpr (sizeof(T) < sizeof(lua_Integer) || (std::is_signed<T>::value && sizeof(T) == sizeof(lua_Integer))) {
					(void)value;
					return true;
				}
				else {
					auto u_min = static_cast<std::intmax_t>((std::numeric_limits<lua_Integer>::min)());
					auto u_max = static_cast<std::uintmax_t>((std::numeric_limits<lua_Integer>::max)());
					auto t_min = static_cast<std::intmax_t>((std::numeric_limits<T>::min)());
					auto t_max = static_cast<std::uintmax_t>((std::numeric_limits<T>::max)());
					return (u_min <= t_min || value >= static_cast<T>(u_min)) && (u_max >= t_max || value <= static_cast<T>(u_max));
				}
			}
		}

		inline int push_environment_of(lua_State* L, int index = -1) {
#if defined(SOL_SAFE_STACK_CHECK) && SOL_SAFE_STACK_CHECK
			luaL_checkstack(L, 1, detail::not_enough_stack_space_environment);
#endif // make sure stack doesn't overflow
#if SOL_LUA_VERSION < 502
			// Use lua_getfenv
			lua_getfenv(L, index);
			return 1;
#else
			// Use upvalues as explained in Lua 5.2 and beyond's manual
			if (lua_getupvalue(L, index, 1) == nullptr) {
				push(L, lua_nil);
				return 1;
			}
#endif
			return 1;
		}

		template <typename T>
		int push_environment_of(const T& target) {
			target.push();
			return push_environment_of(target.lua_state(), -1) + 1;
		}

		template <typename T>
		struct unqualified_pusher<detail::as_value_tag<T>> {
			template <typename F, typename... Args>
			static int push_fx(lua_State* L, F&& f, Args&&... args) {
#if defined(SOL_SAFE_STACK_CHECK) && SOL_SAFE_STACK_CHECK
				luaL_checkstack(L, 1, detail::not_enough_stack_space_userdata);
#endif // make sure stack doesn't overflow
				// Basically, we store all user-data like this:
				// If it's a movable/copyable value (no std::ref(x)), then we store the pointer to the new
				// data in the first sizeof(T*) bytes, and then however many bytes it takes to
				// do the actual object. Things that are std::ref or plain T* are stored as
				// just the sizeof(T*), and nothing else.
				T* obj = detail::usertype_allocate<T>(L);
				std::allocator<T> alloc{};
				std::allocator_traits<std::allocator<T>>::construct(alloc, obj, std::forward<Args>(args)...);
				f();
				return 1;
			}

			template <typename K, typename... Args>
			static int push_keyed(lua_State* L, K&& k, Args&&... args) {
				stack_detail::undefined_metatable<T> fx(L, &k[0]);
				return push_fx(L, fx, std::forward<Args>(args)...);
			}

			template <typename Arg, typename... Args, typename = std::enable_if_t<!std::is_same_v<meta::unqualified_t<Arg>, detail::with_function_tag>>>
			static int push(lua_State* L, Arg&& arg, Args&&... args) {
				return push_keyed(L, usertype_traits<T>::metatable(), std::forward<Arg>(arg), std::forward<Args>(args)...);
			}

			static int push(lua_State* L) {
				return push_keyed(L, usertype_traits<T>::metatable());
			}

			template <typename... Args>
			static int push(lua_State* L, detail::with_function_tag, Args&&... args) {
				return push_fx(L, std::forward<Args>(args)...);
			}
		};

		template <typename T>
		struct unqualified_pusher<detail::as_pointer_tag<T>> {
			typedef meta::unqualified_t<T> U;

			template <typename F>
			static int push_fx(lua_State* L, F&& f, T* obj) {
				if (obj == nullptr)
					return stack::push(L, lua_nil);
#if defined(SOL_SAFE_STACK_CHECK) && SOL_SAFE_STACK_CHECK
				luaL_checkstack(L, 1, detail::not_enough_stack_space_userdata);
#endif // make sure stack doesn't overflow
				T** pref = detail::usertype_allocate_pointer<T>(L);
				*pref = obj;
				f();
				return 1;
			}

			template <typename K>
			static int push_keyed(lua_State* L, K&& k, T* obj) {
				stack_detail::undefined_metatable<U*> fx(L, &k[0]);
				return push_fx(L, fx, obj);
			}

			template <typename Arg, typename... Args, typename = std::enable_if_t<!std::is_same_v<meta::unqualified_t<Arg>, detail::with_function_tag>>>
			static int push(lua_State* L, Arg&& arg, Args&&... args) {
				return push_keyed(L, usertype_traits<U*>::metatable(), std::forward<Arg>(arg), std::forward<Args>(args)...);
			}

			template <typename... Args>
			static int push(lua_State* L, detail::with_function_tag, Args&&... args) {
				return push_fx(L, std::forward<Args>(args)...);
			}
		};

		template <>
		struct unqualified_pusher<detail::as_reference_tag> {
			template <typename T>
			static int push(lua_State* L, T&& obj) {
				return stack::push(L, detail::ptr(obj));
			}
		};

		template <typename T, typename>
		struct unqualified_pusher {
			template <typename... Args>
			static int push(lua_State* L, Args&&... args) {
				return stack::push<detail::as_value_tag<T>>(L, std::forward<Args>(args)...);
			}
		};

		template <typename T>
		struct unqualified_pusher<T*,
		     meta::disable_if_t<meta::any<is_container<meta::unqualified_t<T>>, std::is_function<meta::unqualified_t<T>>,
		          is_lua_reference<meta::unqualified_t<T>>>::value>> {
			template <typename... Args>
			static int push(lua_State* L, Args&&... args) {
				return stack::push<detail::as_pointer_tag<T>>(L, std::forward<Args>(args)...);
			}
		};

		template <typename T>
		struct unqualified_pusher<T, std::enable_if_t<is_unique_usertype<T>::value>> {
			typedef unique_usertype_traits<T> u_traits;
			typedef typename u_traits::type P;
			typedef typename u_traits::actual_type Real;
			typedef typename u_traits::template rebind_base<void> rebind_t;

			template <typename Arg, meta::enable<std::is_base_of<Real, meta::unqualified_t<Arg>>> = meta::enabler>
			static int push(lua_State* L, Arg&& arg) {
				if (unique_usertype_traits<T>::is_null(arg)) {
					return stack::push(L, lua_nil);
				}
				return push_deep(L, std::forward<Arg>(arg));
			}

			template <typename Arg0, typename Arg1, typename... Args>
			static int push(lua_State* L, Arg0&& arg0, Arg0&& arg1, Args&&... args) {
				return push_deep(L, std::forward<Arg0>(arg0), std::forward<Arg1>(arg1), std::forward<Args>(args)...);
			}

			template <typename... Args>
			static int push_deep(lua_State* L, Args&&... args) {
#if defined(SOL_SAFE_STACK_CHECK) && SOL_SAFE_STACK_CHECK
				luaL_checkstack(L, 1, detail::not_enough_stack_space_userdata);
#endif // make sure stack doesn't overflow
				P** pref = nullptr;
				detail::unique_destructor* fx = nullptr;
				detail::unique_tag* id = nullptr;
				Real* mem = detail::usertype_unique_allocate<P, Real>(L, pref, fx, id);
				*fx = detail::usertype_unique_alloc_destroy<P, Real>;
				*id = &detail::inheritance<P>::template type_unique_cast<Real>;
				detail::default_construct::construct(mem, std::forward<Args>(args)...);
				*pref = unique_usertype_traits<T>::get(*mem);
				if (luaL_newmetatable(L, &usertype_traits<detail::unique_usertype<std::remove_cv_t<P>>>::metatable()[0]) == 1) {
					detail::lua_reg_table l{};
					int index = 0;
					detail::indexed_insert insert_fx(l, index);
					detail::insert_default_registrations<P>(insert_fx, detail::property_always_true);
					l[index] = { to_string(meta_function::garbage_collect).c_str(), detail::make_destructor<T>() };
					luaL_setfuncs(L, l, 0);
				}
				lua_setmetatable(L, -2);
				return 1;
			}
		};

		template <typename T>
		struct unqualified_pusher<std::reference_wrapper<T>> {
			static int push(lua_State* L, const std::reference_wrapper<T>& t) {
				return stack::push(L, std::addressof(detail::deref(t.get())));
			}
		};

		template <typename T>
		struct unqualified_pusher<T, std::enable_if_t<std::is_floating_point<T>::value>> {
			static int push(lua_State* L, const T& value) {
#if defined(SOL_SAFE_STACK_CHECK) && SOL_SAFE_STACK_CHECK
				luaL_checkstack(L, 1, detail::not_enough_stack_space_floating);
#endif // make sure stack doesn't overflow
				lua_pushnumber(L, value);
				return 1;
			}
		};

		template <typename T>
		struct unqualified_pusher<T, std::enable_if_t<std::is_integral<T>::value>> {
			static int push(lua_State* L, const T& value) {
#if defined(SOL_SAFE_STACK_CHECK) && SOL_SAFE_STACK_CHECK
				luaL_checkstack(L, 1, detail::not_enough_stack_space_integral);
#endif // make sure stack doesn't overflow
#if SOL_LUA_VERSION >= 503
				if (stack_detail::integer_value_fits<T>(value)) {
					lua_pushinteger(L, static_cast<lua_Integer>(value));
					return 1;
				}
#endif // Lua 5.3 and above
#if (defined(SOL_SAFE_NUMERICS) && SOL_SAFE_NUMERICS) && !(defined(SOL_NO_CHECK_NUMBER_PRECISION) && SOL_NO_CHECK_NUMBER_PRECISION)
				if (static_cast<T>(llround(static_cast<lua_Number>(value))) != value) {
#if defined(SOL_NO_EXCEPTIONS) && SOL_NO_EXCEPTIONS
					// Is this really worth it?
					assert(false && "integer value will be misrepresented in lua");
					lua_pushnumber(L, static_cast<lua_Number>(value));
					return 1;
#else
					throw error(detail::direct_error, "integer value will be misrepresented in lua");
#endif // No Exceptions
				}
#endif // Safe Numerics and Number Precision Check
				lua_pushnumber(L, static_cast<lua_Number>(value));
				return 1;
			}
		};

		template <typename T>
		struct unqualified_pusher<T, std::enable_if_t<std::is_enum<T>::value>> {
			static int push(lua_State* L, const T& value) {
				if (std::is_same<char, std::underlying_type_t<T>>::value) {
					return stack::push(L, static_cast<int>(value));
				}
				return stack::push(L, static_cast<std::underlying_type_t<T>>(value));
			}
		};

		template <typename T>
		struct unqualified_pusher<detail::as_table_tag<T>> {
			static int push(lua_State* L, const T& tablecont) {
				typedef meta::has_key_value_pair<meta::unqualified_t<std::remove_pointer_t<T>>> has_kvp;
				return push(has_kvp(), std::false_type(), L, tablecont);
			}

			static int push(std::true_type, lua_State* L, const T& tablecont) {
				typedef meta::has_key_value_pair<meta::unqualified_t<std::remove_pointer_t<T>>> has_kvp;
				return push(has_kvp(), std::true_type(), L, tablecont);
			}

			static int push(std::false_type, lua_State* L, const T& tablecont) {
				typedef meta::has_key_value_pair<meta::unqualified_t<std::remove_pointer_t<T>>> has_kvp;
				return push(has_kvp(), std::false_type(), L, tablecont);
			}

			template <bool is_nested>
			static int push(std::true_type, std::integral_constant<bool, is_nested>, lua_State* L, const T& tablecont) {
				auto& cont = detail::deref(detail::unwrap(tablecont));
				lua_createtable(L, static_cast<int>(cont.size()), 0);
				int tableindex = lua_gettop(L);
				for (const auto& pair : cont) {
					if (is_nested) {
						set_field(L, pair.first, as_nested_ref(pair.second), tableindex);
					}
					else {
						set_field(L, pair.first, pair.second, tableindex);
					}
				}
				return 1;
			}

			template <bool is_nested>
			static int push(std::false_type, std::integral_constant<bool, is_nested>, lua_State* L, const T& tablecont) {
				auto& cont = detail::deref(detail::unwrap(tablecont));
				lua_createtable(L, stack_detail::get_size_hint(cont), 0);
				int tableindex = lua_gettop(L);
				std::size_t index = 1;
				for (const auto& i : cont) {
#if SOL_LUA_VERSION >= 503
					int p = is_nested ? stack::push(L, as_nested_ref(i)) : stack::push(L, i);
					for (int pi = 0; pi < p; ++pi) {
						lua_seti(L, tableindex, static_cast<lua_Integer>(index++));
					}
#else
#if defined(SOL_SAFE_STACK_CHECK) && SOL_SAFE_STACK_CHECK
					luaL_checkstack(L, 1, detail::not_enough_stack_space_generic);
#endif // make sure stack doesn't overflow
					lua_pushinteger(L, static_cast<lua_Integer>(index));
					int p = is_nested ? stack::push(L, as_nested_ref(i)) : stack::push(L, i);
					if (p == 1) {
						++index;
						lua_settable(L, tableindex);
					}
					else {
						int firstindex = tableindex + 1 + 1;
						for (int pi = 0; pi < p; ++pi) {
							stack::push(L, index);
#if defined(SOL_SAFE_STACK_CHECK) && SOL_SAFE_STACK_CHECK
							luaL_checkstack(L, 1, detail::not_enough_stack_space_generic);
#endif // make sure stack doesn't overflow
							lua_pushvalue(L, firstindex);
							lua_settable(L, tableindex);
							++index;
							++firstindex;
						}
						lua_pop(L, 1 + p);
					}
#endif // Lua Version 5.3 and others
				}
				// TODO: figure out a better way to do this...?
				// set_field(L, -1, cont.size());
				return 1;
			}
		};

		template <typename T>
		struct unqualified_pusher<as_table_t<T>, std::enable_if_t<is_container<std::remove_pointer_t<meta::unwrap_unqualified_t<T>>>::value>> {
			static int push(lua_State* L, const T& tablecont) {
				return stack::push<detail::as_table_tag<T>>(L, tablecont);
			}
		};

		template <typename T>
		struct unqualified_pusher<as_table_t<T>, std::enable_if_t<!is_container<std::remove_pointer_t<meta::unwrap_unqualified_t<T>>>::value>> {
			static int push(lua_State* L, const T& v) {
				return stack::push(L, v);
			}
		};

		template <typename T>
		struct unqualified_pusher<nested<T>, std::enable_if_t<is_container<std::remove_pointer_t<meta::unwrap_unqualified_t<T>>>::value>> {
			static int push(lua_State* L, const T& tablecont) {
				unqualified_pusher<detail::as_table_tag<T>> p{};
				// silence annoying VC++ warning
				(void)p;
				return p.push(std::true_type(), L, tablecont);
			}
		};

		template <typename T>
		struct unqualified_pusher<nested<T>, std::enable_if_t<!is_container<std::remove_pointer_t<meta::unwrap_unqualified_t<T>>>::value>> {
			static int push(lua_State* L, const T& tablecont) {
				unqualified_pusher<meta::unqualified_t<T>> p{};
				// silence annoying VC++ warning
				(void)p;
				return p.push(L, tablecont);
			}
		};

		template <typename T>
		struct unqualified_pusher<std::initializer_list<T>> {
			static int push(lua_State* L, const std::initializer_list<T>& il) {
				unqualified_pusher<detail::as_table_tag<std::initializer_list<T>>> p{};
				// silence annoying VC++ warning
				(void)p;
				return p.push(L, il);
			}
		};

		template <typename T>
		struct unqualified_pusher<T, std::enable_if_t<is_lua_reference<T>::value>> {
			static int push(lua_State* L, const T& ref) {
				return ref.push(L);
			}

			static int push(lua_State* L, T&& ref) {
				return ref.push(L);
			}
		};

		template <>
		struct unqualified_pusher<bool> {
			static int push(lua_State* L, bool b) {
#if defined(SOL_SAFE_STACK_CHECK) && SOL_SAFE_STACK_CHECK
				luaL_checkstack(L, 1, detail::not_enough_stack_space_generic);
#endif // make sure stack doesn't overflow
				lua_pushboolean(L, b);
				return 1;
			}
		};

		template <>
		struct unqualified_pusher<lua_nil_t> {
			static int push(lua_State* L, lua_nil_t) {
#if defined(SOL_SAFE_STACK_CHECK) && SOL_SAFE_STACK_CHECK
				luaL_checkstack(L, 1, detail::not_enough_stack_space_generic);
#endif // make sure stack doesn't overflow
				lua_pushnil(L);
				return 1;
			}
		};

		template <>
		struct unqualified_pusher<stack_count> {
			static int push(lua_State*, stack_count st) {
				return st.count;
			}
		};

		template <>
		struct unqualified_pusher<metatable_t> {
			static int push(lua_State* L, metatable_t) {
#if defined(SOL_SAFE_STACK_CHECK) && SOL_SAFE_STACK_CHECK
				luaL_checkstack(L, 1, detail::not_enough_stack_space_generic);
#endif // make sure stack doesn't overflow
				lua_pushlstring(L, "__mt", 4);
				return 1;
			}
		};

		template <>
		struct unqualified_pusher<std::remove_pointer_t<lua_CFunction>> {
			static int push(lua_State* L, lua_CFunction func, int n = 0) {
#if defined(SOL_SAFE_STACK_CHECK) && SOL_SAFE_STACK_CHECK
				luaL_checkstack(L, 1, detail::not_enough_stack_space_generic);
#endif // make sure stack doesn't overflow
				lua_pushcclosure(L, func, n);
				return 1;
			}
		};

		template <>
		struct unqualified_pusher<lua_CFunction> {
			static int push(lua_State* L, lua_CFunction func, int n = 0) {
#if defined(SOL_SAFE_STACK_CHECK) && SOL_SAFE_STACK_CHECK
				luaL_checkstack(L, 1, detail::not_enough_stack_space_generic);
#endif // make sure stack doesn't overflow
				lua_pushcclosure(L, func, n);
				return 1;
			}
		};

#if defined(SOL_NOEXCEPT_FUNCTION_TYPE) && SOL_NOEXCEPT_FUNCTION_TYPE
		template <>
		struct unqualified_pusher<std::remove_pointer_t<detail::lua_CFunction_noexcept>> {
			static int push(lua_State* L, detail::lua_CFunction_noexcept func, int n = 0) {
#if defined(SOL_SAFE_STACK_CHECK) && SOL_SAFE_STACK_CHECK
				luaL_checkstack(L, 1, detail::not_enough_stack_space_generic);
#endif // make sure stack doesn't overflow
				lua_pushcclosure(L, func, n);
				return 1;
			}
		};

		template <>
		struct unqualified_pusher<detail::lua_CFunction_noexcept> {
			static int push(lua_State* L, detail::lua_CFunction_noexcept func, int n = 0) {
#if defined(SOL_SAFE_STACK_CHECK) && SOL_SAFE_STACK_CHECK
				luaL_checkstack(L, 1, detail::not_enough_stack_space_generic);
#endif // make sure stack doesn't overflow
				lua_pushcclosure(L, func, n);
				return 1;
			}
		};
#endif // noexcept function type

		template <>
		struct unqualified_pusher<c_closure> {
			static int push(lua_State* L, c_closure cc) {
#if defined(SOL_SAFE_STACK_CHECK) && SOL_SAFE_STACK_CHECK
				luaL_checkstack(L, 1, detail::not_enough_stack_space_generic);
#endif // make sure stack doesn't overflow
				lua_pushcclosure(L, cc.c_function, cc.upvalues);
				return 1;
			}
		};

		template <typename Arg, typename... Args>
		struct unqualified_pusher<closure<Arg, Args...>> {
			template <std::size_t... I, typename T>
			static int push(std::index_sequence<I...>, lua_State* L, T&& c) {
				using f_tuple = decltype(std::forward<T>(c).upvalues);
				int pushcount = multi_push(L, std::get<I>(std::forward<f_tuple>(std::forward<T>(c).upvalues))...);
				return stack::push(L, c_closure(c.c_function, pushcount));
			}

			template <typename T>
			static int push(lua_State* L, T&& c) {
				return push(std::make_index_sequence<1 + sizeof...(Args)>(), L, std::forward<T>(c));
			}
		};

		template <>
		struct unqualified_pusher<void*> {
			static int push(lua_State* L, void* userdata) {
#if defined(SOL_SAFE_STACK_CHECK) && SOL_SAFE_STACK_CHECK
				luaL_checkstack(L, 1, detail::not_enough_stack_space_generic);
#endif // make sure stack doesn't overflow
				lua_pushlightuserdata(L, userdata);
				return 1;
			}
		};

		template <>
		struct unqualified_pusher<const void*> {
			static int push(lua_State* L, const void* userdata) {
#if defined(SOL_SAFE_STACK_CHECK) && SOL_SAFE_STACK_CHECK
				luaL_checkstack(L, 1, detail::not_enough_stack_space_generic);
#endif // make sure stack doesn't overflow
				lua_pushlightuserdata(L, const_cast<void*>(userdata));
				return 1;
			}
		};

		template <>
		struct unqualified_pusher<lightuserdata_value> {
			static int push(lua_State* L, lightuserdata_value userdata) {
#if defined(SOL_SAFE_STACK_CHECK) && SOL_SAFE_STACK_CHECK
				luaL_checkstack(L, 1, detail::not_enough_stack_space_generic);
#endif // make sure stack doesn't overflow
				lua_pushlightuserdata(L, userdata);
				return 1;
			}
		};

		template <typename T>
		struct unqualified_pusher<light<T>> {
			static int push(lua_State* L, light<T> l) {
#if defined(SOL_SAFE_STACK_CHECK) && SOL_SAFE_STACK_CHECK
				luaL_checkstack(L, 1, detail::not_enough_stack_space_generic);
#endif // make sure stack doesn't overflow
				lua_pushlightuserdata(L, static_cast<void*>(l.value));
				return 1;
			}
		};

		template <typename T>
		struct unqualified_pusher<user<T>> {
			template <bool with_meta = true, typename Key, typename... Args>
			static int push_with(lua_State* L, Key&& name, Args&&... args) {
#if defined(SOL_SAFE_STACK_CHECK) && SOL_SAFE_STACK_CHECK
				luaL_checkstack(L, 1, detail::not_enough_stack_space_userdata);
#endif // make sure stack doesn't overflow
				// A dumb pusher
				T* data = detail::user_allocate<T>(L);
				std::allocator<T> alloc{};
				std::allocator_traits<std::allocator<T>>::construct(alloc, data, std::forward<Args>(args)...);
				if (with_meta) {
					// Make sure we have a plain GC set for this data
#if defined(SOL_SAFE_STACK_CHECK) && SOL_SAFE_STACK_CHECK
					luaL_checkstack(L, 1, detail::not_enough_stack_space_generic);
#endif // make sure stack doesn't overflow
					if (luaL_newmetatable(L, name) != 0) {
						lua_CFunction cdel = detail::user_alloc_destruct<T>;
						lua_pushcclosure(L, cdel, 0);
						lua_setfield(L, -2, "__gc");
					}
					lua_setmetatable(L, -2);
				}
				return 1;
			}

			template <typename Arg, typename... Args, meta::disable<meta::any_same<meta::unqualified_t<Arg>, no_metatable_t, metatable_t>> = meta::enabler>
			static int push(lua_State* L, Arg&& arg, Args&&... args) {
				const auto name = &usertype_traits<meta::unqualified_t<T>>::user_gc_metatable()[0];
				return push_with(L, name, std::forward<Arg>(arg), std::forward<Args>(args)...);
			}

			template <typename... Args>
			static int push(lua_State* L, no_metatable_t, Args&&... args) {
				const auto name = &usertype_traits<meta::unqualified_t<T>>::user_gc_metatable()[0];
				return push_with<false>(L, name, std::forward<Args>(args)...);
			}

			template <typename Key, typename... Args>
			static int push(lua_State* L, metatable_t, Key&& key, Args&&... args) {
				const auto name = &key[0];
				return push_with<true>(L, name, std::forward<Args>(args)...);
			}

			static int push(lua_State* L, const user<T>& u) {
				const auto name = &usertype_traits<meta::unqualified_t<T>>::user_gc_metatable()[0];
				return push_with(L, name, u.value);
			}

			static int push(lua_State* L, user<T>&& u) {
				const auto name = &usertype_traits<meta::unqualified_t<T>>::user_gc_metatable()[0];
				return push_with(L, name, std::move(u.value));
			}

			static int push(lua_State* L, no_metatable_t, const user<T>& u) {
				const auto name = &usertype_traits<meta::unqualified_t<T>>::user_gc_metatable()[0];
				return push_with<false>(L, name, u.value);
			}

			static int push(lua_State* L, no_metatable_t, user<T>&& u) {
				const auto name = &usertype_traits<meta::unqualified_t<T>>::user_gc_metatable()[0];
				return push_with<false>(L, name, std::move(u.value));
			}
		};

		template <>
		struct unqualified_pusher<userdata_value> {
			static int push(lua_State* L, userdata_value data) {
#if defined(SOL_SAFE_STACK_CHECK) && SOL_SAFE_STACK_CHECK
				luaL_checkstack(L, 1, detail::not_enough_stack_space_userdata);
#endif // make sure stack doesn't overflow
				void** ud = detail::usertype_allocate_pointer<void>(L);
				*ud = data.value;
				return 1;
			}
		};

		template <>
		struct unqualified_pusher<const char*> {
			static int push_sized(lua_State* L, const char* str, std::size_t len) {
#if defined(SOL_SAFE_STACK_CHECK) && SOL_SAFE_STACK_CHECK
				luaL_checkstack(L, 1, detail::not_enough_stack_space_string);
#endif // make sure stack doesn't overflow
				lua_pushlstring(L, str, len);
				return 1;
			}

			static int push(lua_State* L, const char* str) {
				if (str == nullptr)
					return stack::push(L, lua_nil);
				return push_sized(L, str, std::char_traits<char>::length(str));
			}

			static int push(lua_State* L, const char* strb, const char* stre) {
				return push_sized(L, strb, stre - strb);
			}

			static int push(lua_State* L, const char* str, std::size_t len) {
				return push_sized(L, str, len);
			}
		};

		template <>
		struct unqualified_pusher<char*> {
			static int push_sized(lua_State* L, const char* str, std::size_t len) {
				unqualified_pusher<const char*> p{};
				(void)p;
				return p.push_sized(L, str, len);
			}

			static int push(lua_State* L, const char* str) {
				unqualified_pusher<const char*> p{};
				(void)p;
				return p.push(L, str);
			}

			static int push(lua_State* L, const char* strb, const char* stre) {
				unqualified_pusher<const char*> p{};
				(void)p;
				return p.push(L, strb, stre);
			}

			static int push(lua_State* L, const char* str, std::size_t len) {
				unqualified_pusher<const char*> p{};
				(void)p;
				return p.push(L, str, len);
			}
		};

		template <size_t N>
		struct unqualified_pusher<char[N]> {
			static int push(lua_State* L, const char (&str)[N]) {
#if defined(SOL_SAFE_STACK_CHECK) && SOL_SAFE_STACK_CHECK
				luaL_checkstack(L, 1, detail::not_enough_stack_space_string);
#endif // make sure stack doesn't overflow
				lua_pushlstring(L, str, std::char_traits<char>::length(str));
				return 1;
			}

			static int push(lua_State* L, const char (&str)[N], std::size_t sz) {
#if defined(SOL_SAFE_STACK_CHECK) && SOL_SAFE_STACK_CHECK
				luaL_checkstack(L, 1, detail::not_enough_stack_space_string);
#endif // make sure stack doesn't overflow
				lua_pushlstring(L, str, sz);
				return 1;
			}
		};

		template <>
		struct unqualified_pusher<char> {
			static int push(lua_State* L, char c) {
				const char str[2] = { c, '\0' };
				return stack::push(L, str, 1);
			}
		};

		template <typename Traits, typename Al>
		struct unqualified_pusher<std::basic_string<char, Traits, Al>> {
			static int push(lua_State* L, const std::basic_string<char, Traits, Al>& str) {
#if defined(SOL_SAFE_STACK_CHECK) && SOL_SAFE_STACK_CHECK
				luaL_checkstack(L, 1, detail::not_enough_stack_space_string);
#endif // make sure stack doesn't overflow
				lua_pushlstring(L, str.c_str(), str.size());
				return 1;
			}

			static int push(lua_State* L, const std::basic_string<char, Traits, Al>& str, std::size_t sz) {
#if defined(SOL_SAFE_STACK_CHECK) && SOL_SAFE_STACK_CHECK
				luaL_checkstack(L, 1, detail::not_enough_stack_space_string);
#endif // make sure stack doesn't overflow
				lua_pushlstring(L, str.c_str(), sz);
				return 1;
			}
		};

		template <typename Ch, typename Traits>
		struct unqualified_pusher<basic_string_view<Ch, Traits>> {
			static int push(lua_State* L, const basic_string_view<Ch, Traits>& sv) {
				return stack::push(L, sv.data(), sv.length());
			}

			static int push(lua_State* L, const basic_string_view<Ch, Traits>& sv, std::size_t n) {
				return stack::push(L, sv.data(), n);
			}
		};

		template <>
		struct unqualified_pusher<meta_function> {
			static int push(lua_State* L, meta_function m) {
#if defined(SOL_SAFE_STACK_CHECK) && SOL_SAFE_STACK_CHECK
				luaL_checkstack(L, 1, detail::not_enough_stack_space_meta_function_name);
#endif // make sure stack doesn't overflow
				const std::string& str = to_string(m);
				lua_pushlstring(L, str.c_str(), str.size());
				return 1;
			}
		};

		template <>
		struct unqualified_pusher<absolute_index> {
			static int push(lua_State* L, absolute_index ai) {
#if defined(SOL_SAFE_STACK_CHECK) && SOL_SAFE_STACK_CHECK
				luaL_checkstack(L, 1, detail::not_enough_stack_space_generic);
#endif // make sure stack doesn't overflow
				lua_pushvalue(L, ai);
				return 1;
			}
		};

		template <>
		struct unqualified_pusher<raw_index> {
			static int push(lua_State* L, raw_index ri) {
#if defined(SOL_SAFE_STACK_CHECK) && SOL_SAFE_STACK_CHECK
				luaL_checkstack(L, 1, detail::not_enough_stack_space_generic);
#endif // make sure stack doesn't overflow
				lua_pushvalue(L, ri);
				return 1;
			}
		};

		template <>
		struct unqualified_pusher<ref_index> {
			static int push(lua_State* L, ref_index ri) {
#if defined(SOL_SAFE_STACK_CHECK) && SOL_SAFE_STACK_CHECK
				luaL_checkstack(L, 1, detail::not_enough_stack_space_generic);
#endif // make sure stack doesn't overflow
				lua_rawgeti(L, LUA_REGISTRYINDEX, ri);
				return 1;
			}
		};

		template <>
		struct unqualified_pusher<const wchar_t*> {
			static int push(lua_State* L, const wchar_t* wstr) {
				return push(L, wstr, std::char_traits<wchar_t>::length(wstr));
			}

			static int push(lua_State* L, const wchar_t* wstr, std::size_t sz) {
				return push(L, wstr, wstr + sz);
			}

			static int push(lua_State* L, const wchar_t* strb, const wchar_t* stre) {
				if constexpr (sizeof(wchar_t) == 2) {
					const char16_t* sb = reinterpret_cast<const char16_t*>(strb);
					const char16_t* se = reinterpret_cast<const char16_t*>(stre);
					return stack::push(L, sb, se);
				}
				else {
					const char32_t* sb = reinterpret_cast<const char32_t*>(strb);
					const char32_t* se = reinterpret_cast<const char32_t*>(stre);
					return stack::push(L, sb, se);
				}
			}
		};

		template <>
		struct unqualified_pusher<wchar_t*> {
			static int push(lua_State* L, const wchar_t* str) {
				unqualified_pusher<const wchar_t*> p{};
				(void)p;
				return p.push(L, str);
			}

			static int push(lua_State* L, const wchar_t* strb, const wchar_t* stre) {
				unqualified_pusher<const wchar_t*> p{};
				(void)p;
				return p.push(L, strb, stre);
			}

			static int push(lua_State* L, const wchar_t* str, std::size_t len) {
				unqualified_pusher<const wchar_t*> p{};
				(void)p;
				return p.push(L, str, len);
			}
		};

		template <>
		struct unqualified_pusher<const char16_t*> {
			static int convert_into(lua_State* L, char* start, std::size_t, const char16_t* strb, const char16_t* stre) {
				char* target = start;
				char32_t cp = 0;
				for (const char16_t* strtarget = strb; strtarget < stre;) {
					auto dr = unicode::utf16_to_code_point(strtarget, stre);
					if (dr.error != unicode::error_code::ok) {
						cp = unicode::unicode_detail::replacement;
					}
					else {
						cp = dr.codepoint;
					}
					auto er = unicode::code_point_to_utf8(cp);
					const char* utf8data = er.code_units.data();
					std::memcpy(target, utf8data, er.code_units_size);
					target += er.code_units_size;
					strtarget = dr.next;
				}

				return stack::push(L, start, target);
			}

			static int push(lua_State* L, const char16_t* u16str) {
				return push(L, u16str, std::char_traits<char16_t>::length(u16str));
			}

			static int push(lua_State* L, const char16_t* u16str, std::size_t sz) {
				return push(L, u16str, u16str + sz);
			}

			static int push(lua_State* L, const char16_t* strb, const char16_t* stre) {
				char sbo[SOL_STACK_STRING_OPTIMIZATION_SIZE];
				// if our max string space is small enough, use SBO
				// right off the bat
				std::size_t max_possible_code_units = (stre - strb) * 4;
				if (max_possible_code_units <= SOL_STACK_STRING_OPTIMIZATION_SIZE) {
					return convert_into(L, sbo, max_possible_code_units, strb, stre);
				}
				// otherwise, we must manually count/check size
				std::size_t needed_size = 0;
				for (const char16_t* strtarget = strb; strtarget < stre;) {
					auto dr = unicode::utf16_to_code_point(strtarget, stre);
					auto er = unicode::code_point_to_utf8(dr.codepoint);
					needed_size += er.code_units_size;
					strtarget = dr.next;
				}
				if (needed_size < SOL_STACK_STRING_OPTIMIZATION_SIZE) {
					return convert_into(L, sbo, needed_size, strb, stre);
				}
				std::string u8str("", 0);
				u8str.resize(needed_size);
				char* target = &u8str[0];
				return convert_into(L, target, needed_size, strb, stre);
			}
		};

		template <>
		struct unqualified_pusher<char16_t*> {
			static int push(lua_State* L, const char16_t* str) {
				unqualified_pusher<const char16_t*> p{};
				(void)p;
				return p.push(L, str);
			}

			static int push(lua_State* L, const char16_t* strb, const char16_t* stre) {
				unqualified_pusher<const char16_t*> p{};
				(void)p;
				return p.push(L, strb, stre);
			}

			static int push(lua_State* L, const char16_t* str, std::size_t len) {
				unqualified_pusher<const char16_t*> p{};
				(void)p;
				return p.push(L, str, len);
			}
		};

		template <>
		struct unqualified_pusher<const char32_t*> {
			static int convert_into(lua_State* L, char* start, std::size_t, const char32_t* strb, const char32_t* stre) {
				char* target = start;
				char32_t cp = 0;
				for (const char32_t* strtarget = strb; strtarget < stre;) {
					auto dr = unicode::utf32_to_code_point(strtarget, stre);
					if (dr.error != unicode::error_code::ok) {
						cp = unicode::unicode_detail::replacement;
					}
					else {
						cp = dr.codepoint;
					}
					auto er = unicode::code_point_to_utf8(cp);
					const char* data = er.code_units.data();
					std::memcpy(target, data, er.code_units_size);
					target += er.code_units_size;
					strtarget = dr.next;
				}
				return stack::push(L, start, target);
			}

			static int push(lua_State* L, const char32_t* u32str) {
				return push(L, u32str, u32str + std::char_traits<char32_t>::length(u32str));
			}

			static int push(lua_State* L, const char32_t* u32str, std::size_t sz) {
				return push(L, u32str, u32str + sz);
			}

			static int push(lua_State* L, const char32_t* strb, const char32_t* stre) {
				char sbo[SOL_STACK_STRING_OPTIMIZATION_SIZE];
				// if our max string space is small enough, use SBO
				// right off the bat
				std::size_t max_possible_code_units = (stre - strb) * 4;
				if (max_possible_code_units <= SOL_STACK_STRING_OPTIMIZATION_SIZE) {
					return convert_into(L, sbo, max_possible_code_units, strb, stre);
				}
				// otherwise, we must manually count/check size
				std::size_t needed_size = 0;
				for (const char32_t* strtarget = strb; strtarget < stre;) {
					auto dr = unicode::utf32_to_code_point(strtarget, stre);
					auto er = unicode::code_point_to_utf8(dr.codepoint);
					needed_size += er.code_units_size;
					strtarget = dr.next;
				}
				if (needed_size < SOL_STACK_STRING_OPTIMIZATION_SIZE) {
					return convert_into(L, sbo, needed_size, strb, stre);
				}
				std::string u8str("", 0);
				u8str.resize(needed_size);
				char* target = &u8str[0];
				return convert_into(L, target, needed_size, strb, stre);
			}
		};

		template <>
		struct unqualified_pusher<char32_t*> {
			static int push(lua_State* L, const char32_t* str) {
				unqualified_pusher<const char32_t*> p{};
				(void)p;
				return p.push(L, str);
			}

			static int push(lua_State* L, const char32_t* strb, const char32_t* stre) {
				unqualified_pusher<const char32_t*> p{};
				(void)p;
				return p.push(L, strb, stre);
			}

			static int push(lua_State* L, const char32_t* str, std::size_t len) {
				unqualified_pusher<const char32_t*> p{};
				(void)p;
				return p.push(L, str, len);
			}
		};

		template <size_t N>
		struct unqualified_pusher<wchar_t[N]> {
			static int push(lua_State* L, const wchar_t (&str)[N]) {
				return push(L, str, std::char_traits<wchar_t>::length(str));
			}

			static int push(lua_State* L, const wchar_t (&str)[N], std::size_t sz) {
				return stack::push<const wchar_t*>(L, str, str + sz);
			}
		};

		template <size_t N>
		struct unqualified_pusher<char16_t[N]> {
			static int push(lua_State* L, const char16_t (&str)[N]) {
				return push(L, str, std::char_traits<char16_t>::length(str));
			}

			static int push(lua_State* L, const char16_t (&str)[N], std::size_t sz) {
				return stack::push<const char16_t*>(L, str, str + sz);
			}
		};

		template <size_t N>
		struct unqualified_pusher<char32_t[N]> {
			static int push(lua_State* L, const char32_t (&str)[N]) {
				return push(L, str, std::char_traits<char32_t>::length(str));
			}

			static int push(lua_State* L, const char32_t (&str)[N], std::size_t sz) {
				return stack::push<const char32_t*>(L, str, str + sz);
			}
		};

		template <>
		struct unqualified_pusher<wchar_t> {
			static int push(lua_State* L, wchar_t c) {
				const wchar_t str[2] = { c, '\0' };
				return stack::push(L, &str[0], 1);
			}
		};

		template <>
		struct unqualified_pusher<char16_t> {
			static int push(lua_State* L, char16_t c) {
				const char16_t str[2] = { c, '\0' };
				return stack::push(L, &str[0], 1);
			}
		};

		template <>
		struct unqualified_pusher<char32_t> {
			static int push(lua_State* L, char32_t c) {
				const char32_t str[2] = { c, '\0' };
				return stack::push(L, &str[0], 1);
			}
		};

		template <typename Ch, typename Traits, typename Al>
		struct unqualified_pusher<std::basic_string<Ch, Traits, Al>, std::enable_if_t<!std::is_same<Ch, char>::value>> {
			static int push(lua_State* L, const std::basic_string<Ch, Traits, Al>& wstr) {
				return push(L, wstr, wstr.size());
			}

			static int push(lua_State* L, const std::basic_string<Ch, Traits, Al>& wstr, std::size_t sz) {
				return stack::push(L, wstr.data(), wstr.data() + sz);
			}
		};

		template <typename... Args>
		struct unqualified_pusher<std::tuple<Args...>> {
			template <std::size_t... I, typename T>
			static int push(std::index_sequence<I...>, lua_State* L, T&& t) {
#if defined(SOL_SAFE_STACK_CHECK) && SOL_SAFE_STACK_CHECK
				luaL_checkstack(L, static_cast<int>(sizeof...(I)), detail::not_enough_stack_space_generic);
#endif // make sure stack doesn't overflow
				int pushcount = 0;
				(void)detail::swallow{ 0, (pushcount += stack::push(L, std::get<I>(std::forward<T>(t))), 0)... };
				return pushcount;
			}

			template <typename T>
			static int push(lua_State* L, T&& t) {
				return push(std::index_sequence_for<Args...>(), L, std::forward<T>(t));
			}
		};

		template <typename A, typename B>
		struct unqualified_pusher<std::pair<A, B>> {
			template <typename T>
			static int push(lua_State* L, T&& t) {
				int pushcount = stack::push(L, std::get<0>(std::forward<T>(t)));
				pushcount += stack::push(L, std::get<1>(std::forward<T>(t)));
				return pushcount;
			}
		};

		template <typename O>
		struct unqualified_pusher<optional<O>> {
			template <typename T>
			static int push(lua_State* L, T&& t) {
				if (t == nullopt) {
					return stack::push(L, nullopt);
				}
				return stack::push(L, static_cast<std::conditional_t<std::is_lvalue_reference<T>::value, O&, O&&>>(t.value()));
			}
		};

		template <>
		struct unqualified_pusher<nullopt_t> {
			static int push(lua_State* L, nullopt_t) {
				return stack::push(L, lua_nil);
			}
		};

		template <>
		struct unqualified_pusher<std::nullptr_t> {
			static int push(lua_State* L, std::nullptr_t) {
				return stack::push(L, lua_nil);
			}
		};

		template <>
		struct unqualified_pusher<this_state> {
			static int push(lua_State*, const this_state&) {
				return 0;
			}
		};

		template <>
		struct unqualified_pusher<this_main_state> {
			static int push(lua_State*, const this_main_state&) {
				return 0;
			}
		};

		template <>
		struct unqualified_pusher<new_table> {
			static int push(lua_State* L, const new_table& nt) {
				lua_createtable(L, nt.sequence_hint, nt.map_hint);
				return 1;
			}
		};

#if defined(SOL_CXX17_FEATURES) && SOL_CXX17_FEATURES
		template <typename O>
		struct unqualified_pusher<std::optional<O>> {
			template <typename T>
			static int push(lua_State* L, T&& t) {
				if (t == std::nullopt) {
					return stack::push(L, nullopt);
				}
				return stack::push(L, static_cast<std::conditional_t<std::is_lvalue_reference<T>::value, O&, O&&>>(t.value()));
			}
		};

#if defined(SOL_STD_VARIANT) && SOL_STD_VARIANT
		namespace stack_detail {

			struct push_function {
				lua_State* L;

				push_function(lua_State* L) : L(L) {
				}

				template <typename T>
				int operator()(T&& value) const {
					return stack::push<T>(L, std::forward<T>(value));
				}
			};

		} // namespace stack_detail

		template <typename... Tn>
		struct unqualified_pusher<std::variant<Tn...>> {
			static int push(lua_State* L, const std::variant<Tn...>& v) {
				return std::visit(stack_detail::push_function(L), v);
			}

			static int push(lua_State* L, std::variant<Tn...>&& v) {
				return std::visit(stack_detail::push_function(L), std::move(v));
			}
		};
#endif // Variant because Clang is terrible
#endif // C++17 Support
	}
} // namespace sol::stack

#endif // SOL_STACK_PUSH_HPP
