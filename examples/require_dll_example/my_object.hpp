#pragma once

#include "my_object_api.hpp"

namespace my_object {

	struct test {
		int value;

		test() = default;
		test(int val) : value(val) {}
	};

} // my_object

// this function needs to be exported from your
// dll. "extern 'C'" should do the trick, but
// we're including platform-specific stuff here to help
// see my_object_api.hpp for details
extern "C" MY_OBJECT_API int luaopen_my_object(lua_State* L);
