#define SOL_ALL_SAFETIES_ON 1

#include <sol/sol.hpp>

struct vars {
	int boop = 0;

	int bop() const {
		return boop + 1;
	}
};

int main() {
	sol::state lua;
	lua.new_usertype<vars>(
	     "vars", "boop", &vars::boop, "bop", &vars::bop);
	lua.script(
	     "beep = vars.new()\n"
	     "beep.boop = 1\n"
	     "bopvalue = beep:bop()");

	vars& beep = lua["beep"];
	int bopvalue = lua["bopvalue"];

	SOL_ASSERT(beep.boop == 1);
	SOL_ASSERT(lua.get<vars>("beep").boop == 1);
	SOL_ASSERT(beep.bop() == 2);
	SOL_ASSERT(bopvalue == 2);

	return 0;
}
