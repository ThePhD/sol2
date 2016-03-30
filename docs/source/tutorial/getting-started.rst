getting started
===============

Let's get you going with Sol! To start, you'll need to use a lua distribution of some sort. Sol doesn't provide that: it only wraps the API that comes with it, so you can pick whatever distribution you like for your application. There are lots, but the two popular ones are `vanilla Lua`_ and speedy `LuaJIT`_ . We recommend vanilla Lua if you're getting started, LuaJIT if you need speed and can handle some caveats: the interface for Sol doesn't change no matter what Lua version you're using.

When you're ready: try compiling this short snippet:

.. code-block:: cpp
	:linenos:
	:caption: the first snippet
	:name: the-first-snippet

	#include "sol.hpp" // or #include <sol.hpp>, whichever suits your needs

	int main (int argc, char* argv[]) {

		sol::state lua;
		lua.open_libraries( sol::lib::base );

		lua.script( "print('bark bark bark!')" );

		return 0;
	}

If this works, you're ready to start! The first line creates the ``lua_State`` and will hold onto it for the duration of the scope its declared in (e.g., from the opening ``{`` to the closing ``}``). It will automatically close / cleanup that lua state when it gets destructed. The second line opens a single lua-provided library, "base". There are several other libraries that come with lua that you can open by default, and those are included in the :ref:`sol::lib::base<lib-enum>` enumeration.

Next, let's start :doc:`reading/writing some variables<variables>` from Lua into C++, and vice-versa!


.. _vanilla Lua: https://www.lua.org/

.. _LuaJIT: http://luajit.org/