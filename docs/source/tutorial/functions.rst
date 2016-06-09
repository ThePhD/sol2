functions and You
=================

Sol can register all kinds of functions. Many are shown in the :doc:`quick 'n' dirty<all-the-things>`, but here we will discuss many of the additional ways you can register functions into a sol-wrapped Lua system.

Setting a new function
----------------------

Given a C++ function, you can drop it into Sol in several equivalent ways, working similar to how :ref:`setting variables<writing-main-cpp>`

.. code-block:: cpp
	:linenos:
	:caption: Registering C++ functions
	:name: writing-functions

	std::string my_function( int a, std::string b ) {
		// Create a string with the letter 'D' "a" times,
		// append it to 'b'
		return b + std::string( 'D', a ); 
	}

	int main () {

		sol::state lua;

		lua["my_func"] = my_function;

	}