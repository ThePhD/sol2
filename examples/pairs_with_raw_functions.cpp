#define SOL_CHECK_ARGUMENTS 1
#include <sol.hpp>

#include <map>
#include <iostream>

struct my_thing {
	std::map<std::string, int> m{
		{ "bark", 20 },
		{ "woof", 60 },
		{ "borf", 30 },
		{ "awoo", 5 },
	};

	my_thing() {

	}
};

struct lua_iterator_state {
	typedef std::map<std::string, int>::iterator it_t;
	it_t it;
	it_t last;
	std::reference_wrapper<my_thing> source;

	lua_iterator_state(my_thing& mt) : it(mt.m.begin()), last(mt.m.end()), source(mt) {}
};

int my_next(lua_State* L) {
	// this gets called 
	// to start the first iteration, and every
	// iteration there after
	// the state you passed in pairs is argument 1
	// the key value is argument 2
	// we do not care about the key value here
	lua_iterator_state& it_state = sol::stack::get<sol::user<lua_iterator_state>>(L, 1);
	my_thing& source = it_state.source;
	auto& it = it_state.it;
	if (it == it_state.last) {
		return sol::stack::push(L, sol::lua_nil);
	}
	auto itderef = *it;
	// 2 values are returned (pushed onto the stack):
	// the key and the value
	// the state is left alone
	int pushed = sol::stack::push(L, itderef.first);
	pushed += sol::stack::push_reference(L, itderef.second);
	std::advance(it, 1);
	return pushed;
};

auto my_pairs(my_thing& mt) {
	// prepare our state
	lua_iterator_state it_state(mt);
	return std::make_tuple(&my_next, sol::user<lua_iterator_state>(std::move(it_state)), sol::lua_nil);
	// sol::user above is a space/time optimization over regular usertypes,
	// it's incompatible with regular usertypes and stores the type T directly in lua without any pretty setup
	// saves space allocation and a single dereference
};

int my_pairs(lua_State* L) {
	my_thing& mt = sol::stack::get<my_thing>(L, 1);
	lua_iterator_state it_state(mt);
	// pairs expects 3 returns:
	// the "next" function on how to advance,
	// the "table" itself or some state,
	// and an initial key value (can be nil)
	
	// next function controls iteration
	int pushed = sol::stack::push(L, my_next);
	pushed += sol::stack::push<sol::user<lua_iterator_state>>(L, std::move(it_state));
	pushed += sol::stack::push(L, sol::lua_nil);
	return pushed;
};

int main(int argc, char* argv[]) {
	std::cout << "===== pairs (using raw Lua C functions) (advanced) =====" << std::endl;

	sol::state lua;
	lua.open_libraries(sol::lib::base);

	lua.new_usertype<my_thing>("my_thing",
		sol::meta_function::pairs, my_pairs
	);

	lua.safe_script(R"(
local mt = my_thing.new()
for k, v in pairs(mt) do 
	print(k, v) 
end
)");

	std::cout << std::endl;

	return 0;
}
