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

#ifndef SOL_DUMP_HANDLER_HPP
#define SOL_DUMP_HANDLER_HPP

#include "compatibility.hpp"

#include <cstdint>
#include <exception>

namespace sol {

	class dump_error : public error {
	private:
		int ec_;

	public:
		dump_error(int error_code_) : error("dump returned non-zero error of " + std::to_string(error_code_)), ec_(error_code_) {
		}

		int error_code () const {
			return ec_;
		}
	};

	inline int dump_pass_on_error(lua_State* L, int result_code, lua_Writer writer_function, void* userdata, bool strip) {
		(void)L;
		(void)writer_function;
		(void)userdata;
		(void)strip;
		return result_code;
	}

	inline int dump_throw_on_error(lua_State* L, int result_code, lua_Writer writer_function, void* userdata, bool strip) {
		(void)L;
		(void)writer_function;
		(void)userdata;
		(void)strip;
		throw dump_error(result_code);
	}

	inline int dump_panic_on_error(lua_State* L, int result_code, lua_Writer writer_function, void* userdata, bool strip) {
		(void)L;
		(void)writer_function;
		(void)userdata;
		(void)strip;
		return luaL_error(L, "a non-zero error code (%d) was returned by the lua_Writer for the dump function", result_code);
	}

} // namespace sol

#endif // SOL_DUMP_HANDLER_HPP
