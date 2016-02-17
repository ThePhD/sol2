// The MIT License (MIT)

// Copyright (c) 2013-2016 Rapptz and contributors

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

#ifndef SOL_USERTYPE_HPP
#define SOL_USERTYPE_HPP

#include "state.hpp"
#include "function_types.hpp"
#include "usertype_traits.hpp"
#include "default_construct.hpp"
#include "deprecate.hpp"
#include <vector>
#include <array>
#include <algorithm>
#include <map>

namespace sol {
	const std::array<std::string, 2> meta_variable_names = { {
	    "__index",
	    "__newindex",
	} };

	const std::array<std::string, 21> meta_function_names = { {
	    "new",
	    "__index",
	    "__newindex",
	    "__mode",
	    "__call",
	    "__metatable",
	    "__tostring",
	    "__len",
	    "__unm",
	    "__add",
	    "__sub",
	    "__mul",
	    "__div",
	    "__mod",
	    "__pow",
	    "__concat",
	    "__eq",
	    "__lt",
	    "__le",
	    "__gc",
	    "__call",
	} };

	enum class meta_function {
		construct,
		index,
		new_index,
		mode,
		call,
		metatable,
		to_string,
		length,
		unary_minus,
		addition,
		subtraction,
		multiplication,
		division,
		modulus,
		power_of,
		involution = power_of,
		concatenation,
		equal_to,
		less_than,
		less_than_or_equal_to,
		garbage_collect,
		call_function,
	};

	namespace usertype_detail {
		template<typename T, typename Funcs, typename FuncTable, typename MetaFuncTable, typename VarFuncTable>
		inline void push_metatable(lua_State* L, Funcs&& funcs, FuncTable&& functable, MetaFuncTable&& metafunctable, VarFuncTable&& varfunctable) {
			luaL_newmetatable(L, &usertype_traits<T>::metatable[0]);
			int metatableindex = lua_gettop(L);
			if (funcs.size() < 1 || metafunctable.size() < 2) {
				return;
			}
			// Metamethods directly on the metatable itself
			int metaup = stack::stack_detail::push_upvalues(L, funcs);
			if (std::is_pointer<T>::value) {
				// Insert nullptr before new/gc methods for pointer types;
				// prevents calling new/GC on pointer-based tables.
				luaL_Reg& oldref = metafunctable[metafunctable.size() - 3];
				luaL_Reg old = oldref;
				luaL_Reg cutoff = { nullptr, nullptr };
				oldref = cutoff;
				luaL_setfuncs(L, metafunctable.data(), metaup);
				oldref = old;
			}
			else {
				luaL_setfuncs(L, metafunctable.data(), metaup);
			}
			// Functions accessed by regular calls are put into a table
			// that get put into the metatable.__index metamethod
			lua_createtable(L, 0, functable.size());
			int functableindex = lua_gettop(L);
			int up = stack::stack_detail::push_upvalues(L, funcs);
			luaL_setfuncs(L, functable.data(), up);
			
			// Also, set the variable indexer that's inside of the metatable's index
			luaL_newmetatable(L, &usertype_traits<T>::variable_metatable[0]);
			int varup = stack::stack_detail::push_upvalues(L, funcs);
			luaL_setfuncs(L, varfunctable.data(), varup);
			lua_setmetatable(L, functableindex);
			
			lua_setfield(L, metatableindex, "__index");
		}

		template <typename T, typename Functions>
		inline void set_global_deleter(lua_State* L, lua_CFunction cleanup, Functions&& metafunctions) {
			// Automatic deleter table -- stays alive until lua VM dies
			// even if the user calls collectgarbage(), weirdly enough
			lua_createtable(L, 0, 0);
			lua_createtable(L, 0, 1);
			int up = stack::stack_detail::push_upvalues<true>(L, metafunctions);
			lua_pushcclosure(L, cleanup, up);
			lua_setfield(L, -2, "__gc");
			lua_setmetatable(L, -2);
			// gctable name by default has ♻ part of it
			lua_setglobal(L, &usertype_traits<T>::gc_table[0]);
		}
	}

	template<typename T>
	class usertype {
	private:
		typedef std::map<std::string, base_function*> function_map_t;
		std::vector<std::string> functionnames;
		std::vector<std::unique_ptr<base_function>> functions;
		std::vector<luaL_Reg> functiontable;
		std::vector<luaL_Reg> metafunctiontable;
		std::array<luaL_Reg, 3> variablefunctiontable;
		base_function* indexfunc;
		base_function* newindexfunc;
		function_map_t indexwrapper, newindexwrapper;
		lua_CFunction constructfunc;
		lua_CFunction destructfunc;

		template<typename... TTypes>
		struct constructor {
			template<typename... Args>
			static void do_constructor(lua_State* L, T* obj, call_syntax syntax, int, types<Args...>) {
				default_construct fx{};
				stack::call(types<void>(), types<Args...>(), L, -1 + static_cast<int>(syntax), fx, obj);
			}

			static void match_constructor(lua_State*, T*, call_syntax, int) {
				throw error("No matching constructor for the arguments provided");
			}

			template<typename ...CArgs, typename... Args>
			static void match_constructor(lua_State* L, T* obj, call_syntax syntax, int argcount, types<CArgs...> t, Args&&... args) {
				if (argcount == sizeof...(CArgs)) {
					do_constructor(L, obj, syntax, argcount, t);
					return;
				}
				match_constructor(L, obj, syntax, argcount, std::forward<Args>(args)...);
			}

			static int construct(lua_State* L) {
				const auto& meta = usertype_traits<T>::metatable;
				call_syntax syntax = stack::get_call_syntax(L, meta);
				int argcount = lua_gettop(L);

				T** pointerpointer = reinterpret_cast<T**>(lua_newuserdata(L, sizeof(T*) + sizeof(T)));
				T*& referencepointer = *pointerpointer;
				T* obj = reinterpret_cast<T*>(pointerpointer + 1);
				referencepointer = obj;
				match_constructor(L, obj, syntax, argcount - static_cast<int>(syntax), identity_t<TTypes>()...);

				if (luaL_newmetatable(L, &meta[0]) == 1) {
					lua_pop(L, 1);
					std::string err = "Unable to get usertype metatable for ";
					err += meta;
					throw error(err);
				}
				lua_setmetatable(L, -2);

				return 1;
			}
		};

		static int destruct(lua_State* L) {
			userdata udata = stack::get<userdata>(L, 1);
			// The first sizeof(T*) bytes are the reference: the rest is
			// the actual data itself (if there is a reference at all)
			T** pobj = reinterpret_cast<T**>(udata.value);
			T*& obj = *pobj;
			std::allocator<T> alloc{};
			alloc.destroy(obj);
			return 0;
		}

		template<typename... Functions>
		std::unique_ptr<base_function> make_function(const std::string&, overload_set<Functions...> func) {
			return std::make_unique<usertype_overloaded_function<T, Functions...>>(func);
		}

		template<typename Arg, typename... Args, typename Ret>
		std::unique_ptr<base_function> make_function(const std::string&, Ret(*func)(Arg, Args...)) {
			typedef Unqualified<std::remove_pointer_t<Arg>> Argu;
			static_assert(std::is_base_of<Argu, T>::value, "Any non-member-function must have a first argument which is covariant with the desired userdata type.");
			typedef std::decay_t<decltype(func)> function_type;
			return std::make_unique<usertype_function<function_type, T>>(func);
		}

		template<typename Base, typename Ret>
		std::unique_ptr<base_function> make_variable_function(std::true_type, const std::string&, Ret Base::* func) {
			static_assert(std::is_base_of<Base, T>::value, "Any registered function must be part of the class");
			typedef std::decay_t<decltype(func)> function_type;
			return std::make_unique<usertype_variable_function<function_type, T>>(func);
		}

		template<typename Base, typename Ret>
		std::unique_ptr<base_function> make_variable_function(std::false_type, const std::string&, Ret Base::* func) {
			static_assert(std::is_base_of<Base, T>::value, "Any registered function must be part of the class");
			typedef std::decay_t<decltype(func)> function_type;
			return std::make_unique<usertype_function<function_type, T>>(func);
		}

		template<typename Base, typename Ret>
		std::unique_ptr<base_function> make_function(const std::string& name, Ret Base::* func) {
			typedef std::decay_t<decltype(func)> function_type;
			return make_variable_function(std::is_member_object_pointer<function_type>(), name, func);
		}

		template<typename Fx>
		std::unique_ptr<base_function> make_function(const std::string&, Fx&& func) {
			typedef Unqualified<Fx> Fxu;
			typedef std::tuple_element_t<0, typename function_traits<Fxu>::args_tuple_type> Arg0;
			typedef Unqualified<std::remove_pointer_t<Arg0>> Argu;
			static_assert(std::is_base_of<Argu, T>::value, "Any non-member-function must have a first argument which is covariant with the desired usertype.");
			typedef std::decay_t<Fxu> function_type;
			return std::make_unique<usertype_function<function_type, T>>(func);
		}

		template<std::size_t N, typename Fx>
		void build_function(std::string funcname, Fx&& func) {
			typedef std::is_member_object_pointer<Unqualified<Fx>> is_variable;
			typedef std::decay_t<Fx> function_type;
			functionnames.push_back(std::move(funcname));
			std::string& name = functionnames.back();
			auto baseptr = make_function(name, std::forward<Fx>(func));
			functions.emplace_back(std::move(baseptr));
			if (is_variable::value) {
				indexwrapper.insert({ name, functions.back().get() });
				newindexwrapper.insert({ name, functions.back().get() });
				return;
			}
			auto metamethodfind = std::find(meta_function_names.begin(), meta_function_names.end(), name);
			if (metamethodfind != meta_function_names.end()) {
				meta_function metafunction = static_cast<meta_function>(metamethodfind - meta_function_names.begin());
				switch (metafunction) {
				case meta_function::index:
					indexfunc = functions.back().get();
					break;
				case meta_function::new_index:
					newindexfunc = functions.back().get();
					break;
				default:
					break;
				}
				metafunctiontable.push_back({ name.c_str(), &base_function::usertype<N>::call });
				return;
			}
			functiontable.push_back({ name.c_str(), &base_function::usertype<N>::call });
		}

		template<std::size_t N, typename Fx, typename... Args>
		void build_function_tables(std::string funcname, Fx&& func, Args&&... args) {
			build_function<N>(std::move(funcname), std::forward<Fx>(func));
			build_function_tables<N + 1>(std::forward<Args>(args)...);
		}

		template<std::size_t N, typename Fx, typename... Args>
		void build_function_tables(meta_function metafunc, Fx&& func, Args&&... args) {
			std::size_t idx = static_cast<std::size_t>(metafunc);
			const std::string& funcname = meta_function_names[idx];
			build_function_tables<N>(funcname, std::forward<Fx>(func), std::forward<Args>(args)...);
		}

		template<std::size_t N>
		void build_function_tables() {
			int variableend = 0;
			if (!indexwrapper.empty()) {
				functions.push_back(std::make_unique<usertype_indexing_function>("__index", indexfunc, std::move(indexwrapper)));
				variablefunctiontable[0] = { "__index", &base_function::usertype<N>::call };
				++variableend;
			}
			if (!newindexwrapper.empty()) {
				functions.push_back(std::make_unique<usertype_indexing_function>("__newindex", newindexfunc, std::move(newindexwrapper)));
				variablefunctiontable[variableend] = { "__newindex", indexwrapper.empty() ? &base_function::usertype<N>::call : &base_function::usertype<N + 1>::call };
				++variableend;
			}
			variablefunctiontable[variableend] = { nullptr, nullptr };
			switch (variableend) {
			case 2:
				destructfunc = &base_function::usertype<N + 2>::gc;
				break;
			case 1:
				destructfunc = &base_function::usertype<N + 1>::gc;
				break;
			case 0:
				destructfunc = &base_function::usertype<N + 0>::gc;
				break;
			}
		}

	public:
		template<typename... Args>
		usertype(Args&&... args) : usertype(default_constructor, std::forward<Args>(args)...) {}

		template<typename... Args, typename... CArgs>
		usertype(constructors<CArgs...>, Args&&... args) 
		: indexfunc(nullptr), newindexfunc(nullptr), constructfunc(nullptr), destructfunc(nullptr) {
			functionnames.reserve(sizeof...(args)+2);
			functiontable.reserve(sizeof...(args));
			metafunctiontable.reserve(sizeof...(args));

			build_function_tables<0>(std::forward<Args>(args)...);
		
			functionnames.push_back("new");
			metafunctiontable.push_back({ functionnames.back().c_str(), &constructor<CArgs...>::construct });
			functionnames.push_back("__gc");
			metafunctiontable.push_back({ functionnames.back().c_str(), destruct });

			// ptr_functions does not participate in garbage collection/new,
			// as all pointered types are considered
			// to be references. This makes returns of
			// `std::vector<int>&` and `std::vector<int>*` work
			functiontable.push_back({ nullptr, nullptr });
			metafunctiontable.push_back({ nullptr, nullptr });
		}

		int push(lua_State* L) {
			// push pointer tables first,
			usertype_detail::push_metatable<T*>(L, functions, functiontable, metafunctiontable, variablefunctiontable);
			lua_pop(L, 1);
			// but leave the regular T table on last
			// so it can be linked to a type for usage with `.new(...)` or `:new(...)`
			usertype_detail::push_metatable<T>(L, functions, functiontable, metafunctiontable, variablefunctiontable);
			// Make sure to drop a table in the global namespace to properly destroy the pushed functions
			// at some later point in life
			usertype_detail::set_global_deleter<T>(L, destructfunc, functions);
			return 1;
		}
	};

	namespace stack {
		template<typename T>
		struct pusher<usertype<T>> {
			static int push(lua_State* L, usertype<T>& user) {
				return user.push(L);
			}
		};
	} // stack
} // sol

#endif // SOL_USERTYPE_HPP
