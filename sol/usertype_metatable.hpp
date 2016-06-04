// The MIT License (MIT) 

// Copyright (c) 2013-2016 Rapptz, ThePhD and contributors

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

#ifndef SOL_USERTYPE_METATABLE_HPP
#define SOL_USERTYPE_METATABLE_HPP

#include "wrapper.hpp"
#include "call.hpp"
#include "stack.hpp"
#include "types.hpp"

namespace sol {

	namespace usertype_detail {
		inline bool is_index(string_detail::string_shim s) {
			return s == name_of(meta_function::index) || s == name_of(meta_function::new_index);
		}

		inline bool is_index(meta_function mf) {
			return mf == meta_function::index || mf == meta_function::new_index;
		}
	}

	template <typename T, typename Tuple>
	struct usertype_metatable {
		typedef std::make_index_sequence<std::tuple_size<Tuple>::value> indices;
		typedef std::make_index_sequence<std::tuple_size<Tuple>::value / 2> half_indices;
		template <std::size_t I>
		struct check_binding : is_variable_binding<std::tuple_element_t<I, Tuple>> {};
		Tuple functions;
		bool must_index;

		template <std::size_t... I>
		static bool contains_variable(std::index_sequence<I...>) {
			typedef meta::any<check_binding<(I * 2 + 1)>...> has_variables;
			return has_variables::value;
		}

		template <std::size_t... I>
		bool contains_index(std::index_sequence<I...>) const {
			bool idx = false;
			detail::swallow{ 0, ((idx &= usertype_detail::is_index(std::get<I * 2>(functions))), 0) ... };
			return idx;
		}

		usertype_metatable(Tuple t) : functions(std::move(t)), must_index(contains_variable(half_indices()) || contains_index(half_indices())) {}

		template <bool is_index>
		int find_call(std::integral_constant<bool, is_index>, std::index_sequence<>, lua_State* L, const sol::string_detail::string_shim& accessor) {
			return luaL_error(L, "sol: attempt to index nil value \"%s\" on userdata (bad (misspelled?) key name or does not exist)", accessor.data());
		}

		template <bool is_index, std::size_t I0, std::size_t I1, std::size_t... In>
		int find_call(std::integral_constant<bool, is_index> idx, std::index_sequence<I0, I1, In...>, lua_State* L, const sol::string_detail::string_shim& accessor) {
			string_detail::string_shim name(std::get<I0>(functions));
			if (accessor == name) {
				if (is_variable_binding<decltype(std::get<I1>(functions))>::value) {
					return call_with<I1, is_index, true>(L, *this);
				}
				return stack::push(L, c_closure(call<I1, is_index>, stack::push(L, light<usertype_metatable>(*this))));
			}
			return find_call(idx, std::index_sequence<In...>(), L, accessor);
		}

		template <std::size_t I, bool is_index = true, bool is_variable = false>
		static int call(lua_State* L) {
			usertype_metatable& f = stack::get<light<usertype_metatable>>(L, up_value_index(1));
			return call_with<I, is_index, is_variable>(L, f);
		}

		template <std::size_t I, bool is_index = true, bool is_variable = false>
		static int call_with(lua_State* L, usertype_metatable& um) {
			auto& f = call_detail::pick(std::integral_constant<bool, is_index>(), std::get<I>(um.functions));
			return call_detail::call_wrapped<T, is_index, is_variable>(L, f);
		}

		static int index_call(lua_State* L) {
			usertype_metatable& f = stack::get<light<usertype_metatable>>(L, up_value_index(1));
			string_detail::string_shim accessor = stack::get<string_detail::string_shim>(L, -1);
			return f.find_call(std::true_type(), std::make_index_sequence<std::tuple_size<Tuple>::value>(), L, accessor);
		}

		static int new_index_call(lua_State* L) {
			usertype_metatable& f = stack::get<light<usertype_metatable>>(L, up_value_index(1));
			string_detail::string_shim accessor = stack::get<string_detail::string_shim>(L, -2);
			return f.find_call(std::false_type(), std::make_index_sequence<std::tuple_size<Tuple>::value>(), L, accessor);
		}

		static int gc_call(lua_State* L) {
			stack_object o(L, 1);
			type x = o.get_type();
			usertype_metatable& f = stack::get<light<usertype_metatable>>(L, up_value_index(1));
			f.~usertype_metatable();
			return 0;
		}
	};

	namespace stack {

		template <typename T, typename Tuple>
		struct pusher<usertype_metatable<T, Tuple>> {
			typedef usertype_metatable<T, Tuple> umt_t;

			static void cleanup(lua_State* L, int metaidx, usertype_metatable<T, Tuple>&& um) {
				const char* metakey = &usertype_traits<T>::gc_table[0];
				lua_createtable(L, 1, 0);
				stack_table t(L, -1);
				t[meta_function::garbage_collect] = umt_t::gc_call;
				stack::set_field(L, metakey, t, metaidx);
			}

			template <std::size_t... I>
			static int push(std::index_sequence<I...>, lua_State* L, usertype_metatable<T, Tuple>&& um) {
				// Basic index pushing: specialize
				// index and newindex to give variables and stuff
				// make sure to return final T metatable table
				const bool mustindex = um.must_index;
				for (std::size_t i = 0; i < 3; ++i) {
					// Pointer types, AKA "references" from C++
					const char* metakey = nullptr;
					switch (i) {
					case 0:
						metakey = &usertype_traits<T*>::metatable[0];
						break;
					case 1:
						metakey = &usertype_traits<detail::unique_usertype<T>>::metatable[0];
						break;
					case 2:
					default:
						metakey = &usertype_traits<T>::metatable[0];
						break;
					}
					luaL_newmetatable(L, metakey);
					stack_table t(L, -1);
					stack::push(L, make_light(um));
					luaL_Reg l[] = {
						{ &std::get<I * 2>(um.functions)[0], umt_t::call<(I * 2) + 1> }...,
						{ nullptr, nullptr }
					};
					luaL_setfuncs(L, l, 1);

					if (mustindex) {
						t[meta_function::index] = make_closure(umt_t::index_call, make_light(um));
						t[meta_function::new_index] = make_closure(umt_t::new_index_call, make_light(um));
					}
					else {
						t[meta_function::index] = t;
					}
					if (i < 2) {
						// We want to just leave the table
						// in the registry only, otherwise we return it
						t.pop();
					}
				}
				int metaidx = lua_gettop(L);
				cleanup(L, metaidx, std::move(um));
				return 1;
			}
				
			static int push(lua_State* L, usertype_metatable<T, Tuple>&& um) {
				return push(std::make_index_sequence<std::tuple_size<Tuple>::value / 2>(), L, std::move(um));
			}
		};

	} // stack

} // sol

#endif // SOL_USERTYPE_METATABLE_HPP