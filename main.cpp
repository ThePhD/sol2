#include <sol.hpp>
#include <sol/demangle.hpp>

namespace sol {
	
	template <typename... Tn>
	struct constructors;

	template <typename T>
	class lua_class {
	public:
		static const std::string classname;
		static const std::string meta;

	private:
		std::string luaname;
		std::vector<std::string> functionnames;
		std::vector<class_lua_func> functions;
		std::vector<luaL_Reg> functiontable;
		std::vector<luaL_Reg> metatable;

		struct maker {

			static int construct( lua_State* L ) {
				// First argument is now a table that represent the class to instantiate
				luaL_checktype( L, 1, LUA_TTABLE );

				lua_newtable( L ); // Create table to represent instance

				// Set first argument of new to metatable of instance
				lua_pushvalue( L, 1 );
				lua_setmetatable( L, -2 );

				// Do function lookups in metatable
				lua_pushvalue( L, 1 );
				lua_setfield( L, 1, "__index" );

				void* userdata = lua_newuserdata( L, sizeof( T ) );
				T* obj = static_cast<T*>( userdata );
				std::allocator<T> alloc{ };
				alloc.construct( obj );
			}

			template <std::size_t n>
			static int destruct( lua_State* L ) {
				void* userdata = lua_touserdata( L, 0 );
				T* obj = static_cast<T*>( userdata );
				std::allocator<T> alloc{ };
				alloc.destroy( obj );
			}
		};

		void build_function_tables( ) {

		}

		template <typename... Args, typename Ret, typename... MArgs>
		void build_function_tables( Ret( T::* func )( MArgs... ), std::string name, Args&&... args ) {
			functionnames.push_back( std::move( name ) );
			functions.emplace_back( std::move( func ) );
			functiontable.push_back( { functionnames.back().c_str(), &lua_func::call } );
			build_function_tables( std::forward<Args>( args )... );
		}

	public:
		template <typename... Args>
		lua_class( Args&&... args ) : lua_class( classname, std::forward<Args>( args )... ) {

		}

		template <typename... Args>
		lua_class( std::string name, Args&&... args ) : lua_class( name, constructors<>(), std::forward<Args>( args )... ) {

		}

		template <typename... Args, typename... CArgs>
		lua_class( std::string name, constructors<CArgs...> c, Args&&... args ) : luaname( std::move( name ) ) {
			functionnames.reserve( sizeof...( args ) );
			functiontable.reserve( sizeof...( args ) );
			metatable.reserve( sizeof...( args ) );
			build_function_tables( std::forward<Args>( args )... );
			functiontable.
		}
	};

	template <typename T>
	const std::string lua_class<T>::classname = detail::demangle( typeid( T ) );

	template <typename T>
	const std::string lua_class<T>::meta = std::string( "sol.stateful." ).append( classname );

}

struct f {
	int x;
	f(  ) : x( 1 ) {
	}
	f( int x ) : x( x ) { 
	}
	int add( int y ) {
		return x + y;
	}
};

#include <iostream>

int main( ) {
	sol::state s;
	f x( 20 );
	
	sol::lua_class<f> lc{ };
	std::cout << lc.classname << std::endl;
	std::cout << lc.meta << std::endl;

	s.set_function( "add", &f::add, f(10) );
	s.script( "t = add(20)" );
	std::cout << s.get<int>( "t" );
}
