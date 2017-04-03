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
	SECTION("from name") {
		sol::environment env_with_fallback(lua, sol::create, lua.globals());
		lua["env"] = env_with_fallback;
		sol::environment env = lua["env"];
		lua.script("a = 56", env, sol::default_on_error);
		sol::optional<int> maybe_env_a = env["a"];
		sol::optional<int> maybe_global_a = lua["a"];
		sol::optional<int> maybe_env_b = env["b"];
		sol::optional<int> maybe_global_b = lua["b"];

		REQUIRE(maybe_env_a != sol::nullopt);
		REQUIRE(maybe_env_a.value() == 56);
		REQUIRE(maybe_env_b != sol::nullopt);
		REQUIRE(maybe_env_b.value() == 2142);

		REQUIRE(maybe_global_a == sol::nullopt);
		REQUIRE(maybe_global_b != sol::nullopt);
		REQUIRE(maybe_global_b.value() == 2142);
	}
	SECTION("name with newtable") {
		lua["blank_env"] = sol::new_table(0, 1);
		sol::environment plain_env = lua["blank_env"];
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
}

TEST_CASE("environments/functions", "see if environments on functions are working properly") {

	SECTION("basic") {
		sol::state lua;

		lua.script("a = function() return 5 end");

		sol::function a = lua["a"];

		int result0 = a();
		REQUIRE(result0 == 5);

		sol::environment env(lua, sol::create);
		sol::set_environment(env, a);

		int result1 = a();
		REQUIRE(result1 == 5);
	}
	SECTION("return environment value") {
		sol::state lua;

		lua.script("a = function() return test end");

		sol::function a = lua["a"];
		sol::environment env(lua, sol::create);
		env["test"] = 5;
		env.set_on(a);

		// the function returns the value from the environment table
		int result = a();
		REQUIRE(result == 5);
	}

	SECTION("set environment value") {
		sol::state lua;
		lua.script("a = function() test = 5 end");

		sol::function a = lua["a"];
		sol::environment env(lua, sol::create);
		sol::set_environment(env, a);

		a();

		// the value can be retrieved from the env table
		int result = env["test"];
		REQUIRE(result == 5);

		// the global environment is not polluted
		auto gtest = lua["test"];
		REQUIRE(!gtest.valid());
	}
}
