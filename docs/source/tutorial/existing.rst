integrating into existing code
==============================

If you're already using lua and you just want to use ``sol`` in some places, you can use ``state_view``:

.. code-block:: cpp
	:linenos:
	:caption: using state_view
	:name: state-view-snippet

	void something_in_my_system (lua_State* L) {
		// start using Sol with a pre-existing system
		sol::state_view lua(L); // non-owning

		lua.script("print('bark bark bark!')")
	}

Sol has no initialization components that need to deliberately remain alive for the duration of the program. It's entirely self-containing and uses lua's garbage collectors and various implementation techniques to require no state C++-side.