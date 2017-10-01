#define SOL_CHECK_ARGUMENTS

#include <catch.hpp>
#include <sol.hpp>

TEST_CASE("storage/registry construction", "ensure entries from the registry can be retrieved") {
	const auto& code = R"(
function f()
    return 2
end
)";

	sol::state lua;
	lua.script(code);
	sol::function f = lua["f"];
	sol::reference r = lua["f"];
	sol::function regf(lua, f);
	sol::reference regr(lua, sol::ref_index(f.registry_index()));
	bool isequal = f == r;
	REQUIRE(isequal);
	isequal = f == regf;
	REQUIRE(isequal);
	isequal = f == regr;
	REQUIRE(isequal);
}

TEST_CASE("storage/registry construction empty", "ensure entries from the registry can be retrieved") {
	sol::state lua;
	sol::function f = lua["f"];
	sol::reference r = lua["f"];
	sol::function regf(lua, f);
	sol::reference regr(lua, sol::ref_index(f.registry_index()));
	bool isequal = f == r;
	REQUIRE(isequal);
	isequal = f == regf;
	REQUIRE(isequal);
	isequal = f == regr;
	REQUIRE(isequal);
}

TEST_CASE("storage/main thread", "ensure round-tripping and pulling out thread data even on 5.1 with a backup works") {
	sol::state lua;
	{
		sol::stack_guard g(lua);
		lua_State* orig = lua;
		lua_State* ts = sol::main_thread(lua, lua);
		REQUIRE(ts == orig);
	}
	{
		sol::stack_guard g(lua);
		lua_State* orig = lua;
		lua_State* ts = sol::main_thread(lua);
		REQUIRE(ts == orig);
	}
}

TEST_CASE("storage/alignment", "ensure entries in lua are properly aligned") {
	sol::state lua;
	struct alignas(64) aligned_struct {
		float val;
	};
	REQUIRE(std::alignment_of<aligned_struct>() == 64);
	for (int i = 0; i < 10; i++) { // the allocator may return aligned memory by accident, so retry a few times
		aligned_struct mystructonstack;
		mystructonstack.val = 1.f;
		sol::object mystructobject = sol::make_object(lua, mystructonstack);
		REQUIRE(mystructobject.is<aligned_struct*>());
		aligned_struct* mystruct = mystructobject.as<aligned_struct*>();
		REQUIRE(&mystructonstack != mystruct);
		REQUIRE(mystruct->val == 1.f);
		std::uintptr_t p = (std::uintptr_t)mystruct;
		REQUIRE(p % std::alignment_of<aligned_struct>() == 0);
	}
}
