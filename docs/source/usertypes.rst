usertypes
=========

Perhaps the most powerful feature of sol2, ``usertypes`` are the way sol2 and C++ communicate your classes to the Lua runtime and bind things between both tables and to specific blocks of C++ memory, allowing you to treat Lua userdata and other things like classes. 

To learn more about usertypes, visit:

* :doc:`the basic tutorial<tutorial/cxx-in-lua>`
* :doc:`customization point tutorial<tutorial/customization>`
* :doc:`api documentation<api/usertype>`
* :doc:`memory documentation<api/usertype_memory>`

The examples folder also has a number of really great examples for you to see. There are also some notes about guarantees you can find about usertypes, and their associated userdata, below:

* All usertypes are runtime extensible in both `Lua`_ and `C++`_
* Please note that the semi-colon is necessary to "automatically" pass the ``this``/``self`` argument to Lua methods
	- ``obj:method_name()`` is how you call "member" methods in Lua
	- It is purely syntactic sugar that passes the object name as the first argument to the ``method_name`` function
	- ``my_obj:foo(bar, baz)`` is the same as ``my_obj.foo(my_obj, bar, baz)``
	- **Please note** that one uses a semi-colon, and the other uses a dot, and forgetting to do this properly will crash your code
	- There are safety defines outlined in the :ref:`safety page here<config>`
* You can push types classified as userdata before you register a usertype.
	- You can register a usertype with the Lua runtime at any time sol2
	- You can retrieve them from the Lua runtime as well through sol2
	- Methods and properties will be added to the type only after you register the usertype with the Lua runtime
	- All methods and properties will appear on all userdata, even if that object was pushed before the usertype (all userdata will be updated)
* Types either copy once or move once into the memory location, if it is a value type. If it is a pointer, we store only the reference.
	- This means retrieval of class types (not primitive types like strings or integers) by ``T&`` or ``T*`` allow you to modify the data in Lua directly.
	- Retrieve a plain ``T`` to get a copy
	- Return types and passing arguments to ``sol::function``-types use perfect forwarding and reference semantics, which means no copies happen unless you specify a value explicitly. See :ref:`this note for details<function-argument-handling>`.
* The first ``sizeof( void* )`` bytes is always a pointer to the typed C++ memory. What comes after is based on what you've pushed into the system according to :doc:`the memory specification for usertypes<api/usertype_memory>`. This is compatible with a number of systems other than just sol2, making it easy to interop with select other Lua systems.
* Member methods, properties, variables and functions taking ``self&`` arguments modify data directly
	- Work on a copy by taking or returning a copy by value.
* The actual metatable associated with the usertype has a long name and is defined to be opaque by the Sol implementation.
* The actual metatable inner workings is opaque and defined by the Sol implementation, and there are no internal docs because optimizations on the operations are applied based on heuristics we discover from performance testing the system.
* Containers get pushed as special usertypes, but can be disabled if problems arise as detailed :doc:`here<api/containers>`.
* You can use bitfields but it requires some finesse on your part. We have an example to help you get started `here, that uses a few tricks`_.

.. _here, that uses a few tricks: https://github.com/ThePhD/sol2/blob/develop/examples/usertype_bitfields.cpp
.. _Lua: https://github.com/ThePhD/sol2/blob/develop/examples/usertype_advanced.cpp#L81
.. _C++: https://github.com/ThePhD/sol2/blob/develop/examples/usertype_simple.cpp#L51