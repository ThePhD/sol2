.. Sol documentation master file, created by
   sphinx-quickstart on Mon Feb 29 21:49:51 2016.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Sol 2.0.0 
=========
a fast, simple C++ and Lua Binding
----------------------------------

Contents:
---------

.. toctree::
   :maxdepth: 1
   :name: mastertoc
   :caption: Contents
	
   features
   benchmarks
   exceptions
   rtti
   api/top
   licenses

"I need feature X, maybe you have it?"
--------------------------------------
Take a look at the :doc:`Features<features>` page: it links to much of the API. Don't see a feature you want? Send inquiries for support for a particular abstraction to the `issues`_ tracker.


The Basics:
-----------

.. note::
	More examples can be found in the `examples directory`_

.. code-block:: c++
   :caption: functions
   :linenos:

	#include <sol.hpp>
	#include <cassert>

	int main() {
	    sol::state lua;
	    int x = 0;
	    lua.set_function("beep", [&x]{ ++x; });
	    lua.script("beep()");
	    assert(x == 1);

	    sol::function beep = lua["beep"];
	    beep();
	    assert(x == 2);

	    return 0;
	}

.. code-block:: c++
   :caption: linking C++ structures to Lua
   :linenos:

	#include <sol.hpp>
	#include <cassert>

	struct vars {
	    int boop = 0;

	    int bop () const {
	    	return boop + 1;
	    }
	};

	int main() {
	    sol::state lua;
	    lua.new_usertype<vars>("vars", 
	    	"boop", &vars::boop
	    	"bop", &vars::bop);
	    lua.script("beep = vars.new()\n"
	               "beep.boop = 1\n"
	               "bopvalue = beep.bop()");

	    vars& beep = lua["beep"];
	    int bopvalue = lua["bopvalue"];

	    assert(beep.boop == 1);
	    assert(lua.get<vars>("beep").boop == 1);
	    assert(beep.bop() == 2);
	    assert(bopvalue == 2);

	    return 0;
	}



Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

.. _issues: http://github.com/ThePhD/sol2/issues
.. _examples directory: https://github.com/ThePhD/sol2/tree/develop/examples