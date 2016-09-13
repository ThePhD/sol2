simple_usertype
==================
structures and classes from C++ made available to Lua code (simpler)
--------------------------------------------------------------------


This type is no different from :doc:`regular usertype<usertype>`, but allows much of its work to be done at runtime instead of compile-time. The goal here was to avoid compiler complaints about too-large usertypes (some individuals needed to register 190+ functions, and the compiler choked from the templated implementation of ``usertype``). As of Sol 2.14, this implementation has been heavily refactored to allow for all the same syntax and uses of usertype to apply here, with no caveats.

Some developers used ``simple_usertype`` to have variables autoamtically be functions. To achieve this behavior, wrap the desired variable into :doc:`sol::as_function<as_function>`.