as_function
===========
make sure an object is pushed as a function
-------------------------------------------

.. code-block:: cpp
	
	template <typename Sig = sol::function_sig<>, typename... Args>
	function_argumants<Sig, Args...> as_function ( Args&& ... );

This function serves the purpose of ensuring that a callable struct (like a lambda) can be passed to the ``set( key, value )`` calls on :ref:`sol::table<set-value>` and be treated like a function binding instead of a userdata. It is recommended that one uses the :ref:`sol::table::set_function<set-function>` call instead, but if for some reason one must use `set`, then `as_function` can help ensure a callable struct is handled like a lambda / callable, and not as just a userdata structure.

.. code-block:: cpp

	struct callable {
		int operator()( int a, bool b ) {
			return a + b ? 10 : 20;
		}
	};


	sol::state lua;
	// Binds struct as userdata
	lua.set( "not_func", callable() );
	// Binds struct as function
	lua.set( "func", sol::as_function( callable() ) );
	// equivalent: lua.set_function( "func", callable() );
	// equivalent: lua["func"] = callable();

Note that if you actually want a userdata, but you want it to be callable, you simply need to create a :ref:`sol::table::new_usertype<new-usertype>` and then bind the ``"__call"`` metamethod (or just use ``sol::meta_function::call`` :ref:`enumeration<meta_function_enum>`).