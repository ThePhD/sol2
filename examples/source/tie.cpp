#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>


int main(int, char*[]) {

	const auto& code = R"(
	bark_power = 11;

	function woof ( bark_energy )
		return (bark_energy * (bark_power / 4))
	end
)";

	sol::state lua;

	lua.script(code);

	sol::function woof = lua["woof"];
	double numwoof = woof(20);
	SOL_ASSERT(numwoof == 55.0);

	lua.script("function f () return 10, 11, 12 end");

	sol::function f = lua["f"];
	std::tuple<int, int, int> abc = f();
	SOL_ASSERT(std::get<0>(abc) == 10);
	SOL_ASSERT(std::get<1>(abc) == 11);
	SOL_ASSERT(std::get<2>(abc) == 12);
	// or
	int a, b, c;
	sol::tie(a, b, c) = f();
	SOL_ASSERT(a == 10);
	SOL_ASSERT(b == 11);
	SOL_ASSERT(c == 12);

	return 0;
}
