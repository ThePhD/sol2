metatable_key
=============
A key for setting and getting an object's metatable
---------------------------------------------------

.. code-block:: cpp

	struct metatable_key_t {};
	const metatable_key_t metatable_key;

You can use this in conjunction with :doc:`sol::table<table>` to set/get a metatable. Lua metatables are powerful ways to override default behavior of objects for various kinds of operators, among other things. Here is an entirely complete example, showing getting and working with a :doc:`usertype<usertype>`'s metatable defined by Sol:

.. code-block:: cpp
	:caption: messing with metatables
	:linenos:

	#include <sol.hpp>

	int main () {

		struct bark {
			int operator()(int x) {
				return x;
			}
		};

		sol::state lua;
		lua.open_libraries(sol::lib::base);

		lua.new_usertype<bark>("bark",
			sol::meta_function::call_function, &bark::operator()
		);

		bark b;
		lua.set("b", &b);

		sol::table b_as_table = lua["b"];		
		sol::table b_metatable = b_as_table[sol::metatable_key];
		sol::function b_call = b_metatable["__call"];
		sol::function b_as_function = lua["b"];

		int result1 = b_as_function(1);
		int result2 = b_call(b, 1);
		// result1 == result2 == 1
	}