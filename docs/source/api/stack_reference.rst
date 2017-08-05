stack_reference
===============
*zero-overhead object on the stack*


When you work with a :doc:`sol::reference<reference>`, the object gotten from the stack has a reference to it made in the registry, keeping it alive. If you want to work with the Lua stack directly without having any additional references made, ``sol::stack_reference`` is for you. Its API is identical to ``sol::reference`` in every way, except it contains a ``int stack_index()`` member function that allows you to retrieve the stack index.

Note that this will not pin the object since a copy is not made in the registry, meaning things can be pulled out from under it, the stack can shrink under it, things can be added onto the stack before this object's position, and what ``sol::stack_reference`` will point to will change. Please know what the Lua stack is and have discipline while managing your Lua stack when working at this level.

All of the base types have ``stack`` versions of themselves, and the APIs are identical to their non-stack forms. This includes :doc:`sol::stack_table<table>`, :doc:`sol::stack_function<function>`, :doc:`sol::stack_protected_function<protected_function>`, :doc:`sol::stack_(light\_)userdata<userdata>` and :doc:`sol::stack_object<object>`. There is a special case for ``sol::stack_function``, which has an extra type called ``sol::stack_aligned_function`` (and similar ``sol::stack_aligned_protected_function``).


stack_aligned_function
----------------------

This type is particular to working with the stack. It does not push the function object on the stack before pushing the arguments, assuming that the function present is already on the stack before going ahead and invoking the function it is targeted at. It is identical to :doc:`sol::function<function>` and has a protected counterpart as well. If you are working with the stack and know there is a callable object in the right place (i.e., at the top of the Lua stack), use this abstraction to work with the very top of the stack and have it call your function while still having the easy-to-use Lua abstractions on top.
