tutorial: quick 'n' dirty 
=========================

These are all the things. Use your browser's search to find things you want.

You'll need to ``#include <sol.hpp>``/``#include "sol.hpp"`` somewhere in your code. Sol is header-only, so you don't need to compile anything.

opening a state
---------------

Do it.

.. code-block:: cpp
	
	sol::state lua;
	// open some common libraries
	lua.open_libraries(sol::lib::base, sol::lib::package);
	lua.script( "print('bark bark bark!')" );


sol::state on lua_State*
------------------------

For your system/game that already has lua, but you'd like something nice:

.. code-block:: cpp
	
	int pre_existing_system( lua_State* L ) {
		sol::state_view lua(L);
		lua.script( "print('bark bark bark!')" );
	}


running lua code
----------------

.. code-block:: cpp

	sol::state lua;
	// load and execute from string
	lua.script("a = 'test'");
	// load and execute from file
	lua.script_file("path/to/luascript.lua");

	// load file without execute
	sol::load_result script1 = state.load_file("path/to/luascript.lua");
	script1(); //execute
	// load string without execute
	sol::load_result script2 = state.load("a = 'test'");
	script2(); //execute


set and get variables
---------------------

You can set/get everything.
	
.. code-block:: cpp
	
	struct some_class {
		bool some_variable = false;
	};

	sol::lua_state lua;

	lua.open_libraries(sol::lib::base);

	// integer types
	lua.set("number", 24);
	// floating point numbers
	lua["number2"] = 24.5;
	// string types
	lua["important_string"] = "woof woof";
	// non-recognized types is stored as userdata
	// this moves the type in (or copies, depending on class semantics)
	lua["myuserdata"] = some_class();
	// is callable, therefore gets stored as a function
	lua["a_function"] = [](){ return 100; }; 

	// implicit conversion
	int number = lua["number"];
	// explicit get
	auto number2 = lua.get<double>("number2");
	// strings too
	std::string important_string = lua["important_string"];
	
	// returns a plain reference
	some_class& myuserdata = lua["myuserdata"];
	// Modifies this in LUA VM AS WELL
	// its a reference, not a copy!
	myuserdata.some_variable = true;

	// get a function
	sol::function a_function = lua["a_function"];
	int value_is_100 = a_function();

	// get a std::function
	std::function<int()> a_std_function = lua["a_function"];
	int value_is_still_100 = a_std_function();


Retrieve Lua types using ``object`` and other ``sol::`` types.

.. code-block:: cpp

	sol::state lua;

	// ... everything from before

	sol::object number_obj = lua.get<sol::object>( "number" );
	// sol::type::number
	sol::type t1 = number_obj.get_type();

	sol::object function_obj = lua[ "a_function" ];
	// sol::type::function
	sol::type t2 = function_obj.get_type();
	bool is_it_really = function_obj.is<std::function<int()>(); // true

	// will not contain data
	sol::optional<int> check_for_me = lua["a_function"];


functions
---------

They're great. Use them:

.. code-block:: cpp
	
	sol::state lua;

	lua.script("function f (a, b, c, d) return 1 end");
	lua.script("function g (a, b) a + b end");

	// fixed signature std::function<...>
	std::function<int(int, double, int, std::string)> stdfx = lua["f"];
	// sol::function is often easier: takes a variable number/types of arguments...
	sol::function fx = lua["f"];

	int is_one = stdfx(1, 34.5, 3, "bark");
	int is_also_one = fx(1, "boop", 3, "bark");

	// call through operator[]
	int is_three = lua["g"](1, 2);
	// is_three == 3
	double is_2_8 = lua["g"](2.4, 2.4);
	// is_2_8 == 2.8

You can bind member variables as functions too:

.. code-block:: cpp
	
	void some_function () {
		std::cout << "some function!" << std::endl;
	}

	struct some_class {
		int variable = 30;

		double member_function () {
			return 24.5;
		}
	};

	sol::state lua;
	lua.open_libraries(sol::lib::base);

	lua["f1"] = some_function;
	lua.set_function("f2", &some_other_function);

	lua.script(R"(
		 f1() -- some function!
		 f2() -- some function!
	)");

	// put an instance of "some_class" into lua
	lua.set("sc", some_class());

	// binds just the member function
	lua["m1"] = &some_class::member_function;
	// binds the class to the type
	lua.set_function("m2", &some_class::member_function, some_class{});

	lua.script(R"(
		-- need class instance if you don't bind it with the function
		print(m1(sc)) -- 24.5
		-- does not need class instance: was made with one 
		print(m2()) -- 24.5
	)");

	// binds just the member variable as a function
	lua["v1"] = &some_class::variable;
	// binds class with member variable as function
	lua.set_function("v2", &some_class::variable, some_class{});
	
	lua.script(R"(
		-- need class instance if you don't bind it with the function
		print(v1(sc)) -- 30
		-- does not need class instance: was bound with one 
		print(v2()) -- 30

		-- can set: still requires instance
		v1(sc, 212)
		-- can set: does not need class instance: was bound with one 
		v2(254)

		print(v1(sc)) -- 212
		print(v2()) -- 254
	)");

Can use ``sol::readonly( &some_class::variable )`` to make a variable readonly and error if someone tries to write to it.


multiple returns from lua
-------------------------

.. code-block:: cpp
	
	sol::state lua;

	lua.script("function f (a, b, c) return a, b, c end");
	
	std::tuple<int, int, int> result = lua["f"](100, 200, 300); 
	// result == { 100, 200, 300 }
	int a, int b;
	std::string c;
	sol::bond( a, b, c ) = lua["f"](100, 200, "bark");
	// a == 100
	// b == 200
	// c == "bark"


multiple returns to lua
-----------------------

.. code-block:: cpp
	
	sol::state lua;

	lua["f"] = [](int a, int b, sol::object c) {
		// sol::object can be anything here: just pass it through
		return std::make_tuple( 100, 200, c );
	};
	
	std::tuple<int, int, int> result = lua["f"](100, 200, 300); 
	// result == { 100, 200, 300 }
	
	lua[]
	// result == { 100, 200, 300 }
	int a, int b;
	std::string c;
	sol::bond( a, b, c ) = lua["f"](100, 200, "bark");
	// a == 100
	// b == 200
	// c == "bark"


tables
------

:doc:`state<../api/state>` is a table too.

.. code-block:: cpp

	sol::state lua;

	// Raw string literal for easy multiline
	lua.script( R"(
		abc = { [0] = 24 }
		def = { 
			ghi = { 
				bark = 50, 
				woof = abc 
			} 
		}
	)"
	);

	sol::table abc = lua["abc"];
	sol::state def = lua["def"];
	sol::table ghi = lua["def"]["ghi"];

	int bark1 = def["ghi"]["bark"];
	// bark1 == 50
	int bark2 = lua["def"]["ghi"]["bark"];
	// bark2 == 50
	
	bool bark_equal = bark1 == bark2;
	// true

	int abcval1 = abc[0];
	// abcval2 == 24
	int abcval2 = ghi["woof"][0];
	// abcval2 == 24
	
	bool abcval_equal = abcval1 == abcval2; 
	// true

If you're going deep, be safe:

.. code-block:: cpp

	sol::optional<int> will_not_error = lua["abc"]["DOESNOTEXIST"]["ghi"];
	// will_not_error == sol::nullopt
	int will_not_error2 = lua["abc"]["def"]["ghi"]["jklm"].get_or<int>(25);
	// is 25

	// if you don't go safe,
	// will throw (or do at_panic if no exceptions)
	int aaaahhh = lua["abc"]["hope_u_liek_crash"];


make tables
-----------

Make some:

.. code-block:: cpp

	lua["abc"] = lua.create_table_with(
		0, 24
	);

	lua.create_named_table("def",
		"ghi", lua.create_table_with(
			"bark", 50,
			"woof", lua["abc"] // can reference other existing stuff too
		)
	);

Equivalent Lua code:

.. code-block:: lua
	
	abc = { [0] = 24 }
	def = { 
		ghi = { 
			bark = 50, 
			woof = abc 
		} 
	}	
	

userdata + usertypes (metatables)
---------------------------------

Everything that is not a:

	* primitive type: ``bool``, ``char/short/int/long/long long``, ``float/double``
	* string type: ``std::string``, ``const char*``
	* function type: function pointers, ``lua_CFunction``, ``std::function``, :doc:`sol::function/sol::protected_function<../api/function>`, :doc:`sol::coroutine<../api/coroutine>`
	* designated sol type: :doc:`sol::table<../api/table>`, :doc:`sol::thread<../api/thread>`, :doc:`sol::error<../api/error>`, :doc:`sol::object<../api/object>`
	* transparent argument type: :doc:`sol::variadic_arg<../api/variadic_args>`, :doc:`sol::this_state<../api/this_state>`
	* usertype<T> class: :doc:`sol::usertype<../api/usertype>`

Is set as a userdata.

.. code-block:: cpp

	struct Doge { int tailwag = 50; }

	Doge dog{};
	
	// Copy into lua: destroyed when lua VM garbage collects
	lua["dog"] = dog;
	// OR: move semantics - will call move constructor if present instead
	lua["dog"] = std::move( dog );
	lua["dog"] = Doge{};
	lua["dog"] = std::make_unique<Doge>();
	lua["dog"] = std::make_shared<Doge>();
	// Identical to above
	lua.set("dog", dog);
	lua.set("dog", std::move(dog));
	lua.set("dog", Doge{});
	lua.set("dog", std::unique_ptr<Doge>(new Doge()));
	lua.set("dog", std::shared_ptr<Doge>(new Doge()));

``std::unique_ptr``/``std::shared_ptr``'s reference counts / deleters will be respected. If you want it to refer to something, whose memory you know won't die in C++, do the following:

.. code-block:: cpp

	Doge dog{}; // Kept alive somehow

	// Later...
	// The following stores a reference, and does not copy/move
	// lifetime is same as dog in C++ 
	// (access after it is destroyed is bad)
	lua["dog"] = &dog;
	// Same as above: respects std::reference_wrapper
	lua["dog"] = std::ref(dog);
	// These two are identical to above
	lua.set( "dog", &dog );
	lua.set( "dog", std::ref( dog ) );

Get userdata in the same way as everything else:

.. code-block:: cpp

	Doge& dog = lua["dog"]; // References Lua memory
	Doge* dog_pointer = lua["dog"]; // References Lua memory
	Doge dog_copy = lua["dog"]; // Copies, will not affect lua

	dog_copy.tailwag = 525;
	// Still 50
	lua.script("assert(dog.tailwag == 50)");

	dog.tailwag = 100;
	// Now 100
	lua.script("assert(dog.tailwag == 100)");


more userdata + usertypes
-------------------------

Because there's a LOT you can do with Sol:

.. code-block:: cpp
	:caption: test_player.hpp

	struct player {
	public:
		int bullets;
		int speed;

		player() 
		: player(3, 100) {

		}

		player(int ammo) 
		: player(ammo, 100) {

		}

		player(int ammo, int hitpoints) 
		: bullets(ammo), hp(hitpoints) {

		}

		void boost () {
			speed += 10;
		}

		bool shoot () {
			if (bullets < 1)
				return false;
			--bullets;
			return true;
		}

		int set_hp(int value) {
			hp = value;
		}

		int get_hp() const {
			return hp;
		}

	private:
		int hp;
	}

Bind all the things:

.. code-block:: cpp
	:caption: player_script.cpp

	sol::state lua;

	// note that you can set a userdata before you register a usertype,
	// and it will still carry the right metatable if you register it later
	
	lua.set("p2", player(0));
	// p2 has no ammo

	// make usertype metatable
	lua.new_usertype<player>( "player",
		
		// 3 constructors
		sol::constructors<sol::types<>, sol::types<int>, sol::types<int, int>>(),
		
		// typical member function that returns a variable
		"shoot", &player::shoot,
		// typical member function
		"boost", &player::boost,
		
		// gets or set the value using member variable syntax
		"hp", sol::property(&player::get_hp, &player::set_hp),
		
		// read and write variable
		"speed", &player::speed,
		// can only read from, not write to
		"bullets", sol::readonly( &player::bullets )
	);

	lua.script_file("player_script.lua");

And the script:

.. code-block:: lua
	:caption: player_script.lua
	
	-- call single argument integer constructor
	p1 = player.new(2)

	-- p2 is still here from being set with lua.set(...) above
	local p2shoots = p2:shoot()
	assert(not p2shoots)
	-- had 0 ammo
	
	-- set variable property setter
	p1.hp = 545;
	-- get variable through property getter
	print(p1.hp);

	local did_shoot_1 = p1:shoot()
	print(did_shoot_1)
	print(p1.bullets)
	local did_shoot_2 = p1:shoot()
	print(did_shoot_2)
	print(p1.bullets)
	local did_shoot_3 = p1:shoot()
	print(did_shoot_3)
	
	-- can read
	print(p1.bullets)
	-- would error: is a readonly variable, cannot write
	-- p1.bullets = 20

	p1:boost()

Even more stuff :doc:`you can do<../api/usertype>` described elsewhere, like initializer functions (private constructors / destructors support), "static" functions callable with ``name.my_function( ... )``, and overloaded member functions.


pointers
--------

Sol will not take ownership of raw pointers: raw pointers do not own anything.

.. code-block:: cpp

	// AAAHHH BAD
	// dangling pointer!
	lua["my_func"] = []() -> my_type* {
		return new my_type();
	};

Return a ``unique_ptr`` or ``shared_ptr`` instead or just return a value:

.. code-block:: cpp

	// :ok:
	lua["my_func"] = []() -> std::unique_ptr<my_type> {
		return std::make_unique<my_type>();
	};

	// :ok:
	lua["my_func"] = []() -> std::shared_ptr<my_type> {
		return std::make_shared<my_type>();
	};

	// :ok:
	lua["my_func"] = []() -> my_type {
		return my_type();
	};


advanced
--------

Some more advanced things you can do:
	
	* :doc:`stack manipulation<../api/stack>` to safely play with the stack. You can also define customization points for ``stack::get``/``stack::check``/``stack::push`` for your type.
	* :doc:`variadic arguments<../api/variadic_args>` in functions with ``sol::variadic_args``.
	* :doc:`this_state<../api/this_state>` to get the current ``lua_State*``.
	* :doc:`resolve<../api/resolve>` overloads in case you have overloaded functions; a cleaner casting utility.