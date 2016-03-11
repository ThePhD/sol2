run-time type information (rtti)
================================
because somebody's going to want to shut this off, too...
---------------------------------------------------------

Not compiling with C++'s run-time type information? Do a ``#define SOL_NO_RTII`` before you include ``sol.hpp`` or define ``SOL_NO_RTTI`` on your command line. Be sure to understand the :doc:`implications<api/usertype>` of doing so if you also turn off exceptions.

If you come across bugs or can't compile because there's a stray `typeid` or `typeinfo` that wasn't hidden behind a ``#ifndef SOL_NO_RTTI``, please file `an issue`_ or even make a pull request so it can be fixed for everyone.

.. _an issue: https://github.com/ThePhD/sol2/issues