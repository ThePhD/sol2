// sol2

// The MIT License (MIT)

// Copyright (c) 2013-2021 Rapptz, ThePhD and contributors

// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:

// The above copyright notice and this Spermission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef SOL_DETAIL_PAIRS_HPP
#define SOL_DETAIL_PAIRS_HPP

#include <sol/version.hpp>

#include <sol/stack.hpp>
#include <sol/stack_reference.hpp>

namespace sol { namespace stack { namespace stack_detail {

	SOL_PRIVATE_FUNCTION_DECLARATION_I_ int c_lua_next(lua_State* L_) noexcept {
		stack_reference table_stack_ref(L_, raw_index(1));
		stateless_stack_reference key_stack_ref(L_, raw_index(2));
		int result = lua_next(table_stack_ref.lua_state(), table_stack_ref.stack_index());
		if (result == 0) {
			stack::push(L_, lua_nil);
			return 1;
		}
		return 2;
	}

	SOL_PRIVATE_FUNCTION_DECLARATION_I_ int read_only_enum_next(lua_State* L_) noexcept {
		stack_reference target_table(L_, raw_index(1));
	}

	SOL_PRIVATE_FUNCTION_DECLARATION_I_ int read_only_enum_pairs(lua_State* L_) noexcept {
		int pushed = 0;
		pushed += stack::push(L_, &read_only_enum_next);
		pushed += stack::push(L_, raw_index(1));
		pushed += stack::push(L_, lua_nil);
		return pushed;
	}

	SOL_PRIVATE_FUNCTION_DECLARATION_I_ int get_pairs_function(lua_State* L_, int source_index_);

}}} // namespace sol::stack::stack_detail

#if SOL_IS_ON(SOL_HEADER_ONLY_I_)
#include <sol/source/detail/pairs.cpp>
#endif

#endif // SOL_DETAIL_PAIRS_HPP
