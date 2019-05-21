// sol3

// The MIT License (MIT)

// Copyright (c) 2013-2019 Rapptz, ThePhD and contributors

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


struct factory_test {
private:
	factory_test() {
		a = true_a;
	}
	~factory_test() {
		a = 0;
	}

public:
	static int num_saved;
	static int num_killed;

	struct deleter {
		void operator()(factory_test* f) {
			f->~factory_test();
		}
	};

	static const int true_a;
	int a;

	static std::unique_ptr<factory_test, deleter> make() {
		return std::unique_ptr<factory_test, deleter>(new factory_test(), deleter());
	}

	static void save(factory_test& f) {
		new (&f) factory_test();
		++num_saved;
	}

	static void kill(factory_test& f) {
		f.~factory_test();
		++num_killed;
	}
};

int factory_test::num_saved = 0;
int factory_test::num_killed = 0;
const int factory_test::true_a = 156;


TEST_CASE("usertype/unique-shared-ptr", "manage the conversion and use of unique and shared pointers ('unique usertypes')") {
	const int64_t unique_value = 0x7125679355635963;
	auto uniqueint = std::make_unique<int64_t>(unique_value);
	auto sharedint = std::make_shared<int64_t>(unique_value);
	long preusecount = sharedint.use_count();
	{
		sol::state lua;
		lua.open_libraries(sol::lib::base);
		lua.set("uniqueint", std::move(uniqueint));
		lua.set("sharedint", sharedint);
		std::unique_ptr<int64_t>& uniqueintref = lua["uniqueint"];
		std::shared_ptr<int64_t>& sharedintref = lua["sharedint"];
		int64_t* rawuniqueintref = lua["uniqueint"];
		int64_t* rawsharedintref = lua["sharedint"];
		int siusecount = sharedintref.use_count();
		REQUIRE((uniqueintref.get() == rawuniqueintref && sharedintref.get() == rawsharedintref));
		REQUIRE((uniqueintref != nullptr && sharedintref != nullptr && rawuniqueintref != nullptr && rawsharedintref != nullptr));
		REQUIRE((unique_value == *uniqueintref.get() && unique_value == *sharedintref.get()));
		REQUIRE((unique_value == *rawuniqueintref && unique_value == *rawsharedintref));
		REQUIRE(siusecount == sharedint.use_count());
		std::shared_ptr<int64_t> moreref = sharedint;
		REQUIRE(unique_value == *moreref.get());
		REQUIRE(moreref.use_count() == sharedint.use_count());
		REQUIRE(moreref.use_count() == sharedintref.use_count());
	}
	REQUIRE(preusecount == sharedint.use_count());
	std::cout << "----- end of 2" << std::endl;
}

TEST_CASE("usertype/private-constructible", "Check to make sure special snowflake types from Enterprise thingamahjongs work properly.") {
	int numsaved = factory_test::num_saved;
	int numkilled = factory_test::num_killed;
	{
		sol::state lua;
		lua.open_libraries(sol::lib::base);

		lua.new_usertype<factory_test>(
		     "factory_test", "new", sol::initializers(factory_test::save), "__gc", sol::destructor(factory_test::kill), "a", &factory_test::a);

		std::unique_ptr<factory_test, factory_test::deleter> f = factory_test::make();
		lua.set("true_a", factory_test::true_a, "f", f.get());
		{
			auto result = lua.safe_script("assert(f.a == true_a)", sol::script_pass_on_error);
			REQUIRE(result.valid());
		}

		auto code1
		     = "local fresh_f = factory_test:new()\n"
		       "assert(fresh_f.a == true_a)\n";
		auto result1 = lua.safe_script(code1, sol::script_pass_on_error);
		REQUIRE(result1.valid());
	}
	int expectednumsaved = numsaved + 1;
	int expectednumkilled = numkilled + 1;
	REQUIRE(expectednumsaved == factory_test::num_saved);
	REQUIRE(expectednumkilled == factory_test::num_killed);
	std::cout << "----- end of 5" << std::endl;
}

TEST_CASE("usertype/unique_usertype-check", "make sure unique usertypes don't get pushed as references with function calls and the like") {
	class Entity {
	public:
		std::string GetName() {
			return "Charmander";
		}
	};

	sol::state lua;
	lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::string, sol::lib::io);

	lua.new_usertype<Entity>("Entity", "new", sol::no_constructor, "get_name", &Entity::GetName);

	lua.safe_script(R"(
		function my_func(entity)
		print("INSIDE LUA")
		print(entity:get_name())
		end
)");

	sol::function my_func = lua["my_func"];
	REQUIRE_NOTHROW([&] {
		auto ent = std::make_shared<Entity>();
		my_func(ent);
		Entity ent2;
		my_func(ent2);
		my_func(std::make_shared<Entity>());
	}());
}

TEST_CASE("usertype/unique void pointers", "can compile shared_ptr<void> types and not trip the compiler or sol3's internals") {
	sol::state lua;
	lua.set_function("f", [](std::shared_ptr<void> d) { 
		int* pi = static_cast<int*>(d.get());
		REQUIRE(*pi == 567);
	});

	std::shared_ptr<void> s = std::make_shared<int>(567);
	lua["s"] = std::move(s);
	auto result = lua.safe_script("f(s)", sol::script_pass_on_error);
	REQUIRE(result.valid());
}
