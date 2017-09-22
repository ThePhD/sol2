simple_usertype<T>
==================
*structures and classes from C++ made available to Lua code (simpler)*


This usertype is no difference from :doc:`regular usertype<usertype>`, but allows much of its work to be done at runtime instead of compile-time. You can reduce compilation times from a plain ``usertype`` when you have an exceedingly bulky registration listing.

You can set functions incrementally to reduce compile-time burden with ``simple_usertype`` as well, as shown in `this example`_. This means both adding incrementally during registration and even adding at runt=time.

You can add functions to both regular and simple usertypes afterwards by adding items to the metatable directly at runtime (e.g., by accessing the named metatable yourself and setting functions on it).

.. note::

	You cannot add functions to an individual object. You can only add functions to the whole class / usertype.

Some developers used ``simple_usertype`` in older versions to have variables automatically be functions. To achieve this behavior, wrap the desired variable into :doc:`sol::as_function<as_function>`.

The performance `seems to be good enough`_ (see below graphs as well) to not warn about any implications of having to serialize things at runtime. You do run the risk of using (slightly?) more memory, since variables and functions need to be stored differently and separately from the metatable data itself like with a regular ``usertype``. The goal here was to avoid compiler complaints about too-large usertypes (some individuals needed to register 190+ functions, and the compiler choked from the templated implementation of ``usertype``). As of Sol 2.14, this implementation has been heavily refactored to allow for all the same syntax and uses of usertype to apply here, with no caveats/exceptions.


.. image:: /media/bench/lua_bench_graph_member_function_calls_(simple).png
	:target: https://raw.githubusercontent.com/ThePhD/lua-bench/master/lua%20-%20results/lua_bench_graph_member_function_calls_(simple).png
	:alt: bind several member functions to an object and call them in Lua code


.. image:: /media/bench/lua_bench_graph_userdata_variable_access_(simple).png
	:target: https://raw.githubusercontent.com/ThePhD/lua-bench/master/lua%20-%20results/lua_bench_graph_userdata_variable_access_(simple).png
	:alt: bind a member variable to an object and modify it with Lua code


.. image:: /media/bench/lua_bench_graph_many_userdata_variables_access_last_registered_(simple).png
	:target: https://raw.githubusercontent.com/ThePhD/lua-bench/master/lua%20-%20results/lua_bench_graph_many_userdata_variables_access_last_registered_(simple).png
	:alt: bind MANY member variables to an object and modify it with Lua code



.. _seems to be good enough: https://github.com/ThePhD/sol2/issues/202#issuecomment-246767629
.. _this example: https://github.com/ThePhD/sol2/blob/develop/examples/usertype_simple.cpp
