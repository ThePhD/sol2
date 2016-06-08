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
#include "stack_reference.hpp"
#include "usertype_traits.hpp"
#include "inheritance.hpp"
#include "raii.hpp"
#include "deprecate.hpp"

namespace sol {

	namespace usertype_detail {
		inline bool is_indexer(string_detail::string_shim s) {
			return s == name_of(meta_function::index) || s == name_of(meta_function::new_index);
		}

		inline bool is_indexer(meta_function mf) {
			return mf == meta_function::index || mf == meta_function::new_index;
		}

		inline bool is_indexer(call_construction) {
			return false;
		}

		inline bool is_indexer(base_classes_tag) {
			return false;
		}

		inline auto make_shim(string_detail::string_shim s) {
			return s;
		}

		inline auto make_shim(call_construction) {
			return string_detail::string_shim(name_of(meta_function::call_function));
		}

		inline auto make_shim(meta_function mf) {
			return string_detail::string_shim(name_of(mf));
		}

		inline auto make_shim(base_classes_tag) {
			return string_detail::string_shim(detail::base_class_cast_key());
		}

		template <typename N, typename F>
		inline luaL_Reg make_reg(N&& n, F&& f) {
			luaL_Reg l{ make_shim(std::forward<N>(n)).c_str(), std::forward<F>(f) };
			return l;
		}

		struct registrar {
			virtual int push_um(lua_State* L) = 0;
			virtual ~registrar() {}
		};

		template <bool is_index>
		inline int indexing_fail(lua_State* L) {
			string_detail::string_shim accessor = stack::get<string_detail::string_shim>(L, -1);
			if (is_index)
				return luaL_error(L, "sol: attempt to index (get) nil value \"%s\" on userdata (bad (misspelled?) key name or does not exist)", accessor.data());
			else
				return luaL_error(L, "sol: attempt to index (set) nil value \"%s\" on userdata (bad (misspelled?) key name or does not exist)", accessor.data());

		}
	}

	template <typename T, typename Tuple>
	struct usertype_metatable : usertype_detail::registrar {
		typedef std::make_index_sequence<std::tuple_size<Tuple>::value> indices;
		typedef std::make_index_sequence<std::tuple_size<Tuple>::value / 2> half_indices;
		typedef luaL_Reg regs_t[std::tuple_size<Tuple>::value / 2 + 1];
		template <std::size_t I>
		struct check_binding : is_variable_binding<meta::unqualified_t<std::tuple_element_t<I, Tuple>>> {};
		Tuple functions;
		lua_CFunction indexfunc;
		lua_CFunction newindexfunc;
		lua_CFunction destructfunc;
		lua_CFunction callconstructfunc;
		void* baseclasscheck;
		void* baseclasscast;
		bool mustindex;
		bool secondarymeta;

		template <std::size_t... I>
		static bool contains_variable(std::index_sequence<I...>) {
			typedef meta::any<check_binding<(I * 2 + 1)>...> has_variables;
			return has_variables::value;
		}

		template <std::size_t... I>
		bool contains_index(std::index_sequence<I...>) const {
			bool idx = false;
			detail::swallow{ 0, ((idx &= usertype_detail::is_indexer(std::get<I * 2>(functions))), 0) ... };
			return idx;
		}

		template <std::size_t I = 0>
		int make_regs(regs_t& l, int index ) {
			if (destructfunc != nullptr) {
				l[index] = { name_of(meta_function::garbage_collect).c_str(), destructfunc };
				++index;
			}
			if (baseclasscast != nullptr)
				return index;
#ifndef SOL_NO_EXCEPTIONS
			static_assert(sizeof(void*) <= sizeof(detail::throw_cast), "The size of this data pointer is too small to fit the inheritance checking function: file a bug report.");
			baseclasscheck = baseclasscast = (void*)&detail::throw_as<T>;
#elif !defined(SOL_NO_RTTI)
			static_assert(sizeof(void*) <= sizeof(detail::inheritance_check_function), "The size of this data pointer is too small to fit the inheritance checking function: file a bug report.");
			static_assert(sizeof(void*) <= sizeof(detail::inheritance_cast_function), "The size of this data pointer is too small to fit the inheritance checking function: file a bug report.");
			baseclasscheck = (void*)&detail::inheritance<T>::type_check;
			baseclasscast = (void*)&detail::inheritance<T>::type_cast;
#else
			static_assert(sizeof(void*) <= sizeof(detail::inheritance_check_function), "The size of this data pointer is too small to fit the inheritance checking function: file a bug report.");
			static_assert(sizeof(void*) <= sizeof(detail::inheritance_cast_function), "The size of this data pointer is too small to fit the inheritance checking function: file a bug report.");
			baseclasscheck = (void*)&detail::inheritance<T>::type_check;
			baseclasscast = (void*)&detail::inheritance<T>::type_cast;
#endif // No Runtime Type Information vs. Throw-Style Inheritance

			return index;
		}

		template <std::size_t I = 0, typename F, typename... Args>
		int make_regs(regs_t& l, int index, sol::call_construction&, F&, Args&&... args) {
			callconstructfunc = call<I + 1>;
			secondarymeta = true;
			int endindex = make_regs<I + 2>(l, index + 1, std::forward<Args>(args)...);
			return endindex;
		}

		template <std::size_t I = 0, typename... Bases, typename... Args>
		int make_regs(regs_t& l, int index, base_classes_tag&, bases<Bases...>&, Args&&... args) {
			int endindex = make_regs<I + 2>(l, index + 1, std::forward<Args>(args)...);
			if (sizeof...(Bases) < 1)
				return endindex;
#ifndef SOL_NO_EXCEPTIONS
			static_assert(sizeof(void*) <= sizeof(detail::throw_cast), "The size of this data pointer is too small to fit the inheritance checking function: file a bug report.");
			baseclasscheck = baseclasscast = (void*)&detail::throw_as<T>;
#elif !defined(SOL_NO_RTTI)
			static_assert(sizeof(void*) <= sizeof(detail::inheritance_check_function), "The size of this data pointer is too small to fit the inheritance checking function: file a bug report.");
			static_assert(sizeof(void*) <= sizeof(detail::inheritance_cast_function), "The size of this data pointer is too small to fit the inheritance checking function: file a bug report.");
			baseclasscheck = (void*)&detail::inheritance<T, Args...>::type_check;
			baseclasscast = (void*)&detail::inheritance<T, Args...>::type_cast;
#else
			static_assert(sizeof(void*) <= sizeof(detail::inheritance_check_function), "The size of this data pointer is too small to fit the inheritance checking function: file a bug report.");
			static_assert(sizeof(void*) <= sizeof(detail::inheritance_cast_function), "The size of this data pointer is too small to fit the inheritance checking function: file a bug report.");
			baseclasscheck = (void*)&detail::inheritance<T, Args...>::type_check;
			baseclasscast = (void*)&detail::inheritance<T, Args...>::type_cast;
#endif // No Runtime Type Information vs. Throw-Style Inheritance
		}

		template <std::size_t I = 0, typename N, typename F, typename... Args>
		int make_regs(regs_t& l, int index, N&& n, F&&, Args&&... args) {
			string_detail::string_shim shimname = usertype_detail::make_shim(n);
			// Returnable scope
			// That would be a neat keyword for C++
			// returnable { ... };
			[&]() {
				if (shimname == name_of(meta_function::garbage_collect)) {
					destructfunc = call<I + 1>;
					return;
				}
				else if (shimname == name_of(meta_function::index)) {
					indexfunc = call<I + 1>;
					mustindex = true;
					return;
				}
				l[index] = usertype_detail::make_reg(std::forward<N>(n), call<I + 1>);
				++index;
			}();
			return make_regs<I + 2>(l, index, std::forward<Args>(args)...);
		}

		usertype_metatable(Tuple t) : functions(std::move(t)),
		indexfunc(usertype_detail::indexing_fail<true>), newindexfunc(usertype_detail::indexing_fail<false>),
		destructfunc(nullptr), callconstructfunc(nullptr), baseclasscheck(nullptr), baseclasscast(nullptr), 
		mustindex(contains_variable(half_indices()) || contains_index(half_indices())), secondarymeta(false) {
		}

		template <bool is_index>
		int find_call(std::integral_constant<bool, is_index>, std::index_sequence<>, lua_State* L, const sol::string_detail::string_shim&) {
			if (is_index)
				return indexfunc(L);
			else
				return newindexfunc(L);
		}

		template <bool is_index, std::size_t I0, std::size_t I1, std::size_t... In>
		int find_call(std::integral_constant<bool, is_index> idx, std::index_sequence<I0, I1, In...>, lua_State* L, const sol::string_detail::string_shim& accessor) {
			string_detail::string_shim name = usertype_detail::make_shim(std::get<I0>(functions));
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
			usertype_metatable& f = stack::get<light<usertype_metatable>>(L, up_value_index(1));
			f.~usertype_metatable();
			return 0;
		}

		virtual int push_um(lua_State* L) override {
			return stack::push(L, std::move(*this));
		}
	};

	namespace stack {

		template <typename T, typename Tuple>
		struct pusher<usertype_metatable<T, Tuple>> {
			typedef usertype_metatable<T, Tuple> umt_t;
			typedef typename umt_t::regs_t regs_t;

			template <std::size_t... I>
			static int push(std::index_sequence<I...>, lua_State* L, usertype_metatable<T, Tuple>&& umx) {
				// Make sure userdata's memory is properly in lua first,
				// otherwise all the light userdata we make later will become invalid
				stack::push(L, make_user(std::move(umx)));
				usertype_metatable<T, Tuple>& um = stack::get<light<usertype_metatable<T, Tuple>>>(L, -1);
				reference umt(L, -1);
				umt.pop();

				// Now use um
				const bool& mustindex = um.mustindex;
				stack_reference t;
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
					t = stack_reference(L, -1);
					stack::push(L, make_light(um));
					regs_t l{};
					int lastreg = um.make_regs(l, 0, std::get<I>(um.functions)... );
					bool hasdestructor = lastreg > 0 && name_of(meta_function::garbage_collect) == l[lastreg - 1].name;
					if (i < 2 && hasdestructor) {
						l[lastreg - 1] = { nullptr, nullptr };
					}
					l[lastreg] = { nullptr, nullptr };
					luaL_setfuncs(L, l, 1);

					if (um.baseclasscheck != nullptr) {
						stack::set_field(L, detail::base_class_check_key(), um.baseclasscheck, t.stack_index());
					}
					if (um.baseclasscast != nullptr) {
						stack::set_field(L, detail::base_class_cast_key(), um.baseclasscast, t.stack_index());
					}

					if (mustindex) {
						// Basic index pushing: specialize
						// index and newindex to give variables and stuff
						stack::set_field(L, meta_function::index, make_closure(umt_t::index_call, make_light(um)), t.stack_index());
						stack::set_field(L, meta_function::new_index, make_closure(umt_t::new_index_call, make_light(um)), t.stack_index());
					}
					else {
						// If there's only functions, we can use the fast index version
						stack::set_field(L, meta_function::index, t, t.stack_index());
					}
					// metatable on the metatable
					// for call constructor purposes and such
					lua_createtable(L, 0, 1);
					stack_reference metabehind(L, -1);
					if (um.callconstructfunc != nullptr) {
						stack::set_field(L, sol::meta_function::call_function, make_closure(um.callconstructfunc, make_light(um)), metabehind.stack_index());
					}
					stack::set_field(L, metatable_key, metabehind, t.stack_index());
					metabehind.pop();
					// We want to just leave the table
					// in the registry only, otherwise we return it
					if (i < 2) {
						t.pop();
					}
					else {
						// Add cleanup to metatable
						// Essentially, when the metatable dies,
						// this too will call the class and kill itself
						const char* metakey = &usertype_traits<T>::gc_table[0];
						lua_createtable(L, 1, 0);
						stack_reference cleanup(L, -1);
						stack::set_field(L, meta_function::garbage_collect, make_closure(umt_t::gc_call, umt), cleanup.stack_index());
						stack::set_field(L, metatable_key, cleanup, cleanup.stack_index());
						// Needs to be raw since we
						// override the metatable's metatable on 't'
						// otherwise, it will trigger the __index metamethod
						// we just set
						stack::raw_set_field(L, metakey, t, t.stack_index());
						cleanup.pop();
					}
				}
				
				return 1;
			}
				
			static int push(lua_State* L, usertype_metatable<T, Tuple>&& um) {
				typedef typename umt_t::indices indices;
				return push(indices(), L, std::move(um));
			}
		};

	} // stack

} // sol

#endif // SOL_USERTYPE_METATABLE_HPP