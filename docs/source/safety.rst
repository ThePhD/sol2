safety
======

Sol was designed to be correct and fast, and in the pursuit of both uses the regular ``lua_to{x}`` functions of Lua rather than the checking versions (``lua_check{X}``) functions. The API defaults to paranoidly-safe alternatives if you have a ``#define SOL_CHECK_ARGUMENTS`` before you include Sol, or if you pass the ``SOL_CHECK_ARGUMENTS`` define on the build command for your build system. By default, it is off and remains off unless you define this, even in debug mode. The same goes for ``#define SOL_SAFE_USERTYPE``.

.. _config:

config
------

Note that you can obtain safety with regards to functions you bind by using the :doc:`protect<api/protect>` wrapper around function/variable bindings you set into Lua. Additionally, you can have basic boolean checks when using the API by just converting to a :doc:`sol::optional\<T><api/optional>` when necessary for getting things out of Lua and for function arguments.

``SOL_SAFE_USERTYPE`` triggers the following change:
	* If the userdata to a usertype function is nil, will trigger an error instead of letting things go through and letting the system segfault/crash
	* Turned on by default with clang++, g++ and VC++ if a basic check for building in debug mode is detected

``SOL_SAFE_FUNCTION`` triggers the following change:
	* All uses of ``sol::function`` and ``sol::stack_function`` will default to ``sol::protected_function`` and ``sol::stack_protected_function``, respectively, rather than ``sol::unsafe_function`` and ``sol::stack_unsafe_function``.
	* Not turned on by default under any detectible compiler settings: you must turn this one on manually

``SOL_CHECK_ARGUMENTS`` triggers the following changes:
	* ``sol::stack::get`` (used everywhere) defaults to using ``sol::stack::check_get`` and dereferencing the argument. It uses ``sol::type_panic`` as the handler if something goes wrong
	* ``lua_tolstring`` conversions are not permitted on numbers: through the API: only actual strings are allowed. This is necessary to allow :doc:`sol::overload<api/overload>` to work properly
	* ``sol::stack::call`` and its variants will, if no templated boolean is specified, check all of the arguments for a function call
	* If ``SOL_SAFE_USERTYPE`` is not defined, it gets defined to turn being on and the effects described above kick in

Tests are compiled with this on to ensure everything is going as expected. Remember that if you want these features, you must explicitly turn them on all of them to be sure you are getting them.

memory
------

Memory safety can be tricky. Lua is handled by a garbage-collected runtime, meaning object deletion is not cleary defined or deterministic. If you need to keep an object from the Lua Runtime alive, use :doc:`sol::reference<api/reference>` or one of its derived types, such as :doc:`sol::table<api/table>`, :doc:`sol::object<api/object>`, or similar. These will pin a reference down to an object controlled in C++, and Lua will not delete an object that you still have a reference to through one of these types. You can then retrieve whatever you need from that Lua slot using object's ``obj.as<T>()`` member function or other things, and work on the memory from there.

The usertype memory layout for all Lua-instantiated userdata and for all objects pushed/set into the Lua Runtime is also described :doc:`here<api/usertype_memory>`. Things before or after that specified memory slot is implementation-defined and no assumptions are to be made about it.

functions
---------

The *vast majority* of all users are going to want to work with :doc:`sol::safe_function/sol::protected_function<api/protected_function>`. This version allows for error checking, prunes results, and responds to the defines listed above by throwing errors if you try to use the result of a function without checking. :doc:`sol::function/sol::unsafe_function<api/function>` is unsafe. It assumes that its contents run correctly and throw no errors, which can result in crashes that are hard to debug while offering a very tiny performance boost for not checking error codes or catching exceptions.

If you find yourself crashing inside of ``sol::function``, try changing it to a ``sol::protected_function`` and seeing if the error codes and such help you find out what's going on. You can read more about the API on :doc:`the page itself<api/protected_function>`. You can also define ``SOL_SAFE_FUNCTION`` as described above, but be warned that the ``protected_function`` API is a superset of the regular default ``function`` API: trying to revert back after defining ``SOL_SAFE_FUNCTION`` may result in some compiler errors if you use things beyond the basic, shared interface of the two types.

As a side note, binding functions with default parameters does not magically bind multiple versions of the function to be called with the default parameters. You must instead use :doc:`sol::overload<api/overload>`.

.. warning::

	Do NOT save the return type of a :ref:`function_result<function-result>` with ``auto``, as in ``auto numwoof = woof(20);``, and do NOT store it anywhere. See :ref:`here<function-result-warning>`.
