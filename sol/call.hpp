// The MIT License (MIT)

// Copyright (c) 2013-2016 Rappt1101010z, ThePhD and contributors

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

#ifndef SOL_CALL_HPP
#define SOL_CALL_HPP

#include "wrapper.hpp"
#include "property.hpp"
#include "stack.hpp"

namespace sol { 
namespace call_detail {
	
	template <bool b, typename F>
	inline decltype(auto) pick(std::integral_constant<bool, b>, F&& f) {
		return std::forward<F>(f);
	}

	template <typename R, typename W>
	inline auto& pick(std::true_type, property_wrapper<R, W>& f) {
		return f.read;
	}

	template <typename R, typename W>
	inline auto& pick(std::false_type, property_wrapper<R, W>& f) {
		return f.write;
	}

	namespace overload_detail {
		template <std::size_t... M, typename Match, typename... Args>
		inline int overload_match_arity(sol::types<>, std::index_sequence<>, std::index_sequence<M...>, Match&&, lua_State* L, int, int, Args&&...) {
			return luaL_error(L, "sol: no matching function call takes this number of arguments and the specified types");
		}

		template <typename Fx, typename... Fxs, std::size_t I, std::size_t... In, std::size_t... M, typename Match, typename... Args>
		inline int overload_match_arity(sol::types<Fx, Fxs...>, std::index_sequence<I, In...>, std::index_sequence<M...>, Match&& matchfx, lua_State* L, int fxarity, int start, Args&&... args) {
			typedef lua_bind_traits<meta::unqualified_t<Fx>> traits;
			typedef meta::tuple_types<typename traits::return_type> return_types;
			typedef typename traits::free_args_list args_list;
			typedef typename args_list::indices args_indices;
			int farity = traits::free_arity;
			// compile-time eliminate any functions that we know ahead of time are of improper arity
			if (meta::find_in_pack_v<index_value<traits::free_arity>, index_value<M>...>::value) {
				return overload_match_arity(types<Fxs...>(), std::index_sequence<In...>(), std::index_sequence<M...>(), std::forward<Match>(matchfx), L, fxarity, start, std::forward<Args>(args)...);
			}
			if (traits::free_arity != fxarity) {
				return overload_match_arity(types<Fxs...>(), std::index_sequence<In...>(), std::index_sequence<traits::arity, M...>(), std::forward<Match>(matchfx), L, fxarity, start, std::forward<Args>(args)...);
			}
			if (!stack::stack_detail::check_types<true>().check(args_list(), args_indices(), L, start, no_panic)) {
				return overload_match_arity(types<Fxs...>(), std::index_sequence<In...>(), std::index_sequence<M...>(), std::forward<Match>(matchfx), L, fxarity, start, std::forward<Args>(args)...);
			}
			return matchfx(types<Fx>(), index_value<I>(), return_types(), args_list(), L, fxarity, start, std::forward<Args>(args)...);
		}
	} // overload_detail

	template <typename... Functions, typename Match, typename... Args>
	inline int overload_match_arity(Match&& matchfx, lua_State* L, int fxarity, int start, Args&&... args) {
		return overload_detail::overload_match_arity(types<Functions...>(), std::make_index_sequence<sizeof...(Functions)>(), std::index_sequence<>(), std::forward<Match>(matchfx), L, fxarity, start, std::forward<Args>(args)...);
	}

	template <typename... Functions, typename Match, typename... Args>
	inline int overload_match(Match&& matchfx, lua_State* L, int start, Args&&... args) {
		int fxarity = lua_gettop(L) - (start - 1);
		return overload_match_arity<Functions...>(std::forward<Match>(matchfx), L, fxarity, start, std::forward<Args>(args)...);
	}

	template <typename F, bool is_index, bool is_variable, typename = void>
	struct agnostic_lua_call_wrapper {
		static int var_call(std::true_type, lua_State* L, F f) {
			typedef wrapper<meta::unqualified_t<F>> wrap;
			typedef typename wrap::returns_list returns_list;
			typedef typename wrap::free_args_list args_list;
			return stack::call_into_lua<is_index ? 1 : 2>(returns_list(), args_list(), L, is_index ? 2 : 3, wrap::caller(), f);
		}

		static int var_call(std::false_type, lua_State* L, F f) {
			typedef wrapper<meta::unqualified_t<F>> wrap;
			typedef typename wrap::free_args_list args_list;
			typedef typename wrap::returns_list returns_list;
			return stack::call_into_lua(returns_list(), args_list(), L, 1, wrap::caller(), f);
		}

		static int call(lua_State* L, F f) {
			return var_call(std::integral_constant<bool, is_variable>(), L, f);
		}
	};

	template <typename F, bool is_index, bool is_variable>
	struct agnostic_lua_call_wrapper<F, is_index, is_variable, std::enable_if_t<std::is_member_function_pointer<F>::value>> {
		static int call(lua_State* L, F f) {
			typedef wrapper<meta::unqualified_t<F>> wrap;
			typedef typename wrap::returns_list returns_list;
			typedef typename wrap::args_list args_list;
#ifdef SOL_SAFE_USERTYPE
			typedef typename wrap::object_type object_type;
			object_type* o = stack::get<object_type*>(L, 1);
			if (o == nullptr) {
				return luaL_error(L, "sol: received null for 'self' argument (use ':' for accessing member functions, make sure member variables are preceeded by the actual object with '.' syntax)");
			}
			return stack::call_into_lua<is_variable ? 2 : 1>(returns_list(), args_list(), L, is_variable ? 3 : 2, wrap::caller(), f, *o);
#else
			object_type& o = stack::get<object_type&>(L, 1);
			return stack::call_into_lua<is_variable ? 2 : 1>(returns_list(), args_list(), L, is_variable ? 3 : 2, wrap::caller(), f);
#endif // Safety
		}
	};

	template <bool is_index, bool is_variable, typename C>
	struct agnostic_lua_call_wrapper<no_prop, is_index, is_variable, C> {
		static int call(lua_State* L, no_prop) {
			return luaL_error(L, is_index ? "sol: cannot read from a writeonly property" : "sol: cannot write to a readonly property");
		}
	};

	template <typename F, bool is_variable>
	struct agnostic_lua_call_wrapper<F, false, is_variable, std::enable_if_t<std::is_member_object_pointer<F>::value>> {
		typedef sol::lua_bind_traits<F> traits_type;

		static int call_assign(std::true_type, lua_State* L, F f) {
			typedef wrapper<meta::unqualified_t<F>> wrap;
			typedef typename wrap::args_list args_list;
			typedef typename wrap::object_type object_type;
#ifdef SOL_SAFE_USERTYPE
			object_type* o = stack::get<object_type*>(L, 1);
			if (o == nullptr) {
				if (is_variable) {
					return luaL_error(L, "sol: received null for 'self' argument (bad '.' access?)");
				}
				return luaL_error(L, "sol: received null for 'self' argument (pass 'self' as first argument)");
			}
			return stack::call_into_lua<is_variable ? 2 : 1>(types<void>(), args_list(), L, is_variable ? 3 : 2, wrap::caller(), f, *o);
#else
			object_type& o = stack::get<object_type&>(L, 1);
			return stack::call_into_lua<is_variable ? 2 : 1>(types<void>(), args_list(), L, is_variable ? 3 : 2, wrap::caller(), f, o);
#endif // Safety
		}

		static int call_assign(std::false_type, lua_State* L, F f) {
			return luaL_error(L, "sol: cannot write to this variable: copy assignment/constructor not available");
		}

		static int call_const(std::false_type, lua_State* L, F f) {
			typedef typename traits_type::return_type R;
			return call_assign(std::is_assignable<std::add_lvalue_reference_t<meta::unqualified_t<R>>, R>(), L, f);
		}

		static int call_const(std::true_type, lua_State* L, F f) {
			return luaL_error(L, "sol: cannot write to a readonly (const) variable");
		}

		static int call(lua_State* L, F f) {
			return call_const(std::is_const<typename traits_type::return_type>(), L, f);
		}
	};

	template <typename F, bool is_variable>
	struct agnostic_lua_call_wrapper<F, true, is_variable, std::enable_if_t<std::is_member_object_pointer<F>::value>> {
		typedef sol::lua_bind_traits<F> traits_type;

		static int call(lua_State* L, F f) {
			typedef wrapper<meta::unqualified_t<F>> wrap;
			typedef typename wrap::object_type object_type;
			typedef typename wrap::returns_list returns_list;
#ifdef SOL_SAFE_USERTYPE
			object_type* o = stack::get<object_type*>(L, 1);
			if (o == nullptr) {
				if (is_variable) {
					return luaL_error(L, "sol: 'self' argument is nullptr (bad '.' access?)");
				}
				return luaL_error(L, "sol: 'self' argument is nullptr (pass 'self' as first argument)");
			}
			return stack::call_into_lua<is_variable ? 2 : 1>(returns_list(), types<>(), L, is_variable ? 3 : 2, wrap::caller(), f, *o);
#else
			object_type& o = stack::get<object_type&>(L, 1);
			return stack::call_into_lua<is_variable ? 2 : 1>(returns_list(), types<>(), L, is_variable ? 3 : 2, wrap::caller(), f, o);
#endif // Safety
		}
	};

	template <typename... Fs, bool is_index, bool is_variable, typename C>
	struct agnostic_lua_call_wrapper<sol::overload_set<Fs...>, is_index, is_variable, C> {
		typedef sol::overload_set<Fs...> F;

		template <typename Fx, std::size_t I, typename... R, typename... Args>
		static int select_call(sol::types<Fx>, sol::index_value<I>, sol::types<R...> r, sol::types<Args...> a, lua_State* L, int, int start, F& fx) {
			auto& f = std::get<I>(fx.set);
			return agnostic_lua_call_wrapper<Fx, is_index, is_variable>{}.call(L, f);
		}

		static int call(lua_State* L, F& fx) {
			auto mfx = [&](auto&&... args) { return select_call(std::forward<decltype(args)>(args)...); };
			return overload_match_arity<Fs...>(mfx, L, lua_gettop(L), 1, fx);
		}
	};

	template <bool is_index, bool is_variable, typename C>
	struct agnostic_lua_call_wrapper<sol::no_construction, is_index, is_variable, C> {
		static int call(lua_State* L, sol::no_construction&) {
			return luaL_error(L, "cannot call something tagged with 'no construction'");
		}
	};

	template <typename T, typename F, bool is_index, bool is_variable, typename = void>
	struct lua_call_wrapper : agnostic_lua_call_wrapper<F, is_index, is_variable> {};

	template <typename T, typename... Args, bool is_index, bool is_variable, typename C>
	struct lua_call_wrapper<T, sol::constructor_list<Args...>, is_index, is_variable, C> {
		typedef sol::constructor_list<Args...> F;

		static int call(lua_State* L, F&) {
			static const auto& meta = usertype_traits<T>::metatable;
			int argcount = lua_gettop(L);
			call_syntax syntax = argcount > 0 ? stack::get_call_syntax(L, meta, 1) : call_syntax::dot;
			argcount -= static_cast<int>(syntax);

			T** pointerpointer = reinterpret_cast<T**>(lua_newuserdata(L, sizeof(T*) + sizeof(T)));
			T*& referencepointer = *pointerpointer;
			T* obj = reinterpret_cast<T*>(pointerpointer + 1);
			referencepointer = obj;
			reference userdataref(L, -1);
			userdataref.pop();

			function_detail::construct<T, Args...>(detail::constructor_match<T>(obj), L, argcount, 1 + static_cast<int>(syntax));

			userdataref.push();
			luaL_getmetatable(L, &meta[0]);
			if (stack::get<type>(L) == type::nil) {
				lua_pop(L, 1);
				return luaL_error(L, "sol: unable to get usertype metatable");
			}

			lua_setmetatable(L, -2);
			return 1;
		}
	};

	template <typename T, bool is_index, bool is_variable, typename Fx>
	int call_wrapped(lua_State* L, Fx&& fx) {
		return lua_call_wrapper<T, meta::unqualified_t<Fx>, is_index, is_variable>{}.call(L, std::forward<Fx>(fx));
	}

	template <typename T, typename = void>
	struct is_var_bind : std::false_type {};

	template <typename T>
	struct is_var_bind<T, std::enable_if_t<std::is_member_object_pointer<T>::value>> : std::true_type {};

	template <>
	struct is_var_bind<no_prop> : std::true_type {};

	template <typename R, typename W>
	struct is_var_bind<property_wrapper<R, W>> : std::true_type {};

} // call_detail

template <typename T>
struct is_variable_binding : call_detail::is_var_bind<meta::unqualified_t<T>> {};

template <typename T>
struct is_function_binding : meta::neg<is_variable_binding<T>> {};

} // sol

#endif // SOL_CALL_HPP