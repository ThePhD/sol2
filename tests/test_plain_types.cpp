#define SOL_CHECK_ARGUMENTS 1
#define SOL_ENABLE_INTEROP 1

#include <sol.hpp>
#include <catch.hpp>

TEST_CASE("plain/alignment", "test that aligned classes in certain compilers don't trigger compiler errors") {
#ifdef _MSC_VER
	__declspec(align(16)) class aligned_class {
		int var;
	};

	class A {
		aligned_class a;
	};

	sol::state lua;
	lua.open_libraries(sol::lib::base, sol::lib::math);

	lua.new_usertype<A>("A");
	A a;
	lua["a"] = &a;
	A& la = lua["a"];
	REQUIRE(&a == &la);
#else
	REQUIRE(true);
#endif // VC++ stuff
}

TEST_CASE("plain/indestructible", "test that we error for types that are innately indestructible") {
	struct indestructible {
	public:
		int v = 20;

		struct insider {
			void operator()(indestructible* i) const {
				i->~indestructible();
			}
		};

	private:
		~indestructible() {
		}
	};

	SECTION("doomed") {
		sol::state lua;
		lua.open_libraries(sol::lib::base);

		std::unique_ptr<indestructible, indestructible::insider> i = sol::detail::make_unique_deleter<indestructible, indestructible::insider>();
		lua["i"] = *i;
		lua.safe_script("i = nil");
		auto result = lua.safe_script("collectgarbage()", sol::script_pass_on_error);
		REQUIRE_FALSE(result.valid());
	}
	SECTION("saved") {
		sol::state lua;
		lua.open_libraries(sol::lib::base);

		std::unique_ptr<indestructible, indestructible::insider> i = sol::detail::make_unique_deleter<indestructible, indestructible::insider>();
		lua["i"] = *i;
		lua.new_usertype<indestructible>("indestructible",
			sol::default_constructor,
			sol::meta_function::garbage_collect, sol::destructor([](indestructible& i) {
				indestructible::insider del;
				del(&i);
			}));
		lua.safe_script("i = nil");
		auto result = lua.safe_script("collectgarbage()", sol::script_pass_on_error);
		REQUIRE(result.valid());
	}
}