getting started
===============

Let's get you going with Sol! To start, you'll need to use a lua distribution of some sort. Sol doesn't provide that: it only wraps the API that comes with it, so you can pick whatever distribution you like for your application. There are lots, but the two popular ones are `vanilla Lua`_ and speedy `LuaJIT`_ . We recommend vanilla Lua if you're getting started, LuaJIT if you need speed and can handle some caveats: the interface for Sol doesn't change no matter what Lua version you're using.

After that, make sure you grab either the `single header file release`_, or just perform a clone of the `github repository here`_ and set your include paths up so that you can get at ``sol.hpp`` somehow. We recommend the single-header-file release, since it's easier to move around, manage and update if you commit it with some form of version control. If you use the github release, you *must* update submodules in order to make sure Optional is present in the repository: clone with:

>>> git clone https://github.com/ThePhD/sol2.git
>>> git submodule update --init

or, just run

>>> git clone --recursive https://github.com/ThePhD/sol2.git

When you're ready, try compiling this short snippet:

.. code-block:: cpp
	:linenos:
	:caption: test.cpp: the first snippet
	:name: the-first-snippet

	#include <sol.hpp> // or #include "sol.hpp", whichever suits your needs

	int main (int argc, char* argv[]) {

		sol::state lua;
		lua.open_libraries( sol::lib::base );

		lua.script( "print('bark bark bark!')" );

		return 0;
	}

Using this simple command line:

>>> g++ -std=c++14 test.cpp -llua -I"path/to/lua/include" -L"path/to/lua/lib"

Or using your favorite IDE / tool after setting up your include paths and library paths to lua according to the documentation of the Lua distribution you got. Remember your  linked lua library (``-llua``) and include / library paths will depend on your OS, file system, Lua distribution and your installation / compilation method of your Lua distribution.

.. note::
	
	If you get an avalanche of errors (particularly referring to ``auto``), you may not have enabled C++14 / C++17 mode for your compiler. Add one of ``std=c++14``, ``std=c++1z`` OR ``std=c++1y`` to your compiler options. By default, this is always-on for VC++ compilers in Visual Studio and friends, but g++ and clang++ require a flag (unless you're on `GCC 6.0`_).

If this works, you're ready to start! The first line creates the ``lua_State`` and will hold onto it for the duration of the scope its declared in (e.g., from the opening ``{`` to the closing ``}``). It will automatically close / cleanup that lua state when it gets destructed. The second line opens a single lua-provided library, "base". There are several other libraries that come with lua that you can open by default, and those are included in the :ref:`sol::lib<lib-enum>` enumeration.

If you're interested in integrating Sol with a project that already uses some other library or Lua in the codebase, check out the :doc:`existing example<existing>` to see how to work with Sol when you add it to a project!

Next, let's start :doc:`reading/writing some variables<variables>` from Lua into C++, and vice-versa!


.. _vanilla Lua: https://www.lua.org/

.. _LuaJIT: http://luajit.org/

.. _GCC 6.0: https://gcc.gnu.org/gcc-6/changes.html

.. _single header file release: https://github.com/ThePhD/sol2/releases

.. _github repository here: https://github.com/ThePhD/sol2