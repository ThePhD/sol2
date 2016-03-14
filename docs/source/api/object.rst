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