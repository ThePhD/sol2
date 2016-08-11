adding your own types
=====================

Sometimes, overriding Sol to make it handle certain ``struct``s and ``class``es as something other than just userdata is desirable. The way to do this is to take advantage of the 4 customization points for Sol. These are ``sol::lua_size<T>``, ``sol::stack::pusher<T, C>``, ``sol::stack::getter<T, C>``, ``sol::stack::checker<T, sol::type t,  C>``.

The first thing to do is decide whether or not your type can be gotten from the stack, and whether or not it should also be pushed as arguments or such into Lua. If you need to retrieve it (as a return using one or multiple values from a Lua return), you should override but ``sol::stack::getter`` and ``sol::stack::checker``. If you need to push it into Lua at some point, then you'll want to override ``sol::stack::pusher``. For both cases, you need to override ``sol::lua_size``.

These are structures, so you'll override them using a technique C++ calls *class/struct specialization*. Below is an example of a struct that gets broken apart into 2 pieces when being pushed into Lua, and then pulled back into a struct when retrieved from Lua:

.. code-block:: cpp
	:caption: two_things.hpp
	:name: customization-overriding

	#include <sol.hpp>

	struct two_things {
		int a;
		bool b;
	};

	namespace sol {

		// First, the expected size
		// Specialization of a struct
		template <>
		struct lua_size<two_things> : std::integral_constant<int, 2> {};

		// Now, specialize various stack structures
		namespace stack {

			template <>
			struct checker<two_things> {
				template <typename Handler>
				static bool check(lua_State* L, int index, Handler&& handler, record& tracking) {
					// Check first and second second index for being the proper types
					bool success = stack::check<int>(L, index, handler) 
						&& stack::check<bool>(L, index + 1, handler);
					tracking.use(2);
					return success;
				}
			};

			template <>
			struct getter<two_things> {
				static two_things get(lua_State* L, int index, record& tracking) {
					// Get the first element
					int a = stack::get<int>(L, index);
					// Get the second element, 
					// in the +1 position from the first
					bool b = stack::get<bool>(L, index + 1);
					// we use 2 slots, each of the previous takes 1
					tracking.use(2);
					return two_things{ a, b };
				}
			};

			template <>
			struct pusher<two_things> {
				static int push(lua_State* L, const two_things& things) {
					int amount = stack::push(L, things.a);
					amount += stack::push(L, things.b);
					// Return 2 things
					return amount;
				}
			};

		}
	}


This is the base formula that you can follow to extend to your own classes. Using it in the rest of the framework should then be seamless:

.. code-block:: cpp
	:caption: customization: using it
	:name: customization-using

	#include <sol.hpp>
	#include <two_things.hpp>

	int main () {

		sol::state lua;

		// Create a pass-through style of function
		lua.script("function f ( a, b ) return a, b end");

		// get the function out of Lua
		sol::function f = lua["f"];

		two_things things = f(two_things{24, true});
		// things.a == 24
		// things.b == true
				
		return 0;
	}


And that's it!

A few things of note about the implementation: First, there's an auxiliary parameter of type :doc:`sol::stack::record<../api/stack>` for the getters and checkers. This keeps track of what the last complete operation performed. Since we retrieved 2 members, we use ``tracking.use(2);`` to indicate that we used 2 stack positions (one for ``bool``, one for ``int``). The second thing to note here is that we made sure to use the ``index`` parameter, and then proceeded to add 1 to it for the next one.

In general, this is fine since most getters/checkers only use 1 stack point. But, if you're doing more complex nested classes, it would be useful to use ``tracking.last`` to understand how many stack indices the last getter/checker operation did and increment it by ``index + tracking.last`` after using a ``stack::check<..>( L, index, tracking)`` call.

You can read more about the structs themselves :ref:`over on the API page for stack<extension_points>`, and if there's something that goes wrong or you have anymore questions, please feel free to drop a line on the Github Issues page or send an e-mail!