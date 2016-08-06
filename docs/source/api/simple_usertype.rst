simple_usertype
==================
structures and classes from C++ made available to Lua code (simpler)
--------------------------------------------------------------------


This type is no different from :doc:`regular usertype<usertype>`, but with the following caveats:

* Dot (".") syntax is not natively supported for simple usertypes (e.g., typical member variable / property bindings)
    - All member variables become functions of that name and are get/set in Lua with the syntax ``local v = obj:value()`` to get, and ``obj:value(24)`` to set
    - :doc:`properties<property>` also become functions, similar to how member variables are treated above
    - ``sol::var`` takes the wrapped up type and pushes it directly into that named slot
* Automatic "__index" and "__newindex" handling is not done
    - Overriding either of these properties leaves it entirely up to you to handle how you find variables
    - If you override "__index" or "__newindex", you must perform a raw get on the original table and return a valid function / value if you want it to find the members you already set on the ``simple_usertype``
