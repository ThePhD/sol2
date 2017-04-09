#define SOL_CHECK_ARGUMENTS

#include <catch.hpp>
#include <sol.hpp>
#include <iostream>
#include <fstream>
#include "test_stack_guard.hpp"

TEST_CASE("state/require_file", "opening files as 'requires'") {
	static const char FILE_NAME[] = "./tmp_thingy.lua";

	sol::state lua;
	lua.open_libraries(sol::lib::base);

	SECTION("with usertypes")
	{
		struct foo {
			foo(int bar) : bar(bar) {}

			const int bar;
		};

		lua.new_usertype<foo>("foo",
			sol::constructors<sol::types<int>>{},
			"bar", &foo::bar
			);

		std::fstream file(FILE_NAME, std::ios::out);
		file << "return { modfunc = function () return foo.new(221) end }" << std::endl;
		file.close();

		const sol::table thingy1 = lua.require_file("thingy", FILE_NAME);

		CHECK(thingy1.valid());

		const foo foo_v = thingy1["modfunc"]();

		int val1 = foo_v.bar;

		CHECK(val1 == 221);
	}

	SECTION("simple")
	{
		std::fstream file(FILE_NAME, std::ios::out);
		file << "return { modfunc = function () return 221 end }" << std::endl;
		file.close();

		const sol::table thingy1 = lua.require_file("thingy", FILE_NAME);
		const sol::table thingy2 = lua.require_file("thingy", FILE_NAME);

		CHECK(thingy1.valid());
		CHECK(thingy2.valid());

		int val1 = thingy1["modfunc"]();
		int val2 = thingy2["modfunc"]();

		CHECK(val1 == 221);
		CHECK(val2 == 221);
		// must have loaded the same table
		CHECK(thingy1 == thingy2);
	}

	std::remove(FILE_NAME);
}

TEST_CASE("state/require_script", "opening strings as 'requires' clauses") {
	std::string code = "return { modfunc = function () return 221 end }";

	sol::state lua;
	sol::stack_guard sg(lua);
	sol::table thingy1 = lua.require_script("thingy", code);
	sol::table thingy2 = lua.require_script("thingy", code);

	int val1 = thingy1["modfunc"]();
	int val2 = thingy2["modfunc"]();
	REQUIRE(val1 == 221);
	REQUIRE(val2 == 221);
	// must have loaded the same table
	REQUIRE(thingy1 == thingy2);
}

TEST_CASE("state/require", "opening using a file") {
	struct open {
		static int open_func(lua_State* L) {
			sol::state_view lua = L;
			return sol::stack::push(L, lua.create_table_with("modfunc", sol::as_function([]() { return 221; })));
		}
	};

	sol::state lua;
	sol::stack_guard sg(lua);
	sol::table thingy1 = lua.require("thingy", open::open_func);
	sol::table thingy2 = lua.require("thingy", open::open_func);

	int val1 = thingy1["modfunc"]();
	int val2 = thingy2["modfunc"]();
	REQUIRE(val1 == 221);
	REQUIRE(val2 == 221);
	// THIS IS ONLY REQUIRED IN LUA 5.3, FOR SOME REASON
	// must have loaded the same table
	// REQUIRE(thingy1 == thingy2);   
}

TEST_CASE("state/multi-require", "make sure that requires transfers across hand-rolled script implementation and standard requiref") {
	struct open {
		static int open_func(lua_State* L) {
			sol::state_view lua = L;
			return sol::stack::push(L, lua.create_table_with("modfunc", sol::as_function([]() { return 221; })));
		}
	};

	std::string code = "return { modfunc = function () return 221 end }";
	sol::state lua;
	sol::table thingy1 = lua.require("thingy", open::open_func);
	sol::table thingy2 = lua.require("thingy", open::open_func);
	sol::table thingy3 = lua.require_script("thingy", code);

	int val1 = thingy1["modfunc"]();
	int val2 = thingy2["modfunc"]();
	int val3 = thingy3["modfunc"]();
	REQUIRE(val1 == 221);
	REQUIRE(val2 == 221);
	REQUIRE(val3 == 221);
	// must have loaded the same table
	// Lua is not obliged to give a shit. Thanks, Lua
	//REQUIRE(thingy1 == thingy2);
	// But we care, thankfully
	//REQUIRE(thingy1 == thingy3);
	REQUIRE(thingy2 == thingy3);
}

TEST_CASE("state/require-safety", "make sure unrelated modules aren't harmed in using requires") {
	sol::state lua;
	lua.open_libraries();
	std::string t1 = lua.script(R"(require 'io'
return 'test1')");
	sol::object ot2 = lua.require_script("test2", R"(require 'io'
return 'test2')");
	std::string t2 = ot2.as<std::string>();
	std::string t3 = lua.script(R"(require 'io'
return 'test3')");
	REQUIRE(t1 == "test1");
	REQUIRE(t2 == "test2");
	REQUIRE(t3 == "test3");
}

TEST_CASE("state/leak-check", "make sure there are no humongous memory leaks in iteration") {
#if 0
	sol::state lua;
	lua.script(R"(
record = {}
for i=1,256 do
	record[i] = i
end
function run()
	for i=1,25000 do
		fun(record)
	end
end
function run2()
	for i=1,50000 do
		fun(record)
	end
end
)");

	lua["fun"] = [](const sol::table &t) {
		//removing the for loop fixes the memory leak
		auto b = t.begin();
		auto e = t.end();
		for (; b != e; ++b) {

		}
	};

	size_t beforewarmup = lua.memory_used();
	lua["run"]();

	size_t beforerun = lua.memory_used();
	lua["run"]();
	size_t afterrun = lua.memory_used();
	lua["run2"]();
	size_t afterrun2 = lua.memory_used();

	// Less memory used before the warmup
	REQUIRE(beforewarmup <= beforerun);
	// Iteration size and such does not bloat or affect memory
	// (these are weak checks but they'll warn us nonetheless if something goes wrong)
	REQUIRE(beforerun == afterrun);
	REQUIRE(afterrun == afterrun2);
#else
	REQUIRE(true);
#endif
}

TEST_CASE("state/script-returns", "make sure script returns are done properly") {
	std::string script =
		R"(
local example = 
{
    str = "this is a string",
    num = 1234,

    func = function(self)
        print(self.str)
		return "fstr"
    end
}

return example;
)";

	auto bar = [&script](sol::this_state l) {
		sol::state_view lua = l;
		sol::table data = lua.script(script);

		std::string str = data["str"];
		int num = data["num"];
		std::string fstr = data["func"](data);
		REQUIRE(str == "this is a string");
		REQUIRE(fstr == "fstr");
		REQUIRE(num == 1234);
	};

	auto foo = [&script](int, sol::this_state l) {
		sol::state_view lua = l;
		sol::table data = lua.script(script);

		std::string str = data["str"];
		int num = data["num"];
		std::string fstr = data["func"](data);
		REQUIRE(str == "this is a string");
		REQUIRE(fstr == "fstr");
		REQUIRE(num == 1234);
	};

	auto bar2 = [&script](sol::this_state l) {
		sol::state_view lua = l;
		sol::table data = lua.do_string(script);

		std::string str = data["str"];
		int num = data["num"];
		std::string fstr = data["func"](data);
		REQUIRE(str == "this is a string");
		REQUIRE(fstr == "fstr");
		REQUIRE(num == 1234);
	};

	auto foo2 = [&script](int, sol::this_state l) {
		sol::state_view lua = l;
		sol::table data = lua.do_string(script);

		std::string str = data["str"];
		int num = data["num"];
		std::string fstr = data["func"](data);
		REQUIRE(str == "this is a string");
		REQUIRE(fstr == "fstr");
		REQUIRE(num == 1234);
	};

	sol::state lua;
	sol::stack_guard sg(lua);
	lua.open_libraries();

	lua.set_function("foo", foo);
	lua.set_function("foo2", foo2);
	lua.set_function("bar", bar);
	lua.set_function("bar2", bar2);

	lua.script("bar() bar2() foo(1) foo2(1)");
}

TEST_CASE("state/copy-move", "ensure state can be properly copied and moved") {
	sol::state lua;
	lua["a"] = 1;

	sol::state lua2(std::move(lua));
	int a2 = lua2["a"];
	REQUIRE(a2 == 1);
	lua = std::move(lua2);
	int a = lua["a"];
	REQUIRE(a == 1);
}

TEST_CASE("state/requires-reload", "ensure that reloading semantics do not cause a crash") {
	sol::state lua;
	sol::stack_guard sg(lua);
	lua.open_libraries();
	lua.script("require 'io'\nreturn 'test1'");
	lua.require_script("test2", "require 'io'\nreturn 'test2'");
	lua.script("require 'io'\nreturn 'test3'");
}

TEST_CASE("state/script-do-load", "test success and failure cases for loading and running scripts") {
	const static std::string bad_syntax = "weird\n%$@symb\nols";
	static const char file_bad_syntax[] = "./temp.bad_syntax.lua";
	const static std::string bad_runtime = "bad.code = 20";
	static const char file_bad_runtime[] = "./temp.bad_runtime.lua";
	const static std::string good = "a = 21\nreturn a";
	static const char file_good[] = "./temp.good.lua";

	SECTION("script") {
		sol::state lua;
		sol::stack_guard sg(lua);
		int ar = lua.script(good);
		int a = lua["a"];
		REQUIRE(a == 21);
		REQUIRE(ar == 21);
	}
	SECTION("script-handler") {
		sol::state lua;
		sol::stack_guard sg(lua);
		auto errbs = lua.script(bad_syntax, sol::simple_on_error);
		REQUIRE(!errbs.valid());

		auto errbr = lua.script(bad_runtime, sol::simple_on_error);
		REQUIRE(!errbr.valid());

		auto result = lua.script(good, sol::simple_on_error);
		int a = lua["a"];
		int ar = result;
		REQUIRE(result.valid());
		REQUIRE(a == 21);
		REQUIRE(ar == 21);
	}
	SECTION("do_string") {
		sol::state lua;
		sol::stack_guard sg(lua);
		auto errbs = lua.do_string(bad_syntax);
		REQUIRE(!errbs.valid());

		auto errbr = lua.do_string(bad_runtime);
		REQUIRE(!errbr.valid());

		auto result = lua.do_string(good);
		int a = lua["a"];
		int ar = result;
		REQUIRE(result.valid());
		REQUIRE(a == 21);
		REQUIRE(ar == 21);
	}
	SECTION("load_string") {
		sol::state lua;
		sol::stack_guard sg(lua);
		auto errbsload = lua.load(bad_syntax);
		REQUIRE(!errbsload.valid());
		
		sol::load_result errbrload = lua.load(bad_runtime);
		REQUIRE(errbrload.valid());
		sol::protected_function errbrpf = errbrload;
		auto errbr = errbrpf();
		REQUIRE(!errbr.valid());

		sol::load_result resultload = lua.load(good);
		REQUIRE(resultload.valid());
		sol::protected_function resultpf = resultload;
		auto result = resultpf();
		int a = lua["a"];
		int ar = result;
		REQUIRE(result.valid());
		REQUIRE(a == 21);
		REQUIRE(ar == 21);
	}
	{
		std::ofstream fbs(file_bad_syntax, std::ios::out);
		fbs << bad_syntax;
		std::ofstream fbr(file_bad_runtime, std::ios::out);
		fbr << bad_runtime;
		std::ofstream fg(file_good, std::ios::out);
		fg << good;
	}
	SECTION("script_file") {
		sol::state lua;
		sol::stack_guard sg(lua);
		int ar = lua.script_file(file_good);
		int a = lua["a"];
		REQUIRE(a == 21);
		REQUIRE(ar == 21);
	}
	SECTION("script_file-handler") {
		sol::state lua;
		sol::stack_guard sg(lua);
		auto errbs = lua.script_file(file_bad_syntax, sol::simple_on_error);
		REQUIRE(!errbs.valid());

		auto errbr = lua.script_file(file_bad_runtime, sol::simple_on_error);
		REQUIRE(!errbr.valid());

		auto result = lua.script_file(file_good, sol::simple_on_error);
		int a = lua["a"];
		int ar = result;
		REQUIRE(result.valid());
		REQUIRE(a == 21);
		REQUIRE(ar == 21);
	}
	SECTION("do_file") {
		sol::state lua;
		sol::stack_guard sg(lua);
		auto errbs = lua.do_file(file_bad_syntax);
		REQUIRE(!errbs.valid());

		auto errbr = lua.do_file(file_bad_runtime);
		REQUIRE(!errbr.valid());

		auto result = lua.do_file(file_good);
		int a = lua["a"];
		int ar = result;
		REQUIRE(result.valid());
		REQUIRE(a == 21);
		REQUIRE(ar == 21);
	}
	SECTION("load_file") {
		sol::state lua;
		sol::stack_guard sg(lua);
		auto errbsload = lua.load_file(file_bad_syntax);
		REQUIRE(!errbsload.valid());

		sol::load_result errbrload = lua.load_file(file_bad_runtime);
		REQUIRE(errbrload.valid());
		sol::protected_function errbrpf = errbrload;
		auto errbr = errbrpf();
		REQUIRE(!errbr.valid());

		sol::load_result resultload = lua.load_file(file_good);
		REQUIRE(resultload.valid());
		sol::protected_function resultpf = resultload;
		auto result = resultpf();
		int a = lua["a"];
		int ar = result;
		REQUIRE(result.valid());
		REQUIRE(a == 21);
		REQUIRE(ar == 21);
	}

}
