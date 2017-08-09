overload
========
*calling different functions based on argument number/type*


.. code-block:: cpp
	:caption: function: create overloaded set
	:linenos:

	template <typename... Args>
	struct overloaded_set : std::tuple<Args...> { /* ... */ };

	template <typename... Args>
	overloaded_set<Args...> overload( Args&&... args );

The actual class produced by ``sol::overload`` is essentially a type-wrapper around ``std::tuple`` that signals to the library that an overload is being created. The function helps users make overloaded functions that can be called from Lua using 1 name but multiple arguments. It is meant to replace the spaghetti of code where users mock this up by doing strange if statements and switches on what version of a function to call based on `luaL_check{number/udata/string}`_.

.. note::

	Please note that default parameters in a function (e.g., ``int func(int a = 20)``) do not exist beyond C++'s compile-time fun. When that function gets bound or serialized into Lua's framework, it is bound as a function taking 1 argument, not 2 functions taking either 0 or 1 argument. If you want to achieve the same effect, then you need to use overloading and explicitly call the version of the function you want. There is no magic in C++ that allows me to retrieve default parameters and set this up automatically.


Its use is simple: wherever you can pass a function type to Lua, whether its on a :doc:`usertype<usertype>` or if you are just setting any kind of function with ``set`` or ``set_function`` (for :doc:`table<table>` or :doc:`state(_view)<state>`), simply wrap up the functions you wish to be considered for overload resolution on one function like so:

.. code-block:: cpp
	
	sol::overload( func1, func2, ... funcN );


The functions can be any kind of function / function object (lambda). Given these functions and struct:

.. code-block:: cpp
	:linenos:

	struct pup {
		int barks = 0;

		void bark () {
			++barks; // bark!
		}

		bool is_cute () const { 
			return true;
		}
	};

	void ultra_bark( pup& p, int barks) {
		for (; barks --> 0;) p.bark();
	}

	void picky_bark( pup& p, std::string s) {
		if ( s == "bark" )
		    p.bark();
	}

You then use it just like you would for any other part of the api:

.. code-block:: cpp
	:linenos:

	sol::state lua;

	lua.set_function( "bark", sol::overload( 
		ultra_bark, 
		[]() { return "the bark from nowhere"; } 
	) );

	lua.new_usertype<pup>( "pup",
		// regular function
		"is_cute", &pup::is_cute,
		// overloaded function
		"bark", sol::overload( &pup::bark, &picky_bark )
	);

Thusly, doing the following in Lua:

.. code-block:: Lua
	:caption: pup.lua
	:linenos:

	local barker = pup.new()
	pup:bark() -- calls member function pup::bark
	pup:bark(20) -- calls ultra_bark
	pup:bark("meow") -- picky_bark, no bark
	pup:bark("bark") -- picky_bark, bark

	bark(pup, 20) -- calls ultra_bark
	local nowherebark = bark() -- calls lambda which returns that string

.. note::

	Overloading is done on a first-come, first-serve system. This means if two overloads are compatible, workable overloads, it will choose the first one in the list.

Note that because of this system, you can use :doc:`sol::variadic_args<variadic_args>` to make a function that serves as a "fallback". Be sure that it is the last specified function in the listed functions for ``sol::overload( ... )``. `This example shows how`_.

.. note::

	Please keep in mind that doing this bears a runtime cost to find the proper overload. The cost scales directly not exactly with the number of overloads, but the number of functions that have the same argument count as each other (Sol will early-eliminate any functions that do not match the argument count).

.. _luaL_check{number/udata/string}: http://www.Lua.org/manual/5.3/manual.html#luaL_checkinteger
.. _This example shows how: https://github.com/ThePhD/sol2/blob/develop/examples/overloading_with_fallback.cpp
