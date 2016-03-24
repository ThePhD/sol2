features
========
what does Sol (and other libraries) support?
--------------------------------------------

The goal of Sol is to provide an incredibly clean API that provides high performance (comparable or better than the C it was written on) and extreme ease of use. That is, users should be able to say: "this works pretty much how I expected it to."

For the hard technical components of Lua and its ecosystem we support, here is the full rundown:

what Sol supports
-----------------

* Support for Lua 5.1, 5.2, and 5.3. We achieve this through our "doc:`compatibility<compatibility>` header.

* :doc:`Table<api/table>` support: setting values, getting values of multiple (different) types
	- :doc:`Lazy evaluation<api/proxy>` for nested/chained queries
		``table["a"]["b"]["c"] = 24;``
	- Implicit conversion to the types you want
		``double b = table["computed_value"];``

* Support for callables (functions, lambdas, member functions)
 	- Pull out any Lua function with :doc:`sol::function<api/function>`
 		``sol::function fx = table["socket_send"];``
 	- Can also set callables into :doc:`operator[] proxies<api/proxy>`
 		``table["move_dude"] = engine::move_dude;``
 	- Safety: use :doc:`sol::protected_function<api/protected_function>` to catch any kind of error
 		+ ANY kind: C++ exception or Lua erors are trapped and run through the optional ``error_handler`` variable
 	- *Advanced:* Overloading of a single function so you don't need to do boring typechecks

* User-Defined Type (:doc:`sol::usertype<api/usertype>` in the API) support:
	- Set member functions to be called
	- Set member variables
	- Use free-functions that take the Type as a first argument (pointer or reference)
	- Support for "Factory" classes that do not expose constructor or destructor
	- Modifying memory of userdata in C++ directly affects Lua without copying, and
	- Modifying userdata in Lua directly affects C++ references/pointers
		``my_class& a = table["a"];`` 
		``my_class* a_ptr = table["a"];`` 
	- If you want a copy, just use value semantics and get copies:
		``my_class a = table["a"];``

* Thread/Coroutine support
	- Use, resume, and play with :doc:`coroutines<api/coroutine>` like regular functions
	- Get and use them even on a separate Lua :doc:`thread<api/thread>` 
	- Monitor status and get check errors

* *Advanced:* Customizable and extensible to your own types if you override :doc:`getter/pusher/checker<api/stack>` template struct definitions.


The Feature Matrix™
-------------------

The below feature table checks for the presence of something. It, however, does not actually account for any kind of laborious syntax.

✔ full support

~ partial support / wonky support

✗ no support


+---------------------------+-------------+------------+----------+---------+----------+-----------+-----------+----------------+----------+
|                           |   plain C   | luawrapper | lua-intf | luabind |  Selene  |    Sol    |   oolua   |   lua-api-pp   |  kaguya  |
|                           |             |            |          |         |          |           |           |                |          |
+===========================+=============+============+==========+=========+==========+===========+===========+================+==========+
| tables                    |      ✔      |     ✔      |     ✔    |    ✔    |     ✔    |     ✔     |     ✔     |        ✔       |     ✔    |
+---------------------------+-------------+------------+----------+---------+----------+-----------+-----------+----------------+----------+
| table chaining            |      ✔      |     ✔      |     ✔    |    ✔    |     ✔    |     ✔     |     ✔     |        ✔       |     ✔    |
+---------------------------+-------------+------------+----------+---------+----------+-----------+-----------+----------------+----------+
| arbitrary keys            |      ✔      |     ✔      |     ✔    |    ✔    |     ✔    |     ✔     |     ✔     |        ✔       |     ✔    |
+---------------------------+-------------+------------+----------+---------+----------+-----------+-----------+----------------+----------+
| user-defined types (udts) |      ~      |     ✔      |     ✔    |    ✔    |     ✔    |     ✔     |     ~     |        ✔       |     ✔    |
+---------------------------+-------------+------------+----------+---------+----------+-----------+-----------+----------------+----------+
| udts: member functions    |      ~      |     ✔      |     ✔    |    ✔    |     ✔    |     ✔     |     ~     |        ✔       |     ✔    |
+---------------------------+-------------+------------+----------+---------+----------+-----------+-----------+----------------+----------+
| udts: variables           |      ~      |     ~      |     ~    |    ~    |     ~    |     ✔     |     ~     |        ~       |     ~    |
+---------------------------+-------------+------------+----------+---------+----------+-----------+-----------+----------------+----------+
| stack abstractions        |      ~      |     ✔      |     ~    |    ✔    |     ✔    |     ✔     |     ✔     |        ✔       |     ✔    |
+---------------------------+-------------+------------+----------+---------+----------+-----------+-----------+----------------+----------+
| function binding          |      ~      |     ✔      |     ✔    |    ✔    |     ✔    |     ✔     |     ✔     |        ✔       |     ✔    |
+---------------------------+-------------+------------+----------+---------+----------+-----------+-----------+----------------+----------+
| protected function        |      ✔      |     ✗      |     ~    |    ~    |     ~    |     ✔     |     ~     |        ✔       |     ~    |
+---------------------------+-------------+------------+----------+---------+----------+-----------+-----------+----------------+----------+
| multi-return              |      ~      |     ✗      |     ✗    |    ✔    |     ✔    |     ✔     |     ~     |        ✔       |     ✔    |
+---------------------------+-------------+------------+----------+---------+----------+-----------+-----------+----------------+----------+
| inheritance               |      ~      |     ✗      |     ✗    |    ✔    |     ✔    |     ✔     |     ~     |        ~       |     ✔    |
+---------------------------+-------------+------------+----------+---------+----------+-----------+-----------+----------------+----------+
| overloading               |      ~      |     ✗      |     ✗    |    ✗    |     ✗    |     ✔     |     ✗     |        ✗       |     ✔    |
+---------------------------+-------------+------------+----------+---------+----------+-----------+-----------+----------------+----------+
| thread                    |      ✔      |     ✗      |     ✗    |    ✗    |     ✗    |     ✔     |     ✔     |        ✗       |     ✔    |
+---------------------------+-------------+------------+----------+---------+----------+-----------+-----------+----------------+----------+
| coroutines                |      ✔      |     ✗      |     ✗    |    ✔    |     ✔    |     ✔     |     ✗     |        ✗       |     ✔    |
+---------------------------+-------------+------------+----------+---------+----------+-----------+-----------+----------------+----------+
| no-rtti support           |      ✔      |     ✗      |     ~    |    ✗    |     ✗    |     ✔     |     ✔     |        ✗       |     ✔    |
+---------------------------+-------------+------------+----------+---------+----------+-----------+-----------+----------------+----------+
| no-exception support      |      ✔      |     ✗      |     ~    |    ~    |     ✗    |     ✔     |     ✔     |        ✗       |     ✔    |
+---------------------------+-------------+------------+----------+---------+----------+-----------+-----------+----------------+----------+
| Lua 5.1                   |      ✔      |     ✔      |     ~    |    ✔    |     ✗    |     ✔     |     ✔     |        ✔       |     ✔    |
+---------------------------+-------------+------------+----------+---------+----------+-----------+-----------+----------------+----------+
| Lua 5.2                   |      ✔      |     ✔      |     ✔    |    ✔    |     ✔    |     ✔     |     ✔     |        ✔       |     ✔    |
+---------------------------+-------------+------------+----------+---------+----------+-----------+-----------+----------------+----------+
| Lua 5.3                   |      ✔      |     ✔      |     ✔    |    ✔    |     ✔    |     ✔     |     ✔     |        ✔       |     ✔    |
+---------------------------+-------------+------------+----------+---------+----------+-----------+-----------+----------------+----------+
| luajit                    |      ✔      |     ✔      |     ✔    |    ✔    |     ✔    |     ✔     |     ✔     |        ✔       |     ✔    |
+---------------------------+-------------+------------+----------+---------+----------+-----------+-----------+----------------+----------+
| distribution              |   compile   |    header  |   both   | compile |  header  |   header  |  compile  |     compile    |  header  |
+---------------------------+-------------+------------+----------+---------+----------+-----------+-----------+----------------+----------+

notes on implementations
------------------------

Plain C - 

* Obviously you can do anything you want with Plain C, but the effort involved is astronomical in comparison to what frameworks offer
* Does not scale very well (in terms of developer ease of use)
* Compilation (or package manager use) is obviously required for your platform and required to use ANY of these libraries whatsoever

luawrapper -

* Takes the approach of writing and reading tables using ``readVariable`` and ``writeVariable`` functions
* C++11
* No macros
* The interface can be clunky (no table-like data structures: most things go though ``readVariable`` / ``writeVariable``)


lua-intf -

* Can be both header-only or compiled
* C++11
* Macro-based registration (strange pseudo-language)
* Fairly fast in most regards
* Registering classes/"modules" in using C++ code is extremely verbose

luabind -

* One of the older frameworks, but has many people updating it and providing "deboostified" versions
* Strange in-lua keywords and parsing to allow for classes to be written in lua
	- not sure if good feature; vendor lock-in to that library to depend on this specific class syntax?

Selene -

* member variables are automatically turned into ``obj:set_x( value )`` to set and ``obj:x()`` to get
* Registering classes/"modules" using C++ code is extremely verbose

Sol -

* Overloading support can get messy with inheritance, see :doc:`here<api/overload>`
* Only (?) library that supports table-like use of variables as shown in the :doc:`sneak peek<index>`
* C++14/"C++1y" (-std=c++14, -std=c++1y, =std=c++1z) flags are used (available since GCC 4.9 and Clang 3.5)

oolua -

* The syntax for this library is thicker than a brick. No, seriously. `Go read the docs.`_ 
* Supports not having exceptions or rtti turned on (shiny!)

.. _ fn1:

lua-api-pp -

* Compiled, but the recommendation is to add the source files directly to your project
* Userdata registration with nice, thick macros: LUAPP_USERDATA( ... ) plus a bunch of free functions that take a ``T& self`` argument
* C++11-ish in some regards

kaguya -

* Probably the closest in implementation details and interface to Sol itself
* Inspired coroutine support for Sol
* Library author (satoren) is a nice guy!
* C++11/14, or boostified (which makes it C++03 compatible)

TODO:
* SWIG - http://www.swig.org/Doc1.3/Lua.html#Lua_nn2
* SLB3 - https://code.google.com/archive/p/slb/


.. _Go read the docs.: https://oolua.org/docs/index.html