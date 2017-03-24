variadic_args
=============
transparent argument to deal with multiple parameters to a function
-------------------------------------------------------------------


.. code-block:: cpp

	struct variadic_args;

This class is meant to represent every single argument at its current index and beyond in a function list. It does not increment the argument count and is thus transparent. You can place it anywhere in the argument list, and it will represent all of the objects in a function call that come after it, whether they are listed explicitly or not.

``variadic_args`` also has ``begin()`` and ``end()`` functions that return (almost) random-acess iterators. These return a proxy type that can be implicitly converted, much like the :doc:`table proxy type<proxy>`.

.. code-block:: cpp
	:linenos:

	#include <sol.hpp>

	int main () {
		
		sol::state lua;
		
		// Function requires 2 arguments
		// rest can be variadic, but:
		// va will include everything after "a" argument,
		// which means "b" will be part of the varaidic_args list too
		// at position 0
		lua.set_function("v", [](int a, sol::variadic_args va, int b) {
			int r = 0;
			for (auto v : va) {
				int value = v; // get argument out (implicit conversion)
				// can also do int v = va.get<int>(i); with index i
				r += value;
			}
			// Only have to add a, b was included
			return r + a;
		});
	    
		lua.script("x = v(25, 25)");
		lua.script("x2 = v(25, 25, 100, 50, 250, 150)");
		lua.script("x3 = v(1, 2, 3, 4, 5, 6)");
		// will error: not enough arguments
		//lua.script("x4 = v(1)");
		
		lua.script("print(x)"); // 50
		lua.script("print(x2)"); // 600
		lua.script("print(x3)"); // 21
	}

You can also "save" arguments and the like later, by stuffing them into a ``std::vector<sol::object>`` or something similar that pulls out all the arguments. Below is an example of saving all of the arguments provided by ``sol::variadic_args`` in a lambda capture variable called ``args``.

.. code-block:: cpp
	:linenos:

	#include "sol.hpp"
	#include <functional>

	std::function<void()> function_storage;

	void store_routine(const sol::function& f, const sol::variadic_args& va) {
		function_storage = [=, args = std::vector<sol::object>(va.begin(), va.end())]() {
			f(sol::as_args(args));
		};
	}

	int main() {
		sol::state lua;
		lua.open_libraries(sol::lib::base);
		lua.set_function("store_routine", &store_routine);
		lua.script(R"(
	function a(name)
		print(name)
	end
	store_routine(a, "some name")
	)");
		function_storage();

		lua.script(R"(
	function b(number, text)
		print(number, "of", text)
	end
	store_routine(b, 20, "these apples")
	)");
		function_storage();
	}

