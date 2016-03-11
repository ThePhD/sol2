function
========
calling functions bound to Lua
------------------------------

.. code-block:: cpp
	
	class function : public reference;

Function is a correct-assuming version of :doc:`protected_function<protected_function>`, omitting the need for typechecks and error handling. It is the default function type of Sol. When called without the return types being specified by either a ``sol::types<...>`` list or a ``call<Ret...>( ... )`` template type list, it generates a :doc:`function_result<proxy>` class that gets implicitly converted to the requested return type. For example:

.. code-block:: lua
	:caption: func_barks.lua
	:linenos:

	bark_power = 11;

	function woof ( bark_energy )
		return (bark_energy * (bark_power / 4))
	end

The following C++ code will call this function from this file and retrieve the return value:

.. code-block:: cpp
	:linenos:

	sol::state lua;

	lua.script_file( "func_barks.lua" );

	sol::function woof = lua["woof"];
	double numwoof = woof(20);

The call ``woof(20)`` generates a :doc:`function_result<proxy>`, which is then implicitly converted to an ``double`` after being called. The intermediate temporary ``function_result`` is then destructed, popping the Lua function call results off the Lua stack. 

.. warning::

	Do NOT save the return type of a :doc:`function_result<proxy>` with ``auto``, as in ``auto numwoof = woof(20);``, and do NOT store it anywhere. Unlike its counterpart :doc:`protected_function_result<proxy>`, ``function_result`` is NOT safe to store as it assumes that its return types are still at the top of the stack and when its destructor is called will pop the number of results the function was supposed to return off the top of the stack. If you mess with the Lua stack between saving ``function_result`` and it being destructed, you will be subject to an incredible number of surprising and hard-to-track bugs. Don't do it.

.. code-block:: cpp
	:caption: function: call operator / function call

	template<typename... Args>
	protected_function_result operator()( Args&&... args );

	template<typename... Ret, typename... Args>
	decltype(auto) call( Args&&... args );

	template<typename... Ret, typename... Args>
	decltype(auto) operator()( types<Ret...>, Args&&... args );

Calls the function. The second ``operator()`` lets you specify the templated return types using the ``my_func(sol::types<int, std::string>, ...)`` syntax. Function assumes there are no runtime errors, and thusly will call the ``atpanic`` function if an error does occur.