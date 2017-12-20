// sol2 

// The MIT License (MIT)

// Copyright (c) 2013-2017 Rapptz, ThePhD and contributors

// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#define SOL_CHECK_ARGUMENTS 1
#define SOL_ENABLE_INTEROP 1

#include <catch.hpp>
#include <sol.hpp>

TEST_CASE("coroutines/yielding", "ensure calling a coroutine works") {
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

TEST_CASE("coroutines/new thread coroutines", "ensure calling a coroutine works when the work is put on a different thread") {
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

TEST_CASE("coroutines/explicit transfer", "check that the xmove constructors shift things around appropriately") {
	const std::string code = R"(
-- main thread - L1
-- co - L2
-- co2 - L3

x = co_test.new("x")
local co = coroutine.wrap(
	function()
		local t = co_test.new("t")
		local co2 = coroutine.wrap(
			function()
				local t2 = { "SOME_TABLE" }
				t:copy_store(t2) -- t2 = [L3], t.obj = [L2]
			end
		)

		co2()
		co2 = nil

		collectgarbage() -- t2 ref in t remains valid!

		x:store(t:get()) -- t.obj = [L2], x.obj = [L1]
    end
)

co()
collectgarbage()
collectgarbage()
co = nil
)";

	struct co_test {
		std::string identifier;
		sol::reference obj;

		co_test(sol::this_state L, std::string id)
		: identifier(id), obj(L, sol::lua_nil) {
		}

		void store(sol::table ref) {
			// must be explicit
			obj = sol::reference(obj.lua_state(), ref);
		}

		void copy_store(sol::table ref) {
			// must be explicit
			obj = sol::reference(obj.lua_state(), ref);
		}

		sol::reference get() {
			return obj;
		}

		~co_test() {
		}
	};

	sol::state lua;
	lua.open_libraries(sol::lib::coroutine, sol::lib::base);

	lua.new_usertype<co_test>("co_test",
		sol::constructors<co_test(sol::this_state, std::string)>(),
		"store", &co_test::store,
		"copy_store", &co_test::copy_store,
		"get", &co_test::get);

	auto r = lua.safe_script(code);
	REQUIRE(r.valid());

	co_test& ct = lua["x"];

	lua_State* Lmain1 = lua.lua_state();
	lua_State* Lmain2 = sol::main_thread(lua);
	lua_State* Lmain3 = ct.get().lua_state();
	REQUIRE(Lmain1 == Lmain2);
	REQUIRE(Lmain1 == Lmain3);

	sol::table t = ct.get();
	REQUIRE(t.size() == 1);
	std::string s = t[1];
	REQUIRE(s == "SOME_TABLE");
}

TEST_CASE("coroutines/implicit transfer", "check that copy and move assignment constructors implicitly shift things around") {
	const std::string code = R"(
-- main thread - L1
-- co - L2
-- co2 - L3

x = co_test.new("x")
local co = coroutine.wrap(
	function()
		local t = co_test.new("t")
		local co2 = coroutine.wrap(
			function()
				local t2 = { "SOME_TABLE" }
				t:copy_store(t2) -- t2 = [L3], t.obj = [L2]
			end
		)

		co2()
		co2 = nil

		collectgarbage() -- t2 ref in t remains valid!

		x:store(t:get()) -- t.obj = [L2], x.obj = [L1]
    end
)

co()
collectgarbage()
collectgarbage()
co = nil
)";

	struct co_test_implicit {
		std::string identifier;
		sol::reference obj;

		co_test_implicit(sol::this_state L, std::string id)
		: identifier(id), obj(L, sol::lua_nil) {
		}

		void store(sol::table ref) {
			// must be explicit
			obj = std::move(ref);
		}

		void copy_store(sol::table ref) {
			// must be explicit
			obj = ref;
		}

		sol::reference get() {
			return obj;
		}

		~co_test_implicit() {
		}
	};

	sol::state lua;
	lua.open_libraries(sol::lib::coroutine, sol::lib::base);

	lua.new_usertype<co_test_implicit>("co_test",
		sol::constructors<co_test_implicit(sol::this_state, std::string)>(),
		"store", &co_test_implicit::store,
		"copy_store", &co_test_implicit::copy_store,
		"get", &co_test_implicit::get);

	auto r = lua.safe_script(code);
	REQUIRE(r.valid());

	co_test_implicit& ct = lua["x"];

	lua_State* Lmain1 = lua.lua_state();
	lua_State* Lmain2 = sol::main_thread(lua);
	lua_State* Lmain3 = ct.get().lua_state();
	REQUIRE(Lmain1 == Lmain2);
	REQUIRE(Lmain1 == Lmain3);

	sol::table t = ct.get();
	REQUIRE(t.size() == 1);
	std::string s = t[1];
	REQUIRE(s == "SOME_TABLE");
}

TEST_CASE("coroutines/main transfer", "check that copy and move assignment constructors using main-forced types work") {
	const std::string code = R"(
-- main thread - L1
-- co - L2
-- co2 - L3

x = co_test.new("x")
local co = coroutine.wrap(
	function()
		local t = co_test.new("t")
		local co2 = coroutine.wrap(
			function()
				local t2 = { "SOME_TABLE" }
				t:copy_store(t2) -- t2 = [L3], t.obj = [L2]
			end
		)

		co2()
		co2 = nil

		collectgarbage() -- t2 ref in t remains valid!

		x:store(t:get()) -- t.obj = [L2], x.obj = [L1]
    end
)

co()
co = nil
collectgarbage()
)";

	struct co_test_implicit {
		std::string identifier;
		sol::main_reference obj;

		co_test_implicit(sol::this_state L, std::string id)
		: identifier(id), obj(L, sol::lua_nil) {
		}

		void store(sol::table ref) {
			// main_reference does the state shift implicitly
			obj = std::move(ref);
			lua_State* Lmain = sol::main_thread(ref.lua_state());
			REQUIRE(obj.lua_state() == Lmain);
		}

		void copy_store(sol::table ref) {
			// main_reference does the state shift implicitly
			obj = ref;
			lua_State* Lmain = sol::main_thread(ref.lua_state());
			REQUIRE(obj.lua_state() == Lmain);
		}

		sol::reference get() {
			return obj;
		}

		~co_test_implicit() {
		}
	};

	sol::state lua;
	lua.open_libraries(sol::lib::coroutine, sol::lib::base);

	lua.new_usertype<co_test_implicit>("co_test",
		sol::constructors<co_test_implicit(sol::this_state, std::string)>(),
		"store", &co_test_implicit::store,
		"copy_store", &co_test_implicit::copy_store,
		"get", &co_test_implicit::get);

	auto r = lua.safe_script(code);
	REQUIRE(r.valid());

	co_test_implicit& ct = lua["x"];

	lua_State* Lmain1 = lua.lua_state();
	lua_State* Lmain2 = sol::main_thread(lua);
	lua_State* Lmain3 = ct.get().lua_state();
	REQUIRE(Lmain1 == Lmain2);
	REQUIRE(Lmain1 == Lmain3);

	sol::table t = ct.get();
	REQUIRE(t.size() == 1);
	std::string s = t[1];
	REQUIRE(s == "SOME_TABLE");
}

TEST_CASE("coroutines/coroutine.create protection", "ensure that a thread picked up from coroutine.create does not throw off the lua stack entirely when called from C++") {
	sol::state lua;
	lua.open_libraries(sol::lib::base, sol::lib::coroutine);

	lua.script(
		R"(
function loop()
	local i = 0
	while true do
		print("pre-yield in loop")
		coroutine.yield(i)
		print("post-yield in loop")
		i = i+1
	end
end
loop_th = coroutine.create(loop)
)"
);

	sol::thread runner_thread = lua["loop_th"];

	auto test_resume = [&runner_thread]() {
		sol::state_view th_state = runner_thread.state();
		sol::coroutine cr = th_state["loop"];
		int r = cr();
		return r;
	};

	lua.set_function("test_resume", std::ref(test_resume));

	int v0 = test_resume();
	int v1 = test_resume();
	int v2 = lua.script("return test_resume()");
	int v3 = lua.script("return test_resume()");
	REQUIRE(v0 == 0);
	REQUIRE(v1 == 1);
	REQUIRE(v2 == 2);
	REQUIRE(v3 == 3);
}

TEST_CASE("coroutines/stack-check", "check that resumed functions consume the entire execution stack") {
	sol::state lua;
	lua.open_libraries(sol::lib::base, sol::lib::table, sol::lib::coroutine);

	lua.script(
		R"(
unpack = unpack or table.unpack

function loop()
    local i = 0
    while true do
        print("pre-yield in loop")
        coroutine.yield(i)
        print("post-yield in loop")
        i = i+1
    end
end
loop_th = coroutine.create(loop)
loop_res = function(...) 
	returns = { coroutine.resume(loop_th, ...) }
	return unpack(returns, 2)
end 
)"
);

	// Resume from lua via thread and coroutine
	sol::thread runner_thread = lua["loop_th"];
	sol::state_view runner_thread_state = runner_thread.state();
	auto test_resume = [&runner_thread, &runner_thread_state]() {
		sol::coroutine cr = runner_thread_state["loop"];
		sol::stack::push(runner_thread_state, 50);
		sol::stack::push(runner_thread_state, 25);
		int r = cr();
		return r;
	};
	lua.set_function("test_resume", std::ref(test_resume));

	// Resume via getting a sol::function from the state
	sol::function test_resume_lua = lua["loop_res"];

	// Resume via passing a sol::function object
	auto test_resume_func = [](sol::function f) {
		int r = f();
		return r;
	};
	lua.set_function("test_resume_func", std::ref(test_resume_func));

	int v0 = test_resume();
	int s0 = runner_thread_state.stack_top();
	int v1 = test_resume();
	int s1 = runner_thread_state.stack_top();
	int v2 = lua.script("return test_resume()");
	int s2 = runner_thread_state.stack_top();
	int v3 = lua.script("return test_resume()");
	int s3 = runner_thread_state.stack_top();
	int v4 = test_resume_lua();
	int s4 = runner_thread_state.stack_top();
	int v5 = lua.script("return test_resume_func(loop_res)");
	int s5 = runner_thread_state.stack_top();
	REQUIRE(v0 == 0);
	REQUIRE(v1 == 1);
	REQUIRE(v2 == 2);
	REQUIRE(v3 == 3);
	REQUIRE(v4 == 4);
	REQUIRE(v5 == 5);

	REQUIRE(s0 == 0);
	REQUIRE(s1 == 0);
	REQUIRE(s2 == 0);
	REQUIRE(s3 == 0);
	REQUIRE(s4 == 0);
	REQUIRE(s5 == 0);
}
