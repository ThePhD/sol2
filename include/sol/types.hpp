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

#ifndef SOL_TYPES_HPP
#define SOL_TYPES_HPP

#include <sol/error.hpp>
#include <sol/optional.hpp>
#include <sol/compatibility.hpp>
#include <sol/forward.hpp>
#include <sol/forward_detail.hpp>
#include <sol/traits.hpp>
#include <sol/string_view.hpp>
#include <sol/raii.hpp>
#include <sol/policies.hpp>
#include <sol/ebco.hpp>

#include <array>
#include <initializer_list>
#include <string>
#include <string_view>
#include <optional>
#if SOL_IS_ON(SOL_STD_VARIANT_I_)
#include <variant>
#endif // variant shenanigans (thanks, Mac OSX)

namespace sol {
	namespace detail {
#if SOL_IS_ON(SOL_USE_NOEXCEPT_FUNCTION_TYPE_I_)
		typedef int (*lua_CFunction_noexcept)(lua_State* L) noexcept;
#else
		typedef int (*lua_CFunction_noexcept)(lua_State* L);
#endif // noexcept function type for lua_CFunction

		template <typename T>
		struct unique_usertype { };

		template <typename T>
		struct implicit_wrapper {
			T& item;
			implicit_wrapper(T* item) : item(*item) {
			}
			implicit_wrapper(T& item) : item(item) {
			}
			operator T&() {
				return item;
			}
			operator T*() {
				return std::addressof(item);
			}
		};

		struct yield_tag_t { };
		const yield_tag_t yield_tag = yield_tag_t {};
	} // namespace detail

	struct lua_nil_t { };
	inline constexpr lua_nil_t lua_nil {};
	inline bool operator==(lua_nil_t, lua_nil_t) {
		return true;
	}
	inline bool operator!=(lua_nil_t, lua_nil_t) {
		return false;
	}
#if SOL_IS_ON(SOL_NIL_I_)
	using nil_t = lua_nil_t;
	inline constexpr const nil_t& nil = lua_nil;
#endif

	namespace detail {
		struct non_lua_nil_t { };
	} // namespace detail

	struct metatable_key_t { };
	const metatable_key_t metatable_key = {};

	struct env_key_t { };
	const env_key_t env_key = {};

	struct no_metatable_t { };
	const no_metatable_t no_metatable = {};

	template <typename T>
	struct yielding_t {
		T func;

		yielding_t() = default;
		yielding_t(const yielding_t&) = default;
		yielding_t(yielding_t&&) = default;
		yielding_t& operator=(const yielding_t&) = default;
		yielding_t& operator=(yielding_t&&) = default;
		template <typename Arg,
		     meta::enable<meta::neg<std::is_same<meta::unqualified_t<Arg>, yielding_t>>,
		          meta::neg<std::is_base_of<proxy_base_tag, meta::unqualified_t<Arg>>>> = meta::enabler>
		yielding_t(Arg&& arg) : func(std::forward<Arg>(arg)) {
		}
		template <typename Arg0, typename Arg1, typename... Args>
		yielding_t(Arg0&& arg0, Arg1&& arg1, Args&&... args) : func(std::forward<Arg0>(arg0), std::forward<Arg1>(arg1), std::forward<Args>(args)...) {
		}
	};

	template <typename F>
	inline yielding_t<std::decay_t<F>> yielding(F&& f) {
		return yielding_t<std::decay_t<F>>(std::forward<F>(f));
	}

	typedef std::remove_pointer_t<lua_CFunction> lua_CFunction_ref;

	template <typename T>
	struct non_null { };

	template <typename... Args>
	struct function_sig { };

	struct upvalue_index {
		int index;
		upvalue_index(int idx) : index(lua_upvalueindex(idx)) {
		}

		operator int() const {
			return index;
		}
	};

	struct raw_index {
		int index;
		raw_index(int i) : index(i) {
		}

		operator int() const {
			return index;
		}
	};

	struct absolute_index {
		int index;
		absolute_index(lua_State* L, int idx) : index(lua_absindex(L, idx)) {
		}

		operator int() const {
			return index;
		}
	};

	struct ref_index {
		int index;
		ref_index(int idx) : index(idx) {
		}

		operator int() const {
			return index;
		}
	};

	struct stack_count {
		int count;

		stack_count(int cnt) : count(cnt) {
		}
	};

	struct lightuserdata_value {
		void* value;
		lightuserdata_value(void* data) : value(data) {
		}
		operator void*() const {
			return value;
		}
	};

	struct userdata_value {
		void* value;
		userdata_value(void* data) : value(data) {
		}
		operator void*() const {
			return value;
		}
	};

	template <typename L>
	struct light {
		L* value;

		light(L& x) : value(std::addressof(x)) {
		}
		light(L* x) : value(x) {
		}
		light(void* x) : value(static_cast<L*>(x)) {
		}
		operator L*() const {
			return value;
		}
		operator L&() const {
			return *value;
		}
	};

	template <typename T>
	auto make_light(T& l) {
		typedef meta::unwrapped_t<std::remove_pointer_t<std::remove_pointer_t<T>>> L;
		return light<L>(l);
	}

	template <typename U>
	struct user {
		U value;

		user(U&& x) : value(std::forward<U>(x)) {
		}
		operator std::add_pointer_t<std::remove_reference_t<U>>() {
			return std::addressof(value);
		}
		operator std::add_lvalue_reference_t<U>() {
			return value;
		}
		operator std::add_const_t<std::add_lvalue_reference_t<U>> &() const {
			return value;
		}
	};

	template <typename T>
	auto make_user(T&& u) {
		typedef meta::unwrapped_t<meta::unqualified_t<T>> U;
		return user<U>(std::forward<T>(u));
	}

	template <typename T>
	struct metatable_registry_key {
		T key;

		metatable_registry_key(T key) : key(std::forward<T>(key)) {
		}
	};

	template <typename T>
	auto meta_registry_key(T&& key) {
		typedef meta::unqualified_t<T> K;
		return metatable_registry_key<K>(std::forward<T>(key));
	}

	template <typename... Upvalues>
	struct closure {
		lua_CFunction c_function;
		std::tuple<Upvalues...> upvalues;
		closure(lua_CFunction f, Upvalues... targetupvalues) : c_function(f), upvalues(std::forward<Upvalues>(targetupvalues)...) {
		}
	};

	template <>
	struct closure<> {
		lua_CFunction c_function;
		int upvalues;
		closure(lua_CFunction f, int upvalue_count = 0) : c_function(f), upvalues(upvalue_count) {
		}
	};

	typedef closure<> c_closure;

	template <typename... Args>
	closure<Args...> make_closure(lua_CFunction f, Args&&... args) {
		return closure<Args...>(f, std::forward<Args>(args)...);
	}

	template <typename Sig, typename... Ps>
	struct function_arguments {
		std::tuple<Ps...> arguments;
		template <typename Arg, typename... Args, meta::disable<std::is_same<meta::unqualified_t<Arg>, function_arguments>> = meta::enabler>
		function_arguments(Arg&& arg, Args&&... args) : arguments(std::forward<Arg>(arg), std::forward<Args>(args)...) {
		}
	};

	template <typename Sig = function_sig<>, typename... Args>
	auto as_function(Args&&... args) {
		return function_arguments<Sig, std::decay_t<Args>...>(std::forward<Args>(args)...);
	}

	template <typename Sig = function_sig<>, typename... Args>
	auto as_function_reference(Args&&... args) {
		return function_arguments<Sig, Args...>(std::forward<Args>(args)...);
	}

	template <typename T>
	struct as_table_t {
	private:
		T value_;

	public:
		as_table_t() = default;
		as_table_t(const as_table_t&) = default;
		as_table_t(as_table_t&&) = default;
		as_table_t& operator=(const as_table_t&) = default;
		as_table_t& operator=(as_table_t&&) = default;
		template <typename Arg,
		     meta::enable<meta::neg<std::is_same<meta::unqualified_t<Arg>, as_table_t>>,
		          meta::neg<std::is_base_of<proxy_base_tag, meta::unqualified_t<Arg>>>> = meta::enabler>
		as_table_t(Arg&& arg) : value_(std::forward<Arg>(arg)) {
		}
		template <typename Arg0, typename Arg1, typename... Args>
		as_table_t(Arg0&& arg0, Arg1&& arg1, Args&&... args) : value_(std::forward<Arg0>(arg0), std::forward<Arg1>(arg1), std::forward<Args>(args)...) {
		}

		T& value() & {
			return value_;
		}

		T&& value() && {
			return std::move(value_);
		}

		const T& value() const& {
			return value_;
		}

		operator std::add_lvalue_reference_t<T>() {
			return value_;
		}
	};

	template <typename T>
	struct nested {
	private:
		T value_;

	public:
		using nested_type = T;

		nested() = default;
		nested(const nested&) = default;
		nested(nested&&) = default;
		nested& operator=(const nested&) = default;
		nested& operator=(nested&&) = default;
		template <typename Arg,
		     meta::enable<meta::neg<std::is_same<meta::unqualified_t<Arg>, nested>>,
		          meta::neg<std::is_base_of<proxy_base_tag, meta::unqualified_t<Arg>>>> = meta::enabler>
		nested(Arg&& arg) : value_(std::forward<Arg>(arg)) {
		}
		template <typename Arg0, typename Arg1, typename... Args>
		nested(Arg0&& arg0, Arg1&& arg1, Args&&... args) : value_(std::forward<Arg0>(arg0), std::forward<Arg1>(arg1), std::forward<Args>(args)...) {
		}

		T& value() & {
			return value_;
		}

		T&& value() && {
			return std::move(value_);
		}

		const T& value() const& {
			return value_;
		}

		operator std::add_lvalue_reference_t<T>() {
			return value_;
		}
	};

	struct nested_tag_t { };
	constexpr inline nested_tag_t nested_tag {};

	template <typename T>
	as_table_t<T> as_table_ref(T&& container) {
		return as_table_t<T>(std::forward<T>(container));
	}

	template <typename T>
	as_table_t<meta::unqualified_t<T>> as_table(T&& container) {
		return as_table_t<meta::unqualified_t<T>>(std::forward<T>(container));
	}

	template <typename T>
	nested<T> as_nested_ref(T&& container) {
		return nested<T>(std::forward<T>(container));
	}

	template <typename T>
	nested<meta::unqualified_t<T>> as_nested(T&& container) {
		return nested<meta::unqualified_t<T>>(std::forward<T>(container));
	}

	template <typename T>
	struct as_container_t {
	private:
		T value_;

	public:
		using type = T;

		as_container_t() = default;
		as_container_t(const as_container_t&) = default;
		as_container_t(as_container_t&&) = default;
		as_container_t& operator=(const as_container_t&) = default;
		as_container_t& operator=(as_container_t&&) = default;
		template <typename Arg,
		     meta::enable<meta::neg<std::is_same<meta::unqualified_t<Arg>, as_container_t>>,
		          meta::neg<std::is_base_of<proxy_base_tag, meta::unqualified_t<Arg>>>> = meta::enabler>
		as_container_t(Arg&& arg) : value_(std::forward<Arg>(arg)) {
		}
		template <typename Arg0, typename Arg1, typename... Args>
		as_container_t(Arg0&& arg0, Arg1&& arg1, Args&&... args) : value_(std::forward<Arg0>(arg0), std::forward<Arg1>(arg1), std::forward<Args>(args)...) {
		}

		T& value() & {
			return value_;
		}

		T&& value() && {
			return std::move(value_);
		}

		const T& value() const& {
			return value_;
		}
	};

	template <typename T>
	struct as_container_t<T&> {
	private:
		std::reference_wrapper<T> value_;

	public:
		as_container_t(T& value) : value_(value) {
		}

		T& value() {
			return value_;
		}

		operator T&() {
			return value();
		}
	};

	template <typename T>
	auto as_container(T&& value) {
		return as_container_t<T>(std::forward<T>(value));
	}

	template <typename T>
	struct push_invoke_t {
	private:
		T value_;

	public:
		push_invoke_t() = default;
		push_invoke_t(const push_invoke_t&) = default;
		push_invoke_t(push_invoke_t&&) = default;
		push_invoke_t& operator=(const push_invoke_t&) = default;
		push_invoke_t& operator=(push_invoke_t&&) = default;
		template <typename Arg,
		     meta::enable<meta::neg<std::is_same<meta::unqualified_t<Arg>, push_invoke_t>>,
		          meta::neg<std::is_base_of<proxy_base_tag, meta::unqualified_t<Arg>>>> = meta::enabler>
		push_invoke_t(Arg&& arg) : value_(std::forward<Arg>(arg)) {
		}
		template <typename Arg0, typename Arg1, typename... Args>
		push_invoke_t(Arg0&& arg0, Arg1&& arg1, Args&&... args) : value_(std::forward<Arg0>(arg0), std::forward<Arg1>(arg1), std::forward<Args>(args)...) {
		}

		T& value() & {
			return value_;
		}

		T&& value() && {
			return std::move(value_);
		}

		const T& value() const& {
			return value_;
		}
	};

	template <typename T>
	struct push_invoke_t<T&> {
		std::reference_wrapper<T> value_;

		push_invoke_t(T& value) : value_(value) {
		}

		T& value() {
			return value_;
		}
	};

	template <typename Fx>
	auto push_invoke(Fx&& fx) {
		return push_invoke_t<Fx>(std::forward<Fx>(fx));
	}

	struct override_value_t { };
	constexpr inline override_value_t override_value = override_value_t();
	struct update_if_empty_t { };
	constexpr inline update_if_empty_t update_if_empty = update_if_empty_t();
	struct create_if_nil_t { };
	constexpr inline create_if_nil_t create_if_nil = create_if_nil_t();

	namespace detail {
		enum insert_mode { none = 0x0, update_if_empty = 0x01, override_value = 0x02, create_if_nil = 0x04 };

		template <typename T, typename...>
		using is_insert_mode = std::integral_constant<bool,
		     std::is_same_v<T, override_value_t> || std::is_same_v<T, update_if_empty_t> || std::is_same_v<T, create_if_nil_t>>;

		template <typename T, typename...>
		using is_not_insert_mode = meta::neg<is_insert_mode<T>>;
	} // namespace detail

	struct this_state {
		lua_State* L;

		this_state(lua_State* Ls) : L(Ls) {
		}

		operator lua_State*() const noexcept {
			return lua_state();
		}

		lua_State* operator->() const noexcept {
			return lua_state();
		}

		lua_State* lua_state() const noexcept {
			return L;
		}
	};

	struct this_main_state {
		lua_State* L;

		this_main_state(lua_State* Ls) : L(Ls) {
		}

		operator lua_State*() const noexcept {
			return lua_state();
		}

		lua_State* operator->() const noexcept {
			return lua_state();
		}

		lua_State* lua_state() const noexcept {
			return L;
		}
	};

	struct new_table {
		int sequence_hint = 0;
		int map_hint = 0;

		new_table() = default;
		new_table(const new_table&) = default;
		new_table(new_table&&) = default;
		new_table& operator=(const new_table&) = default;
		new_table& operator=(new_table&&) = default;

		new_table(int sequence_hint, int map_hint = 0) : sequence_hint(sequence_hint), map_hint(map_hint) {
		}
	};

	const new_table create = {};

	enum class lib : char {
		// print, assert, and other base functions
		base,
		// require and other package functions
		package,
		// coroutine functions and utilities
		coroutine,
		// string library
		string,
		// functionality from the OS
		os,
		// all things math
		math,
		// the table manipulator and observer functions
		table,
		// the debug library
		debug,
		// the bit library: different based on which you're using
		bit32,
		// input/output library
		io,
		// LuaJIT only
		ffi,
		// LuaJIT only
		jit,
		// library for handling utf8: new to Lua
		utf8,
		// do not use
		count
	};

	enum class call_syntax { dot = 0, colon = 1 };

	enum class load_mode {
		any = 0,
		text = 1,
		binary = 2,
	};

	enum class call_status : int {
		ok = LUA_OK,
		yielded = LUA_YIELD,
		runtime = LUA_ERRRUN,
		memory = LUA_ERRMEM,
		handler = LUA_ERRERR,
		gc = LUA_ERRGCMM,
		syntax = LUA_ERRSYNTAX,
		file = LUA_ERRFILE,
	};

	enum class thread_status : int {
		ok = LUA_OK,
		yielded = LUA_YIELD,
		runtime = LUA_ERRRUN,
		memory = LUA_ERRMEM,
		gc = LUA_ERRGCMM,
		handler = LUA_ERRERR,
		dead = -1,
	};

	enum class load_status : int {
		ok = LUA_OK,
		syntax = LUA_ERRSYNTAX,
		memory = LUA_ERRMEM,
		gc = LUA_ERRGCMM,
		file = LUA_ERRFILE,
	};

	enum class gc_mode : int {
		incremental = 0,
		generational = 1,
		default_value = incremental,
	};

	enum class type : int {
		none = LUA_TNONE,
		lua_nil = LUA_TNIL,
#if SOL_IS_ON(SOL_NIL_I_)
		nil = lua_nil,
#endif // Objective C/C++ Keyword that's found in OSX SDK and OBJC -- check for all forms to protect
		string = LUA_TSTRING,
		number = LUA_TNUMBER,
		thread = LUA_TTHREAD,
		boolean = LUA_TBOOLEAN,
		function = LUA_TFUNCTION,
		userdata = LUA_TUSERDATA,
		lightuserdata = LUA_TLIGHTUSERDATA,
		table = LUA_TTABLE,
		poly = -0xFFFF
	};

	inline const std::string& to_string(call_status c) {
		static const std::array<std::string, 10> names { { "ok",
			"yielded",
			"runtime",
			"memory",
			"handler",
			"gc",
			"syntax",
			"file",
			"CRITICAL_EXCEPTION_FAILURE",
			"CRITICAL_INDETERMINATE_STATE_FAILURE" } };
		switch (c) {
		case call_status::ok:
			return names[0];
		case call_status::yielded:
			return names[1];
		case call_status::runtime:
			return names[2];
		case call_status::memory:
			return names[3];
		case call_status::handler:
			return names[4];
		case call_status::gc:
			return names[5];
		case call_status::syntax:
			return names[6];
		case call_status::file:
			return names[7];
		}
		if (static_cast<std::ptrdiff_t>(c) == -1) {
			// One of the many cases where a critical exception error has occurred
			return names[8];
		}
		return names[9];
	}

	inline bool is_indeterminate_call_failure(call_status c) {
		switch (c) {
		case call_status::ok:
		case call_status::yielded:
		case call_status::runtime:
		case call_status::memory:
		case call_status::handler:
		case call_status::gc:
		case call_status::syntax:
		case call_status::file:
			return false;
		}
		return true;
	}

	inline const std::string& to_string(load_status c) {
		static const std::array<std::string, 7> names {
			{ "ok", "memory", "gc", "syntax", "file", "CRITICAL_EXCEPTION_FAILURE", "CRITICAL_INDETERMINATE_STATE_FAILURE" }
		};
		switch (c) {
		case load_status::ok:
			return names[0];
		case load_status::memory:
			return names[1];
		case load_status::gc:
			return names[2];
		case load_status::syntax:
			return names[3];
		case load_status::file:
			return names[4];
		}
		if (static_cast<int>(c) == -1) {
			// One of the many cases where a critical exception error has occurred
			return names[5];
		}
		return names[6];
	}

	inline const std::string& to_string(load_mode c) {
		static const std::array<std::string, 3> names { {
			"bt",
			"t",
			"b",
		} };
		return names[static_cast<std::size_t>(c)];
	}

	enum class meta_function {
		construct,
		index,
		new_index,
		mode,
		call,
		call_function = call,
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
		floor_division,
		bitwise_left_shift,
		bitwise_right_shift,
		bitwise_not,
		bitwise_and,
		bitwise_or,
		bitwise_xor,
		pairs,
		ipairs,
		next,
		type,
		type_info,
		call_construct,
		storage,
		gc_names,
		static_index,
		static_new_index,
	};

	typedef meta_function meta_method;

	inline const std::array<std::string, 37>& meta_function_names() {
		static const std::array<std::string, 37> names = { { "new",
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

			"__idiv",
			"__shl",
			"__shr",
			"__bnot",
			"__band",
			"__bor",
			"__bxor",

			"__pairs",
			"__ipairs",
			"next",

			"__type",
			"__typeinfo",
			"__sol.call_new",
			"__sol.storage",
			"__sol.gc_names",
			"__sol.static_index",
			"__sol.static_new_index" } };
		return names;
	}

	inline const std::string& to_string(meta_function mf) {
		return meta_function_names()[static_cast<int>(mf)];
	}

	inline type type_of(lua_State* L, int index) {
		return static_cast<type>(lua_type(L, index));
	}

	inline std::string type_name(lua_State* L, type t) {
		return lua_typename(L, static_cast<int>(t));
	}

	template <typename T>
	struct is_lua_reference
	: std::integral_constant<bool, std::is_base_of_v<reference, T> || std::is_base_of_v<main_reference, T> || std::is_base_of_v<stack_reference, T>> { };

	template <typename T>
	inline constexpr bool is_lua_reference_v = is_lua_reference<T>::value;

	template <typename T>
	struct is_lua_reference_or_proxy : std::integral_constant<bool, is_lua_reference_v<T> || meta::is_specialization_of_v<T, table_proxy>> { };

	template <typename T>
	inline constexpr bool is_lua_reference_or_proxy_v = is_lua_reference_or_proxy<T>::value;

	template <typename T>
	struct is_transparent_argument : std::false_type { };

	template <typename T>
	constexpr inline bool is_transparent_argument_v = is_transparent_argument<T>::value;

	template <>
	struct is_transparent_argument<this_state> : std::true_type { };
	template <>
	struct is_transparent_argument<this_main_state> : std::true_type { };
	template <>
	struct is_transparent_argument<this_environment> : std::true_type { };
	template <>
	struct is_transparent_argument<variadic_args> : std::true_type { };
	template <typename T>
	struct is_variadic_arguments : std::is_same<T, variadic_args> { };

	template <typename T>
	struct is_container
	: std::integral_constant<bool,
	       !std::is_same_v<state_view,
	            T> && !std::is_same_v<state, T> && !meta::is_initializer_list_v<T> && !meta::is_string_like_v<T> && !meta::is_string_literal_array_v<T> && !is_transparent_argument_v<T> && !is_lua_reference_v<T> && (meta::has_begin_end_v<T> || std::is_array_v<T>)> {
	};

	template <typename T>
	constexpr inline bool is_container_v = is_container<T>::value;

	template <typename T>
	struct is_to_stringable : meta::any<meta::supports_to_string_member<meta::unqualified_t<T>>, meta::supports_adl_to_string<meta::unqualified_t<T>>,
	                               meta::supports_op_left_shift<std::ostream, meta::unqualified_t<T>>> { };

	namespace detail {
		template <typename T, typename = void>
		struct lua_type_of : std::integral_constant<type, type::userdata> { };

		template <typename C, typename T, typename A>
		struct lua_type_of<std::basic_string<C, T, A>> : std::integral_constant<type, type::string> { };

		template <typename C, typename T>
		struct lua_type_of<basic_string_view<C, T>> : std::integral_constant<type, type::string> { };

		template <std::size_t N>
		struct lua_type_of<char[N]> : std::integral_constant<type, type::string> { };

		template <std::size_t N>
		struct lua_type_of<wchar_t[N]> : std::integral_constant<type, type::string> { };

		template <std::size_t N>
		struct lua_type_of<char16_t[N]> : std::integral_constant<type, type::string> { };

		template <std::size_t N>
		struct lua_type_of<char32_t[N]> : std::integral_constant<type, type::string> { };

		template <>
		struct lua_type_of<char> : std::integral_constant<type, type::string> { };

		template <>
		struct lua_type_of<wchar_t> : std::integral_constant<type, type::string> { };

		template <>
		struct lua_type_of<char16_t> : std::integral_constant<type, type::string> { };

		template <>
		struct lua_type_of<char32_t> : std::integral_constant<type, type::string> { };

		template <>
		struct lua_type_of<const char*> : std::integral_constant<type, type::string> { };

		template <>
		struct lua_type_of<const char16_t*> : std::integral_constant<type, type::string> { };

		template <>
		struct lua_type_of<const char32_t*> : std::integral_constant<type, type::string> { };

		template <>
		struct lua_type_of<bool> : std::integral_constant<type, type::boolean> { };

		template <>
		struct lua_type_of<lua_nil_t> : std::integral_constant<type, type::lua_nil> { };

		template <>
		struct lua_type_of<nullopt_t> : std::integral_constant<type, type::lua_nil> { };

		template <>
		struct lua_type_of<lua_value> : std::integral_constant<type, type::poly> { };

		template <>
		struct lua_type_of<detail::non_lua_nil_t> : std::integral_constant<type, type::poly> { };

		template <>
		struct lua_type_of<std::nullptr_t> : std::integral_constant<type, type::lua_nil> { };

		template <>
		struct lua_type_of<error> : std::integral_constant<type, type::string> { };

		template <bool b, typename Base>
		struct lua_type_of<basic_table_core<b, Base>> : std::integral_constant<type, type::table> { };

		template <typename Base>
		struct lua_type_of<basic_lua_table<Base>> : std::integral_constant<type, type::table> { };

		template <typename Base>
		struct lua_type_of<basic_metatable<Base>> : std::integral_constant<type, type::table> { };

		template <typename T, typename Base>
		struct lua_type_of<basic_usertype<T, Base>> : std::integral_constant<type, type::table> { };

		template <>
		struct lua_type_of<metatable_key_t> : std::integral_constant<type, type::table> { };

		template <typename B>
		struct lua_type_of<basic_environment<B>> : std::integral_constant<type, type::poly> { };

		template <>
		struct lua_type_of<env_key_t> : std::integral_constant<type, type::poly> { };

		template <>
		struct lua_type_of<new_table> : std::integral_constant<type, type::table> { };

		template <typename T>
		struct lua_type_of<as_table_t<T>> : std::integral_constant<type, type::table> { };

		template <typename T>
		struct lua_type_of<std::initializer_list<T>> : std::integral_constant<type, type::table> { };

		template <bool b>
		struct lua_type_of<basic_reference<b>> : std::integral_constant<type, type::poly> { };

		template <>
		struct lua_type_of<stack_reference> : std::integral_constant<type, type::poly> { };

		template <typename Base>
		struct lua_type_of<basic_object<Base>> : std::integral_constant<type, type::poly> { };

		template <typename... Args>
		struct lua_type_of<std::tuple<Args...>> : std::integral_constant<type, type::poly> { };

		template <typename A, typename B>
		struct lua_type_of<std::pair<A, B>> : std::integral_constant<type, type::poly> { };

		template <>
		struct lua_type_of<void*> : std::integral_constant<type, type::lightuserdata> { };

		template <>
		struct lua_type_of<const void*> : std::integral_constant<type, type::lightuserdata> { };

		template <>
		struct lua_type_of<lightuserdata_value> : std::integral_constant<type, type::lightuserdata> { };

		template <>
		struct lua_type_of<userdata_value> : std::integral_constant<type, type::userdata> { };

		template <typename T>
		struct lua_type_of<light<T>> : std::integral_constant<type, type::lightuserdata> { };

		template <typename T>
		struct lua_type_of<user<T>> : std::integral_constant<type, type::userdata> { };

		template <typename Base>
		struct lua_type_of<basic_lightuserdata<Base>> : std::integral_constant<type, type::lightuserdata> { };

		template <typename Base>
		struct lua_type_of<basic_userdata<Base>> : std::integral_constant<type, type::userdata> { };

		template <>
		struct lua_type_of<lua_CFunction> : std::integral_constant<type, type::function> { };

		template <>
		struct lua_type_of<std::remove_pointer_t<lua_CFunction>> : std::integral_constant<type, type::function> { };

		template <typename Base, bool aligned>
		struct lua_type_of<basic_function<Base, aligned>> : std::integral_constant<type, type::function> { };

		template <typename Base, bool aligned, typename Handler>
		struct lua_type_of<basic_protected_function<Base, aligned, Handler>> : std::integral_constant<type, type::function> { };

		template <typename Base>
		struct lua_type_of<basic_coroutine<Base>> : std::integral_constant<type, type::function> { };

		template <typename Base>
		struct lua_type_of<basic_thread<Base>> : std::integral_constant<type, type::thread> { };

		template <typename Signature>
		struct lua_type_of<std::function<Signature>> : std::integral_constant<type, type::function> { };

		template <typename T>
		struct lua_type_of<optional<T>> : std::integral_constant<type, type::poly> { };

		template <typename T>
		struct lua_type_of<std::optional<T>> : std::integral_constant<type, type::poly> { };

		template <>
		struct lua_type_of<variadic_args> : std::integral_constant<type, type::poly> { };

		template <>
		struct lua_type_of<variadic_results> : std::integral_constant<type, type::poly> { };

		template <>
		struct lua_type_of<stack_count> : std::integral_constant<type, type::poly> { };

		template <>
		struct lua_type_of<this_state> : std::integral_constant<type, type::poly> { };

		template <>
		struct lua_type_of<this_main_state> : std::integral_constant<type, type::poly> { };

		template <>
		struct lua_type_of<this_environment> : std::integral_constant<type, type::poly> { };

		template <>
		struct lua_type_of<type> : std::integral_constant<type, type::poly> { };

#if SOL_IS_ON(SOL_GET_FUNCTION_POINTER_UNSAFE_I_)
		template <typename T>
		struct lua_type_of<T*> : std::integral_constant<type, std::is_function_v<T> ? type::function : type::userdata> { };
#else
		template <typename T>
		struct lua_type_of<T*> : std::integral_constant<type, type::userdata> { };
#endif

		template <typename T>
		struct lua_type_of<T, std::enable_if_t<std::is_arithmetic_v<T> || std::is_same_v<T, lua_Number> || std::is_same_v<T, lua_Integer>>>
		: std::integral_constant<type, type::number> { };

		template <typename T>
		struct lua_type_of<T, std::enable_if_t<std::is_function_v<T>>> : std::integral_constant<type, type::function> { };

		template <typename T>
		struct lua_type_of<T, std::enable_if_t<std::is_enum_v<T>>> : std::integral_constant<type, type::number> { };

		template <>
		struct lua_type_of<meta_function> : std::integral_constant<type, type::string> { };

#if SOL_IS_ON(SOL_STD_VARIANT_I_)
		template <typename... Tn>
		struct lua_type_of<std::variant<Tn...>> : std::integral_constant<type, type::poly> { };
#endif // std::variant deployment sucks on Clang

		template <typename T>
		struct lua_type_of<nested<T>> : meta::conditional_t<::sol::is_container_v<T>, std::integral_constant<type, type::table>, lua_type_of<T>> { };

		template <typename C, C v, template <typename...> class V, typename... Args>
		struct accumulate : std::integral_constant<C, v> { };

		template <typename C, C v, template <typename...> class V, typename T, typename... Args>
		struct accumulate<C, v, V, T, Args...> : accumulate<C, v + V<T>::value, V, Args...> { };

		template <typename C, C v, template <typename...> class V, typename List>
		struct accumulate_list;

		template <typename C, C v, template <typename...> class V, typename... Args>
		struct accumulate_list<C, v, V, types<Args...>> : accumulate<C, v, V, Args...> { };
	} // namespace detail

	template <typename T>
	struct lua_type_of : detail::lua_type_of<T> {
		typedef int SOL_INTERNAL_UNSPECIALIZED_MARKER_;
	};

	template <typename T>
	inline constexpr type lua_type_of_v = lua_type_of<T>::value;

	template <typename T>
	struct lua_size : std::integral_constant<int, 1> {
		typedef int SOL_INTERNAL_UNSPECIALIZED_MARKER_;
	};

	template <typename A, typename B>
	struct lua_size<std::pair<A, B>> : std::integral_constant<int, lua_size<A>::value + lua_size<B>::value> { };

	template <typename... Args>
	struct lua_size<std::tuple<Args...>> : std::integral_constant<int, detail::accumulate<int, 0, lua_size, Args...>::value> { };

	template <typename T>
	inline constexpr int lua_size_v = lua_size<T>::value;

	namespace detail {
		template <typename...>
		struct void_ {
			typedef void type;
		};
		template <typename T, typename = void>
		struct has_internal_marker_impl : std::false_type { };
		template <typename T>
		struct has_internal_marker_impl<T, typename void_<typename T::SOL_INTERNAL_UNSPECIALIZED_MARKER_>::type> : std::true_type { };

		template <typename T>
		using has_internal_marker = has_internal_marker_impl<T>;

		template <typename T>
		constexpr inline bool has_internal_marker_v = has_internal_marker<T>::value;
	} // namespace detail

	template <typename T>
	struct is_lua_primitive
	: std::integral_constant<bool,
	       type::userdata
	                 != lua_type_of_v<
	                      T> || ((type::userdata == lua_type_of_v<T>)&&detail::has_internal_marker_v<lua_type_of<T>> && !detail::has_internal_marker_v<lua_size<T>>)
	            || is_lua_reference_or_proxy_v<T> || meta::is_specialization_of_v<T, std::tuple> || meta::is_specialization_of_v<T, std::pair>> { };

	template <typename T>
	constexpr inline bool is_lua_primitive_v = is_lua_primitive<T>::value;

	template <typename T>
	struct is_main_threaded : std::is_base_of<main_reference, T> { };

	template <typename T>
	struct is_stack_based : std::is_base_of<stack_reference, T> { };
	template <>
	struct is_stack_based<variadic_args> : std::true_type { };
	template <>
	struct is_stack_based<unsafe_function_result> : std::true_type { };
	template <>
	struct is_stack_based<protected_function_result> : std::true_type { };
	template <>
	struct is_stack_based<stack_proxy> : std::true_type { };
	template <>
	struct is_stack_based<stack_proxy_base> : std::true_type { };
	template <>
	struct is_stack_based<stack_count> : std::true_type { };

	template <typename T>
	constexpr inline bool is_stack_based_v = is_stack_based<T>::value;

	template <typename T>
	struct is_lua_primitive<T*> : std::true_type { };
	template <>
	struct is_lua_primitive<unsafe_function_result> : std::true_type { };
	template <>
	struct is_lua_primitive<protected_function_result> : std::true_type { };
	template <typename T>
	struct is_lua_primitive<std::reference_wrapper<T>> : std::true_type { };
	template <typename T>
	struct is_lua_primitive<user<T>> : std::true_type { };
	template <typename T>
	struct is_lua_primitive<light<T>> : is_lua_primitive<T*> { };
	template <typename T>
	struct is_lua_primitive<optional<T>> : std::true_type { };
	template <typename T>
	struct is_lua_primitive<std::optional<T>> : std::true_type { };
	template <typename T>
	struct is_lua_primitive<as_table_t<T>> : std::true_type { };
	template <typename T>
	struct is_lua_primitive<nested<T>> : std::true_type { };
	template <>
	struct is_lua_primitive<userdata_value> : std::true_type { };
	template <>
	struct is_lua_primitive<lightuserdata_value> : std::true_type { };
	template <>
	struct is_lua_primitive<stack_proxy> : std::true_type { };
	template <>
	struct is_lua_primitive<stack_proxy_base> : std::true_type { };
	template <typename T>
	struct is_lua_primitive<non_null<T>> : is_lua_primitive<T*> { };

	template <typename T>
	struct is_lua_index : std::is_integral<T> { };
	template <>
	struct is_lua_index<raw_index> : std::true_type { };
	template <>
	struct is_lua_index<absolute_index> : std::true_type { };
	template <>
	struct is_lua_index<ref_index> : std::true_type { };
	template <>
	struct is_lua_index<upvalue_index> : std::true_type { };

	template <typename Signature>
	struct lua_bind_traits : meta::bind_traits<Signature> {
	private:
		typedef meta::bind_traits<Signature> base_t;

	public:
		typedef std::integral_constant<bool, meta::count_for<is_variadic_arguments, typename base_t::args_list>::value != 0> runtime_variadics_t;
		static const std::size_t true_arity = base_t::arity;
		static const std::size_t arity = detail::accumulate_list<std::size_t, 0, lua_size, typename base_t::args_list>::value
		     - meta::count_for<is_transparent_argument, typename base_t::args_list>::value;
		static const std::size_t true_free_arity = base_t::free_arity;
		static const std::size_t free_arity = detail::accumulate_list<std::size_t, 0, lua_size, typename base_t::free_args_list>::value
		     - meta::count_for<is_transparent_argument, typename base_t::args_list>::value;
	};

	template <typename T>
	struct is_table : std::false_type { };
	template <bool x, typename T>
	struct is_table<basic_table_core<x, T>> : std::true_type { };
	template <typename T>
	struct is_table<basic_lua_table<T>> : std::true_type { };

	template <typename T>
	inline constexpr bool is_table_v = is_table<T>::value;

	template <typename T>
	struct is_stack_table : std::false_type { };
	template <bool x, typename T>
	struct is_stack_table<basic_table_core<x, T>> : std::integral_constant<bool, std::is_base_of_v<stack_reference, T>> { };
	template <typename T>
	struct is_stack_table<basic_lua_table<T>> : std::integral_constant<bool, std::is_base_of_v<stack_reference, T>> { };

	template <typename T>
	inline constexpr bool is_stack_table_v = is_stack_table<T>::value;

	template <typename T>
	struct is_function : std::false_type { };
	template <typename T, bool aligned>
	struct is_function<basic_function<T, aligned>> : std::true_type { };
	template <typename T, bool aligned, typename Handler>
	struct is_function<basic_protected_function<T, aligned, Handler>> : std::true_type { };


	template <typename T>
	using is_lightuserdata = meta::is_specialization_of<T, basic_lightuserdata>;

	template <typename T>
	inline constexpr bool is_lightuserdata_v = is_lightuserdata<T>::value;

	template <typename T>
	using is_userdata = meta::is_specialization_of<T, basic_userdata>;

	template <typename T>
	inline constexpr bool is_userdata_v = is_userdata<T>::value;

	template <typename T>
	using is_environment = std::integral_constant<bool, is_userdata_v<T> || is_table_v<T> || meta::is_specialization_of_v<T, basic_environment>>;

	template <typename T>
	inline constexpr bool is_environment_v = is_environment<T>::value;

	template <typename T>
	using is_table_like = std::integral_constant<bool, is_table_v<T> || is_environment_v<T> || is_userdata_v<T>>;

	template <typename T>
	inline constexpr bool is_table_like_v = is_table_like<T>::value;

	template <typename T>
	struct is_automagical
	: std::integral_constant<bool,
	       (SOL_IS_ON(SOL_DEFAULT_AUTOMAGICAL_USERTYPES_I_))
	            || (std::is_array_v<
	                     meta::unqualified_t<T>> || (!std::is_same_v<meta::unqualified_t<T>, state> && !std::is_same_v<meta::unqualified_t<T>, state_view>))> {
	};

	template <typename T>
	inline type type_of() {
		return lua_type_of<meta::unqualified_t<T>>::value;
	}

	namespace detail {
		template <typename T>
		struct is_non_factory_constructor : std::false_type { };

		template <typename... Args>
		struct is_non_factory_constructor<constructors<Args...>> : std::true_type { };

		template <typename... Args>
		struct is_non_factory_constructor<constructor_wrapper<Args...>> : std::true_type { };

		template <>
		struct is_non_factory_constructor<no_construction> : std::true_type { };

		template <typename T>
		inline constexpr bool is_non_factory_constructor_v = is_non_factory_constructor<T>::value;

		template <typename T>
		struct is_constructor : is_non_factory_constructor<T> { };

		template <typename... Args>
		struct is_constructor<factory_wrapper<Args...>> : std::true_type { };

		template <typename T>
		struct is_constructor<protect_t<T>> : is_constructor<meta::unqualified_t<T>> { };

		template <typename F, typename... Policies>
		struct is_constructor<policy_wrapper<F, Policies...>> : is_constructor<meta::unqualified_t<F>> { };

		template <typename T>
		inline constexpr bool is_constructor_v = is_constructor<T>::value;

		template <typename... Args>
		using any_is_constructor = meta::any<is_constructor<meta::unqualified_t<Args>>...>;

		template <typename... Args>
		inline constexpr bool any_is_constructor_v = any_is_constructor<Args...>::value;

		template <typename T>
		struct is_destructor : std::false_type { };

		template <typename Fx>
		struct is_destructor<destructor_wrapper<Fx>> : std::true_type { };

		template <typename... Args>
		using any_is_destructor = meta::any<is_destructor<meta::unqualified_t<Args>>...>;

		template <typename... Args>
		inline constexpr bool any_is_destructor_v = any_is_destructor<Args...>::value;
	} // namespace detail

	template <typename T>
	using is_lua_c_function = meta::any<std::is_same<lua_CFunction, T>, std::is_same<detail::lua_CFunction_noexcept, T>, std::is_same<lua_CFunction_ref, T>>;

	template <typename T>
	inline constexpr bool is_lua_c_function_v = is_lua_c_function<T>::value;

	struct automagic_enrollments {
		bool default_constructor = true;
		bool destructor = true;
		bool pairs_operator = true;
		bool to_string_operator = true;
		bool call_operator = true;
		bool less_than_operator = true;
		bool less_than_or_equal_to_operator = true;
		bool length_operator = true;
		bool equal_to_operator = true;
	};


} // namespace sol

#endif // SOL_TYPES_HPP
