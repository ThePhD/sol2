benchmarks
==========
because somebody is going to ask eventually...
----------------------------------------------


Here are measurements of the *overhead that libraries impose around the Lua C API*: that is, the cost of abstracting / wrapping the plain Lua C API. Measurements are (at the time of writing) done with all libraries compiled against a DLL version of Lua 5.3.3 to make sure each Lua call has the same overhead between libraries (no Link Time Optimizations or other static library optimizations).

These are some informal and formal benchmarks done by both the developers of sol and other library developers / users. We leave you to interpret the data as you see fit.

* `lua_binding_benchmarks`_ by satoren (developer of `kaguya`_) (`Sol`_ is the "sol2" entry)
* `lua-bench`_ by ThePhD (developer of `Sol`_)

As of the writing of this documentation (August 8th, 2017), :doc:`Sol<index>` (Sol2) seems to take the cake in most categories for speed! Below are some graphs from `lua-bench`_. You can read the benchmarking code there if you think something was done wrong, and submit a pull requests or comment on something to make sure that ThePhD is being honest about his work. All categories are the performance of things described at the top of the :doc:`feature table<features>`.

Note that Sol here makes use of its more performant variants (see :doc:`c_call<api/c_call>` and others), and ThePhD also does his best to make use of the most performant variants for other frameworks by disabling type checks where possible as well (Thanks to Liam Devine of OOLua for explaining how to turn off type checks in OOLua).

Bars go up to the average execution time. Lower is better. Reported times are for the desired operation run through `nonius`_. Results are sorted from top to bottom by best to worst. Note that there are error bars to show potential variance in performance: generally, same-sized errors bars plus very close average execution time implies no significant difference in speed, despite the vastly different abstraction techniques used.

.. image:: /media/bench/lua_bench_graph_member_function_calls.png
	:target: https://raw.githubusercontent.com/ThePhD/lua-bench/master/lua%20-%20results/lua_bench_graph_member_function_calls.png
	:alt: bind several member functions to an object and call them in Lua code

.. image:: /media/bench/lua_bench_graph_userdata_variable_access.png
	:target: https://raw.githubusercontent.com/ThePhD/lua-bench/master/lua%20-%20results/lua_bench_graph_userdata_variable_access.png
	:alt: bind a variable to an object and call it in Lua code

.. image:: /media/bench/lua_bench_graph_many_userdata_variables_access.png
	:target: https://raw.githubusercontent.com/ThePhD/lua-bench/master/lua%20-%20results/lua_bench_graph_many_userdata_variables_access.png
	:alt: bind MANY variables to an object and call it in Lua code

.. image:: /media/bench/lua_bench_graph_c_function_through_lua.png
	:target: https://raw.githubusercontent.com/ThePhD/lua-bench/master/lua%20-%20results/lua_bench_graph_c_function_through_lua.png
	:alt: retrieve a C function bound in Lua and call it from C++

.. image:: /media/bench/lua_bench_graph_stateful_c_function.png
	:target: https://raw.githubusercontent.com/ThePhD/lua-bench/master/lua%20-%20results/lua_bench_graph_stateful_c_function.png
	:alt: bind a stateful C function (e.g., a mutable lambda), retrieve it, and call it from C++

.. image:: /media/bench/lua_bench_graph_c_function.png
	:target: https://raw.githubusercontent.com/ThePhD/lua-bench/master/lua%20-%20results/lua_bench_graph_c_function.png
	:alt: call a C function through Lua code

.. image:: /media/bench/lua_bench_graph_lua_function.png
	:target: https://raw.githubusercontent.com/ThePhD/lua-bench/master/lua%20-%20results/lua_bench_graph_lua_function.png
	:alt: retrieve a plain Lua function and call it from C++

.. image:: /media/bench/lua_bench_graph_multi_return.png
	:target: https://raw.githubusercontent.com/ThePhD/lua-bench/master/lua%20-%20results/lua_bench_graph_multi_return.png
	:alt: return mutliple objects from C++ using std::tuple or through a library-defined mechanism

.. image:: /media/bench/lua_bench_graph_global_get.png
	:target: https://raw.githubusercontent.com/ThePhD/lua-bench/master/lua%20-%20results/lua_bench_graph_global_get.png
	:alt: retrieve a value from the global table

.. image:: /media/bench/lua_bench_graph_global_set.png
	:target: https://raw.githubusercontent.com/ThePhD/lua-bench/master/lua%20-%20results/lua_bench_graph_global_set.png
	:alt: set a value into the global table

.. image:: /media/bench/lua_bench_graph_table_chained_get.png
	:target: https://raw.githubusercontent.com/ThePhD/lua-bench/master/lua%20-%20results/lua_bench_graph_table_chained_get.png
	:alt: measures the cost of doing consecutive lookups into a table that exists from C++; some libraries fail here because they do not do lazy evaluation or chaining properly

.. image:: /media/bench/lua_bench_graph_table_get.png
	:target: https://raw.githubusercontent.com/ThePhD/lua-bench/master/lua%20-%20results/lua_bench_graph_table_get.png
	:alt: measures the cost of retrieving a value from a table in C++; this nests 1 level so as to not be equivalent to any measured global table get optimzations

.. image:: /media/bench/lua_bench_graph_table_chained_set.png
	:target: https://raw.githubusercontent.com/ThePhD/lua-bench/master/lua%20-%20results/lua_bench_graph_table_chained_set.png
	:alt: measures the cost of doing consecutive lookups into a table that exists from C++ and setting the final value; some libraries fail here because they do not do lazy evaluation or chaining properly

.. image:: /media/bench/lua_bench_graph_table_set.png
	:target: https://raw.githubusercontent.com/ThePhD/lua-bench/master/lua%20-%20results/lua_bench_graph_table_set.png
	:alt: measures the cost of setting a value into a table in C++; this nests 1 level so as to not be equivalent to any measured global table set optimzations

.. image:: /media/bench/lua_bench_graph_return_userdata.png
	:target: https://raw.githubusercontent.com/ThePhD/lua-bench/master/lua%20-%20results/lua_bench_graph_return_userdata.png
	:alt: bind a C function which returns a custom class by-value and call it through Lua code

.. image:: /media/bench/lua_bench_graph_get_optional.png
	:target: https://raw.githubusercontent.com/ThePhD/lua-bench/master/lua%20-%20results/lua_bench_graph_get_optional.png
	:alt: retrieve an item from a table that does not exist in Lua and check for its existence (testing the cost of the failure case)

.. image:: /media/bench/lua_bench_graph_base_from_derived.png
	:target: https://raw.githubusercontent.com/ThePhD/lua-bench/master/lua%20-%20results/lua_bench_graph_base_from_derived.png
	:alt: retrieve base class pointer out of Lua without knowing exact derived at compile-time, and have it be correct for multiple-inheritance



.. _lua-bench: https://github.com/ThePhD/lua-bench
.. _lua_binding_benchmarks: http://satoren.github.io/lua_binding_benchmark/
.. _kaguya: https://github.com/satoren/kaguya
.. _Sol: https://github.com/ThePhD/sol2
.. _nonius: https://github.com/rmartinho/nonius/
