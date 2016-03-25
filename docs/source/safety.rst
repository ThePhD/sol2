safety
======

Sol was designed to be correct and fast, and in the pursuit of both uses the regular ``lua_to{x}`` functions of Lua rather than the checking versions (``lua_check{X}``) functions. The API defaults to safe alternatives if you have a ``#define SOL_CHECK_ARGUMENTS`` before you include Sol, or if you pass the ``SOL_CHECK_ARGUMENTS`` define on the build command for your build system. By default, it is off and remains off unless you define this, even in debug mode.

``SOL_CHECK_ARGUMENTS`` triggers the following changes:
	* ``stack::get`` (used everywhere) defaults to using ``stack::check_get`` and dereferencing the argument. It uses ``type_panic`` as the handler if something goes wrong.
	* ``stack::call`` will, if no template boolean is specified, check all of the arguments for a function call.

Remember that if you want these features, you must explicitly turn them on. Additionally, you can have basic boolean checks when using the API by just converting to a :ref:`sol::optional\<T><optional>` when necessary.