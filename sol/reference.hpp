// The MIT License (MIT) 

// Copyright (c) 2013-2017 Rapptz, ThePhD and contributors

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

namespace sol {
	namespace stack {
		inline void remove(lua_State* L, int rawindex, int count) {
			if (count < 1)
				return;
			int top = lua_gettop(L);
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
			push_popper_at(lua_State* luastate, int index = -1, int count = 1) : L(luastate), index(index), count(count) { }
			~push_popper_at() { remove(L, index, count); }
		};

		template <bool top_level>
		struct push_popper_n {
			lua_State* L;
			int t;
			push_popper_n(lua_State* luastate, int x) : L(luastate), t(x) { }
			~push_popper_n() { lua_pop(L, t); }
		};
		template <>
		struct push_popper_n<true> {
			push_popper_n(lua_State*, int) { }
		};
		template <bool, typename T, typename = void>
		struct push_popper {
			T t;
			push_popper(T x) : t(x) { t.push(); }
			~push_popper() { t.pop(); }
		};
		template <typename T, typename C>
		struct push_popper<true, T, C> {
			push_popper(T) {}
			~push_popper() {}
		};
		template <typename T>
		struct push_popper<false, T, std::enable_if_t<std::is_base_of<stack_reference, meta::unqualified_t<T>>::value>> {
			push_popper(T) {}
			~push_popper() {}
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
	} // stack

	namespace detail {
		struct global_tag { } const global_{};
		struct no_safety_tag {} const no_safety{};
	} // detail

	class reference {
	private:
		lua_State* luastate = nullptr; // non-owning
		int ref = LUA_NOREF;

		int copy() const noexcept {
			if (ref == LUA_NOREF)
				return LUA_NOREF;
			push();
			return luaL_ref(lua_state(), LUA_REGISTRYINDEX);
		}

	protected:
		reference(lua_State* L, detail::global_tag) noexcept : luastate(L) {
			lua_pushglobaltable(lua_state());
			ref = luaL_ref(lua_state(), LUA_REGISTRYINDEX);
		}

		int stack_index() const noexcept {
			return -1;
		}

		void deref() const noexcept {
			luaL_unref(lua_state(), LUA_REGISTRYINDEX, ref);
		}

	public:
		reference() noexcept = default;
		reference(lua_nil_t) noexcept : reference() {}
		reference(const stack_reference& r) noexcept : reference(r.lua_state(), r.stack_index()) {}
		reference(stack_reference&& r) noexcept : reference(r.lua_state(), r.stack_index()) {}
		reference(lua_State* L, const reference& r) noexcept : luastate(L) {
			if (r.ref == LUA_NOREF) {
				ref = LUA_NOREF;
				return;
			}
			int p = r.push();
			if (r.lua_state() != luastate) {
				lua_xmove(r.lua_state(), L, p);
			}
			ref = luaL_ref(lua_state(), LUA_REGISTRYINDEX);
		}
		reference(lua_State* L, reference&& r) noexcept : luastate(L) {
			if (r.ref == LUA_NOREF) {
				ref = LUA_NOREF;
				return;
			}
			if (r.lua_state() != luastate) {
				int p = r.push();
				lua_xmove(r.lua_state(), L, p);
				ref = luaL_ref(lua_state(), LUA_REGISTRYINDEX);
			}
			else {
				ref = r.ref;
				r.luastate = nullptr;
				r.ref = LUA_NOREF;
			}
		}
		reference(lua_State* L, const stack_reference& r) noexcept : luastate(L) {
			if (!r.valid()) {
				ref = LUA_NOREF;
				return;
			}
			r.push(luastate);
			ref = luaL_ref(lua_state(), LUA_REGISTRYINDEX);
		}
		reference(lua_State* L, int index = -1) noexcept : luastate(L) {
			lua_pushvalue(lua_state(), index);
			ref = luaL_ref(lua_state(), LUA_REGISTRYINDEX);
		}
		reference(lua_State* L, ref_index index) noexcept : luastate(L) {
			lua_rawgeti(L, LUA_REGISTRYINDEX, index.index);
			ref = luaL_ref(lua_state(), LUA_REGISTRYINDEX);
		}
		reference(lua_State* L, lua_nil_t) noexcept : luastate(L) {}

		~reference() noexcept {
			deref();
		}

		reference(reference&& o) noexcept {
			luastate = o.luastate;
			ref = o.ref;

			o.luastate = nullptr;
			o.ref = LUA_NOREF;
		}

		reference& operator=(reference&& o) noexcept {
			if (valid()) {
				deref();
			}
			luastate = o.luastate;
			ref = o.ref;

			o.luastate = nullptr;
			o.ref = LUA_NOREF;

			return *this;
		}

		reference(const reference& o) noexcept {
			luastate = o.luastate;
			ref = o.copy();
		}

		reference& operator=(const reference& o) noexcept {
			luastate = o.luastate;
			deref();
			ref = o.copy();
			return *this;
		}

		int push() const noexcept {
			return push(lua_state());
		}

		int push(lua_State* Ls) const noexcept {
			if (lua_state() == nullptr) {
				lua_pushnil(Ls);
				return 1;
			}
			lua_rawgeti(lua_state(), LUA_REGISTRYINDEX, ref);
			if (Ls != lua_state()) {
				lua_xmove(lua_state(), Ls, 1);
			}
			return 1;
		}

		void pop() const noexcept {
			pop(lua_state());
		}

		void pop(lua_State* Ls, int n = 1) const noexcept {
			lua_pop(Ls, n);
		}

		int registry_index() const noexcept {
			return ref;
		}

		bool valid() const noexcept {
			return !(ref == LUA_NOREF || ref == LUA_REFNIL);
		}

		explicit operator bool() const noexcept {
			return valid();
		}

		type get_type() const noexcept {
			auto pp = stack::push_pop(*this);
			int result = lua_type(lua_state(), -1);
			return static_cast<type>(result);
		}

		lua_State* lua_state() const noexcept {
			return luastate;
		}
	};

	inline bool operator== (const reference& l, const reference& r) {
		auto ppl = stack::push_pop(l);
		auto ppr = stack::push_pop(r);
		return lua_compare(l.lua_state(), -1, -2, LUA_OPEQ) == 1;
	}

	inline bool operator!= (const reference& l, const reference& r) {
		return !operator==(l, r);
	}

	inline bool operator==(const reference& lhs, const lua_nil_t&) {
		return !lhs.valid();
	}

	inline bool operator==(const lua_nil_t&, const reference& rhs) {
		return !rhs.valid();
	}

	inline bool operator!=(const reference& lhs, const lua_nil_t&) {
		return lhs.valid();
	}

	inline bool operator!=(const lua_nil_t&, const reference& rhs) {
		return rhs.valid();
	}
} // sol

#endif // SOL_REFERENCE_HPP
