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

#ifndef SOL_USERTYPE_STORAGE_HPP
#define SOL_USERTYPE_STORAGE_HPP

#include "usertype_core.hpp"
#include "make_reference.hpp"
#include "map.hpp"

#include <bitset>

namespace sol {

namespace u_detail {

	using index_call_function = int(lua_State*, void*);

	struct index_call_storage {
		index_call_function* index;
		index_call_function* new_index;
		void* binding_data;
	};

	struct binding_base {
		virtual ~binding_base() {
		}
	};

	template <typename K, typename F, typename T = void>
	struct binding : binding_base {
		std::decay_t<F> data_;

		template <typename... Args>
		binding(Args&&... args)
		: data_(std::forward<Args>(args)...) {
		}

		void* data() {
			return static_cast<void*>(std::addressof(data_));
		}

		template <bool is_index = true, bool is_variable = false>
		static int call_with_(lua_State* L, void* target) {
			constexpr int boost = !detail::is_non_factory_constructor<F>::value
					&& std::is_same<K, call_construction>::value
				? 1
				: 0;
			auto& f = *static_cast<F*>(target);
			return call_detail::call_wrapped<T, is_index, is_variable, boost>(L, f);
		}

		template <bool is_index = true, bool is_variable = false>
		static int call_(lua_State* L) {
			void* f = stack::get<void*>(L, upvalue_index(usertype_storage_index));
			return call_with_<is_index, is_variable>(L, f);
		}

		template <bool is_index = true, bool is_variable = false>
		static int call(lua_State* L) {
			return detail::typed_static_trampoline<decltype(&call_<is_index, is_variable>), (&call_<is_index, is_variable>)>(L);
		}

		template <bool is_index = true, bool is_variable = false>
		static int index_call_with_(lua_State* L, void* target) {
			if constexpr (!is_variable) {
				if constexpr (std::is_same_v<std::decay_t<F>, lua_CFunction> || std::is_same_v<std::decay_t<F>, lua_CFunction_ref>) {
					auto& f = *static_cast<std::decay_t<F>*>(target);
					return stack::push(L, f);
				}
				else {
					// set up upvalues
					// for a chained call
					int upvalues = 0;
					upvalues += stack::push(L, nullptr);
					upvalues += stack::push(L, target);
					auto cfunc = &call<is_index, is_variable>;
					return stack::push(L, c_closure(cfunc, upvalues));
				}
			}
			else {
				constexpr int boost = !detail::is_non_factory_constructor<F>::value
						&& std::is_same<K, call_construction>::value
					? 1
					: 0;
				auto& f = *static_cast<F*>(target);
				return call_detail::call_wrapped<T, is_index, is_variable, boost>(L, f);
			}
		}

		template <bool is_index = true, bool is_variable = false>
		static int index_call_(lua_State* L) {
			void* f = stack::get<void*>(L, upvalue_index(usertype_storage_index));
			return index_call_with_<is_index, is_variable>(L, f);
		}

		template <bool is_index = true, bool is_variable = false>
		static int index_call(lua_State* L) {
			return detail::typed_static_trampoline<decltype(&index_call_<is_index, is_variable>), (&index_call_<is_index, is_variable>)>(L);
		}
	};

	inline int index_fail(lua_State* L) {
		if (lua_getmetatable(L, 1) == 1) {
			int metatarget = lua_gettop(L);
			stack::get_field(L, stack_reference(L, raw_index(2)), metatarget);
			return 1;
		}
		// With runtime extensibility, we can't hard-error things. They have to return nil, like regular table types
		return stack::push(L, lua_nil);
	}

	inline int new_index_fail(lua_State* L) {
		return luaL_error(L, "sol: cannot set (new_index) into this object: no defined new_index operation on usertype");
	}

	struct usertype_storage_base {
	public:
		std::vector<std::unique_ptr<binding_base>> storage;
		detail::unordered_map<std::string, index_call_storage> string_keys;
		detail::unordered_map<reference, reference, reference_hash, reference_equals> auxiliary_keys;
		reference value_index_table;
		reference reference_index_table;
		reference unique_index_table;
		reference const_reference_index_table;
		reference const_value_index_table;
		reference named_index_table;
		reference type_table;
		reference gc_names_table;
		reference metametatable;
		std::bitset<64> properties;
		lua_CFunction base_index;
		lua_CFunction base_new_index;
		bool is_using_index;
		bool is_using_new_index;

		usertype_storage_base(lua_State* L)
		: storage(), string_keys(), auxiliary_keys(), value_index_table(), reference_index_table(), unique_index_table(), const_reference_index_table(), type_table(make_reference(L, create)), gc_names_table(make_reference(L, create)), metametatable(make_reference(L, create)), properties(), base_index(index_fail), base_new_index(new_index_fail), is_using_index(false), is_using_new_index(false) {
		}

		void clear() {
			storage.clear();
			string_keys.clear();
			auxiliary_keys.clear();
			// TODO: also nuke individual lua tables,
			// one by one,
			// then replace getter/setter
		}

		template <typename Base>
		static void base_walk_index(lua_State* L, usertype_storage_base& self, bool& keep_going, int& base_result) {
			using bases = typename base<Base>::type;
			if (!keep_going) {
				return;
			}
			(void)L;
			(void)self;
			// TODO: get base table, dump it out
			usertype_storage_base& base_storage = get_usertype_storage<Base>(L);
			base_result = self_index_call<true>(bases(), L, base_storage);
			keep_going = base_result == base_walking_failed_index;
		}

		template <typename Base>
		static void base_walk_new_index(lua_State* L, usertype_storage_base& self, bool& keep_going, int& base_result) {
			using bases = typename base<Base>::type;
			if (!keep_going) {
				return;
			}
			(void)L;
			(void)self;
			// TODO: get base table, dump it out
			usertype_storage_base& base_storage = get_usertype_storage<Base>(L);
			base_result = self_new_index_call<true>(bases(), L, base_storage);
			keep_going = base_result == base_walking_failed_index;
		}

		template <bool base_walking = false, bool from_named_metatable = false, typename... Bases>
		static int self_index_call(types<Bases...>, lua_State* L, usertype_storage_base& self) {
			type k_type = stack::get<type>(L, 2);
			if (k_type == type::string) {
				index_call_storage* target = nullptr;
				{
					std::string k = stack::get<std::string>(L, 2);
					auto it = self.string_keys.find(k);
					if (it != self.string_keys.cend()) {
						target = &it->second;
					}
				}
				if (target != nullptr) {
					// let the target decide what to do
					return (target->index)(L, target->binding_data);
				}
			}
			else if (k_type != type::nil && k_type != type::none) {
				reference* target = nullptr;
				{
					stack_reference k = stack::get<stack_reference>(L, 2);
					auto it = self.auxiliary_keys.find(k);
					if (it != self.auxiliary_keys.cend()) {
						target = &it->second;
					}
				}
				if (target != nullptr) {
					// push target to return
					// what we found
					return stack::push(L, *target);
				}
			}

			// retrieve bases and walk through them.
			bool keep_going = true;
			int base_result;
			detail::swallow { 1, (base_walk_index<Bases>(L, self, keep_going, base_result), 1)... };
			if (sizeof...(Bases) > 0 && !keep_going) {
				return base_result;
			}
			if (base_walking) {
				// if we're JUST base-walking then don't index-fail, just
				// return the false bits
				return base_walking_failed_index;
			}
			if constexpr (from_named_metatable) {
				return index_fail(L);
			}
			else {
				return self.base_index(L);
			}
		}

		template <bool base_walking = false, bool from_named_metatable = false, typename... Bases>
		static int self_new_index_call(types<Bases...>, lua_State* L, usertype_storage_base& self) {
			type k_type = stack::get<type>(L, 2);
			if (k_type == type::string) {
				index_call_storage* target = nullptr;
				{
					std::string k = stack::get<std::string>(L, 2);
					auto it = self.string_keys.find(k);
					if (it != self.string_keys.cend()) {
						target = &it->second;
					}
				}
				if (target != nullptr) {
					// set value through
					// new_index call, whatever that entails,
					// and return
					return (target->new_index)(L, target->binding_data);
				}
			}
			else if (k_type != type::nil && k_type != type::none) {
				reference* target = nullptr;
				{
					stack_reference k = stack::get<stack_reference>(L, 2);
					auto it = self.auxiliary_keys.find(k);
					if (it != self.auxiliary_keys.cend()) {
						target = &it->second;
					}
				}
				if (target != nullptr) {
					// set value and return
					*target = reference(L, 3);
					return 0;
				}
			}

			// retrieve bases and walk through them.
			bool keep_going = true;
			int base_result;
			detail::swallow { 1, (base_walk_new_index<Bases>(L, self, keep_going, base_result), 1)... };
			if (sizeof...(Bases) > 0 && !keep_going) {
				return base_result;
			}
			if (base_walking) {
				// if we're JUST base-walking then don't index-fail, just
				// return the false bits
				return base_walking_failed_index;
			}
			if constexpr (from_named_metatable) {
				self.set(L, reference(L, raw_index(2)), reference(L, raw_index(3)));
				return 0;
			}
			else {
				return self.base_new_index(L);
			}
		}

		template <typename T>
		void change_indexing(lua_State* L, submetatable submetatable_type, stack_reference& t) {
			using uts = usertype_storage<T>;
			usertype_storage_base& this_base = *this;
			this->is_using_index |= true;
			this->is_using_new_index |= true;
			if constexpr (std::is_void_v<T>) {
				static_assert(!std::is_void_v<T>, "You cannot add variables or index methods without going through the strongly-typed usertype<T> metatable!");
			}
			else {
				if (submetatable_type == submetatable::named) {
					stack::set_field(L, metatable_key, metametatable, t.stack_index());
					stack_reference stack_metametatable(L, -metametatable.push());
					stack::set_field<false, true>(L, meta_function::index, make_closure(uts::meta_index_call, nullptr, make_light(*this), make_light(this_base), nullptr, toplevel_magic), stack_metametatable.stack_index());
					stack::set_field<false, true>(L, meta_function::new_index, make_closure(uts::meta_new_index_call, nullptr, make_light(*this), make_light(this_base), nullptr, toplevel_magic), stack_metametatable.stack_index());
					stack_metametatable.pop();
				}
				else {
					stack::set_field<false, true>(L, meta_function::index, make_closure(uts::index_call, nullptr, make_light(*this), make_light(this_base), nullptr, toplevel_magic), t.stack_index());
					stack::set_field<false, true>(L, meta_function::new_index, make_closure(uts::new_index_call, nullptr, make_light(*this), make_light(this_base), nullptr, toplevel_magic), t.stack_index());
				}
			}
		}

		template <typename Fx>
		void for_each_table(lua_State* L, Fx&& fx) {
			for (int i = 0; i < 6; ++i) {
				submetatable submetatable_type = static_cast<submetatable>(i);
				reference* p_fast_index_table = nullptr;
				switch (submetatable_type) {
				case submetatable::const_value:
					p_fast_index_table = &this->const_value_index_table;
					break;
				case submetatable::reference:
					p_fast_index_table = &this->reference_index_table;
					break;
				case submetatable::unique:
					p_fast_index_table = &this->unique_index_table;
					break;
				case submetatable::const_reference:
					p_fast_index_table = &this->const_reference_index_table;
					break;
				case submetatable::named:
					p_fast_index_table = &this->named_index_table;
					break;
				case submetatable::value:
				default:
					p_fast_index_table = &this->value_index_table;
					break;
				}
				fx(L, submetatable_type, *p_fast_index_table);
			}
		}

		template <typename T = void, typename Key, typename Value>
		void set(lua_State* L, Key&& key, Value&& value);
	};

	template <typename T>
	struct usertype_storage : usertype_storage_base {

		using usertype_storage_base::usertype_storage_base;

		template <bool from_named_metatable>
		static int index_call_(lua_State* L) {
			using bases = typename base<T>::type;
			usertype_storage_base& self = stack::get<light<usertype_storage_base>>(L, upvalue_index(usertype_storage_index));
			return self_index_call<false, from_named_metatable>(bases(), L, self);
		}

		template <bool from_named_metatable>
		static int new_index_call_(lua_State* L) {
			using bases = typename base<T>::type;
			usertype_storage_base& self = stack::get<light<usertype_storage_base>>(L, upvalue_index(usertype_storage_index));
			return self_new_index_call<false, from_named_metatable>(bases(), L, self);
		}

		static int index_call(lua_State* L) {
			return detail::static_trampoline<&index_call_<false>>(L);
		}

		static int new_index_call(lua_State* L) {
			return detail::static_trampoline<&new_index_call_<false>>(L);
		}

		static int meta_index_call(lua_State* L) {
			return detail::static_trampoline<&index_call_<true>>(L);
		}

		static int meta_new_index_call(lua_State* L) {
			return detail::static_trampoline<&new_index_call_<true>>(L);
		}

		template <typename Key, typename Value>
		void set(lua_State* L, Key&& key, Value&& value) {
			static_cast<usertype_storage_base&>(*this).set<T>(L, std::forward<Key>(key), std::forward<Value>(value));
		}
	};

	template <typename T, typename Key, typename Value>
	void usertype_storage_base::set(lua_State* L, Key&& key, Value&& value) {
		using ValueU = meta::unwrap_unqualified_t<Value>;
		using KeyU = meta::unwrap_unqualified_t<Key>;
		using Binding = binding<KeyU, ValueU, T>;
		using is_var_bind = is_variable_binding<ValueU>;
		if constexpr (std::is_same_v<Key, call_construction>) {
			std::unique_ptr<Binding> p_binding = std::make_unique<Binding>(std::forward<Value>(value));
			Binding& b = *p_binding;
			this->storage.push_back(std::move(p_binding));

			this->named_index_table.push();
			absolute_index metametatable_index(L, -1);
			stack::push(L, nullptr);
			stack::push(L, b.data());
			lua_CFunction target_func = &b.call<false, false>;
			lua_pushcclosure(L, target_func, 2);
			lua_setfield(L, metametatable_index, to_string(meta_function::call).c_str());
			this->named_index_table.pop();
		}
		else if constexpr ((meta::is_string_constructible<KeyU>::value || std::is_same_v<KeyU, meta_function>)&&(!is_lua_reference_or_proxy<ValueU>::value && !is_lua_reference_or_proxy<KeyU>::value)) {
			std::unique_ptr<Binding> p_binding = std::make_unique<Binding>(std::forward<Value>(value));
			Binding& b = *p_binding;
			this->storage.push_back(std::move(p_binding));

			std::string s = u_detail::make_string(std::forward<Key>(key));
			bool is_index = is_var_bind::value || (s == to_string(meta_function::index));
			bool is_new_index = is_var_bind::value || (s == to_string(meta_function::new_index));
			bool no_use_named = s == to_string(meta_function::call);
			index_call_storage ics;
			ics.binding_data = b.data();
			ics.index = &b.index_call_with_<true, is_var_bind::value>;
			ics.new_index = &b.index_call_with_<false, is_var_bind::value>;
			// need to swap everything to use fast indexing here
			auto fet = [&](lua_State* L, submetatable submetatable_type, reference& fast_index_table) {
				if (submetatable_type == submetatable::named && no_use_named) {
					// do not override __call or
					// other specific meta functions on named metatable:
					// we need that for call construction
					// and other amenities
					return;
				}
				int fast_index_table_push = fast_index_table.push();
				stack_reference t(L, -fast_index_table_push);
				if constexpr (std::is_same_v<ValueU, lua_CFunction> || std::is_same_v<ValueU, lua_CFunction_ref>) {
					stack::set_field<false, true>(L, s, b.data_, t.stack_index());
				}
				else {
					stack::set_field<false, true>(L, s, make_closure(&b.call<false, is_var_bind::value>, nullptr, ics.binding_data), t.stack_index());
				}
				if (is_index || is_new_index) {
					change_indexing<T>(L, submetatable_type, t);
				}
				t.pop();
			};
			this->for_each_table(L, fet);
			this->string_keys.insert_or_assign(std::move(s), std::move(ics));
		}
		else {
			// the reference-based implementation might compare poorly and hash
			// poorly in some cases...
			if constexpr (is_lua_reference<KeyU>::value && is_lua_reference<ValueU>::value) {
				auto ref_additions_fx = [&](lua_State* L, submetatable, reference& fast_index_table) {
					int fast_index_table_push = fast_index_table.push();
					stack_reference t(L, -fast_index_table_push);
					stack::set_field<false, true>(L, key, value, t.stack_index());
					t.pop();
				};
				this->for_each_table(L, ref_additions_fx);
				key.push();
				optional<std::string> maybe_string_key = stack::get<optional<std::string>>(L, -1);
				if (maybe_string_key) {
					std::string s = *maybe_string_key;
					std::unique_ptr<Binding> p_binding = std::make_unique<Binding>(std::forward<Value>(value));
					Binding& b = *p_binding;
					this->storage.push_back(std::move(p_binding));

					index_call_storage ics;
					ics.binding_data = b.data();
					ics.index = &b.index_call_with_<true, true>;
					ics.new_index = &b.index_call_with_<false, true>;

					this->string_keys.insert_or_assign(std::move(s), ics);
				}
				else {
					// its auxiliary and must be
					// indexed with weirdness
					this->auxiliary_keys.insert_or_assign(std::forward<Key>(key), std::forward<Value>(value));
				}
				lua_pop(L, 1);
			}
			else {
				reference ref_key = make_reference(L, std::forward<Key>(key));
				reference ref_value = make_reference(L, std::forward<Value>(value));
				auto ref_additions_fx = [&](lua_State* L, submetatable, reference& fast_index_table) {
					int fast_index_table_push = fast_index_table.push();
					stack_reference t(L, -fast_index_table_push);
					stack::set_field<false, true>(L, ref_key, ref_value, t.stack_index());
					t.pop();
				};
				this->for_each_table(L, ref_additions_fx);
				this->auxiliary_keys.insert_or_assign(std::move(ref_key), std::move(ref_value));
			}
		}
	}

	template <typename T>
	inline usertype_storage<T>& create_usertype_storage(lua_State* L) {
		const char* gcmetakey = &usertype_traits<T>::gc_table()[0];

		// Make sure userdata's memory is properly in lua first,
		// otherwise all the light userdata we make later will become invalid
		int usertype_storage_push_count = stack::push<user<usertype_storage<T>>>(L, no_metatable, L);
		stack_reference usertype_storage_ref(L, -usertype_storage_push_count);

		// create and push onto the stack a table to use as metatable for this GC
		// we create a metatable to attach to the regular gc_table
		// so that the destructor is called for the usertype storage
		int usertype_storage_metatabe_count = stack::push(L, new_table(0, 1));
		stack_table usertype_storage_metatable(L, -usertype_storage_metatabe_count);
		// set the destruction routine on the metatable
		stack::set_field(L, meta_function::garbage_collect, detail::user_alloc_destruct<T>, usertype_storage_metatable.stack_index());
		// set the metatable on the usertype storage userdata
		stack::set_field(L, metatable_key, usertype_storage_metatable, usertype_storage_ref.stack_index());
		usertype_storage_metatable.pop();

		// set the usertype storage and its metatable
		// into the global table...
		stack::set_field<true>(L, gcmetakey, usertype_storage_ref);
		usertype_storage_ref.pop();

		// then retrieve the lua-stored version so we have a well-pinned
		// reference that does not die
		stack::get_field<true>(L, gcmetakey);
		usertype_storage<T>& target_umt = stack::pop<user<usertype_storage<T>>>(L);
		return target_umt;
	}

	inline optional<usertype_storage_base&> maybe_get_usertype_storage_base(lua_State* L, const char* gcmetakey) {
		stack::get_field<true>(L, gcmetakey);
		stack::record tracking;
		usertype_storage_base& target_umt = stack::stack_detail::unchecked_unqualified_get<user<usertype_storage_base>>(L, -1, tracking);
		return target_umt;
	}

	inline usertype_storage_base& get_usertype_storage_base(lua_State* L, const char* gcmetakey) {
		stack::get_field<true>(L, gcmetakey);
		stack::record tracking;
		usertype_storage_base& target_umt = stack::stack_detail::unchecked_unqualified_get<user<usertype_storage_base>>(L, -1, tracking);
		return target_umt;
	}

	template <typename T>
	inline optional<usertype_storage<T>&> maybe_get_usertype_storage(lua_State* L) {
		const char* gcmetakey = &usertype_traits<T>::gc_table()[0];
		stack::get_field<true>(L, gcmetakey);
		if (!stack::check<user<usertype_storage<T>>>(L)) {
			return nullopt;
		}
		usertype_storage<T>& target_umt = stack::pop<user<usertype_storage<T>>>(L);
		return target_umt;
	}

	template <typename T>
	inline usertype_storage<T>& get_usertype_storage(lua_State* L) {
		const char* gcmetakey = &usertype_traits<T>::gc_table()[0];
		stack::get_field<true>(L, gcmetakey);
		usertype_storage<T>& target_umt = stack::pop<user<usertype_storage<T>>>(L);
		return target_umt;
	}

	template <typename T>
	inline void delete_usertype_storage(lua_State* L) {
		using u_traits = usertype_traits<T>;
		using u_const_traits = usertype_traits<const T>;
		using u_unique_traits = usertype_traits<detail::unique_usertype<T>>;
		using u_ref_traits = usertype_traits<T*>;
		using u_const_ref_traits = usertype_traits<T const*>;
		using uts = usertype_storage<T>;

		const char* gcmetakey = &u_traits::gc_table()[0];
		stack::get_field<true>(L, gcmetakey);
		if (!stack::check<user<uts>>(L)) {
			lua_pop(L, 1);
			return;
		}
		lua_pop(L, 1);

		// get the registry
		stack_reference registry(L, raw_index(LUA_REGISTRYINDEX));
		registry.push();
		// eliminate all named entries for this usertype
		// in the registry (luaL_newmetatable does
		// [name] = new table
		// in registry upon creation
		stack::set_field(L, &u_traits::metatable()[0], lua_nil, registry.stack_index());
		stack::set_field(L, &u_const_traits::metatable()[0], lua_nil, registry.stack_index());
		stack::set_field(L, &u_const_ref_traits::metatable()[0], lua_nil, registry.stack_index());
		stack::set_field(L, &u_ref_traits::metatable()[0], lua_nil, registry.stack_index());
		stack::set_field(L, &u_unique_traits::metatable()[0], lua_nil, registry.stack_index());
		registry.pop();

		stack::set_field<true>(L, gcmetakey, lua_nil);
	}

	template <typename T>
	inline int register_usertype(lua_State* L, optional<no_construction> no_default_constructor = nullopt) {
		using u_traits = usertype_traits<T>;
		using u_const_traits = usertype_traits<const T>;
		using u_unique_traits = usertype_traits<detail::unique_usertype<T>>;
		using u_ref_traits = usertype_traits<T*>;
		using u_const_ref_traits = usertype_traits<T const*>;
		using uts = usertype_storage<T>;

		// always have __new_index point to usertype_storage method
		// have __index always point to regular fast-lookup
		// meta_method table
		// if __new_index is invoked, runtime-swap
		// to slow __index if necessary
		// (no speed penalty because function calls
		// are all read-only -- only depend on __index
		// to retrieve function and then call happens VIA Lua)

		// __type entry:
		// table contains key -> value lookup,
		// where key is entry in metatable
		// and value is type information as a string as
		// best as we can give it

		// name entry:
		// string that contains raw class name,
		// as defined from C++

		// is entry:
		// checks if argument supplied is of type T

		// __storage entry:
		// a light userdata pointing to the storage
		// mostly to enable this new abstraction
		// to not require the type name `T`
		// to get at the C++ usertype storage within


		// we then let typical definitions potentially override these intrinsics
		// it's the user's fault if they override things or screw them up:
		// these names have been reserved and documented since sol2

		// STEP 0: tell the old usertype (if it exists)
		// to fuck off
		delete_usertype_storage<T>(L);

		// STEP 1: Create backing store for usertype storage
		// Pretty much the most important step.
		// STEP 2: Create Lua tables used for fast method indexing.
		// This is done inside of the storage table's constructor
		usertype_storage<T>& storage = create_usertype_storage<T>(L);
		usertype_storage_base& base_storage = storage;

		// STEP 3: set up GC escape hatch table entirely
		storage.gc_names_table.push();
		stack_reference gnt(L, -1);
		stack::set_field(L, submetatable::named, &u_traits::gc_table()[0], gnt.stack_index());
		stack::set_field(L, submetatable::const_value, &u_const_traits::metatable()[0], gnt.stack_index());
		stack::set_field(L, submetatable::const_reference, &u_const_ref_traits::metatable()[0], gnt.stack_index());
		stack::set_field(L, submetatable::reference, &u_ref_traits::metatable()[0], gnt.stack_index());
		stack::set_field(L, submetatable::unique, &u_unique_traits::metatable()[0], gnt.stack_index());
		stack::set_field(L, submetatable::value, &u_traits::metatable()[0], gnt.stack_index());
		gnt.pop();

		// STEP 4: add some useful information to the type table
		stack_reference stacked_type_table(L, -storage.type_table.push());
		stack::set_field(L, "name", detail::demangle<T>(), stacked_type_table.stack_index());
		stack::set_field(L, "is", &u_detail::is_check<T>, stacked_type_table.stack_index());
		stacked_type_table.pop();

		// STEP 5: create and hook up metatable,
		// add intrinsics
		// this one is the actual meta-handling table,
		// the next one will be the one for
		for (std::size_t subusertype_storage_index = 0; subusertype_storage_index < 6; ++subusertype_storage_index) {
			submetatable submetatable_type = static_cast<submetatable>(subusertype_storage_index);
			// Pointer types, AKA "references" from C++
			const char* metakey = nullptr;
			reference* p_fast_index_table = nullptr;
			switch (submetatable_type) {
			case submetatable::const_value:
				metakey = &u_const_traits::metatable()[0];
				p_fast_index_table = &storage.const_value_index_table;
				break;
			case submetatable::reference:
				metakey = &u_ref_traits::metatable()[0];
				p_fast_index_table = &storage.reference_index_table;
				break;
			case submetatable::unique:
				metakey = &u_unique_traits::metatable()[0];
				p_fast_index_table = &storage.unique_index_table;
				break;
			case submetatable::const_reference:
				metakey = &u_const_ref_traits::metatable()[0];
				p_fast_index_table = &storage.const_reference_index_table;
				break;
			case submetatable::named:
				metakey = &u_traits::user_metatable()[0];
				p_fast_index_table = &storage.named_index_table;
				break;
			case submetatable::value:
			default:
				metakey = &u_traits::metatable()[0];
				p_fast_index_table = &storage.value_index_table;
				break;
			}
			reference& fast_index_table = *p_fast_index_table;

			luaL_newmetatable(L, metakey);
			stack_reference t(L, -1);
			stack::set_field(L, meta_function::type, storage.type_table, t.stack_index());
			if constexpr (std::is_destructible_v<T>) {
				// destructible: serialize default
				// destructor here
				switch (submetatable_type) {
				case submetatable::const_reference:
				case submetatable::reference:
					break;
				case submetatable::unique:
					stack::set_field(L, meta_function::garbage_collect, &detail::unique_destruct<T>, t.stack_index());
					break;
				case submetatable::value:
				case submetatable::const_value:
				default:
					stack::set_field(L, meta_function::garbage_collect, &detail::usertype_alloc_destruct<T>, t.stack_index());
					break;
				}
			}
			else {
				// not destructible: serialize a
				// "hey you messed up"
				// destructor
				stack::set_field(L, meta_function::garbage_collect, &detail::cannot_destruct<T>, t.stack_index());
			}

			if (base<T>::value) {
				static_assert(sizeof(void*) <= sizeof(detail::inheritance_check_function), "The size of this data pointer is too small to fit the inheritance checking function: file a bug report.");
				stack::set_field(L, detail::base_class_check_key(), (void*)&detail::inheritance<T>::type_check, t.stack_index());
			}
			if (base<T>::value) {
				static_assert(sizeof(void*) <= sizeof(detail::inheritance_cast_function), "The size of this data pointer is too small to fit the inheritance checking function: file a bug report.");
				stack::set_field(L, detail::base_class_cast_key(), (void*)&detail::inheritance<T>::type_cast, t.stack_index());
			}

			auto prop_fx = [&](meta_function mf) {
				return !storage.properties[static_cast<int>(mf)];
			};
			auto insert_fx = [&](meta_function mf, lua_CFunction reg) {
				stack::set_field(L, mf, reg, t.stack_index());
				storage.properties[static_cast<int>(mf)] = true;
			};
			u_detail::insert_default_registrations<T>(insert_fx, prop_fx);

			// There are no variables, so serialize the fast function stuff
			// be sure to reset the index stuff to the non-fast version
			// if the user ever adds something later!
			stack::set_field(L, meta_function::index, t, t.stack_index());
			if (submetatable_type == submetatable::named) {
				// add escape hatch storage pointer and gc names
				stack::set_field(L, meta_function::storage, make_light(base_storage), t.stack_index());
				stack::set_field(L, meta_function::gc_names, storage.gc_names_table, t.stack_index());

				// fancy new_indexing when using the named table
				stack::set_field<false, true>(L, metatable_key, storage.metametatable, t.stack_index());
				stack_reference stack_metametatable(L, -storage.metametatable.push());
				stack::set_field<false, true>(L, meta_function::index, make_closure(uts::meta_index_call, nullptr, make_light(storage), make_light(base_storage), nullptr, toplevel_magic), stack_metametatable.stack_index());
				stack::set_field<false, true>(L, meta_function::new_index, make_closure(uts::meta_new_index_call, nullptr, make_light(storage), make_light(base_storage), nullptr, toplevel_magic), stack_metametatable.stack_index());
				stack_metametatable.pop();
			}
			else {
				// otherwise just plain
				stack::set_field(L, meta_function::new_index, t, t.stack_index());
			}

			fast_index_table = reference(L, t);
			if (submetatable_type != submetatable::named) {
				t.pop();
			}
		}

		// can only use set AFTER we initialize all the metatables
		if constexpr (std::is_default_constructible_v<T>) {
			if (no_default_constructor == nullopt) {
				storage.set(L, meta_function::construct, constructors<T()>());
			}
		}

		// return the named metatable we left on top of the stack
		return 1;
	}
}

} // namespace sol::u_detail

#endif SOL_USERTYPE_STORAGE_HPP
