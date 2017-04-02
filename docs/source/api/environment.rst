environment
===========
encapsulation table for script sandboxing
-----------------------------------------

.. code-block:: cpp
	:caption: environment

	class environment : public table;

	void set_environment( const reference& target, const environment& env);


This type is passed to :ref:`sol::state(_view)::script/do_x<state-script-function>` to provide an environment where local variables that are set and get retrieve. It is just a plain table, and all the same operations :doc:`from table still apply<table>`. This is important because it allows you to do things like set the table's metatable (using :doc:`sol::metatable_key<metatable_key>` for instance) and having its ``__index`` entry point to the global table, meaning you can get -- but not set -- variables from a Global environment.

Please see `an example demonstrating the above and more`_ for further information.

members
-------

.. code-block:: cpp
	:caption: constructor: environment

	environment(lua_State* L, sol::new_table nt, const sol::reference& fallback);

The ones from table are used here (of particular note is the ability to use ``sol::environment(my_lua_state, sol::create);`` to make a fresh, unnamed environment), plus the one unique constructor shown above. It is generally used as ``sol::environmeny my_env(my_lua_state, sol::create, my_fallback_table);``. The fallback table serves as the backup to lookup attempts on the environment table being created. It is achieved by simply creating a metatable for the ``sol::environment`` being created, and then doing ``env_metatable["__index"] = fallback;``. You can achieve fancier effects by changing the metatable of the environment to your liking, by creating it in some fashion and then setting the metatable explicitly and populating it with data, particularly with :doc:`sol::metatable_key<metatable_key>`.

.. an example demonstrating the above and more: 
