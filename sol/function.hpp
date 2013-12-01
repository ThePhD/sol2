#ifndef SOL_FUNCTION_HPP
#define SOL_FUNCTION_HPP

#include "reference.hpp"
#include "tuple.hpp"
#include "stack.hpp"

namespace sol {
class function : virtual public reference {
private:
	template<typename... Args>
	void push_args(Args&&... args) {
		auto L = state();
		using swallow = char [];
		void( swallow{ (stack::push(L, std::forward<Args>(args)), '\0')... } );
	}
public:
	function() : reference() {}
	function(lua_State* L, int index = -1) : reference(L, index) {
		type_assert(L, index, type::function);
	}

	template<typename T, typename... Args>
	T invoke(Args&&... args) {
		push();
		push_args(std::forward<Args>(args)...);
		lua_pcall(state(), sizeof...(Args), 1, 0);
		return stack::pop<T>(state());
	}

	template<typename... Args>
	void invoke(Args&&... args) {
		push();
		push_args(std::forward<Args>(args)...);
		lua_pcall(state(), sizeof...(Args), 0, 0);
	}
};
} // sol


#endif // SOL_FUNCTION_HPP