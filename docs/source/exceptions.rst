exceptions
==========
since somebody is going to ask about it...
------------------------------------------

Yes, you can turn off exceptions in Sol with ``#define SOL_NO_EXCEPTIONS`` before including or by passing the command line argument that defines ``SOL_NO_EXCEPTIONS``. We don't recommend it unless you're playing with a Lua distro that also doesn't play nice with exceptions (like non-x64 versions of :ref:`LuaJIT<LuaJIT and exceptions>` ).

If you turn this off, the default `at_panic`_ function :doc:`state<api/state>` set for you will not throw. Instead, the default Lua behavior of aborting will take place (and give you no chance of escape unless you implement your own at_panic function and decide to try ``longjmp`` out).

Note that this will also disable :doc:`protected_function<api/protected_function>`'s ability to catch C++ errors you throw from C++ functions bound to Lua that you are calling through that API. So, only turn off exceptions in Sol if you're sure you're never going to use them ever. Of course, if you are ALREADY not using Exceptions, you don't have to particularly worry about this and now you can use Sol!

If there is a place where a throw statement is called or a try/catch is used and it is not hidden behind a ``#ifndef SOL_NO_EXCEPTIONS`` block, please file an issue at `issue`_ or submit your very own pull request so everyone can benefit!


inheritance
-----------

Sol uses a nifty feature of exceptions to perform infinitely scaling, compiler-correct casting of derived pointers to their base classes. If you disable this and you want to have a :doc:`user-defined type<api/usertype>` that can be used with functions that take its base class or similar, you must specify those base classes manually using the :ref:`base_classes<usertype-inheritance>` tag with the :ref:`bases\<Types...><usertype-inheritance>` arguments when you create the usertype. If you turn exceptions off and are also completely mad and turn off :doc:`run-time type information<rtti>` as well, we fallback to a id-based base class system that still requires you to specifically list the base classes using the method mentioned previously. But if you have already turned off rtti and exceptions, you must really know what you're doing, so you should be just fine!


.. _LuaJIT and exceptions:

LuaJIT and exceptions
---------------------

It is important to note that a popular 5.1 distribution of Lua, LuaJIT, has some serious `caveats regarding exceptions`_. LuaJIT's exception promises are flaky at best on x64 (64-bit) platforms, and entirely terrible on non-x64 (32-bit, ARM, etc.) platorms. The trampolines we have in place for all functions bound through conventional means in Sol will catch exceptions and turn them into Lua errors so that LuaJIT remainds unperturbed, but if you link up a C function directly yourself and throw, chances are you might have screwed the pooch.

Testing in `this closed issue`_ that it doesn't play nice on 64-bit Linux in many cases either, especially when it hits an error internal to the interpreter (and does not go through Sol). We do have tests, however, that compile for our continuous integration check-ins that check this functionality across several compilers and platforms to keep you protected and given hard, strong guarantees for what happens if you throw in a function bound by Sol. If you stray outside the realm of Sol's protection, however... Good luck.

.. _issue: https://github.com/ThePhD/sol2/issues/
.. _at_panic: http://www.Lua.org/manual/5.3/manual.html#4.6
.. _caveats regarding exceptions: http://luajit.org/extensions.html#exceptions
.. _this closed issue: https://github.com/ThePhD/sol2/issues/28