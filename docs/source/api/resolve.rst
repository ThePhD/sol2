resolve
=======
utility to pick overloaded C++ function calls
---------------------------------------------

.. code-block:: cpp
	:caption: function: resolve C++ overload

	template <typename... Args, typename F>
	auto resolve( F f );

``resolve`` is a function that is meant to help users pick a single function out of a group of overloaded functions in C++. You can use it to pick overloads by specifying the signature as the first template argument. Given a collection of overloaded functions:

.. code-block:: cpp
	:linenos:

	int overloaded(int x);
	int overloaded(int x, int y);
	int overloaded(int x, int y, int z);

You can disambiguate them using ``resolve``:

..  code-block:: cpp
	:linenos:

	auto single = resolve<int(int)>( overloaded );

Note that this resolution is also built into ``set_function`` on :doc:`table<table>` and :doc:`state<state>`:

..  code-block:: cpp
	:linenos:

	sol::state lua;

	lua.set_function<int(int)>("overloaded", overloaded);
	lua.set_function<int(int, int)>("overloaded", overloaded);
	lua.set_function<int(int, int, int)>("overloaded", overloaded);
