threading
=========

Lua has no thread safety. sol does not force thread safety bottlenecks anywhere.

Assume any access or any call on Lua affects the whole global state (because it does, in a fair bit of cases). Therefore, every call to a state should be blocked off in C++ with some kind of access control. When you start hitting the same state from multiple threads, race conditions (data or instruction) can happen. Individual Lua coroutines might be able to run on separate C++-created threads without tanking the state utterly, since each Lua coroutine has the capability to run on an independent Lua thread which has its own stack, as well as some other associated bits and pieces that won't quite interfere with the global state.

To handle multithreaded environments, it is encouraged to either spawn mutliple Lua states for each thread you are working with and keep inter-state communication to synchronized serialization points. Using coroutines and Lua's threads might also buy you some concurrency and parallelism, but remember that Lua's threading technique is ultimately cooperative and requires explicit yielding and resuming (simplified as function calls for :doc:`sol::coroutine<api/coroutine>`).


getting the main thread
-----------------------

Lua 5.1 does not keep a reference to the main thread, therefore the user has to store it themselves. If you create a ``sol::state`` or follow the :ref:`steps for opening up compatibility and default handlers here<state-automatic-handlers>`, you can work with ``sol::main_thread`` to retrieve you the main thread, given a ``lua_State*`` that is either a full state or a thread: ``lua_state* Lmain = sol::main_thread( Lcoroutine )``; This function will always work in Lua 5.2 and above: in Lua 5.1, if you do not follow the ``sol::state`` instructions and do not pass a fallback ``lua_State*`` to the function, this function may not work properly and return ``nullptr``.

working with multiple Lua threads
---------------------------------

You can mitigate some of the pressure of using coroutines and threading by using the ``lua_xmove`` constructors that sol implements. Simply keep a reference to your ``sol::state_view`` or ``sol::state`` or the target ``lua_State*`` pointer, and pass it into the constructor along with the object you want to copy. Note that there is also some implicit ``lua_xmove`` checks that are done for copy and move assignment operators as well, as noted :ref:`at the reference constructor explanations<lua_xmove-note>`.

Furthermore, for every single ``sol::reference`` derived type, there exists a version prefixed with the word ``main_``, such as ``sol::main_table``, ``sol::main_function``, ``sol::main_object`` and similar. These classes, on construction, assignment and other operations, forcibly obtain the ``lua_State*`` associated with the main thread, if possible. Using these classes will allow your code to be immune when a wrapped coroutine or a lua thread is set to ``nil`` and then garbage-collected.

.. note::

	This does **not** provide immunity from typical multithreading issues in C++, such as synchronized access and the like. Lua's coroutines are cooperative in nature and concurrent execution with things like ``std::thread`` and similar still need to follow good C++ practices for multi threading.


Here's an example of explicit state transferring below:

.. code-block:: cpp 
	:caption: transfer from state function
	:name: state-transfer
	
	#define SOL_CHECK_ARGUMENTS
	#include <sol.hpp>

	#include <iostream>

	int main (int, char*[]) {

		sol::state lua;
		lua.open_libraries();
		sol::function transferred_into;
		lua["f"] = [&lua, &transferred_into](sol::object t, sol::this_state this_L) {
			std::cout << "state of main     : "  << (void*)lua.lua_state() << std::endl;
			std::cout << "state of function : "  << (void*)this_L.lua_state() << std::endl;
			// pass original lua_State* (or sol::state/sol::state_view)
			// transfers ownership from the state of "t",
			// to the "lua" sol::state
			transferred_into = sol::function(lua, t);
		};

		lua.script(R"(
		i = 0
		function test()
		co = coroutine.create(
			function()
				local g = function() i = i + 1 end
				f(g)
				g = nil
				collectgarbage()
			end
		)
		coroutine.resume(co)
		co = nil
		collectgarbage()
		end
		)");

		// give it a try
		lua.safe_script("test()");
		// should call 'g' from main thread, increment i by 1
		transferred_into();
		// check
		int i = lua["i"];
		assert(i == 1);

		return 0;
	} 
