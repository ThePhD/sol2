#define SOL_CHECK_ARGUMENTS

#include <catch.hpp>
#include <sol.hpp>
#include <iostream>
#include "test_stack_guard.hpp"

TEST_CASE("environments/shadowing", "Environments can properly shadow and fallback on variables") {

	sol::state lua;
	lua["b"] = 2142;

	SECTION("no fallback") {
		sol::environment plain_env(lua, sol::create);
		lua.script("a = 24", plain_env);
		sol::optional<int> maybe_env_a = plain_env["a"];
		sol::optional<int> maybe_global_a = lua["a"];
		sol::optional<int> maybe_env_b = plain_env["b"];
		sol::optional<int> maybe_global_b = lua["b"];
		
		REQUIRE(maybe_env_a != sol::nullopt);
		REQUIRE(maybe_env_a.value() == 24);
		REQUIRE(maybe_env_b == sol::nullopt);

		REQUIRE(maybe_global_a == sol::nullopt);
		REQUIRE(maybe_global_b != sol::nullopt);
		REQUIRE(maybe_global_b.value() == 2142);
	}
	SECTION("fallback") {
		sol::environment env_with_fallback(lua, sol::create, lua.globals());
		lua.script("a = 56", env_with_fallback, sol::default_on_error);
		sol::optional<int> maybe_env_a = env_with_fallback["a"];
		sol::optional<int> maybe_global_a = lua["a"];
		sol::optional<int> maybe_env_b = env_with_fallback["b"];
		sol::optional<int> maybe_global_b = lua["b"];

		REQUIRE(maybe_env_a != sol::nullopt);
		REQUIRE(maybe_env_a.value() == 56);
		REQUIRE(maybe_env_b != sol::nullopt);
		REQUIRE(maybe_env_b.value() == 2142);

		REQUIRE(maybe_global_a == sol::nullopt);
		REQUIRE(maybe_global_b != sol::nullopt);
		REQUIRE(maybe_global_b.value() == 2142);
	}
}

TEST_CASE("environments/io", "Environment values can be read and written to") {

	sol::state lua;

	SECTION("value set before execution") {
		lua.script("a = function() return test end");

		sol::function a = lua["a"];
		sol::environment env(lua, sol::create);
		env["test"] = 5;
		sol::set_environment(a, env);

		sol::optional<int> result = a();
		REQUIRE(result != sol::nullopt);
		REQUIRE(result.value() == 5);

		sol::optional<int> globalValue = lua["test"];
		REQUIRE(globalValue == sol::nullopt);
	}
	SECTION("value set during execution") {
		lua.script("a = function() test = 5 end");

		sol::function a = lua["a"];
		sol::environment env(lua, sol::create);
		sol::set_environment(a, env);

		a();

		sol::optional<int> result = env["test"];
		REQUIRE(result != sol::nullopt);
		REQUIRE(result.value() == 5);

		sol::optional<int> globalValue = lua["test"];
		REQUIRE(globalValue == sol::nullopt);
	}
}
