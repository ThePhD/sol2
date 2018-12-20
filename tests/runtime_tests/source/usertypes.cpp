// sol3

// The MIT License (MIT)

// Copyright (c) 2013-2018 Rapptz, ThePhD and contributors

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

#include "sol_test.hpp"

#include "common_classes.hpp"

#include <catch.hpp>

#include <iostream>
#include <list>
#include <memory>

struct thing {
	int v = 100;

	thing() {
	}
	thing(int x)
	: v(x) {
	}
};

struct self_test {
	int bark;

	self_test()
	: bark(100) {
	}

	void g(const std::string& str) {
		std::cout << str << '\n';
		bark += 1;
	}

	void f(const self_test& t) {
		std::cout << "got test" << '\n';
		if (t.bark != bark)
			throw sol::error("bark values are not the same for self_test f function");
		if (&t != this)
			throw sol::error("call does not reference self for self_test f function");
	}
};

struct matrix_xf {
	float a, b;

	static matrix_xf from_lua_table(sol::table t) {
		matrix_xf m;
		m.a = t[1][1];
		m.b = t[1][2];
		return m;
	}
};

struct matrix_xi {
	int a, b;

	static matrix_xi from_lua_table(sol::table t) {
		matrix_xi m;
		m.a = t[1][1];
		m.b = t[1][2];
		return m;
	}
};

TEST_CASE("usertype/usertype", "Show that we can create classes from usertype and use them") {
	sol::state lua;

	sol::usertype<fuser> lc = lua.new_usertype<fuser>("fuser", "add", &fuser::add, "add2", &fuser::add2);

	lua.safe_script(
		"a = fuser:new()\n"
		"b = a:add(1)\n"
		"c = a:add2(1)\n");

	sol::object a = lua.get<sol::object>("a");
	sol::object b = lua.get<sol::object>("b");
	sol::object c = lua.get<sol::object>("c");
	REQUIRE((a.is<sol::userdata_value>()));
	auto atype = a.get_type();
	auto btype = b.get_type();
	auto ctype = c.get_type();
	REQUIRE((atype == sol::type::userdata));
	REQUIRE((btype == sol::type::number));
	REQUIRE((ctype == sol::type::number));
	int bresult = b.as<int>();
	int cresult = c.as<int>();
	REQUIRE(bresult == 1);
	REQUIRE(cresult == 3);
}

TEST_CASE("usertype/usertype-constructors", "Show that we can create classes from usertype and use them with multiple constructors") {

	sol::state lua;

	sol::constructors<sol::types<>, sol::types<int>, sol::types<int, int>> con;
	sol::usertype<crapola::fuser> lc = lua.new_usertype<crapola::fuser>("fuser",
		con, 
		"add", &crapola::fuser::add, 
		"add2", &crapola::fuser::add2);

	lua.safe_script(
		"a = fuser.new(2)\n"
		"u = a:add(1)\n"
		"v = a:add2(1)\n"
		"b = fuser:new()\n"
		"w = b:add(1)\n"
		"x = b:add2(1)\n"
		"c = fuser.new(2, 3)\n"
		"y = c:add(1)\n"
		"z = c:add2(1)\n");
	sol::object a = lua.get<sol::object>("a");
	auto atype = a.get_type();
	REQUIRE((atype == sol::type::userdata));
	sol::object u = lua.get<sol::object>("u");
	sol::object v = lua.get<sol::object>("v");
	REQUIRE((u.as<int>() == 3));
	REQUIRE((v.as<int>() == 5));

	sol::object b = lua.get<sol::object>("b");
	auto btype = b.get_type();
	REQUIRE((btype == sol::type::userdata));
	sol::object w = lua.get<sol::object>("w");
	sol::object x = lua.get<sol::object>("x");
	REQUIRE((w.as<int>() == 1));
	REQUIRE((x.as<int>() == 3));

	sol::object c = lua.get<sol::object>("c");
	auto ctype = c.get_type();
	REQUIRE((ctype == sol::type::userdata));
	sol::object y = lua.get<sol::object>("y");
	sol::object z = lua.get<sol::object>("z");
	REQUIRE((y.as<int>() == 7));
	REQUIRE((z.as<int>() == 9));
}

TEST_CASE("usertype/usertype-utility", "Show internal management of classes registered through new_usertype") {
	sol::state lua;

	lua.new_usertype<fuser>("fuser", "add", &fuser::add, "add2", &fuser::add2);

	lua.safe_script(
		"a = fuser.new()\n"
		"b = a:add(1)\n"
		"c = a:add2(1)\n");

	sol::object a = lua.get<sol::object>("a");
	sol::object b = lua.get<sol::object>("b");
	sol::object c = lua.get<sol::object>("c");
	REQUIRE((a.is<sol::userdata_value>()));
	auto atype = a.get_type();
	auto btype = b.get_type();
	auto ctype = c.get_type();
	REQUIRE((atype == sol::type::userdata));
	REQUIRE((btype == sol::type::number));
	REQUIRE((ctype == sol::type::number));
	int bresult = b.as<int>();
	int cresult = c.as<int>();
	REQUIRE(bresult == 1);
	REQUIRE(cresult == 3);
}

TEST_CASE("usertype/usertype-utility-derived", "usertype classes must play nice when a derived class does not overload a publically visible base function") {
	sol::state lua;
	lua.open_libraries(sol::lib::base);
	sol::constructors<sol::types<int>> basector;
	sol::usertype<Base> baseusertype = lua.new_usertype<Base>("Base", basector, "get_num", &Base::get_num);

	lua.safe_script("base = Base.new(5)");
	{
		auto result = lua.safe_script("print(base:get_num())", sol::script_pass_on_error);
		REQUIRE(result.valid());
	}

	sol::constructors<sol::types<int>> derivedctor;
	sol::usertype<Derived> derivedusertype = lua.new_usertype<Derived>("Derived", derivedctor,
		"get_num_10", &Derived::get_num_10,
		"get_num", &Derived::get_num);

	lua.safe_script("derived = Derived.new(7)");
	lua.safe_script(
		"dgn = derived:get_num()\n"
		"print(dgn)");
	lua.safe_script(
		"dgn10 = derived:get_num_10()\n"
		"print(dgn10)");

	REQUIRE((lua.get<int>("dgn10") == 70));
	REQUIRE((lua.get<int>("dgn") == 7));
}

TEST_CASE("usertype/self-referential usertype", "usertype classes must play nice when C++ object types are requested for C++ code") {
	sol::state lua;
	lua.open_libraries(sol::lib::base);

	lua.new_usertype<self_test>("test", "g", &self_test::g, "f", &self_test::f);

	auto result = lua.safe_script(
		"local a = test.new()\n"
		"a:g(\"woof\")\n"
		"a:f(a)\n",
		sol::script_pass_on_error);
	REQUIRE(result.valid());
}

TEST_CASE("usertype/issue-number-twenty-five", "Using pointers and references from C++ classes in Lua") {
	struct test {
		int x = 0;
		test& set() {
			x = 10;
			return *this;
		}

		int get() {
			return x;
		}

		test* pget() {
			return this;
		}

		test create_get() {
			return *this;
		}

		int fun(int xa) {
			return xa * 10;
		}
	};

	sol::state lua;
	lua.open_libraries(sol::lib::base);
	lua.new_usertype<test>("test", "set", &test::set, "get", &test::get, "pointer_get", &test::pget, "fun", &test::fun, "create_get", &test::create_get);
	{
		auto result = lua.safe_script("x = test.new()", sol::script_pass_on_error);
		REQUIRE(result.valid());
	}
	{
		auto result = lua.safe_script("assert(x:set():get() == 10)", sol::script_pass_on_error);
		REQUIRE(result.valid());
	}
	{
		auto result = lua.safe_script("y = x:pointer_get()", sol::script_pass_on_error);
		REQUIRE(result.valid());
	}
	{
		auto result = lua.safe_script("y:set():get()", sol::script_pass_on_error);
		REQUIRE(result.valid());
	}
	{
		auto result = lua.safe_script("y:fun(10)", sol::script_pass_on_error);
		REQUIRE(result.valid());
	}
	{
		auto result = lua.safe_script("x:fun(10)", sol::script_pass_on_error);
		REQUIRE(result.valid());
	}
	{
		auto result = lua.safe_script("assert(y:fun(10) == x:fun(10), '...')", sol::script_pass_on_error);
		REQUIRE(result.valid());
	}
	{
		auto result = lua.safe_script("assert(y:fun(10) == 100, '...')", sol::script_pass_on_error);
		REQUIRE(result.valid());
	}
	{
		auto result = lua.safe_script("assert(y:set():get() == y:set():get(), '...')", sol::script_pass_on_error);
		REQUIRE(result.valid());
	}
	{
		auto result = lua.safe_script("assert(y:set():get() == 10, '...')", sol::script_pass_on_error);
		REQUIRE(result.valid());
	}
}

TEST_CASE("usertype/issue-number-thirty-five", "using value types created from lua-called C++ code, fixing user-defined types with constructors") {
	sol::state lua;
	lua.open_libraries(sol::lib::base);

	sol::constructors<sol::types<float, float, float>> ctor;
	sol::usertype<Vec> udata = lua.new_usertype<Vec>("Vec", ctor, "normalized", &Vec::normalized, "length", &Vec::length);

	{
		auto result = lua.safe_script(
			"v = Vec.new(1, 2, 3)\n"
			"print(v:length())");
		REQUIRE(result.valid());
	}
	{
		auto result = lua.safe_script(
			"v = Vec.new(1, 2, 3)\n"
			"print(v:normalized():length())");
		REQUIRE(result.valid());
	}
}

TEST_CASE("usertype/lua-stored-usertype", "ensure usertype values can be stored without keeping usertype object alive") {
	sol::state lua;
	lua.open_libraries(sol::lib::base);

	{
		sol::constructors<sol::types<float, float, float>> ctor;
		sol::usertype<Vec> udata = lua.new_usertype<Vec>("Vec",
			ctor,
			"normalized", &Vec::normalized,
			"length", &Vec::length);

		// usertype dies, but still usable in lua!
	}

	{
		auto result = lua.safe_script(
			"collectgarbage()\n"
			"v = Vec.new(1, 2, 3)\n"
			"print(v:length())");
		REQUIRE(result.valid());
	}

	{
		auto result = lua.safe_script(
			"v = Vec.new(1, 2, 3)\n"
			"print(v:normalized():length())");
		REQUIRE(result.valid());
	}
}

TEST_CASE("usertype/member-variables", "allow table-like accessors to behave as member variables for usertype") {
	sol::state lua;
	lua.open_libraries(sol::lib::base);
	sol::constructors<sol::types<float, float, float>> ctor;
	sol::usertype<Vec> udata = lua.new_usertype<Vec>("Vec",
		ctor,
		"x", &Vec::x,
		"y", &Vec::y,
		"z", &Vec::z,
		"normalized", &Vec::normalized,
		"length", &Vec::length);

	REQUIRE_NOTHROW(lua.safe_script(
		"v = Vec.new(1, 2, 3)\n"
		"v2 = Vec.new(0, 1, 0)\n"
		"print(v:length())\n"));
	REQUIRE_NOTHROW(lua.safe_script(
		"v.x = 2\n"
		"v2.y = 2\n"
		"print(v.x, v.y, v.z)\n"
		"print(v2.x, v2.y, v2.z)\n"));
	REQUIRE_NOTHROW(lua.safe_script(
		"assert(v.x == 2)\n"
		"assert(v2.x == 0)\n"
		"assert(v2.y == 2)\n"));
	REQUIRE_NOTHROW(lua.safe_script(
		"v.x = 3\n"
		"local x = v.x\n"
		"assert(x == 3)\n"));

	struct breaks {
		sol::function f;
	};

	lua.open_libraries(sol::lib::base);
	lua.set("b", breaks());
	lua.new_usertype<breaks>("breaks",
		"f", &breaks::f);

	breaks& b = lua["b"];
	{
		auto result = lua.safe_script("b.f = function () print('BARK!') end", sol::script_pass_on_error);
		REQUIRE(result.valid());
	}
	{
		auto result = lua.safe_script("b.f()", sol::script_pass_on_error);
		REQUIRE(result.valid());
	}
	REQUIRE_NOTHROW(b.f());
}

TEST_CASE("usertype/nonmember-functions", "let users set non-member functions that take unqualified T as first parameter to usertype") {
	sol::state lua;
	lua.open_libraries(sol::lib::base);

	lua.new_usertype<giver>("giver",
		   "gief_stuff", giver::gief_stuff,
		   "gief", &giver::gief,
		   "__tostring", [](const giver& t) {
			   return std::to_string(t.a) + ": giving value";
		   });
	lua.get<sol::table>("giver").set_function("stuff", giver::stuff);

	{
		auto result = lua.safe_script("assert(giver.stuff() == 97)", sol::script_pass_on_error);
		REQUIRE(result.valid());
	}
	{
		auto result = lua.safe_script(
			"t = giver.new()\n"
			"print(tostring(t))\n"
			"t:gief()\n"
			"t:gief_stuff(20)\n", sol::script_pass_on_error);
		REQUIRE(result.valid());
	}
	giver& g = lua.get<giver>("t");
	REQUIRE(g.a == 20);
	std::cout << "----- end of 1" << std::endl;
}

TEST_CASE("regressions/one", "issue number 48") {
	sol::state lua;
	lua.new_usertype<vars>("vars",
		"boop", &vars::boop);
	auto code =
		"beep = vars.new()\n"
		"beep.boop = 1";
	auto result1 = lua.safe_script(code, sol::script_pass_on_error);
	REQUIRE(result1.valid());
	// test for segfault
	auto my_var = lua.get<vars>("beep");
	auto& my_var_ref = lua.get<vars>("beep");
	auto* my_var_ptr = lua.get<vars*>("beep");
	REQUIRE(my_var.boop == 1);
	REQUIRE(my_var_ref.boop == 1);
	REQUIRE(my_var_ptr->boop == 1);
	REQUIRE(std::addressof(my_var_ref) == my_var_ptr);
	std::cout << "----- end of 3" << std::endl;
}

TEST_CASE("usertype/get-set-references", "properly get and set with std::ref semantics. Note that to get, we must not use Unqualified<T> on the type...") {
	std::cout << "----- in 4" << std::endl;
	sol::state lua;

	lua.new_usertype<vars>("vars",
		"boop", &vars::boop);
	vars var{};
	vars rvar{};
	std::cout << "setting beep" << std::endl;
	lua.set("beep", var);
	std::cout << "setting rbeep" << std::endl;
	lua.set("rbeep", std::ref(rvar));
	std::cout << "getting my_var" << std::endl;
	auto& my_var = lua.get<vars>("beep");
	std::cout << "setting rbeep" << std::endl;
	auto& ref_var = lua.get<std::reference_wrapper<vars>>("rbeep");
	vars& proxy_my_var = lua["beep"];
	std::reference_wrapper<vars> proxy_ref_var = lua["rbeep"];
	var.boop = 2;
	rvar.boop = 5;

	// Was return as a value: var must be diferent from "beep"
	REQUIRE_FALSE(std::addressof(var) == std::addressof(my_var));
	REQUIRE_FALSE(std::addressof(proxy_my_var) == std::addressof(var));
	REQUIRE((my_var.boop == 0));
	REQUIRE(var.boop != my_var.boop);

	REQUIRE(std::addressof(ref_var) == std::addressof(rvar));
	REQUIRE(std::addressof(proxy_ref_var.get()) == std::addressof(rvar));
	REQUIRE(rvar.boop == 5);
	REQUIRE(rvar.boop == ref_var.boop);
	std::cout << "----- end of 4" << std::endl;
}

TEST_CASE("usertype/const-pointer", "Make sure const pointers can be taken") {
	struct A_x {
		int x = 201;
	};
	struct B_foo {
		int foo(const A_x* a) {
			return a->x;
		};
	};

	sol::state lua;
	lua.new_usertype<B_foo>("B",
		"foo", &B_foo::foo);
	lua.set("a", A_x());
	lua.set("b", B_foo());
	lua.safe_script("x = b:foo(a)");
	int x = lua["x"];
	REQUIRE(x == 201);
	std::cout << "----- end of 6" << std::endl;
}

TEST_CASE("usertype/overloading", "Check if overloading works properly for usertypes") {
	struct woof {
		int var;

		int func(int x) {
			return var + x;
		}

		double func2(int x, int y) {
			return var + x + y + 0.5;
		}

		std::string func2s(int x, std::string y) {
			return y + " " + std::to_string(x);
		}
	};
	sol::state lua;
	lua.open_libraries(sol::lib::base);

	lua.new_usertype<woof>("woof",
		"var", &woof::var,
		"func", sol::overload(&woof::func, &woof::func2, &woof::func2s));

	const std::string bark_58 = "bark 58";

	REQUIRE_NOTHROW(lua.safe_script(
		"r = woof:new()\n"
		"a = r:func(1)\n"
		"b = r:func(1, 2)\n"
		"c = r:func(58, 'bark')\n"));
	REQUIRE((lua["a"] == 1));
	REQUIRE((lua["b"] == 3.5));
	REQUIRE((lua["c"] == bark_58));
	auto result = lua.safe_script("r:func(1,2,'meow')", sol::script_pass_on_error);
	REQUIRE_FALSE(result.valid());
	std::cout << "----- end of 7" << std::endl;
}

TEST_CASE("usertype/overloading_values", "ensure overloads handle properly") {
	struct overloading_test {
		int print(int i) {
			INFO("Integer print: " << i);
			return 500 + i;
		}
		int print() {
			INFO("No param print.");
			return 500;
		}
	};

	sol::state lua;
	lua.new_usertype<overloading_test>("overloading_test", sol::constructors<>(),
		"print", sol::overload(static_cast<int (overloading_test::*)(int)>(&overloading_test::print), static_cast<int (overloading_test::*)()>(&overloading_test::print)),
		"print2", sol::overload(static_cast<int (overloading_test::*)()>(&overloading_test::print), static_cast<int (overloading_test::*)(int)>(&overloading_test::print)));
	lua.set("test", overloading_test());

	sol::function f0_0 = lua.load("return test:print()");
	sol::function f0_1 = lua.load("return test:print2()");
	sol::function f1_0 = lua.load("return test:print(24)");
	sol::function f1_1 = lua.load("return test:print2(24)");
	int res = f0_0();
	int res2 = f0_1();
	int res3 = f1_0();
	int res4 = f1_1();

	REQUIRE(res == 500);
	REQUIRE(res2 == 500);

	REQUIRE(res3 == 524);
	REQUIRE(res4 == 524);
	std::cout << "----- end of 8" << std::endl;
}

TEST_CASE("usertype/reference-and-constness", "Make sure constness compiles properly and errors out at runtime") {
	struct bark {
		int var = 50;
	};
	struct woof {
		bark b;
	};

	struct nested {
		const int f = 25;
	};

	struct outer {
		nested n;
	};

	sol::state lua;
	lua.new_usertype<woof>("woof",
		"b", &woof::b);
	lua.new_usertype<bark>("bark",
		"var", &bark::var);
	lua.new_usertype<outer>("outer",
		"n", &outer::n);
	lua.set("w", woof());
	lua.set("n", nested());
	lua.set("o", outer());
	lua.set("f", sol::c_call<decltype(&nested::f), &nested::f>);
	lua.safe_script(R"(
    x = w.b
    x.var = 20
    val = w.b.var == x.var
    v = f(n);
    )");

	woof& w = lua["w"];
	bark& x = lua["x"];
	nested& n = lua["n"];
	int v = lua["v"];
	bool val = lua["val"];
	// enforce reference semantics
	REQUIRE(std::addressof(w.b) == std::addressof(x));
	REQUIRE(n.f == 25);
	REQUIRE(v == 25);
	REQUIRE(val);

	{
		auto result = lua.safe_script("f(n, 50)", sol::script_pass_on_error);
		REQUIRE_FALSE(result.valid());
	}
	{
		auto result = lua.safe_script("o.n = 25", sol::script_pass_on_error);
		REQUIRE_FALSE(result.valid());
	}
}

TEST_CASE("usertype/safety", "crash with an exception -- not a segfault -- on bad userdata calls") {
	class Test {
	public:
		void sayHello() {
			std::cout << "Hey\n";
		}
	};

	sol::state lua;
	lua.new_usertype<Test>("Test", "sayHello", &Test::sayHello);
	static const std::string code = R"(
        local t = Test.new()
        t:sayHello() --Works fine
        t.sayHello() --Uh oh.
    )";
	auto result = lua.safe_script(code, sol::script_pass_on_error);
	REQUIRE_FALSE(result.valid());
}

TEST_CASE("usertype/call_constructor", "make sure lua types can be constructed with function call constructors") {
	sol::state lua;
	lua.open_libraries(sol::lib::base);

	lua.new_usertype<thing>("thing",
		"v", &thing::v, sol::call_constructor, sol::constructors<sol::types<>, sol::types<int>>());

	lua.safe_script(R"(
t = thing(256)
)");

	thing& y = lua["t"];
	INFO(y.v);
	REQUIRE(y.v == 256);
}

TEST_CASE("usertype/call_constructor-factories", "make sure tables can be passed to factory-based call constructors") {
	sol::state lua;
	lua.open_libraries();

	lua.new_usertype<matrix_xf>("mat",
		sol::call_constructor, sol::factories(&matrix_xf::from_lua_table));

	lua.safe_script("m = mat{ {1.1, 2.2} }");

	lua.new_usertype<matrix_xi>("mati",
		sol::call_constructor, sol::factories(&matrix_xi::from_lua_table));

	lua.safe_script("mi = mati{ {1, 2} }");

	matrix_xf& m = lua["m"];
	REQUIRE(m.a == 1.1f);
	REQUIRE(m.b == 2.2f);
	matrix_xi& mi = lua["mi"];
	REQUIRE(mi.a == 1);
	REQUIRE(mi.b == 2);
}

TEST_CASE("usertype/call_constructor_2", "prevent metatable regression") {
	class class01 {
	public:
		int x = 57;
		class01() {
		}
	};

	class class02 {
	public:
		int x = 50;
		class02() {
		}
		class02(const class01& other)
		: x(other.x) {
		}
	};

	sol::state lua;

	lua.new_usertype<class01>("class01",
		sol::call_constructor, sol::constructors<sol::types<>, sol::types<const class01&>>());

	lua.new_usertype<class02>("class02",
		sol::call_constructor, sol::constructors<sol::types<>, sol::types<const class02&>, sol::types<const class01&>>());

	REQUIRE_NOTHROW(lua.safe_script(R"(
x = class01()
y = class02(x)
)"));
	class02& y = lua["y"];
	REQUIRE(y.x == 57);
}

TEST_CASE("usertype/blank_constructor", "make sure lua types cannot be constructed with arguments if a blank / empty constructor is provided") {
	sol::state lua;
	lua.open_libraries(sol::lib::base);

	lua.new_usertype<thing>("thing",
		"v", &thing::v, sol::call_constructor, sol::constructors<>());

	auto result = lua.safe_script("t = thing(256)", sol::script_pass_on_error);
	REQUIRE_FALSE(result.valid());
}

TEST_CASE("usertype/no_constructor", "make sure lua types cannot be constructed if a blank / empty constructor is provided") {
	SECTION("order1") {
		sol::state lua;
		lua.open_libraries(sol::lib::base);
		lua.new_usertype<thing>("thing",
			"v", &thing::v,
			sol::call_constructor, sol::no_constructor);
		auto result = lua.safe_script("t = thing()", sol::script_pass_on_error);
		REQUIRE_FALSE(result.valid());
	}

	SECTION("order2") {
		sol::state lua;
		lua.open_libraries(sol::lib::base);

		lua.new_usertype<thing>("thing",
			sol::call_constructor, sol::no_constructor,
			"v", &thing::v);
		auto result = lua.safe_script("t = thing.new()", sol::script_pass_on_error);
		REQUIRE_FALSE(result.valid());
	}

	SECTION("new no_constructor") {
		sol::state lua;
		lua.open_libraries(sol::lib::base);

		lua.new_usertype<thing>("thing",
			sol::meta_function::construct, sol::no_constructor);
		auto result = lua.safe_script("t = thing.new()", sol::script_pass_on_error);
		REQUIRE_FALSE(result.valid());
	}

	SECTION("call no_constructor") {
		sol::state lua;
		lua.open_libraries(sol::lib::base);

		lua.new_usertype<thing>("thing",
			sol::call_constructor, sol::no_constructor);
		auto result = lua.safe_script("t = thing()", sol::script_pass_on_error);
		REQUIRE_FALSE(result.valid());
	}
}

TEST_CASE("usertype/abstract-base-class", "Ensure that abstract base classes and such can be registered") {
	sol::state lua;
	lua.new_usertype<abstract_A>("A", "a", &abstract_A::a);
	lua.new_usertype<abstract_B>("B", sol::base_classes, sol::bases<abstract_A>());
	REQUIRE_NOTHROW([&]() {
		lua.safe_script(R"(
local b = B.new()
b:a()
		)");
	});
}

TEST_CASE("usertype/as_function", "Ensure that variables can be turned into functions by as_function") {
	class B {
	public:
		int bvar = 24;
	};

	sol::state lua;
	lua.open_libraries();
	lua.new_usertype<B>("B", "b", &B::bvar, "f", sol::as_function(&B::bvar));

	B b;
	lua.set("b", &b);
	lua.safe_script("x = b:f()");
	lua.safe_script("y = b.b");
	int x = lua["x"];
	int y = lua["y"];
	REQUIRE(x == 24);
	REQUIRE(y == 24);
}

TEST_CASE("usertype/call-initializers", "Ensure call constructors with initializers work well") {
	struct A {
		double f = 25.5;

		static void init(A& x, double f) {
			x.f = f;
		}
	};

	sol::state lua;
	lua.open_libraries();

	lua.new_usertype<A>("A",
		sol::call_constructor, sol::initializers(&A::init));

	lua.safe_script(R"(
a = A(24.3)
)");
	A& a = lua["a"];
	REQUIRE(a.f == 24.3);
}

TEST_CASE("usertype/missing-key", "make sure a missing key returns nil") {
	struct thing {};

	sol::state lua;
	lua.open_libraries(sol::lib::base);

	lua.new_usertype<thing>("thing");
	{
		auto result = lua.safe_script("v = thing.missingKey\nprint(v)", sol::script_pass_on_error);
		REQUIRE(result.valid());
	}
	sol::object o = lua["v"];
	bool isnil = o.is<sol::lua_nil_t>();
	REQUIRE(isnil);
}

TEST_CASE("usertype/basic type information", "check that we can query some basic type information") {
	struct my_thing {};

	sol::state lua;
	lua.open_libraries(sol::lib::base);

	lua.new_usertype<my_thing>("my_thing");

	lua.safe_script("obj = my_thing.new()");

	lua.safe_script("assert(my_thing.__type.is(obj))");
	lua.safe_script("assert(not my_thing.__type.is(1))");
	lua.safe_script("assert(not my_thing.__type.is(\"not a thing\"))");
	lua.safe_script("print(my_thing.__type.name)");

	lua.safe_script("assert(obj.__type.is(obj))");
	lua.safe_script("assert(not obj.__type.is(1))");
	lua.safe_script("assert(not obj.__type.is(\"not a thing\"))");
	lua.safe_script("print(obj.__type.name)");

	lua.safe_script("assert(getmetatable(my_thing).__type.is(obj))");
	lua.safe_script("assert(not getmetatable(my_thing).__type.is(1))");
	lua.safe_script("assert(not getmetatable(my_thing).__type.is(\"not a thing\"))");
	lua.safe_script("print(getmetatable(my_thing).__type.name)");

	lua.safe_script("assert(getmetatable(obj).__type.is(obj))");
	lua.safe_script("assert(not getmetatable(obj).__type.is(1))");
	lua.safe_script("assert(not getmetatable(obj).__type.is(\"not a thing\"))");
	lua.safe_script("print(getmetatable(obj).__type.name)");
}

#if !defined(_MSC_VER) || !(defined(_WIN32) && !defined(_WIN64))

TEST_CASE("usertype/noexcept-methods", "make sure noexcept functions and methods can be bound to usertypes without issues") {
	struct T {
		static int noexcept_function() noexcept {
			return 0x61;
		}

		int noexcept_method() noexcept {
			return 0x62;
		}
	};

	sol::state lua;
	lua.new_usertype<T>("T",
		"nf", &T::noexcept_function,
		"nm", &T::noexcept_method);

	lua.safe_script("t = T.new()");
	lua.safe_script("v1 = t.nf()");
	lua.safe_script("v2 = t:nm()");
	int v1 = lua["v1"];
	int v2 = lua["v2"];
	REQUIRE(v1 == 0x61);
	REQUIRE(v2 == 0x62);
}

#endif // VC++ or my path/compiler settings doing strange bullshit (but it happens on Appveyor too???)
