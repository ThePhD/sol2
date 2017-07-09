variadic_results
================
push multiple disparate arguments into lua
------------------------------------------

.. code-block:: cpp
	
	struct variadic_results : std::vector<object> { ... };

	template <typename T>
	as_args_t<T> as_args( T&& );
