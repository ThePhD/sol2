as_returns
==========
*turn an iterable argument into a multiple-return type*


.. code-block:: cpp
	
	template <typename T>
	struct as_returns_t { ... };

	template <typename T>
	as_returns_t<T> as_returns( T&& );


This allows you to wrap up a source that has ``begin`` and ``end`` iterator-returning functions on it and return it as multiple results into Lua. To have more control over the returns, use :doc:`sol::variadic_results<variadic_results>`.


.. code-block:: cpp
	:linenos:
	:caption: as_returns.c++

	#define SOL_CHECK_ARGUMENTS
	#include <sol.hpp>

	#include <string>
	#include <set>

	int main () {
		sol::state lua;

		lua.set_function("f", []() {
			std::set<std::string> results{ "arf", "bark", "woof" };
			return sol::as_returns(std::move(results));
		});
		
		lua.script(R"(
		v1, v2, v3 = f()
		)");
		}());

		std::string v1 = lua["v1"];
		std::string v2 = lua["v2"];
		std::string v3 = lua["v3"];

		assert(v1 == "arf");
		assert(v2 == "bark");
		assert(v3 == "woof");

		return 0;		
	}
