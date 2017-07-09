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

#ifndef SOL_FORWARD_HPP
#define SOL_FORWARD_HPP

#include "feature_test.hpp"

namespace sol {

	class reference;
	class stack_reference;
	template <typename Table, typename Key>
	struct proxy;
	template<typename T>
	class usertype;
	template<typename T>
	class simple_usertype;
	template <bool, typename T>
	class basic_table_core;
	template <bool b>
	using table_core = basic_table_core<b, reference>;
	template <bool b>
	using stack_table_core = basic_table_core<b, stack_reference>;
	template <typename T>
	using basic_table = basic_table_core<false, T>;
	typedef table_core<false> table;
	typedef table_core<true> global_table;
	typedef stack_table_core<false> stack_table;
	typedef stack_table_core<true> stack_global_table;
	template <typename base_t>
	struct basic_environment;
	using environment = basic_environment<reference>;
	using stack_environment = basic_environment<stack_reference>;
	template <typename T>
	class basic_function;
	template <typename T>
	class basic_protected_function;
	using protected_function = basic_protected_function<reference>;
	using stack_protected_function = basic_protected_function<stack_reference>;
	using unsafe_function = basic_function<reference>;
	using safe_function = basic_protected_function<reference>;
	using stack_unsafe_function = basic_function<stack_reference>;
	using stack_safe_function = basic_protected_function<stack_reference>;
#ifdef SOL_SAFE_FUNCTIONS
	using function = protected_function;
	using stack_function = stack_protected_function;
#else
	using function = unsafe_function;
	using stack_function = stack_unsafe_function;
#endif
	template <typename base_t>
	class basic_object;
	template <typename base_t>
	class basic_userdata;
	template <typename base_t>
	class basic_lightuserdata;
	struct variadic_args;
	using object = basic_object<reference>;
	using stack_object = basic_object<stack_reference>;
	using userdata = basic_userdata<reference>;
	using stack_userdata = basic_userdata<stack_reference>;
	using lightuserdata = basic_lightuserdata<reference>;
	using stack_lightuserdata = basic_lightuserdata<stack_reference>;
	class coroutine;
	class thread;
	struct variadic_args;
	struct variadic_results;
	struct this_state;
	struct this_environment;
	template <typename T>
	struct light;
	template <typename T>
	struct user;
	template <typename T>
	struct as_args_t;

} // sol

#endif // SOL_FORWARD_HPP
