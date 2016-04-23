state
=====
owning and non-owning state holders for registry and globals
------------------------------------------------------------

.. code-block:: cpp

	class state_view;
	class state : state_view, std::unique_ptr<lua_State*, deleter>;

The most important class here is ``state_view``. This structure takes a ``lua_State*`` that was already created and gives you simple, easy access to Lua's interfaces without taking ownership. ``state`` derives from ``state_view``, inheriting all of this functionality, but has the additional purpose of creating a fresh ``lua_State*`` and managing its lifetime for you in the default constructor.

The majority of the members between ``state_view`` and :doc:`sol::table<table>` are identical, with added for this higher-level type. Therefore, all of the examples and notes in :doc:`sol::table<table>` apply here as well.

enumerations
------------

.. code-block:: cpp
	:caption: in-lua libraries
	:name: lib-enum

	enum class lib : char {
	    base,
	    package,
	    coroutine,
	    string,
	    os,
	    math,
	    table,
	    debug,
	    bit32,
	    io,
	    ffi,
	    jit,
	    count // do not use
	};

This enumeration details the various base libraries that come with Lua. See the `standard lua libraries`_ for details about the various standard libraries.


members
-------

.. code-block:: cpp
	:caption: function: open standard libraries/modules
	:name: open-libraries

	template<typename... Args>
	void open_libraries(Args&&... args);

This function takes a number of :ref:`sol::lib<lib-enum>` as arguments and opens up the associated Lua core libraries. 

.. code-block:: cpp
	:caption: function: script / script_file

	void script(const std::string& code);
	void script_file(const std::string& filename);

These functions run the desired blob of either code that is in a string, or code that comes from a filename, on the ``lua_State*``. It will not run isolated: any scripts or code run will affect code in other states as well: code ran in this fashion is not isolated. 

.. code-block:: cpp
	:caption: function: load / load_file
	:name: state-load-code

	sol::load_result load(const std::string& code);
	sol::load_result load_file(const std::string& filename);

These functions *load* the desired blob of either code that is in a string, or code that comes from a filename, on the ``lua_State*``. It will not run: it returns a ``load_result`` proxy that can be called, turned into a `sol::function`,. or similar, will run the loaded code.

.. code-block:: cpp
	:caption: function: global table / registry table

	sol::global_table globals() const;
	sol::table registry() const;

Get either the global table or the Lua registry as a :doc:`sol::table<table>`, which allows you to modify either of them directly. Note that getting the global table from a ``state``/``state_view`` is usually unnecessary as it has all the exact same functions as a :doc:`sol::table<table>` anyhow.


.. code-block:: cpp
	:caption: function: Lua set_panic
	:name: set-panic

	void set_panic(lua_CFunction panic);

Overrides the panic function Lua calls when something unrecoverable or unexpected happens in the Lua VM. Must be a function of the that matches the ``int(*)(lua_State*)`` function signature.

.. code-block:: cpp
	:caption: function: make a table

	sol::table create_table(int narr = 0, int nrec = 0);
	template <typename Key, typename Value, typename... Args>
	sol::table create_table(int narr, int nrec, Key&& key, Value&& value, Args&&... args);
	
	static sol::table create_table(lua_State* L, int narr = 0, int nrec = 0);
	template <typename Key, typename Value, typename... Args>
	static sol::table create_table(lua_State* L, int narr, int nrec, Key&& key, Value&& value, Args&&... args);

Creates a table. Forwards its arguments to :ref:`table::create<table-create>`.

.. _standard lua libraries: http://www.lua.org/manual/5.2/manual.html#6 