#define SOL_CHECK_ARGUMENTS

#include <sol.hpp>
#include <catch.hpp>

#include <iostream>
#include <list>
#include <memory>
#include <mutex>

TEST_CASE("simple_usertype/usertypes", "Ensure that simple usertypes properly work here") {
	struct marker {
		bool value = false;
	};
	struct bark {
		int var = 50;
		marker mark;

		void fun() {
			var = 51;
		}

		int get() const {
			return var;
		}

		int set(int x) {
			var = x;
			return var;
		}

		std::string special() const {
			return mark.value ? "woof" : "pantpant";
		}

		const marker& the_marker() const {
			return mark;
		}
	};

	sol::state lua;
	lua.new_simple_usertype<bark>("bark",
		"fun", &bark::fun,
		"get", &bark::get,
		"var", sol::as_function( &bark::var ),
		"the_marker", sol::as_function(&bark::the_marker),
		"x", sol::overload(&bark::get),
		"y", sol::overload(&bark::set),
		"z", sol::overload(&bark::get, &bark::set)
	);

	lua.script("b = bark.new()");
	bark& b = lua["b"];

	lua.script("b:fun()");
	int var = b.var;
	REQUIRE(var == 51);

	lua.script("b:var(20)");
	lua.script("v = b:var()");
	int v = lua["v"];
	REQUIRE(v == 20);
	REQUIRE(b.var == 20);

	lua.script("m = b:the_marker()");
	marker& m = lua["m"];
	REQUIRE_FALSE(b.mark.value);
	REQUIRE_FALSE(m.value);
	m.value = true;
	REQUIRE(&b.mark == &m);
	REQUIRE(b.mark.value);

	sol::table barktable = lua["bark"];
	barktable["special"] = &bark::special;

	lua.script("s = b:special()");
	std::string s = lua["s"];
	REQUIRE(s == "woof");

	lua.script("b:y(24)");
	lua.script("x = b:x()");
	int x = lua["x"];
	REQUIRE(x == 24);

	lua.script("z = b:z(b:z() + 5)");
	int z = lua["z"];
	REQUIRE(z == 29);
}

TEST_CASE("simple_usertype/usertypes-constructors", "Ensure that calls with specific arguments work") {
	struct marker {
		bool value = false;
	};
	struct bark {
		int var = 50;
		marker mark;

		bark() {}
		bark(int v) : var(v) {}

		void fun() {
			var = 51;
		}

		int get() const {
			return var;
		}

		int set(int x) {
			var = x;
			return var;
		}

		std::string special() const {
			return mark.value ? "woof" : "pantpant";
		}

		const marker& the_marker() const {
			return mark;
		}
	};

	sol::state lua;
	lua.new_simple_usertype<bark>("bark",
		sol::constructors<sol::types<>, sol::types<int>>(),
		"fun", sol::protect( &bark::fun ),
		"get", &bark::get,
		"var", sol::as_function( &bark::var ),
		"the_marker", &bark::the_marker,
		"x", sol::overload(&bark::get),
		"y", sol::overload(&bark::set),
		"z", sol::overload(&bark::get, &bark::set)
	);

	lua.script("bx = bark.new(760)");
	bark& bx = lua["bx"];
	REQUIRE(bx.var == 760);

	lua.script("b = bark.new()");
	bark& b = lua["b"];

	lua.script("b:fun()");
	int var = b.var;
	REQUIRE(var == 51);

	lua.script("b:var(20)");
	lua.script("v = b:var()");
	int v = lua["v"];
	REQUIRE(v == 20);

	lua.script("m = b:the_marker()");
	marker& m = lua["m"];
	REQUIRE_FALSE(b.mark.value);
	REQUIRE_FALSE(m.value);
	m.value = true;
	REQUIRE(&b.mark == &m);
	REQUIRE(b.mark.value);

	sol::table barktable = lua["bark"];
	barktable["special"] = &bark::special;

	lua.script("s = b:special()");
	std::string s = lua["s"];
	REQUIRE(s == "woof");

	lua.script("b:y(24)");
	lua.script("x = b:x()");
	int x = lua["x"];
	REQUIRE(x == 24);

	lua.script("z = b:z(b:z() + 5)");
	int z = lua["z"];
	REQUIRE(z == 29);
}

TEST_CASE("simple_usertype/shared-ptr-regression", "simple usertype metatables should not screw over unique usertype metatables") {
	static int created = 0;
	static int destroyed = 0;
	struct test {
		test() {
			++created;
		}

		~test() {
			++destroyed;
		}
	};
	{
		std::list<std::shared_ptr<test>> tests;
		sol::state lua;
		lua.open_libraries();

		lua.new_simple_usertype<test>("test",
			"create", [&]() -> std::shared_ptr<test> {
			tests.push_back(std::make_shared<test>());
			return tests.back();
		}
		);
		REQUIRE(created == 0);
		REQUIRE(destroyed == 0);
		lua.script("x = test.create()");
		REQUIRE(created == 1);
		REQUIRE(destroyed == 0);
		REQUIRE_FALSE(tests.empty());
		std::shared_ptr<test>& x = lua["x"];
		std::size_t xuse = x.use_count();
		std::size_t tuse = tests.back().use_count();
		REQUIRE(xuse == tuse);
	}
	REQUIRE(created == 1);
	REQUIRE(destroyed == 1);
}

TEST_CASE("simple_usertype/vars", "simple usertype vars can bind various values (no ref)") {
	int muh_variable = 10;
	int through_variable = 25;

	sol::state lua;
	lua.open_libraries();

	struct test {};
	lua.new_simple_usertype<test>("test",
		"straight", sol::var(2),
		"global", sol::var(muh_variable),
		"global2", sol::var(through_variable),
		"global3", sol::var(std::ref(through_variable))
	);
	
	through_variable = 20;

	lua.script(R"(
print(test.straight)
s = test.straight
print(test.global)
g = test.global
print(test.global2)
g2 = test.global2
print(test.global3)
g3 = test.global3
)");

	int s = lua["s"];
	int g = lua["g"];
	int g2 = lua["g2"];
	int g3 = lua["g3"];
	REQUIRE(s == 2);
	REQUIRE(g == 10);
	REQUIRE(g2 == 25);
	REQUIRE(g3 == 20);
}

TEST_CASE("simple_usertype/variable-control", "test to see if usertypes respond to inheritance and variable controls") {
	class A {
	public:
		virtual void a() { throw std::runtime_error("entered base pure virtual implementation"); };
	};

	class B : public A {
	public:
		virtual void a() override { }
	};

	class sA {
	public:
		virtual void a() { throw std::runtime_error("entered base pure virtual implementation"); };
	};

	class sB : public sA {
	public:
		virtual void a() override { }
	};

	struct sV {
		int a = 10;
		int b = 20;

		int get_b() const {
			return b + 2;
		}

		void set_b(int value) {
			b = value;
		}
	};

	struct sW : sV {};

	sol::state lua;
	lua.open_libraries();

	lua.new_usertype<A>("A", "a", &A::a);
	lua.new_usertype<B>("B", sol::base_classes, sol::bases<A>());
	lua.new_simple_usertype<sA>("sA", "a", &sA::a);
	lua.new_simple_usertype<sB>("sB", sol::base_classes, sol::bases<sA>());
	lua.new_simple_usertype<sV>("sV", "a", &sV::a, "b", &sV::b, "pb", sol::property(&sV::get_b, &sV::set_b));
	lua.new_simple_usertype<sW>("sW", sol::base_classes, sol::bases<sV>());

	B b;
	lua.set("b", &b);
	lua.script("b:a()");

	sB sb;
	lua.set("sb", &sb);
	lua.script("sb:a()");

	sV sv;
	lua.set("sv", &sv);
	lua.script("print(sv.b)assert(sv.b == 20)");

	sW sw;
	lua.set("sw", &sw);
	lua.script("print(sw.a)assert(sw.a == 10)");
	lua.script("print(sw.b)assert(sw.b == 20)");
	lua.script("print(sw.pb)assert(sw.pb == 22)");
	lua.script("sw.a = 11");
	lua.script("sw.b = 21");
	lua.script("print(sw.a)assert(sw.a == 11)");
	lua.script("print(sw.b)assert(sw.b == 21)");
	lua.script("print(sw.pb)assert(sw.pb == 23)");
	lua.script("sw.pb = 25");
	lua.script("print(sw.b)assert(sw.b == 25)");
	lua.script("print(sw.pb)assert(sw.pb == 27)");
}

TEST_CASE("simple_usertype/factory-constructor-overload-usage", "simple usertypes should probably invoke types") {
	class A {
	public:
		virtual void a() { throw std::runtime_error("entered base pure virtual implementation"); };
	};

	class B : public A {
	public:
		int bvar = 24;
		virtual void a() override { }
		void f() {}
	};

	sol::state lua;
	lua.open_libraries();
	sol::constructors<sol::types<>, sol::types<const B&>> c;
	lua.new_simple_usertype<B>("B",
		sol::call_constructor, c,
		"new", sol::factories([]() { return B(); }),
		"new2", sol::initializers([](B& mem) { new(&mem)B(); }, [](B& mem, int v) { new(&mem)B(); mem.bvar = v; }),
		"f", sol::as_function(&B::bvar),
		"g", sol::overload([](B&) { return 2; }, [](B&, int v) { return v; })
	);

	lua.script("b = B()");
	lua.script("b2 = B.new()");
	lua.script("b3 = B.new2()");
	lua.script("b4 = B.new2(11)");

	lua.script("x = b:f()");
	lua.script("x2 = b2:f()");
	lua.script("x3 = b3:f()");
	lua.script("x4 = b4:f()");
	int x = lua["x"];
	int x2 = lua["x2"];
	int x3 = lua["x3"];
	int x4 = lua["x4"];
	REQUIRE(x == 24);
	REQUIRE(x2 == 24);
	REQUIRE(x3 == 24);
	REQUIRE(x4 == 11);

	lua.script("y = b:g()");
	lua.script("y2 = b2:g(3)");
	lua.script("y3 = b3:g()");
	lua.script("y4 = b4:g(3)");
	int y = lua["y"];
	int y2 = lua["y2"];
	int y3 = lua["y3"];
	int y4 = lua["y4"];
	REQUIRE(y == 2);
	REQUIRE(y2 == 3);
	REQUIRE(y3 == 2);
	REQUIRE(y4 == 3);
}

TEST_CASE("simple_usertype/runtime-append", "allow extra functions to be appended at runtime directly to the metatable itself") {
	class A {
	};

	class B : public A {
	};

	sol::state lua;
	lua.new_simple_usertype<A>("A");
	lua.new_simple_usertype<B>("B", sol::base_classes, sol::bases<A>());
	lua.set("b", std::make_unique<B>());
	lua["A"]["method"] = []() { return 200; };
	lua["B"]["method2"] = [](B&) { return 100; };
	lua.script("x = b.method()");
	lua.script("y = b:method()");

	int x = lua["x"];
	int y = lua["y"];
	REQUIRE(x == 200);
	REQUIRE(y == 200);
	
	lua.script("z = b.method2(b)");
	lua.script("w = b:method2()");
	int z = lua["z"];
	int w = lua["w"];
	REQUIRE(z == 100);
	REQUIRE(w == 100);
}

TEST_CASE("simple_usertype/destruction-test", "make sure usertypes are properly destructed and don't double-delete memory or segfault") {
	sol::state lua;

	class CrashClass {
	public:
		CrashClass() {
		}

		~CrashClass() {
			a = 10; // This will cause a crash.
		}

	private:
		int a;
	};

	lua.new_simple_usertype<CrashClass>("CrashClass",
		sol::call_constructor, sol::constructors<sol::types<>>()
		);

	lua.script(R"(
		function testCrash()
			local x = CrashClass()
			end
		)");

	for (int i = 0; i < 1000; ++i) {
		lua["testCrash"]();
	}
}

TEST_CASE("simple_usertype/table-append", "Ensure that appending to the meta table also affects the internal function table for pointers as well") {
	struct A {
		int func() {
			return 5000;
		}
	};

	sol::state lua;
	lua.open_libraries();

	lua.new_simple_usertype<A>("A");
	sol::table table = lua["A"];
	table["func"] = &A::func;
	A a;
	lua.set("a", &a);
	lua.set("pa", &a);
	lua.set("ua", std::make_unique<A>());
	REQUIRE_NOTHROW([&]{
		lua.script("assert(a:func() == 5000)");
		lua.script("assert(pa:func() == 5000)");
		lua.script("assert(ua:func() == 5000)");
	}());
}

TEST_CASE("simple_usertype/class-propogation", "make sure methods and variables from base classes work properly in SAFE_USERTYPE mode") {
	class A {
	public:
		int var = 200;
		int thing() const { return 123; }
	};

	class B : public A {
	};

	sol::state lua;
	lua.open_libraries(sol::lib::base);

	lua.new_simple_usertype<B>("B",
		sol::default_constructor,
		"thing", &B::thing,
		"var", &B::var
		);

	lua.script(R"(
		b = B.new()
		print(b.var)
		b:thing()	
)");
}

TEST_CASE("simple_usertype/call-constructor", "ensure that all kinds of call-based constructors can be serialized") {
	struct thing {};
	struct v_test {
		
	};
	struct f_test {
		int i; f_test(int i) : i(i) {}
	};
	struct i_test {
		int i; i_test(int i) : i(i) {}
	};
	struct r_test {
		int i; r_test(int i) : i(i) {}
	};

	sol::state lua;
	lua.open_libraries(sol::lib::base);

	auto f = sol::factories([]() {return f_test(30); });
	lua.new_simple_usertype<f_test>("f_test",
		sol::call_constructor, sol::factories([]() {
			return f_test(20);
		}),
		"new", f
		);

	lua.new_simple_usertype<i_test>("i_test",
		sol::call_constructor, sol::initializers([](i_test& obj) {
		new(&obj)i_test(21);
	})
		);

	lua.new_simple_usertype<r_test>("r_test",
		sol::call_constructor, [](sol::table) {
		return r_test(22);
	}
	);

	lua.script("a = f_test()");
	lua.script("b = i_test()");
	lua.script("c = r_test()");
	lua.script("d = f_test.new()");
	f_test& a = lua["a"];
	f_test& d = lua["d"];
	i_test& b = lua["b"];
	r_test& c = lua["c"];
	REQUIRE(a.i == 20);
	REQUIRE(b.i == 21);
	REQUIRE(c.i == 22);
	REQUIRE(d.i == 30);

	auto vfactories = sol::factories(
		[](const sol::table& tbl) {
			for (auto v : tbl) {
				REQUIRE(v.second.valid());
				REQUIRE(v.second.is<thing>());
			}
			return v_test();
		}
	);

	lua.new_simple_usertype<v_test>("v_test",
		sol::meta_function::construct, vfactories,
		sol::call_constructor, vfactories
		);

	lua.new_simple_usertype<thing>("thing");
	lua.script("things = {thing.new(), thing.new()}");

	SECTION("new") {
		REQUIRE_NOTHROW(lua.script("a = v_test.new(things)"));
	}
	SECTION("call_constructor") {
		REQUIRE_NOTHROW(lua.script("b = v_test(things)"));
	}
}

TEST_CASE("simple_usertype/no_constructor", "make sure simple usertype errors when no-constructor types are called") {
	struct thing {};

	SECTION("new no_constructor") {
		sol::state lua;
		lua.open_libraries(sol::lib::base);

		lua.new_simple_usertype<thing>("thing",
			sol::meta_function::construct, sol::no_constructor
			);
		REQUIRE_THROWS(lua.script("a = thing.new()"));
	}

	SECTION("call no_constructor") {
		sol::state lua;
		lua.open_libraries(sol::lib::base);

		lua.new_simple_usertype<thing>("thing",
			sol::call_constructor, sol::no_constructor
			);
		REQUIRE_THROWS(lua.script("a = thing()"));
	}	
}

TEST_CASE("simple_usertype/missing-key", "make sure a missing key returns nil") {
	struct thing {};

	sol::state lua;
	lua.open_libraries(sol::lib::base);

	lua.new_simple_usertype<thing>("thing");
	REQUIRE_NOTHROW(lua.script("print(thing.missingKey)"));
}

TEST_CASE("simple_usertype/runtime-extensibility", "Check if usertypes are runtime extensible") {
	struct thing {
		int v = 20;
		int func(int a) { return a; }
	};
	int val = 0;

	SECTION("just functions") {
		sol::state lua;
		lua.open_libraries(sol::lib::base);

		lua.new_simple_usertype<thing>("thing",
			"func", &thing::func
			);

		lua.script(R"(
t = thing.new()
		)");

		REQUIRE_THROWS([&lua]() {
			lua.script(R"(
t.runtime_func = function (a)
	return a + 50
end
		)");
		}());

		REQUIRE_THROWS([&lua]() {
			lua.script(R"(
function t:runtime_func(a)
	return a + 52
end
		)");
		}());

		lua.script("val = t:func(2)");
		val = lua["val"];
		REQUIRE(val == 2);

		REQUIRE_NOTHROW([&lua]() {
			lua.script(R"(
function thing:runtime_func(a)
	return a + 1
end
		)");
		}());

		lua.script("val = t:runtime_func(2)");
		val = lua["val"];
		REQUIRE(val == 3);
	}
	SECTION("with variable") {
		sol::state lua;
		lua.open_libraries(sol::lib::base);

		lua.new_simple_usertype<thing>("thing",
			"func", &thing::func,
			"v", &thing::v
			);

		lua.script(R"(
t = thing.new()
		)");

		REQUIRE_THROWS([&lua]() {
			lua.script(R"(
t.runtime_func = function (a)
	return a + 50
end
		)");
		}());

		REQUIRE_THROWS([&lua]() {
			lua.script(R"(
function t:runtime_func(a)
	return a + 52
end
		)");
		}());

		lua.script("val = t:func(2)");
		val = lua["val"];
		REQUIRE(val == 2);

		REQUIRE_NOTHROW([&lua]() {
			lua.script(R"(
function thing:runtime_func(a)
	return a + 1
end
		)");
		}());

		lua.script("val = t:runtime_func(2)");
		val = lua["val"];
		REQUIRE(val == 3);
	}
}

TEST_CASE("simple_usertype/runtime-replacement", "ensure that functions can be properly replaced at runtime for non-indexed things") {
	struct heart_base_t {};
	struct heart_t : heart_base_t {
		void func() {}
	};

	SECTION("plain") {
		sol::state lua;
		lua.open_libraries(sol::lib::base);

		lua.new_simple_usertype<heart_t>("a");
		REQUIRE_NOTHROW([&lua]() {
			lua.script("obj = a.new()");
			lua.script("function a:heartbeat () print('arf') return 1 end");
			lua.script("v1 = obj:heartbeat()");
			lua.script("function a:heartbeat () print('bark') return 2 end");
			lua.script("v2 = obj:heartbeat()");
			lua.script("a.heartbeat = function(self) print('woof') return 3 end");
			lua.script("v3 = obj:heartbeat()");
		}());
		int v1 = lua["v1"];
		int v2 = lua["v2"];
		int v3 = lua["v3"];
		REQUIRE(v1 == 1);
		REQUIRE(v2 == 2);
		REQUIRE(v3 == 3);
	}
	SECTION("variables") {
		sol::state lua;
		lua.open_libraries(sol::lib::base);

		lua.new_simple_usertype<heart_t>("a",
			sol::base_classes, sol::bases<heart_base_t>()
			);

		REQUIRE_NOTHROW([&lua]() {
			lua.script("obj = a.new()");
			lua.script("function a:heartbeat () print('arf') return 1 end");
			lua.script("v1 = obj:heartbeat()");
			lua.script("function a:heartbeat () print('bark') return 2 end");
			lua.script("v2 = obj:heartbeat()");
			lua.script("a.heartbeat = function(self) print('woof') return 3 end");
			lua.script("v3 = obj:heartbeat()");
		}());
		int v1 = lua["v1"];
		int v2 = lua["v2"];
		int v3 = lua["v3"];
		REQUIRE(v1 == 1);
		REQUIRE(v2 == 2);
		REQUIRE(v3 == 3);
	}
	SECTION("methods") {
		sol::state lua;
		lua.open_libraries(sol::lib::base);

		lua.new_simple_usertype<heart_t>("a",
			"func", &heart_t::func,
			sol::base_classes, sol::bases<heart_base_t>()
			);

		REQUIRE_NOTHROW([&lua]() {
			lua.script("obj = a.new()");
			lua.script("function a:heartbeat () print('arf') return 1 end");
			lua.script("v1 = obj:heartbeat()");
			lua.script("function a:heartbeat () print('bark') return 2 end");
			lua.script("v2 = obj:heartbeat()");
			lua.script("a.heartbeat = function(self) print('woof') return 3 end");
			lua.script("v3 = obj:heartbeat()");
		}());
		int v1 = lua["v1"];
		int v2 = lua["v2"];
		int v3 = lua["v3"];
		REQUIRE(v1 == 1);
		REQUIRE(v2 == 2);
		REQUIRE(v3 == 3);
	}
}

TEST_CASE("simple_usertype/meta-key-retrievals", "allow for special meta keys (__index, __newindex) to trigger methods even if overwritten directly") {
	SECTION("dynamically") {
		static int writes = 0;
		static std::string keys[4] = {};
		static int values[4] = {};
		struct d_sample {
			void foo(std::string k, int v) {
				keys[writes] = k;
				values[writes] = v;
				++writes;
			}
		};

		sol::state lua;
		lua.new_simple_usertype<d_sample>("sample");
		sol::table s = lua["sample"]["new"]();
		s[sol::metatable_key][sol::meta_function::new_index] = &d_sample::foo;
		lua["var"] = s;

		
		lua.script("var = sample.new()");
		lua.script("var.key = 2");
		lua.script("var.__newindex = 4");
		lua.script("var.__index = 3");
		lua.script("var.__call = 1");
		REQUIRE(values[0] == 2);
		REQUIRE(values[1] == 4);
		REQUIRE(values[2] == 3);
		REQUIRE(values[3] == 1);
		REQUIRE(keys[0] == "key");
		REQUIRE(keys[1] == "__newindex");
		REQUIRE(keys[2] == "__index");
		REQUIRE(keys[3] == "__call");
	}

	SECTION("statically") {
		static int writes = 0;
		static std::string keys[4] = {};
		static int values[4] = {};
		struct sample {
			void foo(std::string k, int v) {
				keys[writes] = k;
				values[writes] = v;
				++writes;
			}
		};

		sol::state lua;
		lua.new_simple_usertype<sample>("sample", sol::meta_function::new_index, &sample::foo);
		
		lua.script("var = sample.new()");
		lua.script("var.key = 2");
		lua.script("var.__newindex = 4");
		lua.script("var.__index = 3");
		lua.script("var.__call = 1");
		REQUIRE(values[0] == 2);
		REQUIRE(values[1] == 4);
		REQUIRE(values[2] == 3);
		REQUIRE(values[3] == 1);
		REQUIRE(keys[0] == "key");
		REQUIRE(keys[1] == "__newindex");
		REQUIRE(keys[2] == "__index");
		REQUIRE(keys[3] == "__call");
	}
}

TEST_CASE("simple_usertype/static-properties", "allow for static functions to get and set things as a property") {
	static int b = 50;
	struct test_t {
		static double s_func() {
			return b + 0.5;
		}

		static void g_func(int v) {
			b = v;
		}

		std::size_t func() {
			return 24;
		}
	};
	test_t manager;

	sol::state lua;

	lua.new_simple_usertype<test_t>("test",
		"f", std::function<std::size_t()>(std::bind(std::mem_fn(&test_t::func), &manager)),
		"g", sol::property(&test_t::s_func, &test_t::g_func)
		);

	lua.script("v1 = test.f()");
	lua.script("v2 = test.g");
	lua.script("test.g = 60");
	lua.script("v2a = test.g");
	int v1 = lua["v1"];
	REQUIRE(v1 == 24);
	double v2 = lua["v2"];
	REQUIRE(v2 == 50.5);
	double v2a = lua["v2a"];
	REQUIRE(v2a == 60.5);
}
