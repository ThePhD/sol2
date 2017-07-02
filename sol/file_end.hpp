// include at the end of sol headers; begin sol headers with inclusions of other sol headers THEN followed by #include "file_begin.hpp"
#ifndef SOL_GLOBAL_HAS_BEGUN

#ifdef SOL_FILE_HAS_BEGUN
#undef SOL_FILE_HAS_BEGUN
#else
#error "sol/file_end.hpp" was included without including "sol/file_begin.hpp" beforehand; something went wrong!
#endif // SOL_FILE_HAS_BEGUN

#ifdef __GNUC__
#pragma GCC diagnostic pop
#elif defined _MSC_VER
#pragma warning( push )
#endif // g++

#ifdef SOL_INSIDE_UNREAL
#ifdef SOL_INSIDE_UNREAL_REMOVED_CHECK
#if DO_CHECK
#define check(expr) { if(UNLIKELY(!(expr))) { FDebug::LogAssertFailedMessage( #expr, __FILE__, __LINE__ ); _DebugBreakAndPromptForRemote(); FDebug::AssertFailed( #expr, __FILE__, __LINE__ ); CA_ASSUME(false); } }
#else
#define check(expr) { CA_ASSUME(expr); }
#endif
#endif
#endif // Unreal Engine 4 Bullshit

#endif // SOL_GLOBAL_HAS_BEGUN
