#define SOL_CHECK_ARGUMENTS

#include <sol.hpp>
#include <catch.hpp>

#include <iterator>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>

auto test_table_return_one() {
	return sol::as_table(std::vector<int>{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 });
}

auto test_table_return_two() {
	return sol::as_table(std::vector<std::pair<std::string, int>>{ { "one", 1 },{ "two", 2 },{ "three", 3 } });
}

auto test_table_return_three() {
	return sol::as_table(std::map<std::string, std::string>{ { "name", "Rapptz" },{ "friend", "ThePhD" },{ "project", "sol" } });
}

auto test_table_return_four() {
	return sol::as_table(std::array<std::pair<std::string, int>, 4>{ { { "one", 1 },{ "two", 2 },{ "three", 3 },{ "four", 4 } } });
}

TEST_CASE("containers/returns", "make sure that even references to vectors are being serialized as tables") {
	sol::state lua;
	std::vector<int> v{ 1, 2, 3 };
	lua.set_function("f", [&]() -> std::vector<int>& {
		return v;
	});
	lua.script("x = f()");
	sol::object x = lua["x"];
	sol::type xt = x.get_type();
	REQUIRE(xt == sol::type::userdata);
	sol::table t = x;
	bool matching;
	matching = t[1] == 1;
	REQUIRE(matching);
	matching = t[2] == 2;
	REQUIRE(matching);
	matching = t[3] == 3;
	REQUIRE(matching);
}

TEST_CASE("containers/vector roundtrip", "make sure vectors can be round-tripped") {
	sol::state lua;
	std::vector<int> v{ 1, 2, 3 };
	lua.set_function("f", [&]() -> std::vector<int>& {
		return v;
	});
	lua.script("x = f()");
	std::vector<int> x = lua["x"];
	bool areequal = x == v;
	REQUIRE(areequal);
}

TEST_CASE("containers/list roundtrip", "make sure lists can be round-tripped") {
	sol::state lua;
	std::list<int> v{ 1, 2, 3 };
	lua.set_function("f", [&]() -> std::list<int>& {
		return v;
	});
	lua.script("x = f()");
	std::list <int> x = lua["x"];
	bool areequal = x == v;
	REQUIRE(areequal);
}

TEST_CASE("containers/map roundtrip", "make sure maps can be round-tripped") {
	sol::state lua;
	std::map<std::string, int> v{ { "a", 1 },{ "b", 2 },{ "c", 3 } };
	lua.set_function("f", [&]() -> std::map<std::string, int>& {
		return v;
	});
	lua.script("x = f()");
	std::map<std::string, int> x = lua["x"];
	bool areequal = x == v;
	REQUIRE(areequal);
}

TEST_CASE("containers/unordered_map roundtrip", "make sure unordered_maps can be round-tripped") {
	sol::state lua;
	std::unordered_map<std::string, int> v{ { "a", 1 },{ "b", 2 },{ "c", 3 } };
	lua.set_function("f", [&]() -> std::unordered_map<std::string, int>& {
		return v;
	});
	lua.script("x = f()");
	std::unordered_map<std::string, int> x = lua["x"];
	bool areequal = x == v;
	REQUIRE(areequal);
}

TEST_CASE("containers/unordered_set roundtrip", "make sure unordered_sets can be round-tripped") {
	sol::state lua;
	std::unordered_set<int> v{ 1, 2, 3 };
	lua.set_function("f", [&]() -> std::unordered_set<int>& {
		return v;
	});
	lua.script("x = f()");
	std::unordered_set<int> x = lua["x"];
	bool areequal = x == v;
	REQUIRE(areequal);
}

TEST_CASE("containers/set roundtrip", "make sure sets can be round-tripped") {
	sol::state lua;
	std::set<int> v{ 1, 2, 3 };
	lua.set_function("f", [&]() -> std::set<int>& {
		return v;
	});
	lua.script("x = f()");
	std::set<int> x = lua["x"];
	bool areequal = x == v;
	REQUIRE(areequal);
}

TEST_CASE("containers/custom usertype", "make sure container usertype metatables can be overridden") {
	typedef std::unordered_map<int, int> bark;
	
	sol::state lua;
	lua.open_libraries();
	lua.new_usertype<bark>("bark",
		"something", [](const bark& b) {
			INFO("It works: " << b.at(24));
		},
		"size", &bark::size,
		"at", sol::resolve<const int&>(&bark::at),
		"clear", &bark::clear
		);
	bark obj{ { 24, 50 } };
	lua.set("a", &obj);
	REQUIRE_NOTHROW(lua.script("assert(a:at(24) == 50)"));
	REQUIRE_NOTHROW(lua.script("a:something()"));
	lua.set("a", obj);
	REQUIRE_NOTHROW(lua.script("assert(a:at(24) == 50)"));
	REQUIRE_NOTHROW(lua.script("a:something()"));
}

TEST_CASE("containers/const serialization kvp", "make sure const keys / values are respected") {
	typedef std::map<int, const int> bark;

	sol::state lua;
	lua.open_libraries();
	bark obj{ { 24, 50 } };
	lua.set("a", &obj);
	REQUIRE_NOTHROW(lua.script("assert(a[24] == 50)"));
	REQUIRE_THROWS(lua.script("a[24] = 51"));
	REQUIRE_NOTHROW(lua.script("assert(a[24] == 50)"));
}

TEST_CASE("containers/basic serialization", "make sure containers are turned into proper userdata and have basic hooks established") {
	typedef std::vector<int> woof;
	sol::state lua;
	lua.open_libraries();
	lua.set("b", woof{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30 });
	REQUIRE_NOTHROW(
	lua.script("for k = 1, #b do assert(k == b[k]) end")
	);
	woof w{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30 };
	lua.set("b", w);
	REQUIRE_NOTHROW(
		lua.script("for k = 1, #b do assert(k == b[k]) end")
	);
	lua.set("b", &w);
	REQUIRE_NOTHROW(
		lua.script("for k = 1, #b do assert(k == b[k]) end")
	);
	lua.set("b", std::ref(w));
	REQUIRE_NOTHROW(
		lua.script("for k = 1, #b do assert(k == b[k]) end")
	);
}

#if 0 // glibc is a fuccboi
TEST_CASE("containers/const-serialization", "make sure containers are turned into proper userdata and the basic hooks respect const-ness") {
	typedef std::vector<const int> woof;
	sol::state lua;
	lua.open_libraries();
	lua.set("b", woof{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30 });
	REQUIRE_NOTHROW(
		lua.script("for k, v in pairs(b) do assert(k == v) end");
	);
	REQUIRE_THROWS(lua.script("b[1] = 20"));
}
#endif // Fuck you, glibc

TEST_CASE("containers/table serialization", "ensure types can be serialized as tables still") {
	typedef std::vector<int> woof;
	sol::state lua;
	lua.open_libraries();
	lua.set("b", sol::as_table(woof{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30 }));
	REQUIRE_NOTHROW(
		lua.script("for k, v in ipairs(b) do assert(k == v) end")
	);
	woof w{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30 };
	lua.set("b", sol::as_table(w));
	REQUIRE_NOTHROW(
	lua.script("for k, v in ipairs(b) do assert(k == v) end")
	);
	lua.set("b", sol::as_table(&w));
	REQUIRE_NOTHROW(
	lua.script("for k, v in ipairs(b) do assert(k == v) end")
	);
	lua.set("b", sol::as_table(std::ref(w)));
	REQUIRE_NOTHROW(
	lua.script("for k, v in ipairs(b) do assert(k == v) end")
	);
}

TEST_CASE("containers/const correctness", "usertype metatable names should reasonably ignore const attributes") {
	struct Vec {
		int x, y, z;
	};

	sol::state lua;
	lua.open_libraries(sol::lib::base);
	lua.new_usertype<Vec>("Vec", "x", &Vec::x, "y", &Vec::y, "z", &Vec::z);

	Vec vec;
	vec.x = 1;
	vec.y = 2;
	vec.z = -3;

	std::vector<Vec> foo;
	foo.push_back(vec);

	std::vector<Vec const *> bar;
	bar.push_back(&vec);

	lua.script(R"(
func = function(vecs)
    for i = 1, #vecs do
		vec = vecs[i]
        print(i, ":", vec.x, vec.y, vec.z)
    end
end
)");

	REQUIRE_NOTHROW([&]{
		lua["func"](foo);
		lua["func"](bar);
	}());
}

TEST_CASE("containers/arbitrary creation", "userdata and tables should be usable from standard containers") {
	sol::state lua;
	lua.open_libraries(sol::lib::base);
	lua.set_function("test_one", test_table_return_one);
	lua.set_function("test_two", test_table_return_two);
	lua.set_function("test_three", test_table_return_three);
	lua.set_function("test_four", test_table_return_four);

	REQUIRE_NOTHROW(lua.script("a = test_one()"));
	REQUIRE_NOTHROW(lua.script("b = test_two()"));
	REQUIRE_NOTHROW(lua.script("c = test_three()"));
	REQUIRE_NOTHROW(lua.script("d = test_four()"));

	REQUIRE_NOTHROW(lua.script("assert(#a == 10, 'error')"));
	REQUIRE_NOTHROW(lua.script("assert(a[3] == 3, 'error')"));
	REQUIRE_NOTHROW(lua.script("assert(b.one == 1, 'error')"));
	REQUIRE_NOTHROW(lua.script("assert(b.three == 3, 'error')"));
	REQUIRE_NOTHROW(lua.script("assert(c.name == 'Rapptz', 'error')"));
	REQUIRE_NOTHROW(lua.script("assert(c.project == 'sol', 'error')"));
	REQUIRE_NOTHROW(lua.script("assert(d.one == 1, 'error')"));
	REQUIRE_NOTHROW(lua.script("assert(d.three == 3, 'error')"));
	REQUIRE_NOTHROW(lua.script("assert(d.four == 4, 'error')"));

	sol::table a = lua.get<sol::table>("a");
	sol::table b = lua.get<sol::table>("b");
	sol::table c = lua.get<sol::table>("c");
	sol::table d = lua["d"];

	REQUIRE(a.size() == 10ULL);
	REQUIRE(a.get<int>(3) == 3);
	REQUIRE(b.get<int>("one") == 1);
	REQUIRE(b.get<int>("three") == 3);
	REQUIRE(c.get<std::string>("name") == "Rapptz");
	REQUIRE(c.get<std::string>("project") == "sol");
	REQUIRE(d.get<int>("one") == 1);
	REQUIRE(d.get<int>("three") == 3);
	REQUIRE(d.get<int>("four") == 4);
}

TEST_CASE("containers/extra functions", "make sure the manipulation functions are present and usable and working across various container types") {
	sol::state lua;
	lua.open_libraries();

	lua.script(R"(
function g (x)
	x:add(20)
end

function h (x)
	x:add(20, 40)
end

function i (x)
	x:clear()
end

function sf (x,v)
	return x:find(v)
end

)");

	// Have the function we 
	// just defined in Lua
	sol::function g = lua["g"];
	sol::function h = lua["h"];
	sol::function i = lua["i"];
	sol::function sf = lua["sf"];

	// Set a global variable called 
	// "arr" to be a vector of 5 lements
	lua["c_arr"] = std::array<int, 5>{ { 2, 4, 6, 8, 10 } };
	lua["arr"] = std::vector<int>{ 2, 4, 6, 8, 10 };
	lua["map"] = std::map<int, int>{ { 1 , 2 },{ 2, 4 },{ 3, 6 },{ 4, 8 },{ 5, 10 } };
	lua["set"] = std::set<int>{ 2, 4, 6, 8, 10 };
	std::array<int, 5>& c_arr = lua["c_arr"];
	std::vector<int>& arr = lua["arr"];
	std::map<int, int>& map = lua["map"];
	std::set<int>& set = lua["set"];
	REQUIRE(c_arr.size() == 5);
	REQUIRE(arr.size() == 5);
	REQUIRE(map.size() == 5);
	REQUIRE(set.size() == 5);

	g(lua["set"]);
	g(lua["arr"]);
	h(lua["map"]);
	REQUIRE(arr.size() == 6);
	REQUIRE(map.size() == 6);
	REQUIRE(set.size() == 6);

	{
		int r = sf(set, 8);
		REQUIRE(r == 8);
		sol::object rn = sf(set, 9);
		REQUIRE(rn == sol::nil);
	}

	{
		int r = sf(map, 3);
		REQUIRE(r == 6);
		sol::object rn = sf(map, 9);
		REQUIRE(rn == sol::nil);
	}

	i(lua["arr"]);
	i(lua["map"]);
	i(lua["set"]);	
	REQUIRE(arr.empty());
	REQUIRE(map.empty());
	REQUIRE(set.empty());

	REQUIRE_NOTHROW([&]() {
		lua.script(R"(
c_arr[1] = 7
c_arr[2] = 7
c_arr[3] = 7
)");
	}());
	SECTION("throw test") {
		sol::state tlua;
		tlua["c_arr"] = std::array<int, 5>{ { 2, 4, 6, 8, 10 } };
		REQUIRE_THROWS([&]() {
			tlua.script(R"(
c_arr[0] = 7
)");
		}());
	}
	SECTION("throw test 2") {
		sol::state tlua;
		tlua["c_arr"] = std::array<int, 5>{ { 2, 4, 6, 8, 10 } };
		REQUIRE_THROWS([&]() {
			tlua.script(R"(
c_arr[-1] = 7
)");
		}());
	}
}

TEST_CASE("containers/usertype transparency", "Make sure containers pass their arguments through transparently and push the results as references, not new values") {
	class A {
	public:
		int a;
		A(int b = 2) : a(b) {};

		void func() { }
	};

	struct B {

		B() {
			for (std::size_t i = 0; i < 20; ++i) {
				a_list.emplace_back(static_cast<int>(i));
			}
		}

		std::vector<A> a_list;
	};

	sol::state lua;
	lua.new_usertype<B>("B",
		"a_list", &B::a_list
		);

	lua.script(R"(
b = B.new()
a_ref = b.a_list[2]
)");

	B& b = lua["b"];
	A& a_ref = lua["a_ref"];
	REQUIRE(&b.a_list[1] == &a_ref);
	REQUIRE(b.a_list[1].a == a_ref.a);
}

struct options {
	static int livingcount;
	static options* last;
	options() {
		++livingcount;
		last = this;
		INFO("constructor: " << this);
	}

	std::string output_help() {
		last = this;
		INFO("func: " << this);
		return "";
	}

	void begin() {}
	void end() {}

	~options() {
		last = this;
		--livingcount;
	}
};

options* options::last = nullptr;
int options::livingcount = 0;

struct machine {
	options opt;
};

namespace sol {
	template <>
	struct is_container<options> : std::false_type {};
}

TEST_CASE("containers/is container", "make sure the is_container trait behaves properly") {
	sol::state lua;
	lua.open_libraries();

	lua.new_usertype<options>("options_type",
		"output_help", &options::output_help
		);

	lua.new_usertype<machine>("machine_type",
		"new", sol::no_constructor,
		"opt", [](machine& m) { return &m.opt; },
		"copy_opt", [](machine& m) { return m.opt; }
	);

	{ 
		machine m;
		lua["machine"] = &m;

		lua.script(R"(
	machine:opt():output_help()
	)");

		REQUIRE(options::last == &m.opt);
		REQUIRE(options::livingcount == 1);
	}
	REQUIRE(options::livingcount == 0);
}

TEST_CASE("containers/readonly", "make sure readonly members are stored appropriately") {
	sol::state lua;
	lua.open_libraries();

	struct bar {
		int x = 24;
	};

	struct foo {
		std::list<bar> seq;
	};

	lua.new_usertype<foo>(
		"foo",
		"seq", &foo::seq,  // this one works
		"readonly_seq", sol::readonly(&foo::seq)  // this one does not work
		);
	lua["value"] = std::list<bar>{ {},{},{} };
		
	lua.script(R"(
a = foo.new()
x = a.seq
a.seq = value
y = a.readonly_seq
)");
	std::list<bar>& seqrefx = lua["x"];
	std::list<bar>& seqrefy = lua["y"];
	REQUIRE(&seqrefx == &seqrefy);
	REQUIRE(seqrefx.size() == 3);
	auto result = lua.do_string(R"(
a.readonly_seq = value;
)");
	REQUIRE_FALSE(result.valid());
}

TEST_CASE("containers/to_args", "Test that the to_args abstractions works") {
	sol::state lua;
	lua.open_libraries();

	lua.script("function f (a, b, c, d) print(a, b, c, d) return a, b, c, d end");

	sol::function f = lua["f"];
	int a, b, c, d;

	std::vector<int> v2{ 3, 4 };
	sol::tie(a, b, c, d) = f(1, 2, sol::as_args(v2));
	REQUIRE(a == 1);
	REQUIRE(b == 2);
	REQUIRE(c == 3);
	REQUIRE(d == 4);

	std::set<int> v4{ 7, 6, 8, 5 };
	sol::tie(a, b, c, d) = f(sol::as_args(v4));
	REQUIRE(a == 5);
	REQUIRE(b == 6);
	REQUIRE(c == 7);
	REQUIRE(d == 8);

	int v3[] = { 10, 11, 12 };
	sol::tie(a, b, c, d) = f(9, sol::as_args(v3));
	REQUIRE(a == 9);
	REQUIRE(b == 10);
	REQUIRE(c == 11);
	REQUIRE(d == 12);

}

TEST_CASE("containers/ipairs test", "ensure that abstractions roundtrip properly") {
	struct thing {
		int x = 20;
	};
	thing t{};
	sol::state lua;
	lua.open_libraries();

	lua.set_function("f", [&t]() {
		return std::vector<thing*>(5, &t);
	});

	lua.script(R"(
c = f()
)");

	lua.script(R"(
check = {}
local i = 1
while c[i] do
	check[i] = c[i]
	i = i + 1
end
)");
	sol::table c = lua["check"];
	for (std::size_t i = 1; i < 6; ++i) {
		thing& ct = c[i];
		REQUIRE(&t == &ct);
		REQUIRE(ct.x == 20);
	}
}

TEST_CASE("containers/append idiom", "ensure the append-idiom works as intended") {
	sol::state lua;
	lua.open_libraries(sol::lib::base);
	lua.script(
R"(
function f_fill(vec)
	print("#vec in lua: " .. #vec)
	for k = 1, #vec do
		vec[k] = k
	end
	print("#vec in lua: " .. #vec)
end
function f_append(vec)
	print("#vec in lua: " .. #vec)
	vec[#vec] = -10456407
	vec[#vec + 1] = -54
	print("#vec in lua: " .. #vec)
end
)");
	std::vector<int> fill_cmp{ 1, 2, 3 };
	std::vector<int> append_cmp{ -1, -1, -10456407, -54 };

	std::vector<int> vec1{ -1, -1, -1 };
	std::vector<int> vec2{ -1, -1, -1 };

	REQUIRE(vec1.size() == 3);
	lua["f_fill"](vec1);
	REQUIRE(vec1.size() == 3);
	REQUIRE(vec1 == fill_cmp);

	REQUIRE(vec2.size() == 3);
	lua["f_append"](vec2);
	REQUIRE(vec2.size() == 4);
	REQUIRE(vec2 == append_cmp);
}

TEST_CASE("containers/non_copyable", "make sure non-copyable types in containers behave properly when stored as a member variable in a bound usertype") {
	struct non_copyable {
		non_copyable(non_copyable&& other) noexcept = default;
		non_copyable& operator=(non_copyable&& other) noexcept = default;
		non_copyable(const non_copyable& other) noexcept = delete;
		non_copyable& operator=(const non_copyable& other) noexcept = delete;
	};
	struct test {
		std::vector<non_copyable> b;

		test() : b() {}
		test(test&&) = default;
		test& operator=(test&&) = default;
		test(const test&) = delete;
		test& operator=(const test&) = delete;
	};

	SECTION("normal") {
		sol::state lua;
		lua.new_usertype<test>("test",
			"b", sol::readonly(&test::b)
		);

		lua["v"] = std::vector<non_copyable>{};

		REQUIRE_THROWS([&lua]() {
			lua.script("t = test.new()\nt.b = v");
		}());
	}
	SECTION("simple") {
		sol::state lua;
		lua.new_simple_usertype<test>("test",
			"b", sol::readonly(&test::b)
		);
		
		lua["v"] = std::vector<non_copyable>{};

		REQUIRE_THROWS([&lua]() {
			lua.script("t = test.new()\nt.b = v");
		}());
	}
}


TEST_CASE("containers/input_iterators", "test shitty input iterators that are all kinds of B L E H") {
	class int_shim {
	public:
		int_shim() = default;

		int_shim(int x) : x_(x) {}

		int val() const { return x_; }

	private:
		int x_ = -1;
	};

	class input_it : public std::iterator<std::input_iterator_tag, int_shim> {
	public:
		input_it() = default;

		input_it(int n, int m) : n_(n), m_(m), value_(n_) {
			assert(n_ >= 0);
			assert(m_ >= 0);
			assert(n_ <= m_);

			if (!n_ && !m_) {
				n_ = -1;
				m_ = -1;
				value_ = -1;
			}
		}

		const int_shim &operator*() const { return value_; }

		const int_shim *operator->() const { return &value_; }

		input_it &operator++() {
			assert(n_ >= 0);
			assert(m_ >= 0);
			if (n_ == m_ - 1) {
				n_ = m_ = -1;
			}
			else {
				++n_;
			}
			value_ = n_;
			return *this;
		}

		bool operator==(const input_it &i) const { return n_ == i.n_ && m_ == i.m_; }

		bool operator!=(const input_it &i) const { return !(*this == i); }

	private:
		int n_ = -1;
		int m_ = -1;
		int_shim value_;
	};

	class not_really_a_container {
	public:
		using value_type = int_shim;
		using iterator = input_it;
		using const_iterator = input_it;

		const_iterator begin() const { return iterator(0, 100); }

		const_iterator end() const { return iterator(); }

		value_type fuck_off_gcc_warning() {
			// "typedef not used locally"
			// but it's used elsewhere in the code GCC
			// so maybe your shitty warning should go
			// fuck itself???
			return int_shim();
		}

		std::size_t size() const { return 100; }
	};

	sol::state lua;
	lua.open_libraries(sol::lib::base, sol::lib::package);
	lua.new_usertype<int_shim>("int_shim", 
		"new", sol::no_constructor, 
		"val", &int_shim::val);

	not_really_a_container c;
	lua["c"] = &c;
#if SOL_LUA_VERSION > 503
	lua.script(R"lua(
for k, v in pairs(c) do
  assert((k - 1) == v:val())
end
)lua");
#else
	lua.script(R"lua(
for k=1,#c do
  v = c[k]
  assert((k - 1) == v:val())
end
)lua");
#endif
}

TEST_CASE("containers/pairs", "test how well pairs work with the underlying system") {
	sol::state lua;

	lua.open_libraries(sol::lib::base);

	std::vector<std::pair<std::string, int>> a{ { "one", 1 },{ "two", 2 },{ "three", 3 },{ "four", 4 },{ "five", 5 } };
	std::array<std::pair<std::string, int>, 5> b{ { { "one", 1 },{ "two", 2 },{ "three", 3 },{ "four", 4 },{ "five", 5 } } };
	//std::pair<std::string, int> c[5]{ { "one", 1 },{ "two", 2 },{ "three", 3 },{ "four", 4 },{ "five", 5 } };
	//int d[5] = { 1, 2, 3, 4, 5 };

	lua["a"] = std::ref(a);
	lua["b"] = &b;
	//lua["c"] = std::ref(c);
	//lua["d"] = &d;

	lua.script("av1, av2 = a:get(1)");
	lua.script("bv1, bv2 = b:get(1)");
	//lua.script("cv1, cv2 = c:get(1)");
	//lua.script("dv1, dv2 = d:get(1)");

	std::vector<std::pair<std::string, int>>& la = lua["a"];
	std::array<std::pair<std::string, int>, 5>& lb = lua["b"];
	//std::pair<std::string, int> (&lc)[5] = lua["c"];
	//int (&lc)[5] = lua["d"];

	std::pair<std::string, int>& va = la[0];
	std::pair<std::string, int>& vb = lb[0];
	//std::pair<std::string, int>& vc = lc[0];
	//int vd = ld[0];
	std::string av1 = lua["av1"];
	int av2 = lua["av2"];
	std::string bv1 = lua["bv1"];
	int bv2 = lua["bv2"];
	//std::string cv1 = lua["cv1"];
	//int cv2 = lua["cv2"];
	//int dv1 = lua["dv1"];
	//sol::lua_nil_t dv2 = lua["dv2"];

	REQUIRE(va.first == "one");
	REQUIRE(va.second == 1);
	REQUIRE(vb.first == "one");
	REQUIRE(vb.second == 1);
	//REQUIRE(vc.first == "one");
	//REQUIRE(vc.second == 1);
	//REQUIRE(vd == 1);

	REQUIRE(av1 == "one");
	REQUIRE(av2 == 1);
	REQUIRE(bv1 == "one");
	REQUIRE(bv2 == 1);
	//REQUIRE(cv1 == "one");
	//REQUIRE(cv2 == 1);
	//REQUIRE(dv1 == 1);
	//REQUIRE(dv2 == sol::lua_nil);
}
