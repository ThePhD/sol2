variables
=========

Working with variables is easy with Sol, and behaves pretty much like any associative array / map structure you've dealt with previously. Given this lua file that gets loaded into Sol:

reading
-------

.. code-block:: lua
	:caption: variables.lua

	config = {
		fullscreen = false,
		resolution = { x = 1024, y = 768 }
	}

.. code-block:: cpp
	:caption: main.cpp

	int main () {

		sol::state lua;
		lua.script_file( variables.lua );

		return 0;
	}

You can interact with the variables like this:

.. code-block:: cpp
	:caption: main.cpp extended

	#include <utility> // for std::pair

	int main () {

		sol::state lua;
		lua.script_file( variables.lua );

		// the type "state" behaves exactly like a table!
		bool isfullscreen = lua["config"]["fullscreen"]; // can get nested variables
		sol::table config = lua["config"];
		
		// can also get it using the "get" member function
		// auto replaces the unqualified type name
		auto resolution = config.get<sol::table>( "config" );

		// table and state can have multiple things pulled out of it too
		std::pair<int, int> xyresolution = resolution.get<int, int>( "x", "y" );
		// As an example, you can also pull out a tuple as well
		// std::tuple<int, int> xyresolution = resolution.get<int, int>( "x", "y" );


		return 0;
	}

From this example, you can see that there's many ways to pull out the varaibles you want. Some can be more safe than others. For example, to determine if a nested variable exists or not, you can use ``auto`` to capture the value of a ``table[key]`` lookup, and then use the ``.valid()`` method:

.. code-block:: cpp
	:caption: safe lookup

	auto bark = lua["config"]["bark"];
	if (bark.valid()) {
		// branch not taken: config / bark is not a variable
	}
	else {
		// Branch taken: config is a not a variable
	}

This comes in handy when you want to check if a nested variable exists. You can also check if a toplevel variable is present or not by using ``sol::optional``, which also checks if the type you're trying to get is of a specific type:

.. code-block:: cpp
	:caption: optional lookup

	sol::optional<int> not_an_integer = lua["config"]["fullscreen"];
	if (not_an_integer) {
		// Branch not taken: value is not an integer
	}

	sol::optoinal<bool> is_a_boolean = lua["config"]["fullscreen"];
	if (is_a_boolean) {
		// Branch taken: the value is a boolean
	}

	sol::optional<double> does_not_exist = lua["not_a_variable"];
	if (does_not_exist) {
		// Branch not taken: that variable is not present
	}

