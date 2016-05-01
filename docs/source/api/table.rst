table
=====
a representation of a Lua (meta)table
-------------------------------------

.. code-block:: cpp
	
	template <bool global>
	class table_core;

	typedef table_core<false> table;
	typedef table_core<true> global_table;

``sol::table`` is an extremely efficient manipulator of state that brings most of the magic of the Sol abstraction. Capable of doing multiple sets at once, multiple gets into a ``std::tuple``, being indexed into using ``[key]`` syntax and setting keys with a similar syntax (see: :doc:`here<proxy>`), ``sol::table`` is the corner of the interaction between Lua and C++.

There are two kinds of tables: the global table and non-global tables: however, both have the exact same interface and all ``sol::global_table`` s are convertible to regular ``sol::table`` s.

Tables are the core of Lua, and they are very much the core of Sol.


members
-------

.. code-block:: cpp
	:caption: constructor: table

	table(lua_State* L, int index = -1);

Takes a table from the Lua stack at the specified index and allows a person to use all of the abstractions therein.

.. code-block:: cpp
	:caption: function: get / traversing get

	template<typename... Args, typename... Keys>
	decltype(auto) get(Keys&&... keys) const;

	template<typename T, typename... Keys>
	decltype(auto) traverse_get(Keys&&... keys) const;

	template<typename T, typename Key>
	decltype(auto) get_or(Key&& key, T&& otherwise) const;

	template<typename T, typename Key, typename D>
	decltype(auto) get_or(Key&& key, D&& otherwise) const;


These functions retrieve items from the table. The first one (``get``) can pull out *multiple* values, 1 for each key value passed into the function. In the case of multiple return values, it is returned in a ``std::tuple<Args...>``. It is similar to doing ``return table["a"], table["b"], table["c"]``. Because it returns a ``std::tuple``, you can use ``std::tie``/``std::make_tuple`` on a multi-get to retrieve all of the necessary variables. The second one (``traverse_get``) pulls out a *single* value,	using each successive key provided to do another lookup into the last. It is similar to doing ``x = table["a"]["b"]["c"][...]``.

If the keys within nested queries try to traverse into a table that doesn't exist, the second lookup into the nil-returned variable and belong will cause a panic to be fired by the lua C API. If you need to check for keys, check with ``auto x = table.get<sol::optional<int>>( std::tie("a", "b", "c" ) );``, and then use the :doc:`optional<optional>` interface to check for errors. As a short-hand, easy method for returning a default if a value doesn't exist, you can use ``get_or`` instead.

.. note::

	Value semantics are applied to all set operations. If you do not ``std::ref( obj )`` or specifically make a pointer with ``std::addressof( obj )`` or ``&obj``, it will copy / move. This is different from how :doc:`sol::function<function>` behaves with its call operator.

.. code-block:: cpp
	:caption: function: set / traversing set

	template<typename... Args>
	table& set(Args&&... args);

	template<typename... Args>
	table& traverse_set(Args&&... args);

These functions set items into the table. The first one (``set``) can set  *multiple* values, in the form ``Key, Value, Key, Value, ...``. It is similar to ``table["a"] = 1; table["b"] = 2, ...``. The second one (``traverse_set``) sets a *single* value, using all but the last argument as keys to do another lookup into the value retrieved prior to it. It is equivalent to ``table["a"]["b"][...] = final_value;``.

.. note::

	Value semantics are applied to all set operations. If you do not ``std::ref( obj )`` or specifically make a pointer with ``std::addressof( obj )`` or ``&obj``, it will copy / move. This is different from how :doc:`sol::function<function>` behaves with its call operator.
	
.. code-block:: cpp
	:caption: function: setting a usertype
	:name: new-usertype

	template<typename Class, typename... Args>
	table& new_usertype(const std::string& name, Args&&... args);
	template<typename Class, typename CTor0, typename... CTor, typename... Args>
	table& new_usertype(const std::string& name, Args&&... args);
	template<typename Class, typename... CArgs, typename... Args>
	table& new_usertype(const std::string& name, constructors<CArgs...> ctor, Args&&... args);

This class of functions creates a new :doc:`usertype<usertype>` with the specified arguments, providing a few extra details for constructors. After creating a usertype with the specified argument, it passes it to :ref:`set_usertype<set_usertype>`.

.. _set_usertype:

.. code-block:: cpp
	:caption: function: setting a pre-created usertype
	:name: set-usertype

	template<typename T>
	table& set_usertype(usertype<T>& user);
	template<typename Key, typename T>
	table& set_usertype(Key&& key, usertype<T>& user);

Sets a previously created usertype with the specified ``key`` into the table. Note that if you do not specify a key, the implementation falls back to setting the usertype with a ``key`` of ``usertype_traits<T>::name``, which is an implementation-defined name that tends to be of the form ``{namespace_name 1}_[{namespace_name 2 ...}_{class name}``.

.. code-block:: cpp
	:caption: function: begin / end for iteration
	:name: table-iterators

	table_iterator begin () const;
	table_iterator end() const;
	table_iterator cbegin() const;
	table_iterator cend() const;

Provides `input iterators`_ for a table. This allows tables to work with single-pass, input-only algorithms (like ``std::for_each``).

.. code-block:: cpp
	:caption: function: iteration with a function
	:name: table-for-each

	template <typename Fx>
	void for_each(Fx&& fx);

A functional ``for_each`` loop that calls the desired function. The passed in function must take either ``sol::object key, sol::object value`` or take a ``std::pair<sol::object, sol::object> key_value_pair``. This version can be a bit safer as allows the implementation to definitively pop the key/value off the Lua stack after each call of the function.

.. code-block:: cpp
	:caption: function: operator[] access

	template<typename T>
	proxy<table&, T> operator[](T&& key);
	template<typename T>
	proxy<const table&, T> operator[](T&& key) const;

Generates a :doc:`proxy<proxy>` that is templated on the table type and the key type. Enables lookup of items and their implicit conversion to a desired type.

.. code-block:: cpp
	:caption: function: set a function with the specified key into lua

	template<typename Key, typename Fx>
	state_view& set_function(Key&& key, Fx&& fx, [...]);

Sets the desired function to the specified key value. Note that it also allows for passing a member function plus a member object or just a single member function: however, using a lambda is almost always better when you want to bind a member function + class instance to a single function call in Lua.

.. code-block:: cpp
	:caption: function: create a table with defaults
	:name: table-create

	table create(int narr = 0, int nrec = 0);
	template <typename Key, typename Value, typename... Args>
	table create(int narr, int nrec, Key&& key, Value&& value, Args&&... args);
	
	static table create(lua_State* L, int narr = 0, int nrec = 0);
	template <typename Key, typename Value, typename... Args>
	static table create(lua_State* L, int narr, int nrec, Key&& key, Value&& value, Args&&... args);

Creates a table, optionally with the specified values pre-set into the table. If ``narr`` or ``nrec`` are 0, then compile-time shenanigans are used to guess the amount of array entries (e.g., integer keys) and the amount of hashable entries (e.g., all other entries).

.. code-block:: cpp
	:caption: function: create a table with compile-time defaults assumed
	:name: table-create-with

	template <typename... Args>
	table create_with(Args&&... args);
	template <typename... Args>
	static table create_with(lua_State* L, Args&&... args);
	

Creates a table, optionally with the specified values pre-set into the table. It checks every 2nd argument (the keys) and generates hints for how many array or map-style entries will be placed into the table.

.. code-block:: cpp
	:caption: function: create a named table with compile-time defaults assumed
	:name: table-create-named

	template <typename Name, typename... Args>
	table create_named(Name&& name, Args&&... args);
	

Creates a table, optionally with the specified values pre-set into the table, and sets it as the key ``name`` in the table.

.. _input iterators: http://en.cppreference.com/w/cpp/concept/InputIterator