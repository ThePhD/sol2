#include "bench_cpp_function.hpp"
#include "bench_lua_function.hpp"
#include "bench_get.hpp"
#include "bench_set.hpp"
#include "bench_usertype.hpp"
#include "nonius/nonius.h++"

int main( int argc, char* argv[] ) {
	using namespace std::literals::string_literals;
	std::string configurationname = argv[ 1 ];
	std::string platformname = argv[ 2 ];
	bench_lua_function( "bench/", configurationname, platformname );
	bench_cpp_function( "bench/", configurationname, platformname );
	bench_get( "bench/", configurationname, platformname );
	bench_set( "bench/", configurationname, platformname );
	bench_usertype( "bench/", configurationname, platformname );
}
