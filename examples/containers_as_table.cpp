#define SOL_CHECK_ARGUMENTS
#include <sol.hpp>

#include <vector>
#include <map>
#include <cassert>
#include <iostream>

// Nota bene the signature here
// Every container-type that's a table must be wrapped in `sol::as_table_t`
// it's verbose, so feel free to use typedefs ot make it easy on you
void demo (sol::as_table_t<std::map<std::string, sol::as_table_t<std::vector<std::string>>>> src) {
	const auto& listmap = src.source;
	assert(listmap.size() == 2);
	for (const auto& kvp : listmap) {
		const std::vector<std::string>& strings = kvp.second.source;
		assert(strings.size() == 3);
		std::cout << kvp.first << " = ";
		for (const auto& s : strings) {
			std::cout << "'" << s << "'" << " ";
		}
		std::cout << std::endl;
	}
}

int main(int, char**) {
	std::cout << "=== containers retrieved from lua tables example ===" << std::endl;
	
	sol::state lua;
	// bind the function
	lua.set_function("f", &demo);
	// Call it with a table that has string sequences set to distinct keys
	lua.script(R"(
f({
	key1 = {'hello', 'there', 'world'},
	key2 = {'bark', 'borf', 'woof'}
})
	)");

	std::cout << std::endl;

	return 0;
}