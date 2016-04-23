usertype<T>
===========
structures and classes from C++ made available to Lua code
----------------------------------------------------------

*Note: ``T`` refers to the type being turned into a usertype.*

While other frameworks extend lua's syntax or create Data Structure Languages (DSLs) to create classes in lua, :doc:`sol<../index>` instead offers the ability to generate easy bindings. These use metatables and userdata in lua for their implementation. Given this C++ class:

.. code-block:: cpp
	:linenos:
	
	struct ship {
		int bullets = 20;
		int life = 100;

		bool shoot () {
			if (bullets > 0) {
				--bullets;
				// successfully shot
				return true;
			}
			// cannot shoot
			return false;
		}

		bool hurt (int by) {
			life -= by;
			// have we died?
			return life < 1;
		}
	};

You can bind the it to lua using the following C++ code:

.. code-block:: cpp
	:linenos:

	sol::state lua;

	lua.new_usertype<ship>( "ship", // the name of the class, as you want it to be used in lua
		// List the member functions you wish to bind:
		// "name_of_item", &class_name::function_or_variable
		"shoot", &ship::shoot,
		"hurt", &ship::hurt,
		// bind variable types, too
		"life", &ship::bullets
		// names in lua don't have to be the same as C++,
		// but it probably helps if they're kept the same,
		// here we change it just to show its possible
		"bullet_count", &ship::bullets
	);


Equivalently, you can also write:

.. code-block:: cpp
	:linenos:
	:emphasize-lines: 4,12

	sol::state lua;

	// Use constructor directly
	usertype<ship> shiptype(
		"shoot", &ship::shoot,
		"hurt", &ship::hurt,
		"life", &ship::bullets
		"bullet_count", &ship::bullets
	);

	// set usertype explicitly, with the given name
	lua.set_usertype<ship>( "ship", shiptype );

	// shiptype is now a useless skeleton type, just let it destruct naturally and don't use it again.


Note that here, because the C++ class is default-constructible, it will automatically generate a creation function that can be called in lua called "new" that takes no arguments. You can use it like this in lua code:

.. code-block:: lua
	:linenos:

	fwoosh = ship.new()
	local success = fwoosh:shoot() -- note the ":" that is there: this is mandatory for member function calls
	local is_dead = fwoosh:hit(20)
	-- check if it works
	print(is_dead) -- the ship is not dead at this point
	print(fwoosh.life .. "life left") -- 80 life left
	print(fwoosh.bullet_count) -- 19


There are more advanced use cases for how to create and use a usertype, which are all based on how to use its constructor (see below).

enumerations
------------

.. _meta_function_enum:

.. code-block:: cpp
	:caption: meta_function enumeration for names
	:linenos:

	enum class meta_function {
	    construct,
	    index,
	    new_index,
	    mode,
	    call,
	    metatable,
	    to_string,
	    length,
	    unary_minus,
	    addition,
	    subtraction,
	    multiplication,
	    division,
	    modulus,
	    power_of,
	    involution = power_of,
	    concatenation,
	    equal_to,
	    less_than,
	    less_than_or_equal_to,
	    garbage_collect,
	    call_function,
	};


Use this enumeration to specify names in a manner friendlier than memorizing the special lua metamethod names for each of these. Each binds to a specific operation indicated by the descriptive name of the enum.

members
-------

.. code-block:: cpp
	:caption: function: usertype<T> constructor
	:name: usertype-constructor

	template<typename... Args>
	usertype<T>(Args&&... args);


The constructor of usertype takes a variable number of arguments. It takes an even number of arguments (except in the case where the very first argument is passed as the :ref:`constructor<constructor>` or in the case of specifying a custom :ref:`destructor<destructor>`). Names can either be strings, :ref:`special meta_function enumerations<meta_function_enum>`, or one of the special indicators for initializers.


.. _constructor:

* ``"{name}", constructors<Type-List-0, Type-List-1, ...>``
    - ``Type-List-N`` must be a ``sol::types<Args...>``, where ``Args...`` is a list of types that a constructor takes. Supports overloading by default
    - If you pass the ``constructors<...>`` argument first when constructing the usertype, then it will automatically be given a ``"{name}"`` of ``"new"``
* ``"{name}", sol::initializers( func1, func2, ... )``
    - Creates initializers that, given one or more functions, provides an overloaded lua function for creating a the specified type.
	   + The function must have the argument signature ``func( T*, Arguments... )`` or ``func( T&, Arguments... )``, where the pointer or reference will point to a place of allocated memory that has an uninitialized ``T``. Note that lua controls the memory.

.. _destructor:

* ``"__gc", sol::destructor( func )`` or ``sol::meta_function::garbage_collect, sol::destructor( func )``
    - Creates a custom destructor that takes an argument ``T*`` or ``T&`` and expects it to be destructed/destroyed. Note that lua controls the memory and thusly will deallocate the necessary space AFTER this function returns (e.g., do not call ``delete`` as that will attempt to deallocate memory you did not ``new``).
    - If you just want the default constructor, you can replace the second argument with ``sol::default_destructor``.
    - The usertype will throw if you specify a destructor specifically but do not map it to ``sol::meta_function::gc`` or a string equivalent to ``"__gc"``.
* ``"{name}", &free_function``
    - Binds a free function / static class function / function object (lambda) to ``"{name}"``. If the first argument is ``T*`` or ``T&``, then it will bind it as a member function. If it is not, it will be bound as a "static" function on the lua table.
* ``"{name}", &type::function_name`` or ``"{name}", &type::member_variable``
    - Binds a typical member function or variable to ``"{name}"``. In the case of a member variable or member function, ``type`` must be ``T`` or a base of ``T``.
* ``"{name}", sol::readonly( &type::member_variable )``
    - Binds a typical variable to ``"{name}"``. Similar to the above, but the variable will be read-only, meaning an error will be generated if anything attemps to write to this variable.
* ``"{name}", sol::property( &type::getter_func, &type::setter_func )``
    - Binds a typical variable to ``"{name}"``, but gets and sets using the specified setter and getter functions. Not that if you do not pass a setter function, the variable will be read-only. Also not that if you do not pass a getter function, it will be write-only.
* ``"{name}", sol::overloaded( Func1, Func2, ... )``
    - Creates an oveloaded member function that discriminates on number of arguments and types.
* ``sol::base_classes, sol::bases<Bases...>``
    - Tells a usertype what its base classes are. If you have exceptions turned on, this need not be necessary: if you do not then you need this to have derived-to-base conversions work properly. See :ref:`inheritance<usertype-inheritance>`.


overloading
-----------

Functions set on a usertype support overloading. See :doc:`here<overload>` for an example.


.. _usertype-inheritance:

inheritance
-----------

Sol can adjust pointers from derived classes to base classes at runtime, but it has some caveats based on what you compile with:

.. _exceptions_enabled:

**With Exceptions Enabled**

You do not need to manually specify the base classes. We use a technique that infinitely scales and automatically casts derived pointers to their base classes by exploiting the necessity of exception type matching.

.. _exceptions_disabled:

**With Exceptions Disabled**

You must specify the ``sol::base_classes`` tag with the ``sol::bases<Types...>()`` argument, where ``Types...`` are all the base classes of the single type ``T`` that you are making a usertype out of. when you create the usertype. If you turn exceptions off and are also completely mad and turn off :doc:`run-time type information<../rtti>` as well, we fallback to a id-based  systemthat still requires you to specifically list the base classes as well. For example:

.. code-block:: cpp
	:linenos:

	struct A { 
		int a = 10;
		virtual int call() { return 0; } 
	};
	struct B : A { 
		int b = 11; 
		virtual int call() override { return 20; } 
	};

Then, to register the base classes explicitly:

.. code-block:: cpp
	:linenos:
	:emphasize-lines: 5,6

	sol::state lua;

	lua.new_usertype<B>( "B",
		"call", &B::call,
		// List bases explicitly if you disable exceptions
		sol::base_classes, sol::bases<A>()
	);

Note that Sol does not support down-casting from a base class to a derived class at runtime.

inheritance + overloading
-------------------------

While overloading is supported regardless of `inheritance<inheritance>` caveats or not, the current version of Sol has a first-match, first-call style of overloading when it comes to inheritance. Put the functions with the most derived arguments first to get the kind of matching you expect.

traits
------

.. code-block:: cpp
	:caption: usertype_traits<T>
	:name: usertype-traits

	template<typename T>
	struct usertype_traits {
	    static const std::string name;
	    static const std::string metatable;
	    static const std::string variable_metatable;
	    static const std::string gc_table;
	};


This trait is used to provide names for the various metatables and global tables used to perform cleanup and lookup. They are automatically generated at runtime. In the case of RTTI being present, Sol will attempt to demangle the name from ``std::type_info`` to produce a valid name. If RTTI is disabled, Sol attempts to parse the output of ``__PRETTY_FUCNTION__`` (``g++``/``clang++``) or ``_FUNCDSIG`` (``vc++``) to get the proper type name. If you have a special need you can override the names for your specific type.

performance note
----------------

.. note::

	Note that performance for member function calls goes down by a fixed overhead if you also bind variables as well as member functions. This is purely a limitation of the lua implementation and there's, unfortunately, nothing that can be done about it. If you bind only functions and no variables, however, Sol will automatically optimize the Lua runtime and give you the maximum performance possible. *Please consider ease of use and maintenance of code before you make everything into functions.*
