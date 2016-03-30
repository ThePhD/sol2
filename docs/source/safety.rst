safety
======

Sol was designed to be correct and fast, and in the pursuit of both uses the regular ``lua_to{x}`` functions of Lua rather than the checking versions (``lua_check{X}``) functions. The API defaults to paranoidly-safe alternatives if you have a ``#define SOL_CHECK_ARGUMENTS`` before you include Sol, or if you pass the ``SOL_CHECK_ARGUMENTS`` define on the build command for your build system. By default, it is off and remains off unless you define this, even in debug mode.

``SOL_CHECK_ARGUMENTS`` triggers the following changes:
	* ``sol::stack::get`` (used everywhere) defaults to using ``sol::stack::check_get`` and dereferencing the argument. It uses ``sol::type_panic`` as the handler if something goes wrong.
	* ``sol::stack::call`` and its variants will, if no templated boolean is specified, check all of the arguments for a function call.

Remember that if you want these features, you must explicitly turn them on. Additionally, you can have basic boolean checks when using the API by just converting to a :doc:`sol::optional\<T><api/optional>` when necessary. Tests are compiled with this on to ensure everythign is going as expected.