functions
=========
working with functions in sol2
------------------------------


There are a number of examples dealing with functions and how they can be bound to sol2:

* For a quicker walkthrough that demonstrates almost everything, see `the examples`_ and the :doc:`the quick and dirty tutorial<tutorial/all-the-things>`
* For a full explanation, :doc:`read the tutorial<tutorial/functions>` and consult the subjects below
* If you have bindings and set-ups that want to leverage the C API without sol2's interference, you can push a raw function, which has certain implications (noted :ref:`below<raw-function-note>`)
* You return multiple values by returning a `std::tuple`
* You can work with **transparent arguments** that provide you with special information, such as
	- :doc:`sol::variadic_args<api/variadic_args>`, for handling variable number of arguments at runtime
	- :doc:`sol::this_state<api/this_state>`, for getting the current Lua state
	- :doc:`sol::this_environment<api/this_environment>`, for potentially retrieving the current Lua environment
* :doc:`Overload function calls on a single name<api/overload>`, discriminating by argument number and type (first-come, first-serve overloading)
	- Note: because of this feature, automatic number to string conversion from Lua is not permitted
* Control serialization of arguments and return types with :doc:`sol::nested<api/nested>`, :doc:`sol::as_table<api/nested>`, :doc:`sol::as_args<api/as_args>` and :doc:`sol::as_function<api/as_function>`
* Set environments for Lua functions and scrips with :doc:`sol::environment<api/environment>`


.. _binding-callable-objects:

working with callables/lambdas
------------------------------

.. note::

	Function objects ``obj`` -- a struct with a ``return_type operator()( ... )`` member defined on them, like all C++ lambdas -- are not interpreted as functions when you use ``set`` for ``mytable.set( key, value )`` and ``state.create_table(_with)( ... )``. This only happens automagically with ``mytable[key] = obj``. To be explicit about wanting a struct to be interpreted as a function, use ``mytable.set_function( key, func_value );``. You can be explicit about wanting a function as well by using the :doc:`sol::as_function<../api/as_function>` call, which will wrap and identify your type as a function.

.. _function-exception-handling:

exception safety/handling
-------------------------

All functions bound to sol2 set up an exception trampoline around the function (unless you are working with a :ref:`raw lua_CFunction you pushed yourself<raw-function-note>`). :doc:`protected_function<api/protected_function>` also has an error handler member and an exception trampoline around its internals, but it is not guaranteed safe if an exception bubbles outside of it. Catching that exception is not safe either: if an exception has exploded out from the sol2 API somehow, you must assume the VM is in some indeterminate and/or busted state.

Please read the :doc:`error page<errors>` and :doc:`exception page<exceptions>` for more details about what to do with exceptions that explode out from the API.


.. _function-argument-handling:

functions and argument passing
------------------------------

.. note::

	All arguments are forwarded. Unlike :doc:`get/set/operator[] on sol::state<api/state>` or :doc:`sol::table<api/table>`, value semantics are not used here. It is forwarding reference semantics, which do not copy/move unless it is specifically done by the receiving functions / specifically done by the user.


.. note::

	This also means that you should pass and receive arguments in certain ways to maximize efficiency. For example, ``sol::table``, ``sol::object``, ``sol::userdata`` and friends are cheap to copy, and should simply by taken as values. This includes primitive types like ``int`` and ``double``. However, C++ types -- if you do not want copies -- should be taken as ``const type&`` or ``type&``, to save on copies if it's important. Note that taking references from Lua also means you can modify the data inside of Lua directly, so be careful. Lua by default deals with things mostly by reference (save for primitive types).

	Please avoid taking special unique_usertype arguments, by either reference or value. In many cases, by-value does not work (e.g., with ``std::unique_ptr``) because many types are move-only and Lua has no concept of "move" semantics. By-reference is dangerous because sol2 will hand you a reference to the original data: but, any pointers stored in Lua can be invalidated if you call ``.reset()`` or similar on the core pointer. Please take a pointer (``T*``) if you anticipate ``nil``/``nullptr`` being passed to your function, or a reference (``const T&`` or ``T&``) if you do not. 

	You can get even more speed out of ``sol::object`` style of types by taking a ``sol::stack_object`` (or ``sol::stack_...``, where ``...`` is ``userdata``, ``reference``, ``table``, etc.). These reference a stack position directly rather than cheaply/safely the internal Lua reference to make sure it can't be swept out from under you. Note that if you manipulate the stack out from under these objects, they may misbehave, so please do not blow up your Lua stack when working with these types.

	``std::string`` (and ``std::wstring``) are special. Lua stores strings as ``const char*`` null-terminated strings. ``std::string`` will copy, so taking a ``std::string`` by value or by const reference still invokes a copy operation. You can take a ``const char*``, but that will mean you're exposed to what happens on the Lua stack (if you change it and start chopping off function arguments from it in your function calls and such, as warned about previously).


.. _function-argument-safety:

function call safety
--------------------

You can have functions here and on usertypes check to definitely make sure that the types passed to C++ functions are what they're supposed to be by adding a ``#define SOL_CHECK_ARGUMENTS`` before including Sol, or passing it on the command line. Otherwise, for speed reasons, these checks are only used where absolutely necessary (like discriminating between :doc:`overloads<api/overload>`). See :doc:`safety<../safety>` for more information.


.. _raw-function-note:

raw functions (``lua_CFunction``)
---------------------------------

When you push a function into Lua using Sol using any methods and that function exactly matches the signature ``int( lua_State* );``, it will be treated as a *raw C function* (a ``lua_CFunction``). This means that the usual exception trampoline Sol wraps your other function calls in will not be present. You will be responsible for catching exceptions and handling them before they explode into the C API (and potentially destroy your code). Sol in all other cases adds an exception-handling trampoline that turns exceptions into Lua errors that can be caught by the above-mentioned protected functions and accessors.

Note that stateless lambdas can be converted to a function pointer, so stateless lambdas similar to the form ``[](lua_State*) -> int { ... }`` will also be pushed as raw functions. If you need to get the Lua state that is calling a function, use :doc:`sol::this_state<api/this_state>`.

.. warning::
	
	Do NOT assume that building Lua as C++ will allow you to throw directly from a raw function. If an exception is raised and it bubbles into the Lua framework, even if you compile as C++, Lua does not recognize exceptions other than the ones that it uses with ``lua_error``. In other words, it will return some completely bogus result, potentially leave your Lua stack thrashed, and the rest of your VM *can* be in a semi-trashed state. Please avoid this!


.. _the examples: https://github.com/ThePhD/sol2/blob/develop/examples/functions.cpp
