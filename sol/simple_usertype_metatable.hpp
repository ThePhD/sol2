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

#ifndef SOL_SIMPLE_USERTYPE_METATABLE_HPP
#define SOL_SIMPLE_USERTYPE_METATABLE_HPP

#include "usertype_metatable.hpp"
#include "object.hpp"
#include <vector>
#include <utility>

namespace sol {

	struct simple_tag {} const simple{};

	template <typename T>
	struct simple_usertype_metatable : usertype_detail::registrar {
		std::vector<std::pair<object, object>> registrations;
		object callconstructfunc;
		
		template <typename N, typename F, meta::enable<meta::is_callable<meta::unwrap_unqualified_t<F>>> = meta::enabler>
		void add(lua_State* L, N&& n, F&& f) {
			registrations.emplace_back(make_object(L, std::forward<N>(n)), make_object(L, function_args(std::forward<F>(f))));
		}

		template <typename N, typename F, meta::disable<meta::is_callable<meta::unwrap_unqualified_t<F>>> = meta::enabler>
		void add(lua_State* L, N&& n, F&& f) {
			registrations.emplace_back(make_object(L, std::forward<N>(n)), make_object(L, std::forward<F>(f)));
		}

		template <typename N, typename... Fxs>
		void add(lua_State* L, N&& n, constructor_wrapper<Fxs...> c) {
			registrations.emplace_back(make_object(L, std::forward<N>(n)), make_object(L, detail::tagged<T, constructor_wrapper<Fxs...>>{std::move(c)}));
		}

		template <typename N, typename... Lists>
		void add(lua_State* L, N&& n, constructor_list<Lists...> c) {
			registrations.emplace_back(make_object(L, std::forward<N>(n)), make_object(L, detail::tagged<T, constructor_list<Lists...>>{std::move(c)}));
		}

		template <typename F>
		void add(lua_State* L, call_construction, F&& f) {
			callconstructfunc = make_object(L, std::forward<F>(f));
		}

		template<std::size_t... I, typename Tuple>
		simple_usertype_metatable(usertype_detail::verified_tag, std::index_sequence<I...>, lua_State* L, Tuple&& args)
		: callconstructfunc(nil) {
			registrations.reserve(std::tuple_size<meta::unqualified_t<Tuple>>::value);
			(void)detail::swallow{ 0,
				(add(L, detail::forward_get<I * 2>(args), detail::forward_get<I * 2 + 1>(args)),0)...
			};
		}

		template<typename... Args>
		simple_usertype_metatable(lua_State* L, usertype_detail::verified_tag v, Args&&... args) : simple_usertype_metatable(v, std::make_index_sequence<sizeof...(Args) / 2>(), L, std::forward_as_tuple(std::forward<Args>(args)...)) {}

		template<typename... Args>
		simple_usertype_metatable(lua_State* L, usertype_detail::add_destructor_tag, Args&&... args) : simple_usertype_metatable(L, usertype_detail::verified, std::forward<Args>(args)..., "__gc", default_destructor) {}

		template<typename... Args>
		simple_usertype_metatable(lua_State* L, usertype_detail::check_destructor_tag, Args&&... args) : simple_usertype_metatable(L, meta::condition<meta::all<std::is_destructible<T>, meta::neg<usertype_detail::has_destructor<Args...>>>, usertype_detail::add_destructor_tag, usertype_detail::verified_tag>(), std::forward<Args>(args)...) {}

	public:
		simple_usertype_metatable(lua_State* L) : simple_usertype_metatable(meta::condition<meta::all<std::is_default_constructible<T>>, decltype(default_constructor), usertype_detail::check_destructor_tag>(), L) {}

		template<typename Arg, typename... Args, meta::disable_any<
			meta::any_same<meta::unqualified_t<Arg>, 
				usertype_detail::verified_tag, 
				usertype_detail::add_destructor_tag,
				usertype_detail::check_destructor_tag
			>,
			meta::is_specialization_of<constructors, meta::unqualified_t<Arg>>,
			meta::is_specialization_of<constructor_wrapper, meta::unqualified_t<Arg>>
		> = meta::enabler>
		simple_usertype_metatable(lua_State* L, Arg&& arg, Args&&... args) : simple_usertype_metatable(L, meta::condition<meta::all<std::is_default_constructible<T>, meta::neg<usertype_detail::has_constructor<Args...>>>, decltype(default_constructor), usertype_detail::check_destructor_tag>(), std::forward<Arg>(arg), std::forward<Args>(args)...) {}

		template<typename... Args, typename... CArgs>
		simple_usertype_metatable(lua_State* L, constructors<CArgs...> constructorlist, Args&&... args) : simple_usertype_metatable(L, usertype_detail::check_destructor_tag(), std::forward<Args>(args)..., "new", constructorlist) {}

		template<typename... Args, typename... Fxs>
		simple_usertype_metatable(lua_State* L, constructor_wrapper<Fxs...> constructorlist, Args&&... args) : simple_usertype_metatable(L, usertype_detail::check_destructor_tag(), std::forward<Args>(args)..., "new", constructorlist) {}

		virtual int push_um(lua_State* L) override {
			return stack::push(L, std::move(*this));
		}
	};

	namespace stack {
		template <typename T>
		struct pusher<simple_usertype_metatable<T>> {
			typedef simple_usertype_metatable<T> umt_t;
			
			static int push(lua_State* L, umt_t&& umx) {
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
					stack_reference t(L, -1);
					for (auto& kvp : umx.registrations) {
						switch (i) {
						case 0:
							if (kvp.first.template is<std::string>() && kvp.first.template as<std::string>() == "__gc") {
								continue;
							}
							break;
						case 1:
							if (kvp.first.template is<std::string>() && kvp.first.template as<std::string>() == "__gc") {
								stack::set_field(L, kvp.first, detail::unique_destruct<T>, t.stack_index());
								continue;
							}
							break;
						case 2:
						default:
							break;
						}
						stack::set_field(L, kvp.first, kvp.second, t.stack_index());
					}

					// Metatable indexes itself
					stack::set_field(L, meta_function::index, t, t.stack_index());

					// metatable on the metatable
					// for call constructor purposes and such
					lua_createtable(L, 0, 1);
					stack_reference metabehind(L, -1);
					if (umx.callconstructfunc.valid()) {
						stack::set_field(L, sol::meta_function::call_function, umx.callconstructfunc, metabehind.stack_index());
					}
					stack::set_field(L, metatable_key, metabehind, t.stack_index());
					metabehind.pop();

					if (i < 2)
						t.pop();
				}
				return 1;
			}
		};
	} // stack
} // sol

#endif // SOL_SIMPLE_USERTYPE_METATABLE_HPP
