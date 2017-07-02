// include at the beginning of sol headers AFTER all other sol header inclusions; end sol headers with #include "file_end.hpp"
#ifndef SOL_GLOBAL_HAS_BEGUN
#define SOL_FILE_HAS_BEGUN

#if defined(UE_BUILD_DEBUG) || defined(UE_BUILD_DEVELOPMENT) || defined(UE_BUILD_TEST) || defined(UE_BUILD_SHIPPING) || defined(UE_SERVER)
#define SOL_INSIDE_UNREAL
#endif // Unreal Engine 4 bullshit

#ifdef SOL_INSIDE_UNREAL
#ifdef check
#define SOL_INSIDE_UNREAL_REMOVED_CHECK
#undef check
#endif
#endif // Unreal Engine 4 Bullshit

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wconversion"
#if __GNUC__ > 6
#pragma GCC diagnostic ignored "-Wnoexcept-type"
#endif
#elif defined _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4324 ) // structure was padded due to alignment specifier
#endif // g++, VS

#endif // SOL_GLOBAL_HAS_BEGUN
