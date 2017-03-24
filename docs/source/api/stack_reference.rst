stack_reference
===============
zero-overhead object on the stack
---------------------------------

When you work with a :doc:`sol::reference<reference>`, the object gotten from the stack has a reference to it made in the registry, keeping it alive. If you want to work with the Lua stack directly without having any additional references made, ``sol::stack_reference`` is for you. Its API is identical to ``sol::reference`` in every way, except it contains a ``int stack_index()`` member function that allows you to retrieve the stack index.

Note that this will not pin the object since a copy is not made in the registry, meaning things can be pulled out from under it, the stack can shrink under it, things can be added onto the stack before this object's position, and what ``sol::stack_reference`` will point to will change. Please know what the Lua stack is and have discipline while managing your Lua stack when working at this level.

All of the base types have ``stack`` versions of themselves, and the APIs are identical to their non-stack forms. This includes :doc:`sol::stack_table<table>`, :doc:`sol::stack_function<function>`, :doc:`sol::stack_protected_function<protected_function>`, :doc:`sol::stack_(light\_)userdata<userdata>` and :doc:`sol::stack_object<object>`.