containers
==========
*for handling ``std::vector/map/set`` and others*


Sol2 automatically converts containers (detected using the ``sol::is_container<T>`` type trait, which simply looks for ``begin`` / ``end``) to be a special kind of userdata with metatable on it. For Lua 5.2 and 5.3, this is extremely helpful as you can make typical containers behave like Lua tables without losing the actual C++ container that they came from, as well as a small amount of indexing and other operations that behave properly given the table type.

An overview of these traits and additional information can be found :doc:`at the top level container page<../containers>`.

If you need to deal with these things from and in Lua to be **actual**, true-blue, Lua tables, please consider :doc:`sol::as_table<as_table>` and :doc:`sol::nested<nested>` for serialization and deserialization into and out of the VM with sol2 operations. You can also force something that is marked as not-a-container by using :doc:`sol::as_container<as_container>` for sol2 to push it as a userdata with the aforementioned special usertype metatable into Lua.

.. note::

	Overriding the detection traits and operation traits listed :doc:`on the top level container page<../containers>` and then trying to use ``sol::as_table`` or similar can result in compilation failures if you do not have a proper ``begin()`` or ``end()`` function on the type. If you want things to behave with special usertype considerations, please do not wrap the container in one of the special table-forcing abstractions.


a complete example
------------------

Here's a complete working example of it working for Lua 5.3 and Lua 5.2, and how you can retrieve out the container in all versions:

.. code-block:: cpp
	:caption: containers.cpp

	#define SOL_CHECK_ARGUMENTS
	#include <sol.hpp>

	int main() {
		sol::state lua;
		lua.open_libraries();

		lua.script(R"(
		function f (x)
			print('--- Calling f ---')
			for k, v in pairs(x) do
				print(k, v)
			end
		end
		)");

		// Have the function we 
		// just defined in Lua
		sol::function f = lua["f"];

		// Set a global variable called 
		// "arr" to be a vector of 5 lements
		lua["arr"] = std::vector<int>{ 2, 4, 6, 8, 10 };
		
		// Call it, see 5 elements
		// printed out
		f(lua["arr"]);

		// Mess with it in C++
		std::vector<int>& reference_to_arr = lua["arr"];
		reference_to_arr.push_back(12);

		// Call it, see *6* elements
		// printed out
		f(lua["arr"]);

		return 0;
	}


Note that this will not work well in Lua 5.1, as it has explicit table checks and does not check metamethods, even when ``pairs`` or ``ipairs`` is passed a table. In that case, you will need to use a more manual iteration scheme or you will have to convert it to a table. In C++, you can use :doc:`sol::as_table<as_table>` when passing something to the library to get a table out of it: ``lua["arr"] = as_table( std::vector<int>{ ... });``. For manual iteration in Lua code without using ``as_table`` for something with indices, try:

.. code-block:: lua
	:caption: iteration.lua

	for i = 1, #vec do
		print(i, vec[i]) 
	end

There are also other ways to iterate over key/values, but they can be difficult AND cost your performance due to not having proper support in Lua 5.1. We recommend that you upgrade to Lua 5.2 or 5.3 if this is integral to your infrastructure.

