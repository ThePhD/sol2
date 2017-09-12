#define SOL_CHECK_ARGUMENTS

#include <catch.hpp>
#include <sol.hpp>

TEST_CASE("threading/coroutines", "ensure calling a coroutine works") {
	const auto& script = R"(counter = 20
 
function loop()
    while counter ~= 30
    do
        coroutine.yield(counter);
        counter = counter + 1;
    end
    return counter
end
)";

	sol::state lua;
	lua.open_libraries(sol::lib::base, sol::lib::coroutine);
	lua.safe_script(script);
	sol::coroutine cr = lua["loop"];

	int counter;
	for (counter = 20; counter < 31 && cr; ++counter) {
		int value = cr();
		if (counter != value) {
			throw std::logic_error("fuck");
		}
	}
	counter -= 1;
	REQUIRE(counter == 30);
}

TEST_CASE("threading/new thread coroutines", "ensure calling a coroutine works when the work is put on a different thread") {
	const auto& script = R"(counter = 20
 
function loop()
    while counter ~= 30
    do
        coroutine.yield(counter);
        counter = counter + 1;
    end
    return counter
end
)";

	sol::state lua;
	lua.open_libraries(sol::lib::base, sol::lib::coroutine);
	lua.safe_script(script);
	sol::thread runner = sol::thread::create(lua.lua_state());
	sol::state_view runnerstate = runner.state();
	sol::coroutine cr = runnerstate["loop"];

	int counter;
	for (counter = 20; counter < 31 && cr; ++counter) {
		int value = cr();
		if (counter != value) {
			throw std::logic_error("fuck");
		}
	}
	counter -= 1;
	REQUIRE(counter == 30);
}

TEST_CASE("coroutines/transfer", "test that things created inside of a coroutine can have their state transferred using lua_xmove constructors") {
	for (std::size_t tries = 0; tries < 200; ++tries) {
		sol::state lua;
		lua.open_libraries();
		{
			sol::function f2;
			lua["f"] = [&lua, &f2](sol::object t) {
				f2 = sol::function(lua, t);
			};

			lua.script(R"(
i = 0
function INIT()
	co = coroutine.create(
		function()
			local g = function() i = i + 1 end
			f(g)
			g = nil
			collectgarbage()
		end
	)
	coroutine.resume(co)
	co = nil
	collectgarbage()
end
)");
			sol::function f3;
			sol::function f1;

			lua.safe_script("INIT()");
			f2();
			sol::function update = lua.safe_script("return function() collectgarbage() end");
			update();
			f3 = f2;
			f3();
			update();
			f1 = f2;
			f1();
			update();
			int i = lua["i"];
			REQUIRE(i == 3);
		}
	}
}
