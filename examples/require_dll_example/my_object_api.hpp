#pragma once

namespace my_object {

#if defined _MSC_VER
#define MY_OBJECT_VC
#elif defined __GNUC__
#define MY_OBJECT_GCC
#elif defined __clang__
#define MY_OBJECT_CLANG
#endif

#if !defined(_NDEBUG)
#define MY_OBJECT_DEBUG
#else
#endif // Debug || Not-Debug

#if defined MY_OBJECT_VC
#if defined MY_OBJECT_DLL
#if defined MY_OBJECT_BUILD
#define MY_OBJECT_API __declspec(dllexport)
#else
#define MY_OBJECT_API __declspec(dllexport)
#endif // MY_OBJECT_BUILD - Building the Library vs. Using the Library
#else
#define MY_OBJECT_API 
#endif // Building a DLL vs. Static Library
#else
#define MY_OBJECT_API
#endif

} // my_object
