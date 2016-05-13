object
======
general-purpose safety reference to an existing object
------------------------------------------------------

.. code-block:: cpp
	
	class object : reference;


``object``'s goal is to allow someone to pass around the most generic form of a reference to something in Lua (or propogate a ``nil``). It is the logical extension of :doc:`sol::reference<reference>`, and is used in :ref:`sol::table's iterators<table-iterators>`.


members
-------

.. code-block:: cpp
    :caption: overloaded constructor: object

    template <typename T>
    object(T&&);
    object(lua_State* L, int index = -1);

There are 2 kinds of constructors here. One allows construction of a object from other reference types such as :doc:`table<table>` and :doc:`stack_reference`. The other creates an object which references the specific element at the given index in the specified ``lua_State*``.

.. code-block:: cpp
	:caption: function: type conversion
	
	template<typename T>
	decltype(auto) as() const;

Performs a cast of the item this reference refers to into the type ``T`` and returns it. It obeys the same rules as :ref:`sol::stack::get\<T><getter>`.

.. code-block:: cpp
	:caption: function: type check
	
	template<typename T>
	bool is() const;

Performs a type check using the :ref:`sol::stack::check<checker>` api, after checking if the reference is valid.


non-members
-----------

.. code-block:: cpp
	:caption: functions: nil comparators

	bool operator==(const object& lhs, const nil_t&);
	bool operator==(const nil_t&, const object& rhs);
	bool operator!=(const object& lhs, const nil_t&);
	bool operator!=(const nil_t&, const object& rhs);

These allow a person to compare an ``sol::object`` against :ref:`nil<nil>`, which essentially checks if an object references a non-nil value, like so:

.. code-block:: cpp

	if (myobj == sol::nil) {
		// doesn't have anything...
	}

Use this to check objects.

.. code-block:: cpp
	:caption: function: make object
	:name: make-object

	template <typename T>
	object make_object(lua_State* L, T&& value);
	template <typename T, typename... Args>
	object make_object(lua_State* L, Args&&... args);

Makes an object out of the value. It pushes it onto the stack, then pops it into the returned ``sol::object``.