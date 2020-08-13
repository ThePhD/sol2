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

#ifndef SOL_STACK_REFERENCE_HPP
#define SOL_STACK_REFERENCE_HPP

#include <sol/types.hpp>

namespace sol {
	namespace detail {
		inline bool xmovable(lua_State* leftL, lua_State* rightL) {
			if (rightL == nullptr || leftL == nullptr || leftL == rightL) {
				return false;
			}
			const void* leftregistry = lua_topointer(leftL, LUA_REGISTRYINDEX);
			const void* rightregistry = lua_topointer(rightL, LUA_REGISTRYINDEX);
			return leftregistry == rightregistry;
		}
	} // namespace detail

	class stateless_stack_reference {
	private:
		friend class stack_reference;
		
		int index = 0;

		int registry_index() const noexcept {
			return LUA_NOREF;
		}

	public:
		stateless_stack_reference() noexcept = default;
		stateless_stack_reference(lua_nil_t) noexcept : stateless_stack_reference(){};
		stateless_stack_reference(lua_State* L, int i) noexcept : stateless_stack_reference(absolute_index(L, i)) {
		}
		stateless_stack_reference(lua_State*, absolute_index i) noexcept : stateless_stack_reference(i) {
		}
		stateless_stack_reference(lua_State*, raw_index i) noexcept : stateless_stack_reference(i) {
		}
		stateless_stack_reference(absolute_index i) noexcept : index(i) {
		}
		stateless_stack_reference(raw_index i) noexcept : index(i) {
		}
		stateless_stack_reference(lua_State*, ref_index) noexcept = delete;
		stateless_stack_reference(ref_index) noexcept = delete;
		stateless_stack_reference(const reference&) noexcept = delete;
		stateless_stack_reference(const stateless_stack_reference&) noexcept = default;
		stateless_stack_reference(stateless_stack_reference&& o) noexcept = default;
		stateless_stack_reference& operator=(stateless_stack_reference&&) noexcept = default;
		stateless_stack_reference& operator=(const stateless_stack_reference&) noexcept = default;

		int push(lua_State* L) const noexcept {
#if SOL_IS_ON(SOL_SAFE_STACK_CHECK_I_)
			luaL_checkstack(L, 1, "not enough Lua stack space to push a single reference value");
#endif // make sure stack doesn't overflow
			lua_pushvalue(L, index);
			return 1;
		}

		void pop(lua_State* L, int n = 1) const noexcept {
			lua_pop(L, n);
		}

		int stack_index() const noexcept {
			return index;
		}

		const void* pointer(lua_State* L) const noexcept {
			const void* vp = lua_topointer(L, stack_index());
			return vp;
		}

		type get_type(lua_State* L) const noexcept {
			int result = lua_type(L, index);
			return static_cast<type>(result);
		}

		bool valid(lua_State* L) const noexcept {
			type t = get_type(L);
			return t != type::lua_nil && t != type::none;
		}

		void abandon(lua_State* = nullptr) {
			index = 0;
		}
	};

	class stack_reference : public stateless_stack_reference {
	private:
		lua_State* luastate = nullptr;

	public:
		stack_reference() noexcept = default;
		stack_reference(lua_nil_t) noexcept
		: stack_reference() {};
		stack_reference(lua_State* L, lua_nil_t) noexcept : stateless_stack_reference(L, 0), luastate(L) {
		}
		stack_reference(lua_State* L, int i) noexcept : stateless_stack_reference(L, i), luastate(L) {
		}
		stack_reference(lua_State* L, absolute_index i) noexcept : stateless_stack_reference(L, i), luastate(L) {
		}
		stack_reference(lua_State* L, raw_index i) noexcept : stateless_stack_reference(L, i), luastate(L) {
		}
		stack_reference(lua_State* L, ref_index i) noexcept = delete;
		stack_reference(lua_State* L, const reference& r) noexcept = delete;
		stack_reference(lua_State* L, const stack_reference& r) noexcept
		: luastate(L) {
			if (!r.valid()) {
				index = 0;
				return;
			}
			int i = r.stack_index();
			if (detail::xmovable(lua_state(), r.lua_state())) {
#if SOL_IS_ON(SOL_SAFE_STACK_CHECK_I_)
				luaL_checkstack(L, 1, "not enough Lua stack space to push a single reference value");
#endif // make sure stack doesn't overflow
				lua_pushvalue(r.lua_state(), r.index);
				lua_xmove(r.lua_state(), luastate, 1);
				i = absolute_index(luastate, -1);
			}
			index = i;
		}
		stack_reference(stack_reference&& o) noexcept = default;
		stack_reference& operator=(stack_reference&&) noexcept = default;
		stack_reference(const stack_reference&) noexcept = default;
		stack_reference& operator=(const stack_reference&) noexcept = default;

		int push() const noexcept {
			return push(lua_state());
		}

		int push(lua_State* Ls) const noexcept {
			return stateless_stack_reference::push(Ls);
		}

		void pop() const noexcept {
			pop(lua_state());
		}

		void pop(lua_State* Ls, int n = 1) const noexcept {
			stateless_stack_reference::pop(Ls, n);
		}

		const void* pointer() const noexcept {
			return stateless_stack_reference::pointer(lua_state());
		}

		type get_type() const noexcept {
			return stateless_stack_reference::get_type(lua_state());
		}

		lua_State* lua_state() const noexcept {
			return luastate;
		}

		bool valid() const noexcept {
			return stateless_stack_reference::valid(lua_state());
		}

		void abandon () {
			stateless_stack_reference::abandon(lua_state());
		}
	};

	inline bool operator==(const stack_reference& l, const stack_reference& r) {
		return lua_compare(l.lua_state(), l.stack_index(), r.stack_index(), LUA_OPEQ) == 0;
	}

	inline bool operator!=(const stack_reference& l, const stack_reference& r) {
		return !operator==(l, r);
	}

	inline bool operator==(const stack_reference& lhs, const lua_nil_t&) {
		return !lhs.valid();
	}

	inline bool operator==(const lua_nil_t&, const stack_reference& rhs) {
		return !rhs.valid();
	}

	inline bool operator!=(const stack_reference& lhs, const lua_nil_t&) {
		return lhs.valid();
	}

	inline bool operator!=(const lua_nil_t&, const stack_reference& rhs) {
		return rhs.valid();
	}

	struct stack_reference_equals {
		bool operator()(const lua_nil_t& lhs, const stack_reference& rhs) const {
			return lhs == rhs;
		}

		bool operator()(const stack_reference& lhs, const lua_nil_t& rhs) const {
			return lhs == rhs;
		}

		bool operator()(const stack_reference& lhs, const stack_reference& rhs) const {
			return lhs == rhs;
		}
	};

	struct stack_reference_hash {
		typedef stack_reference argument_type;
		typedef std::size_t result_type;

		result_type operator()(const argument_type& lhs) const {
			std::hash<const void*> h;
			return h(lhs.pointer());
		}
	};
} // namespace sol

#endif // SOL_STACK_REFERENCE_HPP
