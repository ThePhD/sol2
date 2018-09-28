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

#ifndef SOL_USERTYPE_CORE_HPP
#define SOL_USERTYPE_CORE_HPP

#include "wrapper.hpp"
#include "stack.hpp"
#include "types.hpp"
#include "stack_reference.hpp"
#include "usertype_traits.hpp"
#include "inheritance.hpp"
#include "raii.hpp"
#include "deprecate.hpp"
#include "object.hpp"
#include "usertype_container.hpp"

#include <sstream>

namespace sol {
	namespace u_detail {
		constexpr const lua_Integer toplevel_magic = static_cast<lua_Integer>(0xCCC2CCC1);

		constexpr const int environment_index = 1;
		constexpr const int usertype_storage_index = 2;
		constexpr const int usertype_storage_base_index = 3;
		constexpr const int exact_function_index = 4;
		constexpr const int magic_index = 5;

		constexpr const int simple_usertype_storage_index = 2;
		constexpr const int index_function_index = 3;
		constexpr const int new_index_function_index = 4;

		constexpr const int base_walking_failed_index = -32467;
		constexpr const int lookup_failed_index = -42469;

		enum class submetatable {
			// must be sequential
			value,
			reference,
			unique,
			const_reference,
			const_value,
			// must be LAST!
			named
		};

		struct no_comp {
			template <typename A, typename B>
			bool operator()(A&&, B&&) const {
				return false;
			}
		};

		template <typename T>
		inline int is_check(lua_State* L) {
			return stack::push(L, stack::check<T>(L, 1, &no_panic));
		}

		template <typename T>
		inline int member_default_to_string(std::true_type, lua_State* L) {
			decltype(auto) ts = stack::get<T>(L, 1).to_string();
			return stack::push(L, std::forward<decltype(ts)>(ts));
		}

		template <typename T>
		inline int member_default_to_string(std::false_type, lua_State* L) {
			return luaL_error(L, "cannot perform to_string on '%s': no 'to_string' overload in namespace, 'to_string' member function, or operator<<(ostream&, ...) present", detail::demangle<T>().data());
		}

		template <typename T>
		inline int adl_default_to_string(std::true_type, lua_State* L) {
			using namespace std;
			decltype(auto) ts = to_string(stack::get<T>(L, 1));
			return stack::push(L, std::forward<decltype(ts)>(ts));
		}

		template <typename T>
		inline int adl_default_to_string(std::false_type, lua_State* L) {
			return member_default_to_string<T>(meta::supports_to_string_member<T>(), L);
		}

		template <typename T>
		inline int oss_default_to_string(std::true_type, lua_State* L) {
			std::ostringstream oss;
			oss << stack::unqualified_get<T>(L, 1);
			return stack::push(L, oss.str());
		}

		template <typename T>
		inline int oss_default_to_string(std::false_type, lua_State* L) {
			return adl_default_to_string<T>(meta::supports_adl_to_string<T>(), L);
		}

		template <typename T>
		inline int default_to_string(lua_State* L) {
			return oss_default_to_string<T>(meta::supports_ostream_op<T>(), L);
		}

		template <typename T, typename Op>
		int comparsion_operator_wrap(lua_State* L) {
			auto maybel = stack::unqualified_check_get<T&>(L, 1);
			if (maybel) {
				auto mayber = stack::unqualified_check_get<T&>(L, 2);
				if (mayber) {
					auto& l = *maybel;
					auto& r = *mayber;
					if (std::is_same<no_comp, Op>::value) {
						return stack::push(L, detail::ptr(l) == detail::ptr(r));
					}
					else {
						Op op;
						return stack::push(L, (detail::ptr(l) == detail::ptr(r)) || op(detail::deref(l), detail::deref(r)));
					}
				}
			}
			return stack::push(L, false);
		}

		inline auto make_string_view(string_view s) {
			return s;
		}

		inline auto make_string_view(call_construction) {
			return string_view(to_string(meta_function::call_function));
		}

		inline auto make_string_view(meta_function mf) {
			return string_view(to_string(mf));
		}

		inline auto make_string_view(base_classes_tag) {
			return string_view(detail::base_class_cast_key());
		}

		template <typename Arg>
		inline std::string make_string(Arg&& arg) {
			string_view s = make_string_view(arg);
			return std::string(s.data(), s.size());
		}

		inline int is_indexer(string_view s) {
			if (s == to_string(meta_function::index)) {
				return 1;
			}
			else if (s == to_string(meta_function::new_index)) {
				return 2;
			}
			return 0;
		}

		inline int is_indexer(meta_function mf) {
			if (mf == meta_function::index) {
				return 1;
			}
			else if (mf == meta_function::new_index) {
				return 2;
			}
			return 0;
		}

		inline int is_indexer(call_construction) {
			return 0;
		}

		template <typename T>
		lua_CFunction make_destructor() {
			if constexpr (std::is_destructible_v<T>) {
				if constexpr (is_unique_usertype<T>::value) {
					return &detail::unique_destruct<T>;
				}
				else if constexpr (!std::is_pointer_v<T>) {
					return &detail::unique_destruct<T>;
				}
			}
			else {
				return &detail::cannot_destruct<T>;
			}
		}

		template <typename T, typename IFx, typename Fx>
		void insert_default_registrations(IFx&& ifx, Fx&& fx) {
			if constexpr (is_automagical<T>::value) {
				if (fx(meta_function::less_than)) {
					if constexpr (meta::supports_op_equal<T>::value) {
						lua_CFunction f = &comparsion_operator_wrap<T, std::less<>>;
						ifx(meta_function::less_than, f);
					}
				}
				if (fx(meta_function::less_than_or_equal_to)) {
					if constexpr (meta::supports_op_equal<T>::value) {
						lua_CFunction f = &comparsion_operator_wrap<T, std::less_equal<>>;
						ifx(meta_function::less_than_or_equal_to, f);
					}
				}
				if (fx(meta_function::equal_to)) {
					if constexpr (meta::supports_op_equal<T>::value) {
						lua_CFunction f = &comparsion_operator_wrap<T, std::equal_to<>>;
						ifx(meta_function::equal_to, f);
					}
				}
				if (fx(meta_function::pairs)) {
					ifx(meta_function::pairs, &usertype_container<as_container_t<T>>::pairs_call);
				}
				if (fx(meta_function::length)) {
					if constexpr (meta::has_size<T>::value) {
#if defined(__clang__)
						ifx(meta_function::length, &c_call<decltype(&T::size), &T::size>);
#else
						typedef decltype(std::declval<T>().size()) R;
						using sz_func = R (T::*)();
						ifx(meta_function::length, &c_call<decltype(static_cast<sz_func>(&T::size)), static_cast<sz_func>(&T::size)>);
#endif
					}
					else if constexpr (meta::has_size<const T>::value) {
#if defined(__clang__)
						ifx(meta_function::length, &c_call<decltype(&T::size), &T::size>);
#else
						typedef decltype(std::declval<T>().size()) R;
						using sz_func = R (T::*)() const;
						lua_CFunction f = &c_call<decltype(static_cast<sz_func>(&T::size)), static_cast<sz_func>(&T::size)>;
						ifx(meta_function::length, f);
#endif
					}
				}
				if (fx(meta_function::to_string)) {
					if constexpr (is_to_stringable<T>::value) {
						lua_CFunction f = &detail::static_trampoline<&default_to_string<T>>;
						ifx(meta_function::to_string, f);
					}
				}
				if (fx(meta_function::call_function)) {
					if constexpr (meta::has_deducible_signature<T>::value) {
						lua_CFunction f = &c_call<decltype(&T::operator()), &T::operator()>;
						ifx(meta_function::call_function, f);
					}
				}
			}
		}
	} // namespace u_detail

	namespace stack { namespace stack_detail {
		template <typename T>
		struct undefined_metatable {
			typedef meta::all<meta::neg<std::is_pointer<T>>, std::is_destructible<T>> is_destructible;
			typedef std::remove_pointer_t<T> P;
			lua_State* L;
			const char* key;

			undefined_metatable(lua_State* l, const char* k)
			: L(l), key(k) {
			}

			void operator()() const {
				if (luaL_newmetatable(L, key) == 1) {
					luaL_Reg l[64] {};
					int index = 0;
					auto prop_fx = [](meta_function) { return true; };
					auto insert_fx = [&l, &index](meta_function mf, lua_CFunction f) {
						l[index] = luaL_Reg { to_string(mf).c_str(), f };
						++index;
					};
					u_detail::insert_default_registrations<P>(insert_fx, prop_fx);
					l[index] = luaL_Reg { to_string(meta_function::garbage_collect).c_str(), u_detail::make_destructor<P>() };
					luaL_setfuncs(L, l, 0);

					// __type table
					lua_createtable(L, 0, 2);
					const std::string& name = detail::demangle<T>();
					lua_pushlstring(L, name.c_str(), name.size());
					lua_setfield(L, -2, "name");
					lua_CFunction is_func = &u_detail::is_check<T>;
					lua_pushcclosure(L, is_func, 0);
					lua_setfield(L, -2, "is");
					lua_setfield(L, -2, to_string(meta_function::type).c_str());
				}
				lua_setmetatable(L, -2);
			}
		};
	}
	} // namespace stack::stack_detail
} // namespace sol

#endif // SOL_USERTYPE_CORE_HPP
