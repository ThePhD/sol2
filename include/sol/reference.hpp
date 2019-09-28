// sol3

// The MIT License (MIT)

// Copyright (c) 2013-2019 Rapptz, ThePhD and contributors

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

#ifndef SOL_REFERENCE_HPP
#define SOL_REFERENCE_HPP

#include "types.hpp"
#include "stack_reference.hpp"

#include <functional>

namespace sol {
	namespace detail {
		inline const char (&default_main_thread_name())[9] {
			static const char name[9] = "sol.\xF0\x9F\x93\x8C";
			return name;
		}
	} // namespace detail

	namespace stack {
		inline void remove(lua_State* L, int rawindex, int count) {
			if (count < 1)
				return;
			int top = lua_gettop(L);
			if (top < 1) {
				return;
			}
			if (rawindex == -count || top == rawindex) {
				// Slice them right off the top
				lua_pop(L, static_cast<int>(count));
				return;
			}

			// Remove each item one at a time using stack operations
			// Probably slower, maybe, haven't benchmarked,
			// but necessary
			int index = lua_absindex(L, rawindex);
			if (index < 0) {
				index = lua_gettop(L) + (index + 1);
			}
			int last = index + count;
			for (int i = index; i < last; ++i) {
				lua_remove(L, index);
			}
		}

		struct push_popper_at {
			lua_State* L;
			int index;
			int count;
			push_popper_at(lua_State* luastate, int index = -1, int count = 1)
			: L(luastate), index(index), count(count) {
			}
			~push_popper_at() {
				remove(L, index, count);
			}
		};

		template <bool top_level>
		struct push_popper_n {
			lua_State* L;
			int t;
			push_popper_n(lua_State* luastate, int x)
			: L(luastate), t(x) {
			}
			push_popper_n(const push_popper_n&) = delete;
			push_popper_n(push_popper_n&&) = default;
			push_popper_n& operator=(const push_popper_n&) = delete;
			push_popper_n& operator=(push_popper_n&&) = default;
			~push_popper_n() {
				lua_pop(L, t);
			}
		};
		
		template <>
		struct push_popper_n<true> {
			push_popper_n(lua_State*, int) {
			}
		};

		template <bool, typename T, typename = void>
		struct push_popper {
			using Tu = meta::unqualified_t<T>;
			T t;
			int idx;

			push_popper(T x)
			: t(x), idx(lua_absindex(t.lua_state(), -t.push())) {
				
			}

			int index_of(const Tu&) {
				return idx;
			}

			~push_popper() {
				t.pop();
			}
		};

		template <typename T, typename C>
		struct push_popper<true, T, C> {
			using Tu = meta::unqualified_t<T>;

			push_popper(T) {
			}

			int index_of(const Tu&) {
				return -1;
			}

			~push_popper() {
			}
		};
		
		template <typename T>
		struct push_popper<false, T, std::enable_if_t<is_stack_based_v<meta::unqualified_t<T>>>> {
			using Tu = meta::unqualified_t<T>;
			
			push_popper(T) {
			}

			int index_of(const Tu& r) {
				return r.stack_index();
			}

			~push_popper() {
			}
		};

		template <bool top_level = false, typename T>
		push_popper<top_level, T> push_pop(T&& x) {
			return push_popper<top_level, T>(std::forward<T>(x));
		}

		template <typename T>
		push_popper_at push_pop_at(T&& x) {
			int c = x.push();
			lua_State* L = x.lua_state();
			return push_popper_at(L, lua_absindex(L, -c), c);
		}
		
		template <bool top_level = false>
		push_popper_n<top_level> pop_n(lua_State* L, int x) {
			return push_popper_n<top_level>(L, x);
		}
	} // namespace stack

	inline lua_State* main_thread(lua_State* L, lua_State* backup_if_unsupported = nullptr) {
#if SOL_LUA_VERSION < 502
		if (L == nullptr)
			return backup_if_unsupported;
		lua_getglobal(L, detail::default_main_thread_name());
		auto pp = stack::pop_n(L, 1);
		if (type_of(L, -1) == type::thread) {
			return lua_tothread(L, -1);
		}
		return backup_if_unsupported;
#else
		if (L == nullptr)
			return backup_if_unsupported;
		lua_rawgeti(L, LUA_REGISTRYINDEX, LUA_RIDX_MAINTHREAD);
		lua_State* Lmain = lua_tothread(L, -1);
		lua_pop(L, 1);
		return Lmain;
#endif // Lua 5.2+ has the main thread unqualified_getter
	}

	namespace detail {
		struct global_tag {
		} const global_ {};
		struct no_safety_tag {
		} const no_safety {};

		template <bool b>
		inline lua_State* pick_main_thread(lua_State* L, lua_State* backup_if_unsupported = nullptr) {
			(void)L;
			(void)backup_if_unsupported;
			if (b) {
				return main_thread(L, backup_if_unsupported);
			}
			return L;
		}
	} // namespace detail

	class stateless_reference {
	private:
		template <bool o_main_only>
		friend class basic_reference;

		int ref = LUA_NOREF;

		int copy(lua_State* L) const noexcept {
			if (ref == LUA_NOREF)
				return LUA_NOREF;
			push(L);
			return luaL_ref(L, LUA_REGISTRYINDEX);
		}

		lua_State* copy_assign(lua_State* L, lua_State* rL, const stateless_reference& r) {
			if (valid(L)) {
				deref(L);
			}
			ref = r.copy(L);
			return rL;
		}

		lua_State* move_assign(lua_State* L, lua_State* rL, stateless_reference&& r) {
			if (valid(L)) {
				deref(L);
			}
			ref = r.ref;
			r.ref = LUA_NOREF;
			return rL;
		}

	protected:
		int stack_index () const noexcept {
			return -1;
		}

		stateless_reference(lua_State* L, detail::global_tag) noexcept {
#if defined(SOL_SAFE_STACK_CHECK) && SOL_SAFE_STACK_CHECK
			luaL_checkstack(L, 1, "not enough Lua stack space to push this reference value");
#endif // make sure stack doesn't overflow
			lua_pushglobaltable(L);
			ref = luaL_ref(L, LUA_REGISTRYINDEX);
		}

		stateless_reference(int raw_ref_index) noexcept : ref(raw_ref_index) {
		}

	public:
		stateless_reference() noexcept = default;
		stateless_reference(lua_nil_t) noexcept : stateless_reference() {
		}
		stateless_reference(const stack_reference& r) noexcept : stateless_reference(r.lua_state(), r.stack_index()) {
		}
		stateless_reference(stack_reference&& r) noexcept : stateless_reference(r.lua_state(), r.stack_index()) {
		}
		stateless_reference(lua_State* L, const stateless_reference& r) noexcept {
			if (r.ref == LUA_REFNIL) {
				ref = LUA_REFNIL;
				return;
			}
			if (r.ref == LUA_NOREF || L == nullptr) {
				ref = LUA_NOREF;
				return;
			}
			ref = r.copy(L);
		}

		stateless_reference(lua_State* L, stateless_reference&& r) noexcept {
			if (r.ref == LUA_REFNIL) {
				ref = LUA_REFNIL;
				return;
			}
			if (r.ref == LUA_NOREF || L == nullptr) {
				ref = LUA_NOREF;
				return;
			}
			ref = r.ref;
			r.ref = LUA_NOREF;
		}

		stateless_reference(lua_State* L, const stack_reference& r) noexcept {
			if (L == nullptr || r.lua_state() == nullptr || r.get_type() == type::none) {
				ref = LUA_NOREF;
				return;
			}
			if (r.get_type() == type::lua_nil) {
				ref = LUA_REFNIL;
				return;
			}
			if (L != r.lua_state() && !detail::xmovable(L, r.lua_state())) {
				return;
			}
			r.push(L);
			ref = luaL_ref(L, LUA_REGISTRYINDEX);
		}

		stateless_reference(lua_State* L, int index = -1) noexcept {
			// use L to stick with that state's execution stack
#if defined(SOL_SAFE_STACK_CHECK) && SOL_SAFE_STACK_CHECK
			luaL_checkstack(L, 1, "not enough Lua stack space to push this reference value");
#endif // make sure stack doesn't overflow
			lua_pushvalue(L, index);
			ref = luaL_ref(L, LUA_REGISTRYINDEX);
		}
		stateless_reference(lua_State* L, ref_index index) noexcept {
			lua_rawgeti(L, LUA_REGISTRYINDEX, index.index);
			ref = luaL_ref(L, LUA_REGISTRYINDEX);
		}
		stateless_reference(lua_State*, lua_nil_t) noexcept {
		}

		~stateless_reference() noexcept = default;

		stateless_reference(const stateless_reference& o) noexcept = delete;
		stateless_reference& operator=(const stateless_reference& r) noexcept = delete;

		stateless_reference(stateless_reference&& o) noexcept
		: ref(o.ref) {
			o.ref = LUA_NOREF;
		}

		stateless_reference& operator=(stateless_reference&& o) noexcept {
			ref = o.ref;
			o.ref = LUA_NOREF;
			return *this;
		}


		int push(lua_State* L) const noexcept {
#if defined(SOL_SAFE_STACK_CHECK) && SOL_SAFE_STACK_CHECK
			luaL_checkstack(L, 1, "not enough Lua stack space to push this reference value");
#endif // make sure stack doesn't overflow
			lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
			return 1;
		}

		void pop(lua_State* L, int n = 1) const noexcept {
			lua_pop(L, n);
		}

		int registry_index() const noexcept {
			return ref;
		}

		bool valid(lua_State*) const noexcept {
			return !(ref == LUA_NOREF || ref == LUA_REFNIL);
		}

		const void* pointer(lua_State* L) const noexcept {
			int si = push(L);
			const void* vp = lua_topointer(L, -si);
			lua_pop(L, si);
			return vp;
		}

		type get_type(lua_State* L) const noexcept {
			int p = push(L);
			int result = lua_type(L, -1);
			pop(L, p);
			return static_cast<type>(result);
		}

		void abandon(lua_State* = nullptr) {
			ref = LUA_NOREF;
		}

		void deref(lua_State* L) const noexcept {
			luaL_unref(L, LUA_REGISTRYINDEX, ref);
		}
	};

	template <bool main_only = false>
	class basic_reference : public stateless_reference {
	private:
		template <bool o_main_only>
		friend class basic_reference;
		lua_State* luastate = nullptr; // non-owning

		template <bool r_main_only>
		void copy_assign(const basic_reference<r_main_only>& r) {
			if (valid()) {
				deref();
			}
			if (r.ref == LUA_REFNIL) {
				luastate = detail::pick_main_thread < main_only && !r_main_only > (r.lua_state(), r.lua_state());
				ref = LUA_REFNIL;
				return;
			}
			if (r.ref == LUA_NOREF) {
				luastate = r.luastate;
				ref = LUA_NOREF;
				return;
			}
			if (detail::xmovable(lua_state(), r.lua_state())) {
				r.push(lua_state());
				ref = luaL_ref(lua_state(), LUA_REGISTRYINDEX);
				return;
			}
			luastate = detail::pick_main_thread < main_only && !r_main_only > (r.lua_state(), r.lua_state());
			ref = r.copy();
		}

		template <bool r_main_only>
		void move_assign(basic_reference<r_main_only>&& r) {
			if (valid()) {
				deref();
			}
			if (r.ref == LUA_REFNIL) {
				luastate = detail::pick_main_thread < main_only && !r_main_only > (r.lua_state(), r.lua_state());
				ref = LUA_REFNIL;
				return;
			}
			if (r.ref == LUA_NOREF) {
				luastate = r.luastate;
				ref = LUA_NOREF;
				return;
			}
			if (detail::xmovable(lua_state(), r.lua_state())) {
				r.push(lua_state());
				ref = luaL_ref(lua_state(), LUA_REGISTRYINDEX);
				return;
			}

			luastate = detail::pick_main_thread < main_only && !r_main_only > (r.lua_state(), r.lua_state());
			ref = r.ref;
			r.ref = LUA_NOREF;
			r.luastate = nullptr;
		}

	protected:
		basic_reference(lua_State* L, detail::global_tag) noexcept
		: basic_reference(detail::pick_main_thread<main_only>(L, L), detail::global_, detail::global_) {
		}

		basic_reference(lua_State* L, detail::global_tag, detail::global_tag) noexcept
		: stateless_reference(L, detail::global_), luastate(L) {
		}

		basic_reference(lua_State* oL, const basic_reference<!main_only>& o) noexcept : stateless_reference(oL, o), luastate(oL) {
		}

		void deref() const noexcept {
			return stateless_reference::deref(lua_state());
		}

		int copy() const noexcept {
			return copy(lua_state());
		}

		int copy(lua_State* L) const noexcept {
			return stateless_reference::copy(L);
		}

	public:
		basic_reference() noexcept = default;
		basic_reference(lua_nil_t) noexcept
		: basic_reference() {
		}
		basic_reference(const stack_reference& r) noexcept
		: basic_reference(r.lua_state(), r.stack_index()) {
		}
		basic_reference(stack_reference&& r) noexcept
		: basic_reference(r.lua_state(), r.stack_index()) {
		}
		template <bool r_main_only>
		basic_reference(lua_State* L, const basic_reference<r_main_only>& r) noexcept
		: luastate(detail::pick_main_thread<main_only>(L, L)) {
			if (r.ref == LUA_REFNIL) {
				ref = LUA_REFNIL;
				return;
			}
			if (r.ref == LUA_NOREF || lua_state() == nullptr) {
				ref = LUA_NOREF;
				return;
			}
			if (detail::xmovable(lua_state(), r.lua_state())) {
				r.push(lua_state());
				ref = luaL_ref(lua_state(), LUA_REGISTRYINDEX);
				return;
			}
			ref = r.copy();
		}

		template <bool r_main_only>
		basic_reference(lua_State* L, basic_reference<r_main_only>&& r) noexcept
		: luastate(detail::pick_main_thread<main_only>(L, L)) {
			if (r.ref == LUA_REFNIL) {
				ref = LUA_REFNIL;
				return;
			}
			if (r.ref == LUA_NOREF || lua_state() == nullptr) {
				ref = LUA_NOREF;
				return;
			}
			if (detail::xmovable(lua_state(), r.lua_state())) {
				r.push(lua_state());
				ref = luaL_ref(lua_state(), LUA_REGISTRYINDEX);
				return;
			}
			ref = r.ref;
			r.ref = LUA_NOREF;
			r.luastate = nullptr;
		}

		basic_reference(lua_State* L, const stack_reference& r) noexcept
		: luastate(detail::pick_main_thread<main_only>(L, L)) {
			if (lua_state() == nullptr || r.lua_state() == nullptr || r.get_type() == type::none) {
				ref = LUA_NOREF;
				return;
			}
			if (r.get_type() == type::lua_nil) {
				ref = LUA_REFNIL;
				return;
			}
			if (lua_state() != r.lua_state() && !detail::xmovable(lua_state(), r.lua_state())) {
				return;
			}
			r.push(lua_state());
			ref = luaL_ref(lua_state(), LUA_REGISTRYINDEX);
		}
		basic_reference(lua_State* L, int index = -1) noexcept
		: luastate(detail::pick_main_thread<main_only>(L, L)) {
			// use L to stick with that state's execution stack
#if defined(SOL_SAFE_STACK_CHECK) && SOL_SAFE_STACK_CHECK
			luaL_checkstack(L, 1, "not enough Lua stack space to push this reference value");
#endif // make sure stack doesn't overflow
			lua_pushvalue(L, index);
			ref = luaL_ref(L, LUA_REGISTRYINDEX);
		}
		basic_reference(lua_State* L, ref_index index) noexcept
		: luastate(detail::pick_main_thread<main_only>(L, L)) {
			lua_rawgeti(lua_state(), LUA_REGISTRYINDEX, index.index);
			ref = luaL_ref(lua_state(), LUA_REGISTRYINDEX);
		}
		basic_reference(lua_State* L, lua_nil_t) noexcept
		: luastate(detail::pick_main_thread<main_only>(L, L)) {
		}

		~basic_reference() noexcept {
			if (lua_state() == nullptr || ref == LUA_NOREF)
				return;
			deref();
		}

		basic_reference(const basic_reference& o) noexcept
		: stateless_reference(o.copy()), luastate(o.lua_state()) {
		}

		basic_reference(basic_reference&& o) noexcept
		: stateless_reference(std::move(o)), luastate(o.lua_state()) {
			o.luastate = nullptr;
		}

		basic_reference(const basic_reference<!main_only>& o) noexcept
		: basic_reference(detail::pick_main_thread<main_only>(o.lua_state(), o.lua_state()), o) {
		}

		basic_reference(basic_reference<!main_only>&& o) noexcept
		: stateless_reference(std::move(o)), luastate(detail::pick_main_thread<main_only>(o.lua_state(), o.lua_state())) {
			o.luastate = nullptr;
			o.ref = LUA_NOREF;
		}

		basic_reference& operator=(basic_reference&& r) noexcept {
			move_assign(std::move(r));
			return *this;
		}

		basic_reference& operator=(const basic_reference& r) noexcept {
			copy_assign(r);
			return *this;
		}

		basic_reference& operator=(basic_reference<!main_only>&& r) noexcept {
			move_assign(std::move(r));
			return *this;
		}

		basic_reference& operator=(const basic_reference<!main_only>& r) noexcept {
			copy_assign(r);
			return *this;
		}

		basic_reference& operator=(const lua_nil_t&) noexcept {
			if (valid()) {
				deref();
			}
			luastate = nullptr;
			ref = LUA_NOREF;
			return *this;
		}

		template <typename Super>
		basic_reference& operator=(proxy_base<Super>&& r);

		template <typename Super>
		basic_reference& operator=(const proxy_base<Super>& r);

		int push() const noexcept {
			return push(lua_state());
		}

		int push(lua_State* L) const noexcept {
#if defined(SOL_SAFE_STACK_CHECK) && SOL_SAFE_STACK_CHECK
			luaL_checkstack(L, 1, "not enough Lua stack space to push this reference value");
#endif // make sure stack doesn't overflow
			if (lua_state() == nullptr) {
				lua_pushnil(L);
				return 1;
			}
			lua_rawgeti(lua_state(), LUA_REGISTRYINDEX, ref);
			if (L != lua_state()) {
				lua_xmove(lua_state(), L, 1);
			}
			return 1;
		}

		void pop() const noexcept {
			pop(lua_state());
		}

		void pop(lua_State* L, int n = 1) const noexcept {
			stateless_reference::pop(L, n);
		}

		int registry_index() const noexcept {
			return stateless_reference::registry_index();
		}

		bool valid() const noexcept {
			return stateless_reference::valid(lua_state());
		}

		const void* pointer() const noexcept {
			return stateless_reference::pointer(lua_state());
		}

		explicit operator bool() const noexcept {
			return valid();
		}

		type get_type() const noexcept {
			return stateless_reference::get_type(lua_state());
		}

		lua_State* lua_state() const noexcept {
			return luastate;
		}
	};

	template <bool lb, bool rb>
	inline bool operator==(const basic_reference<lb>& l, const basic_reference<rb>& r) {
		auto ppl = stack::push_pop(l);
		auto ppr = stack::push_pop(r);
		return lua_compare(l.lua_state(), -1, -2, LUA_OPEQ) == 1;
	}

	template <bool lb, bool rb>
	inline bool operator!=(const basic_reference<lb>& l, const basic_reference<rb>& r) {
		return !operator==(l, r);
	}

	template <bool lb>
	inline bool operator==(const basic_reference<lb>& l, const stack_reference& r) {
		auto ppl = stack::push_pop(l);
		return lua_compare(l.lua_state(), -1, r.stack_index(), LUA_OPEQ) == 1;
	}

	template <bool lb>
	inline bool operator!=(const basic_reference<lb>& l, const stack_reference& r) {
		return !operator==(l, r);
	}

	template <bool rb>
	inline bool operator==(const stack_reference& l, const basic_reference<rb>& r) {
		auto ppr = stack::push_pop(r);
		return lua_compare(l.lua_state(), -1, r.stack_index(), LUA_OPEQ) == 1;
	}

	template <bool rb>
	inline bool operator!=(const stack_reference& l, const basic_reference<rb>& r) {
		return !operator==(l, r);
	}

	template <bool lb>
	inline bool operator==(const basic_reference<lb>& lhs, const lua_nil_t&) {
		return !lhs.valid();
	}

	template <bool rb>
	inline bool operator==(const lua_nil_t&, const basic_reference<rb>& rhs) {
		return !rhs.valid();
	}

	template <bool lb>
	inline bool operator!=(const basic_reference<lb>& lhs, const lua_nil_t&) {
		return lhs.valid();
	}

	template <bool rb>
	inline bool operator!=(const lua_nil_t&, const basic_reference<rb>& rhs) {
		return rhs.valid();
	}

	struct reference_equals : public stack_reference_equals {
		template <bool rb>
		bool operator()(const lua_nil_t& lhs, const basic_reference<rb>& rhs) const {
			return lhs == rhs;
		}

		template <bool lb>
		bool operator()(const basic_reference<lb>& lhs, const lua_nil_t& rhs) const {
			return lhs == rhs;
		}

		template <bool lb, bool rb>
		bool operator()(const basic_reference<lb>& lhs, const basic_reference<rb>& rhs) const {
			return lhs == rhs;
		}

		template <bool lb>
		bool operator()(const basic_reference<lb>& lhs, const stack_reference& rhs) const {
			return lhs == rhs;
		}

		template <bool rb>
		bool operator()(const stack_reference& lhs, const basic_reference<rb>& rhs) const {
			return lhs == rhs;
		}
	};

	struct reference_hash : public stack_reference_hash {
		typedef reference argument_type;
		typedef std::size_t result_type;

		template <bool lb>
		result_type operator()(const basic_reference<lb>& lhs) const {
			std::hash<const void*> h;
			return h(lhs.pointer());
		}
	};
} // namespace sol

#endif // SOL_REFERENCE_HPP
