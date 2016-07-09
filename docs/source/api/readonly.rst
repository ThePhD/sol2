readonly
========
Routine to mark a member variable as read-only
----------------------------------------------

.. code-block:: cpp
	
	template <typename T>
	auto readonly( T&& value );

The goal of read-only is to protect a variable set on a usertype or set as a function into Lua. Simply wrap it around a ``&my_class::my_member_variable`` in the appropriate place to use it.
