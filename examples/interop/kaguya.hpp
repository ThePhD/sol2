#ifndef KAGUYA_LUABINDING_HPP_INCLUDED
#define KAGUYA_LUABINDING_HPP_INCLUDED
// Copyright satoren
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


// Copyright satoren
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <string>
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#ifndef KAGUYA_USE_CPP11
#if defined(__cpp_decltype) || __cplusplus >= 201103L ||                       \
    (defined(_MSC_VER) && _MSC_VER >= 1800)
#define KAGUYA_USE_CPP11 1
#else
#define KAGUYA_USE_CPP11 0
#endif
#endif

#if KAGUYA_USE_CPP11
#include <functional>
#include <tuple>
#include <memory>
#include <utility>
#include <type_traits>
#include <initializer_list>
#include <array>
#else
#include <boost/function.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/type_traits.hpp>
#include <boost/static_assert.hpp>
#include <boost/utility/result_of.hpp>
#endif

#ifndef KAGUYA_NO_USERDATA_TYPE_CHECK
#define KAGUYA_NO_USERDATA_TYPE_CHECK 0
#endif

//If you want use registered class by kaguya between multiple shared library,
//please switch to 1 for KAGUYA_SUPPORT_MULTIPLE_SHARED_LIBRARY and KAGUYA_NAME_BASED_TYPE_CHECK
#ifndef KAGUYA_SUPPORT_MULTIPLE_SHARED_LIBRARY
#define KAGUYA_SUPPORT_MULTIPLE_SHARED_LIBRARY 0
#endif

#ifndef KAGUYA_NAME_BASED_TYPE_CHECK
#define KAGUYA_NAME_BASED_TYPE_CHECK KAGUYA_SUPPORT_MULTIPLE_SHARED_LIBRARY
#endif

#ifndef KAGUYA_USE_RVALUE_REFERENCE
#if KAGUYA_USE_CPP11
#define KAGUYA_USE_RVALUE_REFERENCE 1
#else
#define KAGUYA_USE_RVALUE_REFERENCE 0
#endif
#endif

#ifdef KAGUYA_NO_VECTOR_AND_MAP_TO_TABLE
#define KAGUYA_NO_STD_VECTOR_TO_TABLE
#define KAGUYA_NO_STD_MAP_TO_TABLE
#endif

#if !KAGUYA_USE_CPP11
#ifndef KAGUYA_FUNCTION_MAX_ARGS
///! max argumeent number for binding function. this define used C++03 only.
#define KAGUYA_FUNCTION_MAX_ARGS 9
#endif

#ifndef KAGUYA_FUNCTION_MAX_TUPLE_SIZE
///! this define used C++03 only.
#define KAGUYA_FUNCTION_MAX_TUPLE_SIZE 9
#endif

#ifndef KAGUYA_FUNCTION_MAX_OVERLOADS
#define KAGUYA_FUNCTION_MAX_OVERLOADS 9
#endif

#endif

#ifndef KAGUYA_CLASS_MAX_BASE_CLASSES
#define KAGUYA_CLASS_MAX_BASE_CLASSES 9
#endif

#ifndef KAGUYA_USE_CXX_ABI_DEMANGLE
#if defined(__GNUC__) || defined(__clang__)
#define KAGUYA_USE_CXX_ABI_DEMANGLE 1
#else
#define KAGUYA_USE_CXX_ABI_DEMANGLE 0
#endif
#endif

#ifndef KAGUYA_USE_SHARED_LUAREF
#define KAGUYA_USE_SHARED_LUAREF 0
#endif

#ifndef KAGUYA_NOEXCEPT
#if KAGUYA_USE_CPP11 && (!defined(_MSC_VER) || _MSC_VER >= 1900)
#define KAGUYA_NOEXCEPT noexcept
#else
#define KAGUYA_NOEXCEPT throw()
#endif
#endif

#ifndef KAGUYA_DEPRECATED_FEATURE
#if __cplusplus >= 201402L && defined(__has_cpp_attribute)
#if __has_cpp_attribute(deprecated)
// C++ standard depecated
#define KAGUYA_DEPRECATED_FEATURE(MSG) [[deprecated(MSG)]]
#endif
#endif
#endif
#ifndef KAGUYA_DEPRECATED_FEATURE
#if defined(_MSC_VER)
// MSVC depecated
#define KAGUYA_DEPRECATED_FEATURE(MSG) __declspec(deprecated(MSG))
#elif defined(__GNUC__) || defined(__clang__)
#define KAGUYA_DEPRECATED_FEATURE(MSG) __attribute__((deprecated))
#else
#define KAGUYA_DEPRECATED_FEATURE(MSG)
#endif

#endif

#define KAGUYA_UNUSED(V) (void)(V)

namespace kaguya {
#if defined(_MSC_VER) && _MSC_VER <= 1500
typedef unsigned char uint8_t;
typedef int int32_t;
typedef long long int64_t;
#endif

namespace standard {
#if KAGUYA_USE_CPP11
using namespace std;
#define KAGUYA_STATIC_ASSERT static_assert

#else
using namespace boost;
#define KAGUYA_STATIC_ASSERT BOOST_STATIC_ASSERT_MSG
#endif
}

#if LUA_VERSION_NUM > 502
typedef lua_Integer luaInt;
#else
typedef int32_t luaInt;
#endif
}
// Copyright satoren
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#include <vector>
#include <map>
#include <cassert>
#include <algorithm>
#include <ostream>
#include <istream>

// Copyright satoren
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#include <string>

// Copyright satoren
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#include <string>
#include <cstring>

// Copyright satoren
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <cassert>

namespace kaguya {
/// @addtogroup optional
///  @{

struct bad_optional_access : std::exception {};
struct nullopt_t {};

/// @brief self implement for std::optional(C++17 feature).
template <class T> class optional {
  typedef void (optional::*bool_type)() const;
  void this_type_does_not_support_comparisons() const {}

public:
  optional() : value_(0){};
  optional(nullopt_t) : value_(0){};
  optional(const optional &other) : value_(0) {
    if (other) {
      value_ = new (&storage_) T(other.value());
    }
  }
  optional(const T &value) { value_ = new (&storage_) T(value); }

  ~optional() { destruct(); }
  optional &operator=(nullopt_t) {
    destruct();
    return *this;
  }
  optional &operator=(const optional &other) {
    if (other) {
      *this = other.value();
    } else {
      destruct();
    }
    return *this;
  }
  optional &operator=(const T &value) {
    if (value_) {
      *value_ = value;
    } else {
      value_ = new (&storage_) T(value);
    }
    return *this;
  }

#if KAGUYA_USE_CPP11
  optional(optional &&other) : value_(0) {
    if (other) {
      value_ = new (&storage_) T(std::move(other.value()));
    }
  }
  optional(T &&value) { value_ = new (&storage_) T(std::move(value)); }
  optional &operator=(optional &&other) {
    if (other) {
      *this = std::move(other.value());
    } else {
      destruct();
    }
    return *this;
  }
  optional &operator=(T &&value) {
    if (value_) {
      *value_ = std::move(value);
    } else {
      value_ = new (&storage_) T(std::move(value));
    }
    return *this;
  }
#endif

  operator bool_type() const {
    this_type_does_not_support_comparisons();
    return value_ != 0 ? &optional::this_type_does_not_support_comparisons : 0;
  }
  T &value() {
    if (value_) {
      return *value_;
    }
    throw bad_optional_access();
  }
  const T &value() const {
    if (value_) {
      return *value_;
    }
    throw bad_optional_access();
  }

#if KAGUYA_USE_CPP11
  template <class U> T value_or(U &&default_value) const {
    if (value_) {
      return *value_;
    }
    return default_value;
  }
#else
  template <class U> T value_or(const U &default_value) const {
    if (value_) {
      return *value_;
    }
    return default_value;
  }
#endif
  const T *operator->() const {
    assert(value_);
    return value_;
  }
  T *operator->() {
    assert(value_);
    return value_;
  }
  const T &operator*() const {
    assert(value_);
    return *value_;
  }
  T &operator*() {
    assert(value_);
    return *value_;
  }

private:
  void destruct() {
    if (value_) {
      value_->~T();
      value_ = 0;
    }
  }

  typename standard::aligned_storage<
      sizeof(T), standard::alignment_of<T>::value>::type storage_;

  T *value_;
};

/// @brief specialize optional for reference.
/// sizeof(optional<T&>) == sizeof(T*)
template <class T> class optional<T &> {
  typedef void (optional::*bool_type)() const;
  void this_type_does_not_support_comparisons() const {}

public:
  optional() : value_(0){};
  optional(nullopt_t) : value_(0){};

  optional(const optional &other) : value_(other.value_) {}
  optional(T &value) : value_(&value) {}

  ~optional() {}
  optional &operator=(nullopt_t) {
    value_ = 0;
    return *this;
  }
  optional &operator=(const optional &other) {
    value_ = other.value_;
    return *this;
  }
  optional &operator=(T &value) {
    value_ = &value;
    return *this;
  }
  operator bool_type() const {
    this_type_does_not_support_comparisons();
    return value_ != 0 ? &optional::this_type_does_not_support_comparisons : 0;
  }
  T &value() {
    if (value_) {
      return *value_;
    }
    throw bad_optional_access();
  }
  const T &value() const {
    if (value_) {
      return *value_;
    }
    throw bad_optional_access();
  }

#if KAGUYA_USE_CPP11
  T &value_or(T &default_value) const {
    if (value_) {
      return *value_;
    }
    return default_value;
  }
#else
  T &value_or(T &default_value) const {
    if (value_) {
      return *value_;
    }
    return default_value;
  }
#endif

  const T *operator->() const {
    assert(value_);
    return value_;
  }
  T *operator->() {
    assert(value_);
    return value_;
  }
  const T &operator*() const {
    assert(value_);
    return *value_;
  }
  T &operator*() {
    assert(value_);
    return *value_;
  }

private:
  T *value_;
};

/// @name relational operators
/// @brief
///@{
template <class T>
bool operator==(const optional<T> &lhs, const optional<T> &rhs) {
  if (bool(lhs) != bool(rhs)) {
    return false;
  }
  if (bool(lhs) == false) {
    return true;
  }
  return lhs.value() == rhs.value();
}
template <class T>
bool operator!=(const optional<T> &lhs, const optional<T> &rhs) {
  return !(lhs == rhs);
}
template <class T>
bool operator<(const optional<T> &lhs, const optional<T> &rhs) {
  if (!bool(rhs)) {
    return false;
  }
  if (!bool(lhs)) {
    return true;
  }
  return lhs.value() < rhs.value();
}
template <class T>
bool operator<=(const optional<T> &lhs, const optional<T> &rhs) {
  return !(rhs < lhs);
}
template <class T>
bool operator>(const optional<T> &lhs, const optional<T> &rhs) {
  return rhs < lhs;
}
template <class T>
bool operator>=(const optional<T> &lhs, const optional<T> &rhs) {
  return !(lhs < rhs);
}
/// @}

/// @}
}
// Copyright satoren
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#include <string>
#include <vector>
#include <map>


namespace kaguya {
namespace traits {
using standard::integral_constant;
using standard::true_type;
using standard::false_type;
using standard::remove_reference;
using standard::remove_pointer;
using standard::remove_const;
using standard::remove_volatile;
using standard::remove_cv;
using standard::is_function;
using standard::is_floating_point;
using standard::is_integral;
using standard::is_enum;
using standard::is_convertible;
using standard::is_same;
using standard::is_arithmetic;
using standard::is_union;
using standard::is_class;
using standard::is_pointer;
using standard::is_lvalue_reference;
using standard::is_const;
using standard::is_void;
#if KAGUYA_USE_CPP11
using std::enable_if;
#else
template <bool B, class T = void>
struct enable_if : boost::enable_if_c<B, T> {};
#endif

class Helper {};
/// @brief Check if T_Mem is a member object of a type. That is true if it is
/// not a member function
/// Required as MSVC throws a COMDAT error when using is_member_object_pointer
template <typename T_Mem> struct is_object {
  typedef typename standard::is_member_function_pointer<T_Mem Helper::*>::type
      NotResult;
  enum { value = !NotResult::value };
};

/// @brief Similar to std::decay but also removes const and volatile modifiers
/// if T is neither an array nor a function
template <class T> struct decay {
private:
  ///@ If T is a reference type, the type referrered to by T.	Otherwise, T.
  typedef typename standard::remove_reference<T>::type U;

public:
  typedef typename standard::conditional<
      standard::is_array<U>::value, typename standard::remove_extent<U>::type *,
      typename standard::conditional<
          is_function<U>::value, typename standard::add_pointer<U>::type,
          typename standard::remove_cv<U>::type>::type>::type type;
};

/// @brief Trait class that identifies whether T is a const reference type.
template <class T> struct is_const_reference : false_type {};
template <class T> struct is_const_reference<const T &> : true_type {};

/// @brief Obtains the type T without top-level const and reference.
template <typename T> struct remove_const_and_reference {
  /// @brief If T is const or reference, the same type as T but with the const
  /// reference removed.Otherwise, T
  typedef T type;
};
/// @brief Obtains the type T without top-level const and reference.
template <typename T> struct remove_const_and_reference<T &> {
  /// @brief If T is const or reference, the same type as T but with the const
  /// reference removed.Otherwise, T
  typedef T type;
};
/// @brief Obtains the type T without top-level const and reference.
template <typename T> struct remove_const_and_reference<const T> {
  /// @brief If T is const or reference, the same type as T but with the const
  /// reference removed.Otherwise, T
  typedef T type;
};
/// @brief Obtains the type T without top-level const and reference.
template <typename T> struct remove_const_and_reference<const T &> {
  /// @brief If T is const or reference, the same type as T but with the const
  /// reference removed.Otherwise, T
  typedef T type;
};

/// @brief Obtains the type T without top-level const reference.
template <typename T> struct remove_const_reference {
  /// @brief If T is const reference, the same type as T but with the const
  /// reference removed.Otherwise, T
  typedef T type;
};
/// @brief Obtains the type T without top-level const reference.
template <typename T> struct remove_const_reference<const T &> {
  /// @brief If T is const reference, the same type as T but with the const
  /// reference removed.Otherwise, T
  typedef T type;
};

/// @brief Trait class that identifies whether T is a std::vector type.
template <class T> struct is_std_vector : false_type {};
template <class T, class A>
struct is_std_vector<std::vector<T, A> > : true_type {};

/// @brief Trait class that identifies whether T is a std::map type.
template <class T> struct is_std_map : false_type {};
template <class K, class V, class C, class A>
struct is_std_map<std::map<K, V, C, A> > : true_type {};
}

/// @addtogroup lua_type_traits

/// @ingroup lua_type_traits
/// @brief If you want to customize the conversion to type of lua yourself ,
///  implement specialize of this class
template <typename T, typename Enable = void> struct lua_type_traits {
  typedef void Registerable;

  typedef typename traits::decay<T>::type NCRT;
  typedef const NCRT &get_type;
  typedef optional<get_type> opt_type;
  typedef const NCRT &push_type;

  static bool checkType(lua_State *l, int index);
  static bool strictCheckType(lua_State *l, int index);

  static get_type get(lua_State *l, int index);
  static opt_type opt(lua_State *l, int index) KAGUYA_NOEXCEPT;
  static int push(lua_State *l, push_type v);
#if KAGUYA_USE_RVALUE_REFERENCE
  static int push(lua_State *l, NCRT &&v);
#endif
};

/// @brief Trait class that identifies whether T is a userdata type.
template <typename T, typename Enable = void>
struct is_usertype : traits::false_type {};
template <typename T>
struct is_usertype<T, typename lua_type_traits<T>::Registerable>
    : traits::true_type {};

/// @brief Trait class that identifies whether T is a registerable by
/// UserdataMetatable.
template <typename T> struct is_registerable : is_usertype<T> {};
}
// Copyright satoren
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <string>
#include <cstring>
#include <typeinfo>
#include <algorithm>

// Copyright satoren
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#include <iostream>

#if KAGUYA_USE_CXX_ABI_DEMANGLE
#include <cxxabi.h>
#endif

// Copyright satoren
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


namespace kaguya {
// for lua version compatibility
namespace compat {
#if LUA_VERSION_NUM >= 503
inline int lua_rawgetp_rtype(lua_State *L, int idx, const void *ptr) {
  return lua_rawgetp(L, idx, ptr);
}
inline int lua_rawget_rtype(lua_State *L, int idx) {
  return lua_rawget(L, idx);
}
inline int lua_getfield_rtype(lua_State *L, int idx, const char *k) {
  return lua_getfield(L, idx, k);
}
inline int lua_gettable_rtype(lua_State *L, int idx) {
  return lua_gettable(L, idx);
}
#elif LUA_VERSION_NUM == 502
inline int lua_rawgetp_rtype(lua_State *L, int idx, const void *ptr) {
  lua_rawgetp(L, idx, ptr);
  return lua_type(L, -1);
}
#elif LUA_VERSION_NUM < 502
enum LUA_OPEQ { LUA_OPEQ, LUA_OPLT, LUA_OPLE };
inline int lua_compare(lua_State *L, int index1, int index2, int op) {
  switch (op) {
  case LUA_OPEQ:
    return lua_equal(L, index1, index2);
  case LUA_OPLT:
    return lua_lessthan(L, index1, index2);
  case LUA_OPLE:
    return lua_equal(L, index1, index2) || lua_lessthan(L, index1, index2);
  default:
    return 0;
  }
}

inline void lua_pushglobaltable(lua_State *L) {
  lua_pushvalue(L, LUA_GLOBALSINDEX);
}
inline size_t lua_rawlen(lua_State *L, int index) {
  int type = lua_type(L, index);
  if (type != LUA_TSTRING && type != LUA_TTABLE && type != LUA_TUSERDATA &&
      type != LUA_TLIGHTUSERDATA) {
    return 0;
  }
  return lua_objlen(L, index);
}

inline int lua_resume(lua_State *L, lua_State *from, int nargs) {
  KAGUYA_UNUSED(from);
  return ::lua_resume(L, nargs);
}
inline int lua_absindex(lua_State *L, int idx) {
  return (idx > 0 || (idx <= LUA_REGISTRYINDEX)) ? idx
                                                 : lua_gettop(L) + 1 + idx;
}
inline int lua_rawgetp_rtype(lua_State *L, int idx, const void *ptr) {
  int absidx = lua_absindex(L, idx);
  lua_pushlightuserdata(L, (void *)ptr);
  lua_rawget(L, absidx);
  return lua_type(L, -1);
}
inline void lua_rawsetp(lua_State *L, int idx, const void *ptr) {
  int absidx = lua_absindex(L, idx);
  lua_pushvalue(L, -1);
  lua_pushlightuserdata(L, (void *)ptr);
  lua_replace(L, -3);
  lua_rawset(L, absidx);
}
inline void luaL_requiref(lua_State *L, const char *modname,
                          lua_CFunction openf, int glb) {

  lua_pushcfunction(L, openf);
  lua_pushstring(L, modname);
  lua_call(L, 1, 1);

  if (glb) {
    lua_pushvalue(L, -1);
    lua_setglobal(L, modname);
  }
}
inline lua_Number lua_tonumberx(lua_State *L, int index, int *isnum) {
  if (isnum) {
    *isnum = lua_isnumber(L, index);
  }
  return lua_tonumber(L, index);
}
#endif
#if LUA_VERSION_NUM < 503
inline void lua_seti(lua_State *L, int index, lua_Integer n) {
  int absidx = lua_absindex(L, index);
  lua_pushvalue(L, -1);
  lua_pushinteger(L, n);
  lua_replace(L, -3);
  lua_rawset(L, absidx);
}
inline int lua_geti(lua_State *L, int index, lua_Integer i) {
  int absidx = lua_absindex(L, index);
  lua_pushinteger(L, i);
  lua_rawget(L, absidx);
  return lua_type(L, -1);
}
inline int lua_getfield_rtype(lua_State *L, int idx, const char *k) {
  lua_getfield(L, idx, k);
  return lua_type(L, -1);
}
inline int lua_gettable_rtype(lua_State *L, int idx) {
  lua_gettable(L, idx);
  return lua_type(L, -1);
}
inline int lua_rawget_rtype(lua_State *L, int idx) {
  lua_rawget(L, idx);
  return lua_type(L, -1);
}
#endif
#if LUA_VERSION_NUM < 501
void lua_createtable(lua_State *L, int narr, int nrec) { lua_newtable(L); }
#endif
}

using namespace compat;
}

// for c++03 implement

#define KAGUYA_VA_ARG(...) __VA_ARGS__

#define KAGUYA_PP_CAT(F, B) F##B

// generated by generate_preprocess_macro.py

#define KAGUYA_PP_REPEAT0(MACRO)
#define KAGUYA_PP_REPEAT1(MACRO) KAGUYA_PP_REPEAT0(MACRO) MACRO(1)
#define KAGUYA_PP_REPEAT2(MACRO) KAGUYA_PP_REPEAT1(MACRO) MACRO(2)
#define KAGUYA_PP_REPEAT3(MACRO) KAGUYA_PP_REPEAT2(MACRO) MACRO(3)
#define KAGUYA_PP_REPEAT4(MACRO) KAGUYA_PP_REPEAT3(MACRO) MACRO(4)
#define KAGUYA_PP_REPEAT5(MACRO) KAGUYA_PP_REPEAT4(MACRO) MACRO(5)
#define KAGUYA_PP_REPEAT6(MACRO) KAGUYA_PP_REPEAT5(MACRO) MACRO(6)
#define KAGUYA_PP_REPEAT7(MACRO) KAGUYA_PP_REPEAT6(MACRO) MACRO(7)
#define KAGUYA_PP_REPEAT8(MACRO) KAGUYA_PP_REPEAT7(MACRO) MACRO(8)
#define KAGUYA_PP_REPEAT9(MACRO) KAGUYA_PP_REPEAT8(MACRO) MACRO(9)
#define KAGUYA_PP_REPEAT10(MACRO) KAGUYA_PP_REPEAT9(MACRO) MACRO(10)
#define KAGUYA_PP_REPEAT11(MACRO) KAGUYA_PP_REPEAT10(MACRO) MACRO(11)
#define KAGUYA_PP_REPEAT12(MACRO) KAGUYA_PP_REPEAT11(MACRO) MACRO(12)
#define KAGUYA_PP_REPEAT13(MACRO) KAGUYA_PP_REPEAT12(MACRO) MACRO(13)
#define KAGUYA_PP_REPEAT14(MACRO) KAGUYA_PP_REPEAT13(MACRO) MACRO(14)
#define KAGUYA_PP_REPEAT15(MACRO) KAGUYA_PP_REPEAT14(MACRO) MACRO(15)
#define KAGUYA_PP_REPEAT16(MACRO) KAGUYA_PP_REPEAT15(MACRO) MACRO(16)
#define KAGUYA_PP_REPEAT17(MACRO) KAGUYA_PP_REPEAT16(MACRO) MACRO(17)
#define KAGUYA_PP_REPEAT18(MACRO) KAGUYA_PP_REPEAT17(MACRO) MACRO(18)
#define KAGUYA_PP_REPEAT19(MACRO) KAGUYA_PP_REPEAT18(MACRO) MACRO(19)
#define KAGUYA_PP_REPEAT20(MACRO) KAGUYA_PP_REPEAT19(MACRO) MACRO(20)
#define KAGUYA_PP_REPEAT21(MACRO) KAGUYA_PP_REPEAT20(MACRO) MACRO(21)
#define KAGUYA_PP_REPEAT22(MACRO) KAGUYA_PP_REPEAT21(MACRO) MACRO(22)
#define KAGUYA_PP_REPEAT23(MACRO) KAGUYA_PP_REPEAT22(MACRO) MACRO(23)
#define KAGUYA_PP_REPEAT24(MACRO) KAGUYA_PP_REPEAT23(MACRO) MACRO(24)
#define KAGUYA_PP_REPEAT25(MACRO) KAGUYA_PP_REPEAT24(MACRO) MACRO(25)
#define KAGUYA_PP_REPEAT26(MACRO) KAGUYA_PP_REPEAT25(MACRO) MACRO(26)
#define KAGUYA_PP_REPEAT27(MACRO) KAGUYA_PP_REPEAT26(MACRO) MACRO(27)
#define KAGUYA_PP_REPEAT28(MACRO) KAGUYA_PP_REPEAT27(MACRO) MACRO(28)
#define KAGUYA_PP_REPEAT29(MACRO) KAGUYA_PP_REPEAT28(MACRO) MACRO(29)
#define KAGUYA_PP_REPEAT30(MACRO) KAGUYA_PP_REPEAT29(MACRO) MACRO(30)
#define KAGUYA_PP_REPEAT31(MACRO) KAGUYA_PP_REPEAT30(MACRO) MACRO(31)
#define KAGUYA_PP_REPEAT32(MACRO) KAGUYA_PP_REPEAT31(MACRO) MACRO(32)
#define KAGUYA_PP_REPEAT33(MACRO) KAGUYA_PP_REPEAT32(MACRO) MACRO(33)
#define KAGUYA_PP_REPEAT34(MACRO) KAGUYA_PP_REPEAT33(MACRO) MACRO(34)
#define KAGUYA_PP_REPEAT35(MACRO) KAGUYA_PP_REPEAT34(MACRO) MACRO(35)
#define KAGUYA_PP_REPEAT36(MACRO) KAGUYA_PP_REPEAT35(MACRO) MACRO(36)
#define KAGUYA_PP_REPEAT37(MACRO) KAGUYA_PP_REPEAT36(MACRO) MACRO(37)
#define KAGUYA_PP_REPEAT38(MACRO) KAGUYA_PP_REPEAT37(MACRO) MACRO(38)
#define KAGUYA_PP_REPEAT39(MACRO) KAGUYA_PP_REPEAT38(MACRO) MACRO(39)
#define KAGUYA_PP_REPEAT40(MACRO) KAGUYA_PP_REPEAT39(MACRO) MACRO(40)
#define KAGUYA_PP_REPEAT41(MACRO) KAGUYA_PP_REPEAT40(MACRO) MACRO(41)
#define KAGUYA_PP_REPEAT42(MACRO) KAGUYA_PP_REPEAT41(MACRO) MACRO(42)
#define KAGUYA_PP_REPEAT43(MACRO) KAGUYA_PP_REPEAT42(MACRO) MACRO(43)
#define KAGUYA_PP_REPEAT44(MACRO) KAGUYA_PP_REPEAT43(MACRO) MACRO(44)
#define KAGUYA_PP_REPEAT45(MACRO) KAGUYA_PP_REPEAT44(MACRO) MACRO(45)
#define KAGUYA_PP_REPEAT46(MACRO) KAGUYA_PP_REPEAT45(MACRO) MACRO(46)
#define KAGUYA_PP_REPEAT47(MACRO) KAGUYA_PP_REPEAT46(MACRO) MACRO(47)
#define KAGUYA_PP_REPEAT48(MACRO) KAGUYA_PP_REPEAT47(MACRO) MACRO(48)
#define KAGUYA_PP_REPEAT49(MACRO) KAGUYA_PP_REPEAT48(MACRO) MACRO(49)
#define KAGUYA_PP_REPEAT50(MACRO) KAGUYA_PP_REPEAT49(MACRO) MACRO(50)
#define KAGUYA_PP_REPEAT51(MACRO) KAGUYA_PP_REPEAT50(MACRO) MACRO(51)
#define KAGUYA_PP_REPEAT52(MACRO) KAGUYA_PP_REPEAT51(MACRO) MACRO(52)
#define KAGUYA_PP_REPEAT53(MACRO) KAGUYA_PP_REPEAT52(MACRO) MACRO(53)
#define KAGUYA_PP_REPEAT54(MACRO) KAGUYA_PP_REPEAT53(MACRO) MACRO(54)
#define KAGUYA_PP_REPEAT55(MACRO) KAGUYA_PP_REPEAT54(MACRO) MACRO(55)
#define KAGUYA_PP_REPEAT56(MACRO) KAGUYA_PP_REPEAT55(MACRO) MACRO(56)
#define KAGUYA_PP_REPEAT57(MACRO) KAGUYA_PP_REPEAT56(MACRO) MACRO(57)
#define KAGUYA_PP_REPEAT58(MACRO) KAGUYA_PP_REPEAT57(MACRO) MACRO(58)
#define KAGUYA_PP_REPEAT59(MACRO) KAGUYA_PP_REPEAT58(MACRO) MACRO(59)
#define KAGUYA_PP_REPEAT60(MACRO) KAGUYA_PP_REPEAT59(MACRO) MACRO(60)
#define KAGUYA_PP_REPEAT61(MACRO) KAGUYA_PP_REPEAT60(MACRO) MACRO(61)
#define KAGUYA_PP_REPEAT62(MACRO) KAGUYA_PP_REPEAT61(MACRO) MACRO(62)
#define KAGUYA_PP_REPEAT63(MACRO) KAGUYA_PP_REPEAT62(MACRO) MACRO(63)
#define KAGUYA_PP_REPEAT64(MACRO) KAGUYA_PP_REPEAT63(MACRO) MACRO(64)
#define KAGUYA_PP_REPEAT65(MACRO) KAGUYA_PP_REPEAT64(MACRO) MACRO(65)
#define KAGUYA_PP_REPEAT66(MACRO) KAGUYA_PP_REPEAT65(MACRO) MACRO(66)
#define KAGUYA_PP_REPEAT67(MACRO) KAGUYA_PP_REPEAT66(MACRO) MACRO(67)
#define KAGUYA_PP_REPEAT68(MACRO) KAGUYA_PP_REPEAT67(MACRO) MACRO(68)
#define KAGUYA_PP_REPEAT69(MACRO) KAGUYA_PP_REPEAT68(MACRO) MACRO(69)
#define KAGUYA_PP_REPEAT70(MACRO) KAGUYA_PP_REPEAT69(MACRO) MACRO(70)
#define KAGUYA_PP_REPEAT71(MACRO) KAGUYA_PP_REPEAT70(MACRO) MACRO(71)
#define KAGUYA_PP_REPEAT72(MACRO) KAGUYA_PP_REPEAT71(MACRO) MACRO(72)
#define KAGUYA_PP_REPEAT73(MACRO) KAGUYA_PP_REPEAT72(MACRO) MACRO(73)
#define KAGUYA_PP_REPEAT74(MACRO) KAGUYA_PP_REPEAT73(MACRO) MACRO(74)
#define KAGUYA_PP_REPEAT75(MACRO) KAGUYA_PP_REPEAT74(MACRO) MACRO(75)
#define KAGUYA_PP_REPEAT76(MACRO) KAGUYA_PP_REPEAT75(MACRO) MACRO(76)
#define KAGUYA_PP_REPEAT77(MACRO) KAGUYA_PP_REPEAT76(MACRO) MACRO(77)
#define KAGUYA_PP_REPEAT78(MACRO) KAGUYA_PP_REPEAT77(MACRO) MACRO(78)
#define KAGUYA_PP_REPEAT79(MACRO) KAGUYA_PP_REPEAT78(MACRO) MACRO(79)
#define KAGUYA_PP_REPEAT80(MACRO) KAGUYA_PP_REPEAT79(MACRO) MACRO(80)
#define KAGUYA_PP_REPEAT81(MACRO) KAGUYA_PP_REPEAT80(MACRO) MACRO(81)
#define KAGUYA_PP_REPEAT82(MACRO) KAGUYA_PP_REPEAT81(MACRO) MACRO(82)
#define KAGUYA_PP_REPEAT83(MACRO) KAGUYA_PP_REPEAT82(MACRO) MACRO(83)
#define KAGUYA_PP_REPEAT84(MACRO) KAGUYA_PP_REPEAT83(MACRO) MACRO(84)
#define KAGUYA_PP_REPEAT85(MACRO) KAGUYA_PP_REPEAT84(MACRO) MACRO(85)
#define KAGUYA_PP_REPEAT86(MACRO) KAGUYA_PP_REPEAT85(MACRO) MACRO(86)
#define KAGUYA_PP_REPEAT87(MACRO) KAGUYA_PP_REPEAT86(MACRO) MACRO(87)
#define KAGUYA_PP_REPEAT88(MACRO) KAGUYA_PP_REPEAT87(MACRO) MACRO(88)
#define KAGUYA_PP_REPEAT89(MACRO) KAGUYA_PP_REPEAT88(MACRO) MACRO(89)
#define KAGUYA_PP_REPEAT90(MACRO) KAGUYA_PP_REPEAT89(MACRO) MACRO(90)
#define KAGUYA_PP_REPEAT91(MACRO) KAGUYA_PP_REPEAT90(MACRO) MACRO(91)
#define KAGUYA_PP_REPEAT92(MACRO) KAGUYA_PP_REPEAT91(MACRO) MACRO(92)
#define KAGUYA_PP_REPEAT93(MACRO) KAGUYA_PP_REPEAT92(MACRO) MACRO(93)
#define KAGUYA_PP_REPEAT94(MACRO) KAGUYA_PP_REPEAT93(MACRO) MACRO(94)
#define KAGUYA_PP_REPEAT95(MACRO) KAGUYA_PP_REPEAT94(MACRO) MACRO(95)
#define KAGUYA_PP_REPEAT96(MACRO) KAGUYA_PP_REPEAT95(MACRO) MACRO(96)
#define KAGUYA_PP_REPEAT97(MACRO) KAGUYA_PP_REPEAT96(MACRO) MACRO(97)
#define KAGUYA_PP_REPEAT98(MACRO) KAGUYA_PP_REPEAT97(MACRO) MACRO(98)
#define KAGUYA_PP_REPEAT99(MACRO) KAGUYA_PP_REPEAT98(MACRO) MACRO(99)
#define KAGUYA_PP_REPEAT100(MACRO) KAGUYA_PP_REPEAT99(MACRO) MACRO(100)
#define KAGUYA_PP_REPEAT101(MACRO) KAGUYA_PP_REPEAT100(MACRO) MACRO(101)
#define KAGUYA_PP_REPEAT102(MACRO) KAGUYA_PP_REPEAT101(MACRO) MACRO(102)
#define KAGUYA_PP_REPEAT103(MACRO) KAGUYA_PP_REPEAT102(MACRO) MACRO(103)
#define KAGUYA_PP_REPEAT104(MACRO) KAGUYA_PP_REPEAT103(MACRO) MACRO(104)
#define KAGUYA_PP_REPEAT105(MACRO) KAGUYA_PP_REPEAT104(MACRO) MACRO(105)
#define KAGUYA_PP_REPEAT106(MACRO) KAGUYA_PP_REPEAT105(MACRO) MACRO(106)
#define KAGUYA_PP_REPEAT107(MACRO) KAGUYA_PP_REPEAT106(MACRO) MACRO(107)
#define KAGUYA_PP_REPEAT108(MACRO) KAGUYA_PP_REPEAT107(MACRO) MACRO(108)
#define KAGUYA_PP_REPEAT109(MACRO) KAGUYA_PP_REPEAT108(MACRO) MACRO(109)
#define KAGUYA_PP_REPEAT110(MACRO) KAGUYA_PP_REPEAT109(MACRO) MACRO(110)
#define KAGUYA_PP_REPEAT111(MACRO) KAGUYA_PP_REPEAT110(MACRO) MACRO(111)
#define KAGUYA_PP_REPEAT112(MACRO) KAGUYA_PP_REPEAT111(MACRO) MACRO(112)
#define KAGUYA_PP_REPEAT113(MACRO) KAGUYA_PP_REPEAT112(MACRO) MACRO(113)
#define KAGUYA_PP_REPEAT114(MACRO) KAGUYA_PP_REPEAT113(MACRO) MACRO(114)
#define KAGUYA_PP_REPEAT115(MACRO) KAGUYA_PP_REPEAT114(MACRO) MACRO(115)
#define KAGUYA_PP_REPEAT116(MACRO) KAGUYA_PP_REPEAT115(MACRO) MACRO(116)
#define KAGUYA_PP_REPEAT117(MACRO) KAGUYA_PP_REPEAT116(MACRO) MACRO(117)
#define KAGUYA_PP_REPEAT118(MACRO) KAGUYA_PP_REPEAT117(MACRO) MACRO(118)
#define KAGUYA_PP_REPEAT119(MACRO) KAGUYA_PP_REPEAT118(MACRO) MACRO(119)
#define KAGUYA_PP_REPEAT120(MACRO) KAGUYA_PP_REPEAT119(MACRO) MACRO(120)
#define KAGUYA_PP_REPEAT121(MACRO) KAGUYA_PP_REPEAT120(MACRO) MACRO(121)
#define KAGUYA_PP_REPEAT122(MACRO) KAGUYA_PP_REPEAT121(MACRO) MACRO(122)
#define KAGUYA_PP_REPEAT123(MACRO) KAGUYA_PP_REPEAT122(MACRO) MACRO(123)
#define KAGUYA_PP_REPEAT124(MACRO) KAGUYA_PP_REPEAT123(MACRO) MACRO(124)
#define KAGUYA_PP_REPEAT125(MACRO) KAGUYA_PP_REPEAT124(MACRO) MACRO(125)
#define KAGUYA_PP_REPEAT126(MACRO) KAGUYA_PP_REPEAT125(MACRO) MACRO(126)
#define KAGUYA_PP_REPEAT127(MACRO) KAGUYA_PP_REPEAT126(MACRO) MACRO(127)
#define KAGUYA_PP_REPEAT128(MACRO) KAGUYA_PP_REPEAT127(MACRO) MACRO(128)
#define KAGUYA_PP_REPEAT129(MACRO) KAGUYA_PP_REPEAT128(MACRO) MACRO(129)
#define KAGUYA_PP_REPEAT130(MACRO) KAGUYA_PP_REPEAT129(MACRO) MACRO(130)
#define KAGUYA_PP_REPEAT131(MACRO) KAGUYA_PP_REPEAT130(MACRO) MACRO(131)
#define KAGUYA_PP_REPEAT132(MACRO) KAGUYA_PP_REPEAT131(MACRO) MACRO(132)
#define KAGUYA_PP_REPEAT133(MACRO) KAGUYA_PP_REPEAT132(MACRO) MACRO(133)
#define KAGUYA_PP_REPEAT134(MACRO) KAGUYA_PP_REPEAT133(MACRO) MACRO(134)
#define KAGUYA_PP_REPEAT135(MACRO) KAGUYA_PP_REPEAT134(MACRO) MACRO(135)
#define KAGUYA_PP_REPEAT136(MACRO) KAGUYA_PP_REPEAT135(MACRO) MACRO(136)
#define KAGUYA_PP_REPEAT137(MACRO) KAGUYA_PP_REPEAT136(MACRO) MACRO(137)
#define KAGUYA_PP_REPEAT138(MACRO) KAGUYA_PP_REPEAT137(MACRO) MACRO(138)
#define KAGUYA_PP_REPEAT139(MACRO) KAGUYA_PP_REPEAT138(MACRO) MACRO(139)
#define KAGUYA_PP_REPEAT140(MACRO) KAGUYA_PP_REPEAT139(MACRO) MACRO(140)
#define KAGUYA_PP_REPEAT141(MACRO) KAGUYA_PP_REPEAT140(MACRO) MACRO(141)
#define KAGUYA_PP_REPEAT142(MACRO) KAGUYA_PP_REPEAT141(MACRO) MACRO(142)
#define KAGUYA_PP_REPEAT143(MACRO) KAGUYA_PP_REPEAT142(MACRO) MACRO(143)
#define KAGUYA_PP_REPEAT144(MACRO) KAGUYA_PP_REPEAT143(MACRO) MACRO(144)
#define KAGUYA_PP_REPEAT145(MACRO) KAGUYA_PP_REPEAT144(MACRO) MACRO(145)
#define KAGUYA_PP_REPEAT146(MACRO) KAGUYA_PP_REPEAT145(MACRO) MACRO(146)
#define KAGUYA_PP_REPEAT147(MACRO) KAGUYA_PP_REPEAT146(MACRO) MACRO(147)
#define KAGUYA_PP_REPEAT148(MACRO) KAGUYA_PP_REPEAT147(MACRO) MACRO(148)
#define KAGUYA_PP_REPEAT149(MACRO) KAGUYA_PP_REPEAT148(MACRO) MACRO(149)
#define KAGUYA_PP_REPEAT150(MACRO) KAGUYA_PP_REPEAT149(MACRO) MACRO(150)
#define KAGUYA_PP_REPEAT151(MACRO) KAGUYA_PP_REPEAT150(MACRO) MACRO(151)
#define KAGUYA_PP_REPEAT152(MACRO) KAGUYA_PP_REPEAT151(MACRO) MACRO(152)
#define KAGUYA_PP_REPEAT153(MACRO) KAGUYA_PP_REPEAT152(MACRO) MACRO(153)
#define KAGUYA_PP_REPEAT154(MACRO) KAGUYA_PP_REPEAT153(MACRO) MACRO(154)
#define KAGUYA_PP_REPEAT155(MACRO) KAGUYA_PP_REPEAT154(MACRO) MACRO(155)
#define KAGUYA_PP_REPEAT156(MACRO) KAGUYA_PP_REPEAT155(MACRO) MACRO(156)
#define KAGUYA_PP_REPEAT157(MACRO) KAGUYA_PP_REPEAT156(MACRO) MACRO(157)
#define KAGUYA_PP_REPEAT158(MACRO) KAGUYA_PP_REPEAT157(MACRO) MACRO(158)
#define KAGUYA_PP_REPEAT159(MACRO) KAGUYA_PP_REPEAT158(MACRO) MACRO(159)
#define KAGUYA_PP_REPEAT160(MACRO) KAGUYA_PP_REPEAT159(MACRO) MACRO(160)
#define KAGUYA_PP_REPEAT161(MACRO) KAGUYA_PP_REPEAT160(MACRO) MACRO(161)
#define KAGUYA_PP_REPEAT162(MACRO) KAGUYA_PP_REPEAT161(MACRO) MACRO(162)
#define KAGUYA_PP_REPEAT163(MACRO) KAGUYA_PP_REPEAT162(MACRO) MACRO(163)
#define KAGUYA_PP_REPEAT164(MACRO) KAGUYA_PP_REPEAT163(MACRO) MACRO(164)
#define KAGUYA_PP_REPEAT165(MACRO) KAGUYA_PP_REPEAT164(MACRO) MACRO(165)
#define KAGUYA_PP_REPEAT166(MACRO) KAGUYA_PP_REPEAT165(MACRO) MACRO(166)
#define KAGUYA_PP_REPEAT167(MACRO) KAGUYA_PP_REPEAT166(MACRO) MACRO(167)
#define KAGUYA_PP_REPEAT168(MACRO) KAGUYA_PP_REPEAT167(MACRO) MACRO(168)
#define KAGUYA_PP_REPEAT169(MACRO) KAGUYA_PP_REPEAT168(MACRO) MACRO(169)
#define KAGUYA_PP_REPEAT170(MACRO) KAGUYA_PP_REPEAT169(MACRO) MACRO(170)
#define KAGUYA_PP_REPEAT171(MACRO) KAGUYA_PP_REPEAT170(MACRO) MACRO(171)
#define KAGUYA_PP_REPEAT172(MACRO) KAGUYA_PP_REPEAT171(MACRO) MACRO(172)
#define KAGUYA_PP_REPEAT173(MACRO) KAGUYA_PP_REPEAT172(MACRO) MACRO(173)
#define KAGUYA_PP_REPEAT174(MACRO) KAGUYA_PP_REPEAT173(MACRO) MACRO(174)
#define KAGUYA_PP_REPEAT175(MACRO) KAGUYA_PP_REPEAT174(MACRO) MACRO(175)
#define KAGUYA_PP_REPEAT176(MACRO) KAGUYA_PP_REPEAT175(MACRO) MACRO(176)
#define KAGUYA_PP_REPEAT177(MACRO) KAGUYA_PP_REPEAT176(MACRO) MACRO(177)
#define KAGUYA_PP_REPEAT178(MACRO) KAGUYA_PP_REPEAT177(MACRO) MACRO(178)
#define KAGUYA_PP_REPEAT179(MACRO) KAGUYA_PP_REPEAT178(MACRO) MACRO(179)
#define KAGUYA_PP_REPEAT180(MACRO) KAGUYA_PP_REPEAT179(MACRO) MACRO(180)
#define KAGUYA_PP_REPEAT181(MACRO) KAGUYA_PP_REPEAT180(MACRO) MACRO(181)
#define KAGUYA_PP_REPEAT182(MACRO) KAGUYA_PP_REPEAT181(MACRO) MACRO(182)
#define KAGUYA_PP_REPEAT183(MACRO) KAGUYA_PP_REPEAT182(MACRO) MACRO(183)
#define KAGUYA_PP_REPEAT184(MACRO) KAGUYA_PP_REPEAT183(MACRO) MACRO(184)
#define KAGUYA_PP_REPEAT185(MACRO) KAGUYA_PP_REPEAT184(MACRO) MACRO(185)
#define KAGUYA_PP_REPEAT186(MACRO) KAGUYA_PP_REPEAT185(MACRO) MACRO(186)
#define KAGUYA_PP_REPEAT187(MACRO) KAGUYA_PP_REPEAT186(MACRO) MACRO(187)
#define KAGUYA_PP_REPEAT188(MACRO) KAGUYA_PP_REPEAT187(MACRO) MACRO(188)
#define KAGUYA_PP_REPEAT189(MACRO) KAGUYA_PP_REPEAT188(MACRO) MACRO(189)
#define KAGUYA_PP_REPEAT190(MACRO) KAGUYA_PP_REPEAT189(MACRO) MACRO(190)
#define KAGUYA_PP_REPEAT191(MACRO) KAGUYA_PP_REPEAT190(MACRO) MACRO(191)
#define KAGUYA_PP_REPEAT192(MACRO) KAGUYA_PP_REPEAT191(MACRO) MACRO(192)
#define KAGUYA_PP_REPEAT193(MACRO) KAGUYA_PP_REPEAT192(MACRO) MACRO(193)
#define KAGUYA_PP_REPEAT194(MACRO) KAGUYA_PP_REPEAT193(MACRO) MACRO(194)
#define KAGUYA_PP_REPEAT195(MACRO) KAGUYA_PP_REPEAT194(MACRO) MACRO(195)
#define KAGUYA_PP_REPEAT196(MACRO) KAGUYA_PP_REPEAT195(MACRO) MACRO(196)
#define KAGUYA_PP_REPEAT197(MACRO) KAGUYA_PP_REPEAT196(MACRO) MACRO(197)
#define KAGUYA_PP_REPEAT198(MACRO) KAGUYA_PP_REPEAT197(MACRO) MACRO(198)
#define KAGUYA_PP_REPEAT199(MACRO) KAGUYA_PP_REPEAT198(MACRO) MACRO(199)
#define KAGUYA_PP_REPEAT200(MACRO) KAGUYA_PP_REPEAT199(MACRO) MACRO(200)
#define KAGUYA_PP_REPEAT201(MACRO) KAGUYA_PP_REPEAT200(MACRO) MACRO(201)
#define KAGUYA_PP_REPEAT202(MACRO) KAGUYA_PP_REPEAT201(MACRO) MACRO(202)
#define KAGUYA_PP_REPEAT203(MACRO) KAGUYA_PP_REPEAT202(MACRO) MACRO(203)
#define KAGUYA_PP_REPEAT204(MACRO) KAGUYA_PP_REPEAT203(MACRO) MACRO(204)
#define KAGUYA_PP_REPEAT205(MACRO) KAGUYA_PP_REPEAT204(MACRO) MACRO(205)
#define KAGUYA_PP_REPEAT206(MACRO) KAGUYA_PP_REPEAT205(MACRO) MACRO(206)
#define KAGUYA_PP_REPEAT207(MACRO) KAGUYA_PP_REPEAT206(MACRO) MACRO(207)
#define KAGUYA_PP_REPEAT208(MACRO) KAGUYA_PP_REPEAT207(MACRO) MACRO(208)
#define KAGUYA_PP_REPEAT209(MACRO) KAGUYA_PP_REPEAT208(MACRO) MACRO(209)
#define KAGUYA_PP_REPEAT210(MACRO) KAGUYA_PP_REPEAT209(MACRO) MACRO(210)
#define KAGUYA_PP_REPEAT211(MACRO) KAGUYA_PP_REPEAT210(MACRO) MACRO(211)
#define KAGUYA_PP_REPEAT212(MACRO) KAGUYA_PP_REPEAT211(MACRO) MACRO(212)
#define KAGUYA_PP_REPEAT213(MACRO) KAGUYA_PP_REPEAT212(MACRO) MACRO(213)
#define KAGUYA_PP_REPEAT214(MACRO) KAGUYA_PP_REPEAT213(MACRO) MACRO(214)
#define KAGUYA_PP_REPEAT215(MACRO) KAGUYA_PP_REPEAT214(MACRO) MACRO(215)
#define KAGUYA_PP_REPEAT216(MACRO) KAGUYA_PP_REPEAT215(MACRO) MACRO(216)
#define KAGUYA_PP_REPEAT217(MACRO) KAGUYA_PP_REPEAT216(MACRO) MACRO(217)
#define KAGUYA_PP_REPEAT218(MACRO) KAGUYA_PP_REPEAT217(MACRO) MACRO(218)
#define KAGUYA_PP_REPEAT219(MACRO) KAGUYA_PP_REPEAT218(MACRO) MACRO(219)
#define KAGUYA_PP_REPEAT220(MACRO) KAGUYA_PP_REPEAT219(MACRO) MACRO(220)
#define KAGUYA_PP_REPEAT221(MACRO) KAGUYA_PP_REPEAT220(MACRO) MACRO(221)
#define KAGUYA_PP_REPEAT222(MACRO) KAGUYA_PP_REPEAT221(MACRO) MACRO(222)
#define KAGUYA_PP_REPEAT223(MACRO) KAGUYA_PP_REPEAT222(MACRO) MACRO(223)
#define KAGUYA_PP_REPEAT224(MACRO) KAGUYA_PP_REPEAT223(MACRO) MACRO(224)
#define KAGUYA_PP_REPEAT225(MACRO) KAGUYA_PP_REPEAT224(MACRO) MACRO(225)
#define KAGUYA_PP_REPEAT226(MACRO) KAGUYA_PP_REPEAT225(MACRO) MACRO(226)
#define KAGUYA_PP_REPEAT227(MACRO) KAGUYA_PP_REPEAT226(MACRO) MACRO(227)
#define KAGUYA_PP_REPEAT228(MACRO) KAGUYA_PP_REPEAT227(MACRO) MACRO(228)
#define KAGUYA_PP_REPEAT229(MACRO) KAGUYA_PP_REPEAT228(MACRO) MACRO(229)
#define KAGUYA_PP_REPEAT230(MACRO) KAGUYA_PP_REPEAT229(MACRO) MACRO(230)
#define KAGUYA_PP_REPEAT231(MACRO) KAGUYA_PP_REPEAT230(MACRO) MACRO(231)
#define KAGUYA_PP_REPEAT232(MACRO) KAGUYA_PP_REPEAT231(MACRO) MACRO(232)
#define KAGUYA_PP_REPEAT233(MACRO) KAGUYA_PP_REPEAT232(MACRO) MACRO(233)
#define KAGUYA_PP_REPEAT234(MACRO) KAGUYA_PP_REPEAT233(MACRO) MACRO(234)
#define KAGUYA_PP_REPEAT235(MACRO) KAGUYA_PP_REPEAT234(MACRO) MACRO(235)
#define KAGUYA_PP_REPEAT236(MACRO) KAGUYA_PP_REPEAT235(MACRO) MACRO(236)
#define KAGUYA_PP_REPEAT237(MACRO) KAGUYA_PP_REPEAT236(MACRO) MACRO(237)
#define KAGUYA_PP_REPEAT238(MACRO) KAGUYA_PP_REPEAT237(MACRO) MACRO(238)
#define KAGUYA_PP_REPEAT239(MACRO) KAGUYA_PP_REPEAT238(MACRO) MACRO(239)
#define KAGUYA_PP_REPEAT240(MACRO) KAGUYA_PP_REPEAT239(MACRO) MACRO(240)
#define KAGUYA_PP_REPEAT241(MACRO) KAGUYA_PP_REPEAT240(MACRO) MACRO(241)
#define KAGUYA_PP_REPEAT242(MACRO) KAGUYA_PP_REPEAT241(MACRO) MACRO(242)
#define KAGUYA_PP_REPEAT243(MACRO) KAGUYA_PP_REPEAT242(MACRO) MACRO(243)
#define KAGUYA_PP_REPEAT244(MACRO) KAGUYA_PP_REPEAT243(MACRO) MACRO(244)
#define KAGUYA_PP_REPEAT245(MACRO) KAGUYA_PP_REPEAT244(MACRO) MACRO(245)
#define KAGUYA_PP_REPEAT246(MACRO) KAGUYA_PP_REPEAT245(MACRO) MACRO(246)
#define KAGUYA_PP_REPEAT247(MACRO) KAGUYA_PP_REPEAT246(MACRO) MACRO(247)
#define KAGUYA_PP_REPEAT248(MACRO) KAGUYA_PP_REPEAT247(MACRO) MACRO(248)
#define KAGUYA_PP_REPEAT249(MACRO) KAGUYA_PP_REPEAT248(MACRO) MACRO(249)
#define KAGUYA_PP_REPEAT250(MACRO) KAGUYA_PP_REPEAT249(MACRO) MACRO(250)
#define KAGUYA_PP_REPEAT251(MACRO) KAGUYA_PP_REPEAT250(MACRO) MACRO(251)
#define KAGUYA_PP_REPEAT252(MACRO) KAGUYA_PP_REPEAT251(MACRO) MACRO(252)
#define KAGUYA_PP_REPEAT253(MACRO) KAGUYA_PP_REPEAT252(MACRO) MACRO(253)
#define KAGUYA_PP_REPEAT254(MACRO) KAGUYA_PP_REPEAT253(MACRO) MACRO(254)
#define KAGUYA_PP_REPEAT(COUNT, MACRO)                                         \
  KAGUYA_PP_CAT(KAGUYA_PP_REPEAT, COUNT)(MACRO)

#define KAGUYA_PP_REPEAT_DEF0(MACRO)
#define KAGUYA_PP_REPEAT_DEF1(MACRO) KAGUYA_PP_REPEAT_DEF0(MACRO) MACRO(1)
#define KAGUYA_PP_REPEAT_DEF2(MACRO) KAGUYA_PP_REPEAT_DEF1(MACRO) MACRO(2)
#define KAGUYA_PP_REPEAT_DEF3(MACRO) KAGUYA_PP_REPEAT_DEF2(MACRO) MACRO(3)
#define KAGUYA_PP_REPEAT_DEF4(MACRO) KAGUYA_PP_REPEAT_DEF3(MACRO) MACRO(4)
#define KAGUYA_PP_REPEAT_DEF5(MACRO) KAGUYA_PP_REPEAT_DEF4(MACRO) MACRO(5)
#define KAGUYA_PP_REPEAT_DEF6(MACRO) KAGUYA_PP_REPEAT_DEF5(MACRO) MACRO(6)
#define KAGUYA_PP_REPEAT_DEF7(MACRO) KAGUYA_PP_REPEAT_DEF6(MACRO) MACRO(7)
#define KAGUYA_PP_REPEAT_DEF8(MACRO) KAGUYA_PP_REPEAT_DEF7(MACRO) MACRO(8)
#define KAGUYA_PP_REPEAT_DEF9(MACRO) KAGUYA_PP_REPEAT_DEF8(MACRO) MACRO(9)
#define KAGUYA_PP_REPEAT_DEF10(MACRO) KAGUYA_PP_REPEAT_DEF9(MACRO) MACRO(10)
#define KAGUYA_PP_REPEAT_DEF11(MACRO) KAGUYA_PP_REPEAT_DEF10(MACRO) MACRO(11)
#define KAGUYA_PP_REPEAT_DEF12(MACRO) KAGUYA_PP_REPEAT_DEF11(MACRO) MACRO(12)
#define KAGUYA_PP_REPEAT_DEF13(MACRO) KAGUYA_PP_REPEAT_DEF12(MACRO) MACRO(13)
#define KAGUYA_PP_REPEAT_DEF14(MACRO) KAGUYA_PP_REPEAT_DEF13(MACRO) MACRO(14)
#define KAGUYA_PP_REPEAT_DEF15(MACRO) KAGUYA_PP_REPEAT_DEF14(MACRO) MACRO(15)
#define KAGUYA_PP_REPEAT_DEF16(MACRO) KAGUYA_PP_REPEAT_DEF15(MACRO) MACRO(16)
#define KAGUYA_PP_REPEAT_DEF17(MACRO) KAGUYA_PP_REPEAT_DEF16(MACRO) MACRO(17)
#define KAGUYA_PP_REPEAT_DEF18(MACRO) KAGUYA_PP_REPEAT_DEF17(MACRO) MACRO(18)
#define KAGUYA_PP_REPEAT_DEF19(MACRO) KAGUYA_PP_REPEAT_DEF18(MACRO) MACRO(19)
#define KAGUYA_PP_REPEAT_DEF20(MACRO) KAGUYA_PP_REPEAT_DEF19(MACRO) MACRO(20)
#define KAGUYA_PP_REPEAT_DEF21(MACRO) KAGUYA_PP_REPEAT_DEF20(MACRO) MACRO(21)
#define KAGUYA_PP_REPEAT_DEF22(MACRO) KAGUYA_PP_REPEAT_DEF21(MACRO) MACRO(22)
#define KAGUYA_PP_REPEAT_DEF23(MACRO) KAGUYA_PP_REPEAT_DEF22(MACRO) MACRO(23)
#define KAGUYA_PP_REPEAT_DEF24(MACRO) KAGUYA_PP_REPEAT_DEF23(MACRO) MACRO(24)
#define KAGUYA_PP_REPEAT_DEF25(MACRO) KAGUYA_PP_REPEAT_DEF24(MACRO) MACRO(25)
#define KAGUYA_PP_REPEAT_DEF26(MACRO) KAGUYA_PP_REPEAT_DEF25(MACRO) MACRO(26)
#define KAGUYA_PP_REPEAT_DEF27(MACRO) KAGUYA_PP_REPEAT_DEF26(MACRO) MACRO(27)
#define KAGUYA_PP_REPEAT_DEF28(MACRO) KAGUYA_PP_REPEAT_DEF27(MACRO) MACRO(28)
#define KAGUYA_PP_REPEAT_DEF29(MACRO) KAGUYA_PP_REPEAT_DEF28(MACRO) MACRO(29)
#define KAGUYA_PP_REPEAT_DEF30(MACRO) KAGUYA_PP_REPEAT_DEF29(MACRO) MACRO(30)
#define KAGUYA_PP_REPEAT_DEF31(MACRO) KAGUYA_PP_REPEAT_DEF30(MACRO) MACRO(31)
#define KAGUYA_PP_REPEAT_DEF32(MACRO) KAGUYA_PP_REPEAT_DEF31(MACRO) MACRO(32)
#define KAGUYA_PP_REPEAT_DEF33(MACRO) KAGUYA_PP_REPEAT_DEF32(MACRO) MACRO(33)
#define KAGUYA_PP_REPEAT_DEF34(MACRO) KAGUYA_PP_REPEAT_DEF33(MACRO) MACRO(34)
#define KAGUYA_PP_REPEAT_DEF35(MACRO) KAGUYA_PP_REPEAT_DEF34(MACRO) MACRO(35)
#define KAGUYA_PP_REPEAT_DEF36(MACRO) KAGUYA_PP_REPEAT_DEF35(MACRO) MACRO(36)
#define KAGUYA_PP_REPEAT_DEF37(MACRO) KAGUYA_PP_REPEAT_DEF36(MACRO) MACRO(37)
#define KAGUYA_PP_REPEAT_DEF38(MACRO) KAGUYA_PP_REPEAT_DEF37(MACRO) MACRO(38)
#define KAGUYA_PP_REPEAT_DEF39(MACRO) KAGUYA_PP_REPEAT_DEF38(MACRO) MACRO(39)
#define KAGUYA_PP_REPEAT_DEF40(MACRO) KAGUYA_PP_REPEAT_DEF39(MACRO) MACRO(40)
#define KAGUYA_PP_REPEAT_DEF41(MACRO) KAGUYA_PP_REPEAT_DEF40(MACRO) MACRO(41)
#define KAGUYA_PP_REPEAT_DEF42(MACRO) KAGUYA_PP_REPEAT_DEF41(MACRO) MACRO(42)
#define KAGUYA_PP_REPEAT_DEF43(MACRO) KAGUYA_PP_REPEAT_DEF42(MACRO) MACRO(43)
#define KAGUYA_PP_REPEAT_DEF44(MACRO) KAGUYA_PP_REPEAT_DEF43(MACRO) MACRO(44)
#define KAGUYA_PP_REPEAT_DEF45(MACRO) KAGUYA_PP_REPEAT_DEF44(MACRO) MACRO(45)
#define KAGUYA_PP_REPEAT_DEF46(MACRO) KAGUYA_PP_REPEAT_DEF45(MACRO) MACRO(46)
#define KAGUYA_PP_REPEAT_DEF47(MACRO) KAGUYA_PP_REPEAT_DEF46(MACRO) MACRO(47)
#define KAGUYA_PP_REPEAT_DEF48(MACRO) KAGUYA_PP_REPEAT_DEF47(MACRO) MACRO(48)
#define KAGUYA_PP_REPEAT_DEF49(MACRO) KAGUYA_PP_REPEAT_DEF48(MACRO) MACRO(49)
#define KAGUYA_PP_REPEAT_DEF50(MACRO) KAGUYA_PP_REPEAT_DEF49(MACRO) MACRO(50)
#define KAGUYA_PP_REPEAT_DEF51(MACRO) KAGUYA_PP_REPEAT_DEF50(MACRO) MACRO(51)
#define KAGUYA_PP_REPEAT_DEF52(MACRO) KAGUYA_PP_REPEAT_DEF51(MACRO) MACRO(52)
#define KAGUYA_PP_REPEAT_DEF53(MACRO) KAGUYA_PP_REPEAT_DEF52(MACRO) MACRO(53)
#define KAGUYA_PP_REPEAT_DEF54(MACRO) KAGUYA_PP_REPEAT_DEF53(MACRO) MACRO(54)
#define KAGUYA_PP_REPEAT_DEF55(MACRO) KAGUYA_PP_REPEAT_DEF54(MACRO) MACRO(55)
#define KAGUYA_PP_REPEAT_DEF56(MACRO) KAGUYA_PP_REPEAT_DEF55(MACRO) MACRO(56)
#define KAGUYA_PP_REPEAT_DEF57(MACRO) KAGUYA_PP_REPEAT_DEF56(MACRO) MACRO(57)
#define KAGUYA_PP_REPEAT_DEF58(MACRO) KAGUYA_PP_REPEAT_DEF57(MACRO) MACRO(58)
#define KAGUYA_PP_REPEAT_DEF59(MACRO) KAGUYA_PP_REPEAT_DEF58(MACRO) MACRO(59)
#define KAGUYA_PP_REPEAT_DEF60(MACRO) KAGUYA_PP_REPEAT_DEF59(MACRO) MACRO(60)
#define KAGUYA_PP_REPEAT_DEF61(MACRO) KAGUYA_PP_REPEAT_DEF60(MACRO) MACRO(61)
#define KAGUYA_PP_REPEAT_DEF62(MACRO) KAGUYA_PP_REPEAT_DEF61(MACRO) MACRO(62)
#define KAGUYA_PP_REPEAT_DEF63(MACRO) KAGUYA_PP_REPEAT_DEF62(MACRO) MACRO(63)
#define KAGUYA_PP_REPEAT_DEF64(MACRO) KAGUYA_PP_REPEAT_DEF63(MACRO) MACRO(64)
#define KAGUYA_PP_REPEAT_DEF65(MACRO) KAGUYA_PP_REPEAT_DEF64(MACRO) MACRO(65)
#define KAGUYA_PP_REPEAT_DEF66(MACRO) KAGUYA_PP_REPEAT_DEF65(MACRO) MACRO(66)
#define KAGUYA_PP_REPEAT_DEF67(MACRO) KAGUYA_PP_REPEAT_DEF66(MACRO) MACRO(67)
#define KAGUYA_PP_REPEAT_DEF68(MACRO) KAGUYA_PP_REPEAT_DEF67(MACRO) MACRO(68)
#define KAGUYA_PP_REPEAT_DEF69(MACRO) KAGUYA_PP_REPEAT_DEF68(MACRO) MACRO(69)
#define KAGUYA_PP_REPEAT_DEF70(MACRO) KAGUYA_PP_REPEAT_DEF69(MACRO) MACRO(70)
#define KAGUYA_PP_REPEAT_DEF71(MACRO) KAGUYA_PP_REPEAT_DEF70(MACRO) MACRO(71)
#define KAGUYA_PP_REPEAT_DEF72(MACRO) KAGUYA_PP_REPEAT_DEF71(MACRO) MACRO(72)
#define KAGUYA_PP_REPEAT_DEF73(MACRO) KAGUYA_PP_REPEAT_DEF72(MACRO) MACRO(73)
#define KAGUYA_PP_REPEAT_DEF74(MACRO) KAGUYA_PP_REPEAT_DEF73(MACRO) MACRO(74)
#define KAGUYA_PP_REPEAT_DEF75(MACRO) KAGUYA_PP_REPEAT_DEF74(MACRO) MACRO(75)
#define KAGUYA_PP_REPEAT_DEF76(MACRO) KAGUYA_PP_REPEAT_DEF75(MACRO) MACRO(76)
#define KAGUYA_PP_REPEAT_DEF77(MACRO) KAGUYA_PP_REPEAT_DEF76(MACRO) MACRO(77)
#define KAGUYA_PP_REPEAT_DEF78(MACRO) KAGUYA_PP_REPEAT_DEF77(MACRO) MACRO(78)
#define KAGUYA_PP_REPEAT_DEF79(MACRO) KAGUYA_PP_REPEAT_DEF78(MACRO) MACRO(79)
#define KAGUYA_PP_REPEAT_DEF80(MACRO) KAGUYA_PP_REPEAT_DEF79(MACRO) MACRO(80)
#define KAGUYA_PP_REPEAT_DEF81(MACRO) KAGUYA_PP_REPEAT_DEF80(MACRO) MACRO(81)
#define KAGUYA_PP_REPEAT_DEF82(MACRO) KAGUYA_PP_REPEAT_DEF81(MACRO) MACRO(82)
#define KAGUYA_PP_REPEAT_DEF83(MACRO) KAGUYA_PP_REPEAT_DEF82(MACRO) MACRO(83)
#define KAGUYA_PP_REPEAT_DEF84(MACRO) KAGUYA_PP_REPEAT_DEF83(MACRO) MACRO(84)
#define KAGUYA_PP_REPEAT_DEF85(MACRO) KAGUYA_PP_REPEAT_DEF84(MACRO) MACRO(85)
#define KAGUYA_PP_REPEAT_DEF86(MACRO) KAGUYA_PP_REPEAT_DEF85(MACRO) MACRO(86)
#define KAGUYA_PP_REPEAT_DEF87(MACRO) KAGUYA_PP_REPEAT_DEF86(MACRO) MACRO(87)
#define KAGUYA_PP_REPEAT_DEF88(MACRO) KAGUYA_PP_REPEAT_DEF87(MACRO) MACRO(88)
#define KAGUYA_PP_REPEAT_DEF89(MACRO) KAGUYA_PP_REPEAT_DEF88(MACRO) MACRO(89)
#define KAGUYA_PP_REPEAT_DEF90(MACRO) KAGUYA_PP_REPEAT_DEF89(MACRO) MACRO(90)
#define KAGUYA_PP_REPEAT_DEF91(MACRO) KAGUYA_PP_REPEAT_DEF90(MACRO) MACRO(91)
#define KAGUYA_PP_REPEAT_DEF92(MACRO) KAGUYA_PP_REPEAT_DEF91(MACRO) MACRO(92)
#define KAGUYA_PP_REPEAT_DEF93(MACRO) KAGUYA_PP_REPEAT_DEF92(MACRO) MACRO(93)
#define KAGUYA_PP_REPEAT_DEF94(MACRO) KAGUYA_PP_REPEAT_DEF93(MACRO) MACRO(94)
#define KAGUYA_PP_REPEAT_DEF95(MACRO) KAGUYA_PP_REPEAT_DEF94(MACRO) MACRO(95)
#define KAGUYA_PP_REPEAT_DEF96(MACRO) KAGUYA_PP_REPEAT_DEF95(MACRO) MACRO(96)
#define KAGUYA_PP_REPEAT_DEF97(MACRO) KAGUYA_PP_REPEAT_DEF96(MACRO) MACRO(97)
#define KAGUYA_PP_REPEAT_DEF98(MACRO) KAGUYA_PP_REPEAT_DEF97(MACRO) MACRO(98)
#define KAGUYA_PP_REPEAT_DEF99(MACRO) KAGUYA_PP_REPEAT_DEF98(MACRO) MACRO(99)
#define KAGUYA_PP_REPEAT_DEF100(MACRO) KAGUYA_PP_REPEAT_DEF99(MACRO) MACRO(100)
#define KAGUYA_PP_REPEAT_DEF101(MACRO) KAGUYA_PP_REPEAT_DEF100(MACRO) MACRO(101)
#define KAGUYA_PP_REPEAT_DEF102(MACRO) KAGUYA_PP_REPEAT_DEF101(MACRO) MACRO(102)
#define KAGUYA_PP_REPEAT_DEF103(MACRO) KAGUYA_PP_REPEAT_DEF102(MACRO) MACRO(103)
#define KAGUYA_PP_REPEAT_DEF104(MACRO) KAGUYA_PP_REPEAT_DEF103(MACRO) MACRO(104)
#define KAGUYA_PP_REPEAT_DEF105(MACRO) KAGUYA_PP_REPEAT_DEF104(MACRO) MACRO(105)
#define KAGUYA_PP_REPEAT_DEF106(MACRO) KAGUYA_PP_REPEAT_DEF105(MACRO) MACRO(106)
#define KAGUYA_PP_REPEAT_DEF107(MACRO) KAGUYA_PP_REPEAT_DEF106(MACRO) MACRO(107)
#define KAGUYA_PP_REPEAT_DEF108(MACRO) KAGUYA_PP_REPEAT_DEF107(MACRO) MACRO(108)
#define KAGUYA_PP_REPEAT_DEF109(MACRO) KAGUYA_PP_REPEAT_DEF108(MACRO) MACRO(109)
#define KAGUYA_PP_REPEAT_DEF110(MACRO) KAGUYA_PP_REPEAT_DEF109(MACRO) MACRO(110)
#define KAGUYA_PP_REPEAT_DEF111(MACRO) KAGUYA_PP_REPEAT_DEF110(MACRO) MACRO(111)
#define KAGUYA_PP_REPEAT_DEF112(MACRO) KAGUYA_PP_REPEAT_DEF111(MACRO) MACRO(112)
#define KAGUYA_PP_REPEAT_DEF113(MACRO) KAGUYA_PP_REPEAT_DEF112(MACRO) MACRO(113)
#define KAGUYA_PP_REPEAT_DEF114(MACRO) KAGUYA_PP_REPEAT_DEF113(MACRO) MACRO(114)
#define KAGUYA_PP_REPEAT_DEF115(MACRO) KAGUYA_PP_REPEAT_DEF114(MACRO) MACRO(115)
#define KAGUYA_PP_REPEAT_DEF116(MACRO) KAGUYA_PP_REPEAT_DEF115(MACRO) MACRO(116)
#define KAGUYA_PP_REPEAT_DEF117(MACRO) KAGUYA_PP_REPEAT_DEF116(MACRO) MACRO(117)
#define KAGUYA_PP_REPEAT_DEF118(MACRO) KAGUYA_PP_REPEAT_DEF117(MACRO) MACRO(118)
#define KAGUYA_PP_REPEAT_DEF119(MACRO) KAGUYA_PP_REPEAT_DEF118(MACRO) MACRO(119)
#define KAGUYA_PP_REPEAT_DEF120(MACRO) KAGUYA_PP_REPEAT_DEF119(MACRO) MACRO(120)
#define KAGUYA_PP_REPEAT_DEF121(MACRO) KAGUYA_PP_REPEAT_DEF120(MACRO) MACRO(121)
#define KAGUYA_PP_REPEAT_DEF122(MACRO) KAGUYA_PP_REPEAT_DEF121(MACRO) MACRO(122)
#define KAGUYA_PP_REPEAT_DEF123(MACRO) KAGUYA_PP_REPEAT_DEF122(MACRO) MACRO(123)
#define KAGUYA_PP_REPEAT_DEF124(MACRO) KAGUYA_PP_REPEAT_DEF123(MACRO) MACRO(124)
#define KAGUYA_PP_REPEAT_DEF125(MACRO) KAGUYA_PP_REPEAT_DEF124(MACRO) MACRO(125)
#define KAGUYA_PP_REPEAT_DEF126(MACRO) KAGUYA_PP_REPEAT_DEF125(MACRO) MACRO(126)
#define KAGUYA_PP_REPEAT_DEF127(MACRO) KAGUYA_PP_REPEAT_DEF126(MACRO) MACRO(127)
#define KAGUYA_PP_REPEAT_DEF128(MACRO) KAGUYA_PP_REPEAT_DEF127(MACRO) MACRO(128)
#define KAGUYA_PP_REPEAT_DEF129(MACRO) KAGUYA_PP_REPEAT_DEF128(MACRO) MACRO(129)
#define KAGUYA_PP_REPEAT_DEF130(MACRO) KAGUYA_PP_REPEAT_DEF129(MACRO) MACRO(130)
#define KAGUYA_PP_REPEAT_DEF131(MACRO) KAGUYA_PP_REPEAT_DEF130(MACRO) MACRO(131)
#define KAGUYA_PP_REPEAT_DEF132(MACRO) KAGUYA_PP_REPEAT_DEF131(MACRO) MACRO(132)
#define KAGUYA_PP_REPEAT_DEF133(MACRO) KAGUYA_PP_REPEAT_DEF132(MACRO) MACRO(133)
#define KAGUYA_PP_REPEAT_DEF134(MACRO) KAGUYA_PP_REPEAT_DEF133(MACRO) MACRO(134)
#define KAGUYA_PP_REPEAT_DEF135(MACRO) KAGUYA_PP_REPEAT_DEF134(MACRO) MACRO(135)
#define KAGUYA_PP_REPEAT_DEF136(MACRO) KAGUYA_PP_REPEAT_DEF135(MACRO) MACRO(136)
#define KAGUYA_PP_REPEAT_DEF137(MACRO) KAGUYA_PP_REPEAT_DEF136(MACRO) MACRO(137)
#define KAGUYA_PP_REPEAT_DEF138(MACRO) KAGUYA_PP_REPEAT_DEF137(MACRO) MACRO(138)
#define KAGUYA_PP_REPEAT_DEF139(MACRO) KAGUYA_PP_REPEAT_DEF138(MACRO) MACRO(139)
#define KAGUYA_PP_REPEAT_DEF140(MACRO) KAGUYA_PP_REPEAT_DEF139(MACRO) MACRO(140)
#define KAGUYA_PP_REPEAT_DEF141(MACRO) KAGUYA_PP_REPEAT_DEF140(MACRO) MACRO(141)
#define KAGUYA_PP_REPEAT_DEF142(MACRO) KAGUYA_PP_REPEAT_DEF141(MACRO) MACRO(142)
#define KAGUYA_PP_REPEAT_DEF143(MACRO) KAGUYA_PP_REPEAT_DEF142(MACRO) MACRO(143)
#define KAGUYA_PP_REPEAT_DEF144(MACRO) KAGUYA_PP_REPEAT_DEF143(MACRO) MACRO(144)
#define KAGUYA_PP_REPEAT_DEF145(MACRO) KAGUYA_PP_REPEAT_DEF144(MACRO) MACRO(145)
#define KAGUYA_PP_REPEAT_DEF146(MACRO) KAGUYA_PP_REPEAT_DEF145(MACRO) MACRO(146)
#define KAGUYA_PP_REPEAT_DEF147(MACRO) KAGUYA_PP_REPEAT_DEF146(MACRO) MACRO(147)
#define KAGUYA_PP_REPEAT_DEF148(MACRO) KAGUYA_PP_REPEAT_DEF147(MACRO) MACRO(148)
#define KAGUYA_PP_REPEAT_DEF149(MACRO) KAGUYA_PP_REPEAT_DEF148(MACRO) MACRO(149)
#define KAGUYA_PP_REPEAT_DEF150(MACRO) KAGUYA_PP_REPEAT_DEF149(MACRO) MACRO(150)
#define KAGUYA_PP_REPEAT_DEF151(MACRO) KAGUYA_PP_REPEAT_DEF150(MACRO) MACRO(151)
#define KAGUYA_PP_REPEAT_DEF152(MACRO) KAGUYA_PP_REPEAT_DEF151(MACRO) MACRO(152)
#define KAGUYA_PP_REPEAT_DEF153(MACRO) KAGUYA_PP_REPEAT_DEF152(MACRO) MACRO(153)
#define KAGUYA_PP_REPEAT_DEF154(MACRO) KAGUYA_PP_REPEAT_DEF153(MACRO) MACRO(154)
#define KAGUYA_PP_REPEAT_DEF155(MACRO) KAGUYA_PP_REPEAT_DEF154(MACRO) MACRO(155)
#define KAGUYA_PP_REPEAT_DEF156(MACRO) KAGUYA_PP_REPEAT_DEF155(MACRO) MACRO(156)
#define KAGUYA_PP_REPEAT_DEF157(MACRO) KAGUYA_PP_REPEAT_DEF156(MACRO) MACRO(157)
#define KAGUYA_PP_REPEAT_DEF158(MACRO) KAGUYA_PP_REPEAT_DEF157(MACRO) MACRO(158)
#define KAGUYA_PP_REPEAT_DEF159(MACRO) KAGUYA_PP_REPEAT_DEF158(MACRO) MACRO(159)
#define KAGUYA_PP_REPEAT_DEF160(MACRO) KAGUYA_PP_REPEAT_DEF159(MACRO) MACRO(160)
#define KAGUYA_PP_REPEAT_DEF161(MACRO) KAGUYA_PP_REPEAT_DEF160(MACRO) MACRO(161)
#define KAGUYA_PP_REPEAT_DEF162(MACRO) KAGUYA_PP_REPEAT_DEF161(MACRO) MACRO(162)
#define KAGUYA_PP_REPEAT_DEF163(MACRO) KAGUYA_PP_REPEAT_DEF162(MACRO) MACRO(163)
#define KAGUYA_PP_REPEAT_DEF164(MACRO) KAGUYA_PP_REPEAT_DEF163(MACRO) MACRO(164)
#define KAGUYA_PP_REPEAT_DEF165(MACRO) KAGUYA_PP_REPEAT_DEF164(MACRO) MACRO(165)
#define KAGUYA_PP_REPEAT_DEF166(MACRO) KAGUYA_PP_REPEAT_DEF165(MACRO) MACRO(166)
#define KAGUYA_PP_REPEAT_DEF167(MACRO) KAGUYA_PP_REPEAT_DEF166(MACRO) MACRO(167)
#define KAGUYA_PP_REPEAT_DEF168(MACRO) KAGUYA_PP_REPEAT_DEF167(MACRO) MACRO(168)
#define KAGUYA_PP_REPEAT_DEF169(MACRO) KAGUYA_PP_REPEAT_DEF168(MACRO) MACRO(169)
#define KAGUYA_PP_REPEAT_DEF170(MACRO) KAGUYA_PP_REPEAT_DEF169(MACRO) MACRO(170)
#define KAGUYA_PP_REPEAT_DEF171(MACRO) KAGUYA_PP_REPEAT_DEF170(MACRO) MACRO(171)
#define KAGUYA_PP_REPEAT_DEF172(MACRO) KAGUYA_PP_REPEAT_DEF171(MACRO) MACRO(172)
#define KAGUYA_PP_REPEAT_DEF173(MACRO) KAGUYA_PP_REPEAT_DEF172(MACRO) MACRO(173)
#define KAGUYA_PP_REPEAT_DEF174(MACRO) KAGUYA_PP_REPEAT_DEF173(MACRO) MACRO(174)
#define KAGUYA_PP_REPEAT_DEF175(MACRO) KAGUYA_PP_REPEAT_DEF174(MACRO) MACRO(175)
#define KAGUYA_PP_REPEAT_DEF176(MACRO) KAGUYA_PP_REPEAT_DEF175(MACRO) MACRO(176)
#define KAGUYA_PP_REPEAT_DEF177(MACRO) KAGUYA_PP_REPEAT_DEF176(MACRO) MACRO(177)
#define KAGUYA_PP_REPEAT_DEF178(MACRO) KAGUYA_PP_REPEAT_DEF177(MACRO) MACRO(178)
#define KAGUYA_PP_REPEAT_DEF179(MACRO) KAGUYA_PP_REPEAT_DEF178(MACRO) MACRO(179)
#define KAGUYA_PP_REPEAT_DEF180(MACRO) KAGUYA_PP_REPEAT_DEF179(MACRO) MACRO(180)
#define KAGUYA_PP_REPEAT_DEF181(MACRO) KAGUYA_PP_REPEAT_DEF180(MACRO) MACRO(181)
#define KAGUYA_PP_REPEAT_DEF182(MACRO) KAGUYA_PP_REPEAT_DEF181(MACRO) MACRO(182)
#define KAGUYA_PP_REPEAT_DEF183(MACRO) KAGUYA_PP_REPEAT_DEF182(MACRO) MACRO(183)
#define KAGUYA_PP_REPEAT_DEF184(MACRO) KAGUYA_PP_REPEAT_DEF183(MACRO) MACRO(184)
#define KAGUYA_PP_REPEAT_DEF185(MACRO) KAGUYA_PP_REPEAT_DEF184(MACRO) MACRO(185)
#define KAGUYA_PP_REPEAT_DEF186(MACRO) KAGUYA_PP_REPEAT_DEF185(MACRO) MACRO(186)
#define KAGUYA_PP_REPEAT_DEF187(MACRO) KAGUYA_PP_REPEAT_DEF186(MACRO) MACRO(187)
#define KAGUYA_PP_REPEAT_DEF188(MACRO) KAGUYA_PP_REPEAT_DEF187(MACRO) MACRO(188)
#define KAGUYA_PP_REPEAT_DEF189(MACRO) KAGUYA_PP_REPEAT_DEF188(MACRO) MACRO(189)
#define KAGUYA_PP_REPEAT_DEF190(MACRO) KAGUYA_PP_REPEAT_DEF189(MACRO) MACRO(190)
#define KAGUYA_PP_REPEAT_DEF191(MACRO) KAGUYA_PP_REPEAT_DEF190(MACRO) MACRO(191)
#define KAGUYA_PP_REPEAT_DEF192(MACRO) KAGUYA_PP_REPEAT_DEF191(MACRO) MACRO(192)
#define KAGUYA_PP_REPEAT_DEF193(MACRO) KAGUYA_PP_REPEAT_DEF192(MACRO) MACRO(193)
#define KAGUYA_PP_REPEAT_DEF194(MACRO) KAGUYA_PP_REPEAT_DEF193(MACRO) MACRO(194)
#define KAGUYA_PP_REPEAT_DEF195(MACRO) KAGUYA_PP_REPEAT_DEF194(MACRO) MACRO(195)
#define KAGUYA_PP_REPEAT_DEF196(MACRO) KAGUYA_PP_REPEAT_DEF195(MACRO) MACRO(196)
#define KAGUYA_PP_REPEAT_DEF197(MACRO) KAGUYA_PP_REPEAT_DEF196(MACRO) MACRO(197)
#define KAGUYA_PP_REPEAT_DEF198(MACRO) KAGUYA_PP_REPEAT_DEF197(MACRO) MACRO(198)
#define KAGUYA_PP_REPEAT_DEF199(MACRO) KAGUYA_PP_REPEAT_DEF198(MACRO) MACRO(199)
#define KAGUYA_PP_REPEAT_DEF200(MACRO) KAGUYA_PP_REPEAT_DEF199(MACRO) MACRO(200)
#define KAGUYA_PP_REPEAT_DEF201(MACRO) KAGUYA_PP_REPEAT_DEF200(MACRO) MACRO(201)
#define KAGUYA_PP_REPEAT_DEF202(MACRO) KAGUYA_PP_REPEAT_DEF201(MACRO) MACRO(202)
#define KAGUYA_PP_REPEAT_DEF203(MACRO) KAGUYA_PP_REPEAT_DEF202(MACRO) MACRO(203)
#define KAGUYA_PP_REPEAT_DEF204(MACRO) KAGUYA_PP_REPEAT_DEF203(MACRO) MACRO(204)
#define KAGUYA_PP_REPEAT_DEF205(MACRO) KAGUYA_PP_REPEAT_DEF204(MACRO) MACRO(205)
#define KAGUYA_PP_REPEAT_DEF206(MACRO) KAGUYA_PP_REPEAT_DEF205(MACRO) MACRO(206)
#define KAGUYA_PP_REPEAT_DEF207(MACRO) KAGUYA_PP_REPEAT_DEF206(MACRO) MACRO(207)
#define KAGUYA_PP_REPEAT_DEF208(MACRO) KAGUYA_PP_REPEAT_DEF207(MACRO) MACRO(208)
#define KAGUYA_PP_REPEAT_DEF209(MACRO) KAGUYA_PP_REPEAT_DEF208(MACRO) MACRO(209)
#define KAGUYA_PP_REPEAT_DEF210(MACRO) KAGUYA_PP_REPEAT_DEF209(MACRO) MACRO(210)
#define KAGUYA_PP_REPEAT_DEF211(MACRO) KAGUYA_PP_REPEAT_DEF210(MACRO) MACRO(211)
#define KAGUYA_PP_REPEAT_DEF212(MACRO) KAGUYA_PP_REPEAT_DEF211(MACRO) MACRO(212)
#define KAGUYA_PP_REPEAT_DEF213(MACRO) KAGUYA_PP_REPEAT_DEF212(MACRO) MACRO(213)
#define KAGUYA_PP_REPEAT_DEF214(MACRO) KAGUYA_PP_REPEAT_DEF213(MACRO) MACRO(214)
#define KAGUYA_PP_REPEAT_DEF215(MACRO) KAGUYA_PP_REPEAT_DEF214(MACRO) MACRO(215)
#define KAGUYA_PP_REPEAT_DEF216(MACRO) KAGUYA_PP_REPEAT_DEF215(MACRO) MACRO(216)
#define KAGUYA_PP_REPEAT_DEF217(MACRO) KAGUYA_PP_REPEAT_DEF216(MACRO) MACRO(217)
#define KAGUYA_PP_REPEAT_DEF218(MACRO) KAGUYA_PP_REPEAT_DEF217(MACRO) MACRO(218)
#define KAGUYA_PP_REPEAT_DEF219(MACRO) KAGUYA_PP_REPEAT_DEF218(MACRO) MACRO(219)
#define KAGUYA_PP_REPEAT_DEF220(MACRO) KAGUYA_PP_REPEAT_DEF219(MACRO) MACRO(220)
#define KAGUYA_PP_REPEAT_DEF221(MACRO) KAGUYA_PP_REPEAT_DEF220(MACRO) MACRO(221)
#define KAGUYA_PP_REPEAT_DEF222(MACRO) KAGUYA_PP_REPEAT_DEF221(MACRO) MACRO(222)
#define KAGUYA_PP_REPEAT_DEF223(MACRO) KAGUYA_PP_REPEAT_DEF222(MACRO) MACRO(223)
#define KAGUYA_PP_REPEAT_DEF224(MACRO) KAGUYA_PP_REPEAT_DEF223(MACRO) MACRO(224)
#define KAGUYA_PP_REPEAT_DEF225(MACRO) KAGUYA_PP_REPEAT_DEF224(MACRO) MACRO(225)
#define KAGUYA_PP_REPEAT_DEF226(MACRO) KAGUYA_PP_REPEAT_DEF225(MACRO) MACRO(226)
#define KAGUYA_PP_REPEAT_DEF227(MACRO) KAGUYA_PP_REPEAT_DEF226(MACRO) MACRO(227)
#define KAGUYA_PP_REPEAT_DEF228(MACRO) KAGUYA_PP_REPEAT_DEF227(MACRO) MACRO(228)
#define KAGUYA_PP_REPEAT_DEF229(MACRO) KAGUYA_PP_REPEAT_DEF228(MACRO) MACRO(229)
#define KAGUYA_PP_REPEAT_DEF230(MACRO) KAGUYA_PP_REPEAT_DEF229(MACRO) MACRO(230)
#define KAGUYA_PP_REPEAT_DEF231(MACRO) KAGUYA_PP_REPEAT_DEF230(MACRO) MACRO(231)
#define KAGUYA_PP_REPEAT_DEF232(MACRO) KAGUYA_PP_REPEAT_DEF231(MACRO) MACRO(232)
#define KAGUYA_PP_REPEAT_DEF233(MACRO) KAGUYA_PP_REPEAT_DEF232(MACRO) MACRO(233)
#define KAGUYA_PP_REPEAT_DEF234(MACRO) KAGUYA_PP_REPEAT_DEF233(MACRO) MACRO(234)
#define KAGUYA_PP_REPEAT_DEF235(MACRO) KAGUYA_PP_REPEAT_DEF234(MACRO) MACRO(235)
#define KAGUYA_PP_REPEAT_DEF236(MACRO) KAGUYA_PP_REPEAT_DEF235(MACRO) MACRO(236)
#define KAGUYA_PP_REPEAT_DEF237(MACRO) KAGUYA_PP_REPEAT_DEF236(MACRO) MACRO(237)
#define KAGUYA_PP_REPEAT_DEF238(MACRO) KAGUYA_PP_REPEAT_DEF237(MACRO) MACRO(238)
#define KAGUYA_PP_REPEAT_DEF239(MACRO) KAGUYA_PP_REPEAT_DEF238(MACRO) MACRO(239)
#define KAGUYA_PP_REPEAT_DEF240(MACRO) KAGUYA_PP_REPEAT_DEF239(MACRO) MACRO(240)
#define KAGUYA_PP_REPEAT_DEF241(MACRO) KAGUYA_PP_REPEAT_DEF240(MACRO) MACRO(241)
#define KAGUYA_PP_REPEAT_DEF242(MACRO) KAGUYA_PP_REPEAT_DEF241(MACRO) MACRO(242)
#define KAGUYA_PP_REPEAT_DEF243(MACRO) KAGUYA_PP_REPEAT_DEF242(MACRO) MACRO(243)
#define KAGUYA_PP_REPEAT_DEF244(MACRO) KAGUYA_PP_REPEAT_DEF243(MACRO) MACRO(244)
#define KAGUYA_PP_REPEAT_DEF245(MACRO) KAGUYA_PP_REPEAT_DEF244(MACRO) MACRO(245)
#define KAGUYA_PP_REPEAT_DEF246(MACRO) KAGUYA_PP_REPEAT_DEF245(MACRO) MACRO(246)
#define KAGUYA_PP_REPEAT_DEF247(MACRO) KAGUYA_PP_REPEAT_DEF246(MACRO) MACRO(247)
#define KAGUYA_PP_REPEAT_DEF248(MACRO) KAGUYA_PP_REPEAT_DEF247(MACRO) MACRO(248)
#define KAGUYA_PP_REPEAT_DEF249(MACRO) KAGUYA_PP_REPEAT_DEF248(MACRO) MACRO(249)
#define KAGUYA_PP_REPEAT_DEF250(MACRO) KAGUYA_PP_REPEAT_DEF249(MACRO) MACRO(250)
#define KAGUYA_PP_REPEAT_DEF251(MACRO) KAGUYA_PP_REPEAT_DEF250(MACRO) MACRO(251)
#define KAGUYA_PP_REPEAT_DEF252(MACRO) KAGUYA_PP_REPEAT_DEF251(MACRO) MACRO(252)
#define KAGUYA_PP_REPEAT_DEF253(MACRO) KAGUYA_PP_REPEAT_DEF252(MACRO) MACRO(253)
#define KAGUYA_PP_REPEAT_DEF254(MACRO) KAGUYA_PP_REPEAT_DEF253(MACRO) MACRO(254)
#define KAGUYA_PP_REPEAT_DEF(COUNT, MACRO)                                     \
  KAGUYA_PP_CAT(KAGUYA_PP_REPEAT_DEF, COUNT)(MACRO)

#define KAGUYA_PP_REVERSE_REPEAT0(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT1(MACRO)                                       \
  MACRO(1) KAGUYA_PP_REVERSE_REPEAT0(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT2(MACRO)                                       \
  MACRO(2) KAGUYA_PP_REVERSE_REPEAT1(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT3(MACRO)                                       \
  MACRO(3) KAGUYA_PP_REVERSE_REPEAT2(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT4(MACRO)                                       \
  MACRO(4) KAGUYA_PP_REVERSE_REPEAT3(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT5(MACRO)                                       \
  MACRO(5) KAGUYA_PP_REVERSE_REPEAT4(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT6(MACRO)                                       \
  MACRO(6) KAGUYA_PP_REVERSE_REPEAT5(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT7(MACRO)                                       \
  MACRO(7) KAGUYA_PP_REVERSE_REPEAT6(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT8(MACRO)                                       \
  MACRO(8) KAGUYA_PP_REVERSE_REPEAT7(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT9(MACRO)                                       \
  MACRO(9) KAGUYA_PP_REVERSE_REPEAT8(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT10(MACRO)                                      \
  MACRO(10) KAGUYA_PP_REVERSE_REPEAT9(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT11(MACRO)                                      \
  MACRO(11) KAGUYA_PP_REVERSE_REPEAT10(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT12(MACRO)                                      \
  MACRO(12) KAGUYA_PP_REVERSE_REPEAT11(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT13(MACRO)                                      \
  MACRO(13) KAGUYA_PP_REVERSE_REPEAT12(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT14(MACRO)                                      \
  MACRO(14) KAGUYA_PP_REVERSE_REPEAT13(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT15(MACRO)                                      \
  MACRO(15) KAGUYA_PP_REVERSE_REPEAT14(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT16(MACRO)                                      \
  MACRO(16) KAGUYA_PP_REVERSE_REPEAT15(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT17(MACRO)                                      \
  MACRO(17) KAGUYA_PP_REVERSE_REPEAT16(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT18(MACRO)                                      \
  MACRO(18) KAGUYA_PP_REVERSE_REPEAT17(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT19(MACRO)                                      \
  MACRO(19) KAGUYA_PP_REVERSE_REPEAT18(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT20(MACRO)                                      \
  MACRO(20) KAGUYA_PP_REVERSE_REPEAT19(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT21(MACRO)                                      \
  MACRO(21) KAGUYA_PP_REVERSE_REPEAT20(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT22(MACRO)                                      \
  MACRO(22) KAGUYA_PP_REVERSE_REPEAT21(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT23(MACRO)                                      \
  MACRO(23) KAGUYA_PP_REVERSE_REPEAT22(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT24(MACRO)                                      \
  MACRO(24) KAGUYA_PP_REVERSE_REPEAT23(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT25(MACRO)                                      \
  MACRO(25) KAGUYA_PP_REVERSE_REPEAT24(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT26(MACRO)                                      \
  MACRO(26) KAGUYA_PP_REVERSE_REPEAT25(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT27(MACRO)                                      \
  MACRO(27) KAGUYA_PP_REVERSE_REPEAT26(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT28(MACRO)                                      \
  MACRO(28) KAGUYA_PP_REVERSE_REPEAT27(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT29(MACRO)                                      \
  MACRO(29) KAGUYA_PP_REVERSE_REPEAT28(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT30(MACRO)                                      \
  MACRO(30) KAGUYA_PP_REVERSE_REPEAT29(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT31(MACRO)                                      \
  MACRO(31) KAGUYA_PP_REVERSE_REPEAT30(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT32(MACRO)                                      \
  MACRO(32) KAGUYA_PP_REVERSE_REPEAT31(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT33(MACRO)                                      \
  MACRO(33) KAGUYA_PP_REVERSE_REPEAT32(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT34(MACRO)                                      \
  MACRO(34) KAGUYA_PP_REVERSE_REPEAT33(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT35(MACRO)                                      \
  MACRO(35) KAGUYA_PP_REVERSE_REPEAT34(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT36(MACRO)                                      \
  MACRO(36) KAGUYA_PP_REVERSE_REPEAT35(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT37(MACRO)                                      \
  MACRO(37) KAGUYA_PP_REVERSE_REPEAT36(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT38(MACRO)                                      \
  MACRO(38) KAGUYA_PP_REVERSE_REPEAT37(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT39(MACRO)                                      \
  MACRO(39) KAGUYA_PP_REVERSE_REPEAT38(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT40(MACRO)                                      \
  MACRO(40) KAGUYA_PP_REVERSE_REPEAT39(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT41(MACRO)                                      \
  MACRO(41) KAGUYA_PP_REVERSE_REPEAT40(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT42(MACRO)                                      \
  MACRO(42) KAGUYA_PP_REVERSE_REPEAT41(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT43(MACRO)                                      \
  MACRO(43) KAGUYA_PP_REVERSE_REPEAT42(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT44(MACRO)                                      \
  MACRO(44) KAGUYA_PP_REVERSE_REPEAT43(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT45(MACRO)                                      \
  MACRO(45) KAGUYA_PP_REVERSE_REPEAT44(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT46(MACRO)                                      \
  MACRO(46) KAGUYA_PP_REVERSE_REPEAT45(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT47(MACRO)                                      \
  MACRO(47) KAGUYA_PP_REVERSE_REPEAT46(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT48(MACRO)                                      \
  MACRO(48) KAGUYA_PP_REVERSE_REPEAT47(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT49(MACRO)                                      \
  MACRO(49) KAGUYA_PP_REVERSE_REPEAT48(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT50(MACRO)                                      \
  MACRO(50) KAGUYA_PP_REVERSE_REPEAT49(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT51(MACRO)                                      \
  MACRO(51) KAGUYA_PP_REVERSE_REPEAT50(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT52(MACRO)                                      \
  MACRO(52) KAGUYA_PP_REVERSE_REPEAT51(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT53(MACRO)                                      \
  MACRO(53) KAGUYA_PP_REVERSE_REPEAT52(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT54(MACRO)                                      \
  MACRO(54) KAGUYA_PP_REVERSE_REPEAT53(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT55(MACRO)                                      \
  MACRO(55) KAGUYA_PP_REVERSE_REPEAT54(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT56(MACRO)                                      \
  MACRO(56) KAGUYA_PP_REVERSE_REPEAT55(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT57(MACRO)                                      \
  MACRO(57) KAGUYA_PP_REVERSE_REPEAT56(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT58(MACRO)                                      \
  MACRO(58) KAGUYA_PP_REVERSE_REPEAT57(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT59(MACRO)                                      \
  MACRO(59) KAGUYA_PP_REVERSE_REPEAT58(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT60(MACRO)                                      \
  MACRO(60) KAGUYA_PP_REVERSE_REPEAT59(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT61(MACRO)                                      \
  MACRO(61) KAGUYA_PP_REVERSE_REPEAT60(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT62(MACRO)                                      \
  MACRO(62) KAGUYA_PP_REVERSE_REPEAT61(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT63(MACRO)                                      \
  MACRO(63) KAGUYA_PP_REVERSE_REPEAT62(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT64(MACRO)                                      \
  MACRO(64) KAGUYA_PP_REVERSE_REPEAT63(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT65(MACRO)                                      \
  MACRO(65) KAGUYA_PP_REVERSE_REPEAT64(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT66(MACRO)                                      \
  MACRO(66) KAGUYA_PP_REVERSE_REPEAT65(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT67(MACRO)                                      \
  MACRO(67) KAGUYA_PP_REVERSE_REPEAT66(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT68(MACRO)                                      \
  MACRO(68) KAGUYA_PP_REVERSE_REPEAT67(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT69(MACRO)                                      \
  MACRO(69) KAGUYA_PP_REVERSE_REPEAT68(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT70(MACRO)                                      \
  MACRO(70) KAGUYA_PP_REVERSE_REPEAT69(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT71(MACRO)                                      \
  MACRO(71) KAGUYA_PP_REVERSE_REPEAT70(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT72(MACRO)                                      \
  MACRO(72) KAGUYA_PP_REVERSE_REPEAT71(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT73(MACRO)                                      \
  MACRO(73) KAGUYA_PP_REVERSE_REPEAT72(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT74(MACRO)                                      \
  MACRO(74) KAGUYA_PP_REVERSE_REPEAT73(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT75(MACRO)                                      \
  MACRO(75) KAGUYA_PP_REVERSE_REPEAT74(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT76(MACRO)                                      \
  MACRO(76) KAGUYA_PP_REVERSE_REPEAT75(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT77(MACRO)                                      \
  MACRO(77) KAGUYA_PP_REVERSE_REPEAT76(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT78(MACRO)                                      \
  MACRO(78) KAGUYA_PP_REVERSE_REPEAT77(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT79(MACRO)                                      \
  MACRO(79) KAGUYA_PP_REVERSE_REPEAT78(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT80(MACRO)                                      \
  MACRO(80) KAGUYA_PP_REVERSE_REPEAT79(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT81(MACRO)                                      \
  MACRO(81) KAGUYA_PP_REVERSE_REPEAT80(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT82(MACRO)                                      \
  MACRO(82) KAGUYA_PP_REVERSE_REPEAT81(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT83(MACRO)                                      \
  MACRO(83) KAGUYA_PP_REVERSE_REPEAT82(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT84(MACRO)                                      \
  MACRO(84) KAGUYA_PP_REVERSE_REPEAT83(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT85(MACRO)                                      \
  MACRO(85) KAGUYA_PP_REVERSE_REPEAT84(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT86(MACRO)                                      \
  MACRO(86) KAGUYA_PP_REVERSE_REPEAT85(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT87(MACRO)                                      \
  MACRO(87) KAGUYA_PP_REVERSE_REPEAT86(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT88(MACRO)                                      \
  MACRO(88) KAGUYA_PP_REVERSE_REPEAT87(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT89(MACRO)                                      \
  MACRO(89) KAGUYA_PP_REVERSE_REPEAT88(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT90(MACRO)                                      \
  MACRO(90) KAGUYA_PP_REVERSE_REPEAT89(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT91(MACRO)                                      \
  MACRO(91) KAGUYA_PP_REVERSE_REPEAT90(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT92(MACRO)                                      \
  MACRO(92) KAGUYA_PP_REVERSE_REPEAT91(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT93(MACRO)                                      \
  MACRO(93) KAGUYA_PP_REVERSE_REPEAT92(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT94(MACRO)                                      \
  MACRO(94) KAGUYA_PP_REVERSE_REPEAT93(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT95(MACRO)                                      \
  MACRO(95) KAGUYA_PP_REVERSE_REPEAT94(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT96(MACRO)                                      \
  MACRO(96) KAGUYA_PP_REVERSE_REPEAT95(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT97(MACRO)                                      \
  MACRO(97) KAGUYA_PP_REVERSE_REPEAT96(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT98(MACRO)                                      \
  MACRO(98) KAGUYA_PP_REVERSE_REPEAT97(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT99(MACRO)                                      \
  MACRO(99) KAGUYA_PP_REVERSE_REPEAT98(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT100(MACRO)                                     \
  MACRO(100) KAGUYA_PP_REVERSE_REPEAT99(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT101(MACRO)                                     \
  MACRO(101) KAGUYA_PP_REVERSE_REPEAT100(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT102(MACRO)                                     \
  MACRO(102) KAGUYA_PP_REVERSE_REPEAT101(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT103(MACRO)                                     \
  MACRO(103) KAGUYA_PP_REVERSE_REPEAT102(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT104(MACRO)                                     \
  MACRO(104) KAGUYA_PP_REVERSE_REPEAT103(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT105(MACRO)                                     \
  MACRO(105) KAGUYA_PP_REVERSE_REPEAT104(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT106(MACRO)                                     \
  MACRO(106) KAGUYA_PP_REVERSE_REPEAT105(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT107(MACRO)                                     \
  MACRO(107) KAGUYA_PP_REVERSE_REPEAT106(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT108(MACRO)                                     \
  MACRO(108) KAGUYA_PP_REVERSE_REPEAT107(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT109(MACRO)                                     \
  MACRO(109) KAGUYA_PP_REVERSE_REPEAT108(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT110(MACRO)                                     \
  MACRO(110) KAGUYA_PP_REVERSE_REPEAT109(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT111(MACRO)                                     \
  MACRO(111) KAGUYA_PP_REVERSE_REPEAT110(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT112(MACRO)                                     \
  MACRO(112) KAGUYA_PP_REVERSE_REPEAT111(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT113(MACRO)                                     \
  MACRO(113) KAGUYA_PP_REVERSE_REPEAT112(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT114(MACRO)                                     \
  MACRO(114) KAGUYA_PP_REVERSE_REPEAT113(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT115(MACRO)                                     \
  MACRO(115) KAGUYA_PP_REVERSE_REPEAT114(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT116(MACRO)                                     \
  MACRO(116) KAGUYA_PP_REVERSE_REPEAT115(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT117(MACRO)                                     \
  MACRO(117) KAGUYA_PP_REVERSE_REPEAT116(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT118(MACRO)                                     \
  MACRO(118) KAGUYA_PP_REVERSE_REPEAT117(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT119(MACRO)                                     \
  MACRO(119) KAGUYA_PP_REVERSE_REPEAT118(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT120(MACRO)                                     \
  MACRO(120) KAGUYA_PP_REVERSE_REPEAT119(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT121(MACRO)                                     \
  MACRO(121) KAGUYA_PP_REVERSE_REPEAT120(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT122(MACRO)                                     \
  MACRO(122) KAGUYA_PP_REVERSE_REPEAT121(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT123(MACRO)                                     \
  MACRO(123) KAGUYA_PP_REVERSE_REPEAT122(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT124(MACRO)                                     \
  MACRO(124) KAGUYA_PP_REVERSE_REPEAT123(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT125(MACRO)                                     \
  MACRO(125) KAGUYA_PP_REVERSE_REPEAT124(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT126(MACRO)                                     \
  MACRO(126) KAGUYA_PP_REVERSE_REPEAT125(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT127(MACRO)                                     \
  MACRO(127) KAGUYA_PP_REVERSE_REPEAT126(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT128(MACRO)                                     \
  MACRO(128) KAGUYA_PP_REVERSE_REPEAT127(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT129(MACRO)                                     \
  MACRO(129) KAGUYA_PP_REVERSE_REPEAT128(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT130(MACRO)                                     \
  MACRO(130) KAGUYA_PP_REVERSE_REPEAT129(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT131(MACRO)                                     \
  MACRO(131) KAGUYA_PP_REVERSE_REPEAT130(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT132(MACRO)                                     \
  MACRO(132) KAGUYA_PP_REVERSE_REPEAT131(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT133(MACRO)                                     \
  MACRO(133) KAGUYA_PP_REVERSE_REPEAT132(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT134(MACRO)                                     \
  MACRO(134) KAGUYA_PP_REVERSE_REPEAT133(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT135(MACRO)                                     \
  MACRO(135) KAGUYA_PP_REVERSE_REPEAT134(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT136(MACRO)                                     \
  MACRO(136) KAGUYA_PP_REVERSE_REPEAT135(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT137(MACRO)                                     \
  MACRO(137) KAGUYA_PP_REVERSE_REPEAT136(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT138(MACRO)                                     \
  MACRO(138) KAGUYA_PP_REVERSE_REPEAT137(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT139(MACRO)                                     \
  MACRO(139) KAGUYA_PP_REVERSE_REPEAT138(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT140(MACRO)                                     \
  MACRO(140) KAGUYA_PP_REVERSE_REPEAT139(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT141(MACRO)                                     \
  MACRO(141) KAGUYA_PP_REVERSE_REPEAT140(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT142(MACRO)                                     \
  MACRO(142) KAGUYA_PP_REVERSE_REPEAT141(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT143(MACRO)                                     \
  MACRO(143) KAGUYA_PP_REVERSE_REPEAT142(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT144(MACRO)                                     \
  MACRO(144) KAGUYA_PP_REVERSE_REPEAT143(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT145(MACRO)                                     \
  MACRO(145) KAGUYA_PP_REVERSE_REPEAT144(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT146(MACRO)                                     \
  MACRO(146) KAGUYA_PP_REVERSE_REPEAT145(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT147(MACRO)                                     \
  MACRO(147) KAGUYA_PP_REVERSE_REPEAT146(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT148(MACRO)                                     \
  MACRO(148) KAGUYA_PP_REVERSE_REPEAT147(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT149(MACRO)                                     \
  MACRO(149) KAGUYA_PP_REVERSE_REPEAT148(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT150(MACRO)                                     \
  MACRO(150) KAGUYA_PP_REVERSE_REPEAT149(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT151(MACRO)                                     \
  MACRO(151) KAGUYA_PP_REVERSE_REPEAT150(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT152(MACRO)                                     \
  MACRO(152) KAGUYA_PP_REVERSE_REPEAT151(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT153(MACRO)                                     \
  MACRO(153) KAGUYA_PP_REVERSE_REPEAT152(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT154(MACRO)                                     \
  MACRO(154) KAGUYA_PP_REVERSE_REPEAT153(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT155(MACRO)                                     \
  MACRO(155) KAGUYA_PP_REVERSE_REPEAT154(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT156(MACRO)                                     \
  MACRO(156) KAGUYA_PP_REVERSE_REPEAT155(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT157(MACRO)                                     \
  MACRO(157) KAGUYA_PP_REVERSE_REPEAT156(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT158(MACRO)                                     \
  MACRO(158) KAGUYA_PP_REVERSE_REPEAT157(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT159(MACRO)                                     \
  MACRO(159) KAGUYA_PP_REVERSE_REPEAT158(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT160(MACRO)                                     \
  MACRO(160) KAGUYA_PP_REVERSE_REPEAT159(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT161(MACRO)                                     \
  MACRO(161) KAGUYA_PP_REVERSE_REPEAT160(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT162(MACRO)                                     \
  MACRO(162) KAGUYA_PP_REVERSE_REPEAT161(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT163(MACRO)                                     \
  MACRO(163) KAGUYA_PP_REVERSE_REPEAT162(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT164(MACRO)                                     \
  MACRO(164) KAGUYA_PP_REVERSE_REPEAT163(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT165(MACRO)                                     \
  MACRO(165) KAGUYA_PP_REVERSE_REPEAT164(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT166(MACRO)                                     \
  MACRO(166) KAGUYA_PP_REVERSE_REPEAT165(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT167(MACRO)                                     \
  MACRO(167) KAGUYA_PP_REVERSE_REPEAT166(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT168(MACRO)                                     \
  MACRO(168) KAGUYA_PP_REVERSE_REPEAT167(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT169(MACRO)                                     \
  MACRO(169) KAGUYA_PP_REVERSE_REPEAT168(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT170(MACRO)                                     \
  MACRO(170) KAGUYA_PP_REVERSE_REPEAT169(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT171(MACRO)                                     \
  MACRO(171) KAGUYA_PP_REVERSE_REPEAT170(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT172(MACRO)                                     \
  MACRO(172) KAGUYA_PP_REVERSE_REPEAT171(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT173(MACRO)                                     \
  MACRO(173) KAGUYA_PP_REVERSE_REPEAT172(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT174(MACRO)                                     \
  MACRO(174) KAGUYA_PP_REVERSE_REPEAT173(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT175(MACRO)                                     \
  MACRO(175) KAGUYA_PP_REVERSE_REPEAT174(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT176(MACRO)                                     \
  MACRO(176) KAGUYA_PP_REVERSE_REPEAT175(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT177(MACRO)                                     \
  MACRO(177) KAGUYA_PP_REVERSE_REPEAT176(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT178(MACRO)                                     \
  MACRO(178) KAGUYA_PP_REVERSE_REPEAT177(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT179(MACRO)                                     \
  MACRO(179) KAGUYA_PP_REVERSE_REPEAT178(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT180(MACRO)                                     \
  MACRO(180) KAGUYA_PP_REVERSE_REPEAT179(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT181(MACRO)                                     \
  MACRO(181) KAGUYA_PP_REVERSE_REPEAT180(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT182(MACRO)                                     \
  MACRO(182) KAGUYA_PP_REVERSE_REPEAT181(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT183(MACRO)                                     \
  MACRO(183) KAGUYA_PP_REVERSE_REPEAT182(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT184(MACRO)                                     \
  MACRO(184) KAGUYA_PP_REVERSE_REPEAT183(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT185(MACRO)                                     \
  MACRO(185) KAGUYA_PP_REVERSE_REPEAT184(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT186(MACRO)                                     \
  MACRO(186) KAGUYA_PP_REVERSE_REPEAT185(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT187(MACRO)                                     \
  MACRO(187) KAGUYA_PP_REVERSE_REPEAT186(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT188(MACRO)                                     \
  MACRO(188) KAGUYA_PP_REVERSE_REPEAT187(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT189(MACRO)                                     \
  MACRO(189) KAGUYA_PP_REVERSE_REPEAT188(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT190(MACRO)                                     \
  MACRO(190) KAGUYA_PP_REVERSE_REPEAT189(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT191(MACRO)                                     \
  MACRO(191) KAGUYA_PP_REVERSE_REPEAT190(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT192(MACRO)                                     \
  MACRO(192) KAGUYA_PP_REVERSE_REPEAT191(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT193(MACRO)                                     \
  MACRO(193) KAGUYA_PP_REVERSE_REPEAT192(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT194(MACRO)                                     \
  MACRO(194) KAGUYA_PP_REVERSE_REPEAT193(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT195(MACRO)                                     \
  MACRO(195) KAGUYA_PP_REVERSE_REPEAT194(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT196(MACRO)                                     \
  MACRO(196) KAGUYA_PP_REVERSE_REPEAT195(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT197(MACRO)                                     \
  MACRO(197) KAGUYA_PP_REVERSE_REPEAT196(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT198(MACRO)                                     \
  MACRO(198) KAGUYA_PP_REVERSE_REPEAT197(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT199(MACRO)                                     \
  MACRO(199) KAGUYA_PP_REVERSE_REPEAT198(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT200(MACRO)                                     \
  MACRO(200) KAGUYA_PP_REVERSE_REPEAT199(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT201(MACRO)                                     \
  MACRO(201) KAGUYA_PP_REVERSE_REPEAT200(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT202(MACRO)                                     \
  MACRO(202) KAGUYA_PP_REVERSE_REPEAT201(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT203(MACRO)                                     \
  MACRO(203) KAGUYA_PP_REVERSE_REPEAT202(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT204(MACRO)                                     \
  MACRO(204) KAGUYA_PP_REVERSE_REPEAT203(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT205(MACRO)                                     \
  MACRO(205) KAGUYA_PP_REVERSE_REPEAT204(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT206(MACRO)                                     \
  MACRO(206) KAGUYA_PP_REVERSE_REPEAT205(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT207(MACRO)                                     \
  MACRO(207) KAGUYA_PP_REVERSE_REPEAT206(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT208(MACRO)                                     \
  MACRO(208) KAGUYA_PP_REVERSE_REPEAT207(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT209(MACRO)                                     \
  MACRO(209) KAGUYA_PP_REVERSE_REPEAT208(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT210(MACRO)                                     \
  MACRO(210) KAGUYA_PP_REVERSE_REPEAT209(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT211(MACRO)                                     \
  MACRO(211) KAGUYA_PP_REVERSE_REPEAT210(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT212(MACRO)                                     \
  MACRO(212) KAGUYA_PP_REVERSE_REPEAT211(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT213(MACRO)                                     \
  MACRO(213) KAGUYA_PP_REVERSE_REPEAT212(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT214(MACRO)                                     \
  MACRO(214) KAGUYA_PP_REVERSE_REPEAT213(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT215(MACRO)                                     \
  MACRO(215) KAGUYA_PP_REVERSE_REPEAT214(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT216(MACRO)                                     \
  MACRO(216) KAGUYA_PP_REVERSE_REPEAT215(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT217(MACRO)                                     \
  MACRO(217) KAGUYA_PP_REVERSE_REPEAT216(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT218(MACRO)                                     \
  MACRO(218) KAGUYA_PP_REVERSE_REPEAT217(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT219(MACRO)                                     \
  MACRO(219) KAGUYA_PP_REVERSE_REPEAT218(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT220(MACRO)                                     \
  MACRO(220) KAGUYA_PP_REVERSE_REPEAT219(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT221(MACRO)                                     \
  MACRO(221) KAGUYA_PP_REVERSE_REPEAT220(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT222(MACRO)                                     \
  MACRO(222) KAGUYA_PP_REVERSE_REPEAT221(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT223(MACRO)                                     \
  MACRO(223) KAGUYA_PP_REVERSE_REPEAT222(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT224(MACRO)                                     \
  MACRO(224) KAGUYA_PP_REVERSE_REPEAT223(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT225(MACRO)                                     \
  MACRO(225) KAGUYA_PP_REVERSE_REPEAT224(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT226(MACRO)                                     \
  MACRO(226) KAGUYA_PP_REVERSE_REPEAT225(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT227(MACRO)                                     \
  MACRO(227) KAGUYA_PP_REVERSE_REPEAT226(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT228(MACRO)                                     \
  MACRO(228) KAGUYA_PP_REVERSE_REPEAT227(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT229(MACRO)                                     \
  MACRO(229) KAGUYA_PP_REVERSE_REPEAT228(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT230(MACRO)                                     \
  MACRO(230) KAGUYA_PP_REVERSE_REPEAT229(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT231(MACRO)                                     \
  MACRO(231) KAGUYA_PP_REVERSE_REPEAT230(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT232(MACRO)                                     \
  MACRO(232) KAGUYA_PP_REVERSE_REPEAT231(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT233(MACRO)                                     \
  MACRO(233) KAGUYA_PP_REVERSE_REPEAT232(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT234(MACRO)                                     \
  MACRO(234) KAGUYA_PP_REVERSE_REPEAT233(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT235(MACRO)                                     \
  MACRO(235) KAGUYA_PP_REVERSE_REPEAT234(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT236(MACRO)                                     \
  MACRO(236) KAGUYA_PP_REVERSE_REPEAT235(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT237(MACRO)                                     \
  MACRO(237) KAGUYA_PP_REVERSE_REPEAT236(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT238(MACRO)                                     \
  MACRO(238) KAGUYA_PP_REVERSE_REPEAT237(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT239(MACRO)                                     \
  MACRO(239) KAGUYA_PP_REVERSE_REPEAT238(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT240(MACRO)                                     \
  MACRO(240) KAGUYA_PP_REVERSE_REPEAT239(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT241(MACRO)                                     \
  MACRO(241) KAGUYA_PP_REVERSE_REPEAT240(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT242(MACRO)                                     \
  MACRO(242) KAGUYA_PP_REVERSE_REPEAT241(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT243(MACRO)                                     \
  MACRO(243) KAGUYA_PP_REVERSE_REPEAT242(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT244(MACRO)                                     \
  MACRO(244) KAGUYA_PP_REVERSE_REPEAT243(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT245(MACRO)                                     \
  MACRO(245) KAGUYA_PP_REVERSE_REPEAT244(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT246(MACRO)                                     \
  MACRO(246) KAGUYA_PP_REVERSE_REPEAT245(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT247(MACRO)                                     \
  MACRO(247) KAGUYA_PP_REVERSE_REPEAT246(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT248(MACRO)                                     \
  MACRO(248) KAGUYA_PP_REVERSE_REPEAT247(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT249(MACRO)                                     \
  MACRO(249) KAGUYA_PP_REVERSE_REPEAT248(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT250(MACRO)                                     \
  MACRO(250) KAGUYA_PP_REVERSE_REPEAT249(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT251(MACRO)                                     \
  MACRO(251) KAGUYA_PP_REVERSE_REPEAT250(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT252(MACRO)                                     \
  MACRO(252) KAGUYA_PP_REVERSE_REPEAT251(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT253(MACRO)                                     \
  MACRO(253) KAGUYA_PP_REVERSE_REPEAT252(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT254(MACRO)                                     \
  MACRO(254) KAGUYA_PP_REVERSE_REPEAT253(MACRO)
#define KAGUYA_PP_REVERSE_REPEAT(COUNT, MACRO)                                 \
  KAGUYA_PP_CAT(KAGUYA_PP_REVERSE_REPEAT, COUNT)(MACRO)

#define KAGUYA_PP_REPEAT_ARG0(MACRO)
#define KAGUYA_PP_REPEAT_ARG1(MACRO) MACRO(1)
#define KAGUYA_PP_REPEAT_ARG2(MACRO) KAGUYA_PP_REPEAT_ARG1(MACRO), MACRO(2)
#define KAGUYA_PP_REPEAT_ARG3(MACRO) KAGUYA_PP_REPEAT_ARG2(MACRO), MACRO(3)
#define KAGUYA_PP_REPEAT_ARG4(MACRO) KAGUYA_PP_REPEAT_ARG3(MACRO), MACRO(4)
#define KAGUYA_PP_REPEAT_ARG5(MACRO) KAGUYA_PP_REPEAT_ARG4(MACRO), MACRO(5)
#define KAGUYA_PP_REPEAT_ARG6(MACRO) KAGUYA_PP_REPEAT_ARG5(MACRO), MACRO(6)
#define KAGUYA_PP_REPEAT_ARG7(MACRO) KAGUYA_PP_REPEAT_ARG6(MACRO), MACRO(7)
#define KAGUYA_PP_REPEAT_ARG8(MACRO) KAGUYA_PP_REPEAT_ARG7(MACRO), MACRO(8)
#define KAGUYA_PP_REPEAT_ARG9(MACRO) KAGUYA_PP_REPEAT_ARG8(MACRO), MACRO(9)
#define KAGUYA_PP_REPEAT_ARG10(MACRO) KAGUYA_PP_REPEAT_ARG9(MACRO), MACRO(10)
#define KAGUYA_PP_REPEAT_ARG11(MACRO) KAGUYA_PP_REPEAT_ARG10(MACRO), MACRO(11)
#define KAGUYA_PP_REPEAT_ARG12(MACRO) KAGUYA_PP_REPEAT_ARG11(MACRO), MACRO(12)
#define KAGUYA_PP_REPEAT_ARG13(MACRO) KAGUYA_PP_REPEAT_ARG12(MACRO), MACRO(13)
#define KAGUYA_PP_REPEAT_ARG14(MACRO) KAGUYA_PP_REPEAT_ARG13(MACRO), MACRO(14)
#define KAGUYA_PP_REPEAT_ARG15(MACRO) KAGUYA_PP_REPEAT_ARG14(MACRO), MACRO(15)
#define KAGUYA_PP_REPEAT_ARG16(MACRO) KAGUYA_PP_REPEAT_ARG15(MACRO), MACRO(16)
#define KAGUYA_PP_REPEAT_ARG17(MACRO) KAGUYA_PP_REPEAT_ARG16(MACRO), MACRO(17)
#define KAGUYA_PP_REPEAT_ARG18(MACRO) KAGUYA_PP_REPEAT_ARG17(MACRO), MACRO(18)
#define KAGUYA_PP_REPEAT_ARG19(MACRO) KAGUYA_PP_REPEAT_ARG18(MACRO), MACRO(19)
#define KAGUYA_PP_REPEAT_ARG20(MACRO) KAGUYA_PP_REPEAT_ARG19(MACRO), MACRO(20)
#define KAGUYA_PP_REPEAT_ARG21(MACRO) KAGUYA_PP_REPEAT_ARG20(MACRO), MACRO(21)
#define KAGUYA_PP_REPEAT_ARG22(MACRO) KAGUYA_PP_REPEAT_ARG21(MACRO), MACRO(22)
#define KAGUYA_PP_REPEAT_ARG23(MACRO) KAGUYA_PP_REPEAT_ARG22(MACRO), MACRO(23)
#define KAGUYA_PP_REPEAT_ARG24(MACRO) KAGUYA_PP_REPEAT_ARG23(MACRO), MACRO(24)
#define KAGUYA_PP_REPEAT_ARG25(MACRO) KAGUYA_PP_REPEAT_ARG24(MACRO), MACRO(25)
#define KAGUYA_PP_REPEAT_ARG26(MACRO) KAGUYA_PP_REPEAT_ARG25(MACRO), MACRO(26)
#define KAGUYA_PP_REPEAT_ARG27(MACRO) KAGUYA_PP_REPEAT_ARG26(MACRO), MACRO(27)
#define KAGUYA_PP_REPEAT_ARG28(MACRO) KAGUYA_PP_REPEAT_ARG27(MACRO), MACRO(28)
#define KAGUYA_PP_REPEAT_ARG29(MACRO) KAGUYA_PP_REPEAT_ARG28(MACRO), MACRO(29)
#define KAGUYA_PP_REPEAT_ARG30(MACRO) KAGUYA_PP_REPEAT_ARG29(MACRO), MACRO(30)
#define KAGUYA_PP_REPEAT_ARG31(MACRO) KAGUYA_PP_REPEAT_ARG30(MACRO), MACRO(31)
#define KAGUYA_PP_REPEAT_ARG32(MACRO) KAGUYA_PP_REPEAT_ARG31(MACRO), MACRO(32)
#define KAGUYA_PP_REPEAT_ARG33(MACRO) KAGUYA_PP_REPEAT_ARG32(MACRO), MACRO(33)
#define KAGUYA_PP_REPEAT_ARG34(MACRO) KAGUYA_PP_REPEAT_ARG33(MACRO), MACRO(34)
#define KAGUYA_PP_REPEAT_ARG35(MACRO) KAGUYA_PP_REPEAT_ARG34(MACRO), MACRO(35)
#define KAGUYA_PP_REPEAT_ARG36(MACRO) KAGUYA_PP_REPEAT_ARG35(MACRO), MACRO(36)
#define KAGUYA_PP_REPEAT_ARG37(MACRO) KAGUYA_PP_REPEAT_ARG36(MACRO), MACRO(37)
#define KAGUYA_PP_REPEAT_ARG38(MACRO) KAGUYA_PP_REPEAT_ARG37(MACRO), MACRO(38)
#define KAGUYA_PP_REPEAT_ARG39(MACRO) KAGUYA_PP_REPEAT_ARG38(MACRO), MACRO(39)
#define KAGUYA_PP_REPEAT_ARG40(MACRO) KAGUYA_PP_REPEAT_ARG39(MACRO), MACRO(40)
#define KAGUYA_PP_REPEAT_ARG41(MACRO) KAGUYA_PP_REPEAT_ARG40(MACRO), MACRO(41)
#define KAGUYA_PP_REPEAT_ARG42(MACRO) KAGUYA_PP_REPEAT_ARG41(MACRO), MACRO(42)
#define KAGUYA_PP_REPEAT_ARG43(MACRO) KAGUYA_PP_REPEAT_ARG42(MACRO), MACRO(43)
#define KAGUYA_PP_REPEAT_ARG44(MACRO) KAGUYA_PP_REPEAT_ARG43(MACRO), MACRO(44)
#define KAGUYA_PP_REPEAT_ARG45(MACRO) KAGUYA_PP_REPEAT_ARG44(MACRO), MACRO(45)
#define KAGUYA_PP_REPEAT_ARG46(MACRO) KAGUYA_PP_REPEAT_ARG45(MACRO), MACRO(46)
#define KAGUYA_PP_REPEAT_ARG47(MACRO) KAGUYA_PP_REPEAT_ARG46(MACRO), MACRO(47)
#define KAGUYA_PP_REPEAT_ARG48(MACRO) KAGUYA_PP_REPEAT_ARG47(MACRO), MACRO(48)
#define KAGUYA_PP_REPEAT_ARG49(MACRO) KAGUYA_PP_REPEAT_ARG48(MACRO), MACRO(49)
#define KAGUYA_PP_REPEAT_ARG50(MACRO) KAGUYA_PP_REPEAT_ARG49(MACRO), MACRO(50)
#define KAGUYA_PP_REPEAT_ARG51(MACRO) KAGUYA_PP_REPEAT_ARG50(MACRO), MACRO(51)
#define KAGUYA_PP_REPEAT_ARG52(MACRO) KAGUYA_PP_REPEAT_ARG51(MACRO), MACRO(52)
#define KAGUYA_PP_REPEAT_ARG53(MACRO) KAGUYA_PP_REPEAT_ARG52(MACRO), MACRO(53)
#define KAGUYA_PP_REPEAT_ARG54(MACRO) KAGUYA_PP_REPEAT_ARG53(MACRO), MACRO(54)
#define KAGUYA_PP_REPEAT_ARG55(MACRO) KAGUYA_PP_REPEAT_ARG54(MACRO), MACRO(55)
#define KAGUYA_PP_REPEAT_ARG56(MACRO) KAGUYA_PP_REPEAT_ARG55(MACRO), MACRO(56)
#define KAGUYA_PP_REPEAT_ARG57(MACRO) KAGUYA_PP_REPEAT_ARG56(MACRO), MACRO(57)
#define KAGUYA_PP_REPEAT_ARG58(MACRO) KAGUYA_PP_REPEAT_ARG57(MACRO), MACRO(58)
#define KAGUYA_PP_REPEAT_ARG59(MACRO) KAGUYA_PP_REPEAT_ARG58(MACRO), MACRO(59)
#define KAGUYA_PP_REPEAT_ARG60(MACRO) KAGUYA_PP_REPEAT_ARG59(MACRO), MACRO(60)
#define KAGUYA_PP_REPEAT_ARG61(MACRO) KAGUYA_PP_REPEAT_ARG60(MACRO), MACRO(61)
#define KAGUYA_PP_REPEAT_ARG62(MACRO) KAGUYA_PP_REPEAT_ARG61(MACRO), MACRO(62)
#define KAGUYA_PP_REPEAT_ARG63(MACRO) KAGUYA_PP_REPEAT_ARG62(MACRO), MACRO(63)
#define KAGUYA_PP_REPEAT_ARG64(MACRO) KAGUYA_PP_REPEAT_ARG63(MACRO), MACRO(64)
#define KAGUYA_PP_REPEAT_ARG65(MACRO) KAGUYA_PP_REPEAT_ARG64(MACRO), MACRO(65)
#define KAGUYA_PP_REPEAT_ARG66(MACRO) KAGUYA_PP_REPEAT_ARG65(MACRO), MACRO(66)
#define KAGUYA_PP_REPEAT_ARG67(MACRO) KAGUYA_PP_REPEAT_ARG66(MACRO), MACRO(67)
#define KAGUYA_PP_REPEAT_ARG68(MACRO) KAGUYA_PP_REPEAT_ARG67(MACRO), MACRO(68)
#define KAGUYA_PP_REPEAT_ARG69(MACRO) KAGUYA_PP_REPEAT_ARG68(MACRO), MACRO(69)
#define KAGUYA_PP_REPEAT_ARG70(MACRO) KAGUYA_PP_REPEAT_ARG69(MACRO), MACRO(70)
#define KAGUYA_PP_REPEAT_ARG71(MACRO) KAGUYA_PP_REPEAT_ARG70(MACRO), MACRO(71)
#define KAGUYA_PP_REPEAT_ARG72(MACRO) KAGUYA_PP_REPEAT_ARG71(MACRO), MACRO(72)
#define KAGUYA_PP_REPEAT_ARG73(MACRO) KAGUYA_PP_REPEAT_ARG72(MACRO), MACRO(73)
#define KAGUYA_PP_REPEAT_ARG74(MACRO) KAGUYA_PP_REPEAT_ARG73(MACRO), MACRO(74)
#define KAGUYA_PP_REPEAT_ARG75(MACRO) KAGUYA_PP_REPEAT_ARG74(MACRO), MACRO(75)
#define KAGUYA_PP_REPEAT_ARG76(MACRO) KAGUYA_PP_REPEAT_ARG75(MACRO), MACRO(76)
#define KAGUYA_PP_REPEAT_ARG77(MACRO) KAGUYA_PP_REPEAT_ARG76(MACRO), MACRO(77)
#define KAGUYA_PP_REPEAT_ARG78(MACRO) KAGUYA_PP_REPEAT_ARG77(MACRO), MACRO(78)
#define KAGUYA_PP_REPEAT_ARG79(MACRO) KAGUYA_PP_REPEAT_ARG78(MACRO), MACRO(79)
#define KAGUYA_PP_REPEAT_ARG80(MACRO) KAGUYA_PP_REPEAT_ARG79(MACRO), MACRO(80)
#define KAGUYA_PP_REPEAT_ARG81(MACRO) KAGUYA_PP_REPEAT_ARG80(MACRO), MACRO(81)
#define KAGUYA_PP_REPEAT_ARG82(MACRO) KAGUYA_PP_REPEAT_ARG81(MACRO), MACRO(82)
#define KAGUYA_PP_REPEAT_ARG83(MACRO) KAGUYA_PP_REPEAT_ARG82(MACRO), MACRO(83)
#define KAGUYA_PP_REPEAT_ARG84(MACRO) KAGUYA_PP_REPEAT_ARG83(MACRO), MACRO(84)
#define KAGUYA_PP_REPEAT_ARG85(MACRO) KAGUYA_PP_REPEAT_ARG84(MACRO), MACRO(85)
#define KAGUYA_PP_REPEAT_ARG86(MACRO) KAGUYA_PP_REPEAT_ARG85(MACRO), MACRO(86)
#define KAGUYA_PP_REPEAT_ARG87(MACRO) KAGUYA_PP_REPEAT_ARG86(MACRO), MACRO(87)
#define KAGUYA_PP_REPEAT_ARG88(MACRO) KAGUYA_PP_REPEAT_ARG87(MACRO), MACRO(88)
#define KAGUYA_PP_REPEAT_ARG89(MACRO) KAGUYA_PP_REPEAT_ARG88(MACRO), MACRO(89)
#define KAGUYA_PP_REPEAT_ARG90(MACRO) KAGUYA_PP_REPEAT_ARG89(MACRO), MACRO(90)
#define KAGUYA_PP_REPEAT_ARG91(MACRO) KAGUYA_PP_REPEAT_ARG90(MACRO), MACRO(91)
#define KAGUYA_PP_REPEAT_ARG92(MACRO) KAGUYA_PP_REPEAT_ARG91(MACRO), MACRO(92)
#define KAGUYA_PP_REPEAT_ARG93(MACRO) KAGUYA_PP_REPEAT_ARG92(MACRO), MACRO(93)
#define KAGUYA_PP_REPEAT_ARG94(MACRO) KAGUYA_PP_REPEAT_ARG93(MACRO), MACRO(94)
#define KAGUYA_PP_REPEAT_ARG95(MACRO) KAGUYA_PP_REPEAT_ARG94(MACRO), MACRO(95)
#define KAGUYA_PP_REPEAT_ARG96(MACRO) KAGUYA_PP_REPEAT_ARG95(MACRO), MACRO(96)
#define KAGUYA_PP_REPEAT_ARG97(MACRO) KAGUYA_PP_REPEAT_ARG96(MACRO), MACRO(97)
#define KAGUYA_PP_REPEAT_ARG98(MACRO) KAGUYA_PP_REPEAT_ARG97(MACRO), MACRO(98)
#define KAGUYA_PP_REPEAT_ARG99(MACRO) KAGUYA_PP_REPEAT_ARG98(MACRO), MACRO(99)
#define KAGUYA_PP_REPEAT_ARG100(MACRO) KAGUYA_PP_REPEAT_ARG99(MACRO), MACRO(100)
#define KAGUYA_PP_REPEAT_ARG101(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG100(MACRO), MACRO(101)
#define KAGUYA_PP_REPEAT_ARG102(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG101(MACRO), MACRO(102)
#define KAGUYA_PP_REPEAT_ARG103(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG102(MACRO), MACRO(103)
#define KAGUYA_PP_REPEAT_ARG104(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG103(MACRO), MACRO(104)
#define KAGUYA_PP_REPEAT_ARG105(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG104(MACRO), MACRO(105)
#define KAGUYA_PP_REPEAT_ARG106(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG105(MACRO), MACRO(106)
#define KAGUYA_PP_REPEAT_ARG107(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG106(MACRO), MACRO(107)
#define KAGUYA_PP_REPEAT_ARG108(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG107(MACRO), MACRO(108)
#define KAGUYA_PP_REPEAT_ARG109(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG108(MACRO), MACRO(109)
#define KAGUYA_PP_REPEAT_ARG110(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG109(MACRO), MACRO(110)
#define KAGUYA_PP_REPEAT_ARG111(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG110(MACRO), MACRO(111)
#define KAGUYA_PP_REPEAT_ARG112(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG111(MACRO), MACRO(112)
#define KAGUYA_PP_REPEAT_ARG113(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG112(MACRO), MACRO(113)
#define KAGUYA_PP_REPEAT_ARG114(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG113(MACRO), MACRO(114)
#define KAGUYA_PP_REPEAT_ARG115(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG114(MACRO), MACRO(115)
#define KAGUYA_PP_REPEAT_ARG116(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG115(MACRO), MACRO(116)
#define KAGUYA_PP_REPEAT_ARG117(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG116(MACRO), MACRO(117)
#define KAGUYA_PP_REPEAT_ARG118(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG117(MACRO), MACRO(118)
#define KAGUYA_PP_REPEAT_ARG119(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG118(MACRO), MACRO(119)
#define KAGUYA_PP_REPEAT_ARG120(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG119(MACRO), MACRO(120)
#define KAGUYA_PP_REPEAT_ARG121(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG120(MACRO), MACRO(121)
#define KAGUYA_PP_REPEAT_ARG122(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG121(MACRO), MACRO(122)
#define KAGUYA_PP_REPEAT_ARG123(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG122(MACRO), MACRO(123)
#define KAGUYA_PP_REPEAT_ARG124(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG123(MACRO), MACRO(124)
#define KAGUYA_PP_REPEAT_ARG125(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG124(MACRO), MACRO(125)
#define KAGUYA_PP_REPEAT_ARG126(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG125(MACRO), MACRO(126)
#define KAGUYA_PP_REPEAT_ARG127(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG126(MACRO), MACRO(127)
#define KAGUYA_PP_REPEAT_ARG128(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG127(MACRO), MACRO(128)
#define KAGUYA_PP_REPEAT_ARG129(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG128(MACRO), MACRO(129)
#define KAGUYA_PP_REPEAT_ARG130(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG129(MACRO), MACRO(130)
#define KAGUYA_PP_REPEAT_ARG131(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG130(MACRO), MACRO(131)
#define KAGUYA_PP_REPEAT_ARG132(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG131(MACRO), MACRO(132)
#define KAGUYA_PP_REPEAT_ARG133(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG132(MACRO), MACRO(133)
#define KAGUYA_PP_REPEAT_ARG134(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG133(MACRO), MACRO(134)
#define KAGUYA_PP_REPEAT_ARG135(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG134(MACRO), MACRO(135)
#define KAGUYA_PP_REPEAT_ARG136(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG135(MACRO), MACRO(136)
#define KAGUYA_PP_REPEAT_ARG137(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG136(MACRO), MACRO(137)
#define KAGUYA_PP_REPEAT_ARG138(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG137(MACRO), MACRO(138)
#define KAGUYA_PP_REPEAT_ARG139(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG138(MACRO), MACRO(139)
#define KAGUYA_PP_REPEAT_ARG140(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG139(MACRO), MACRO(140)
#define KAGUYA_PP_REPEAT_ARG141(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG140(MACRO), MACRO(141)
#define KAGUYA_PP_REPEAT_ARG142(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG141(MACRO), MACRO(142)
#define KAGUYA_PP_REPEAT_ARG143(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG142(MACRO), MACRO(143)
#define KAGUYA_PP_REPEAT_ARG144(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG143(MACRO), MACRO(144)
#define KAGUYA_PP_REPEAT_ARG145(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG144(MACRO), MACRO(145)
#define KAGUYA_PP_REPEAT_ARG146(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG145(MACRO), MACRO(146)
#define KAGUYA_PP_REPEAT_ARG147(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG146(MACRO), MACRO(147)
#define KAGUYA_PP_REPEAT_ARG148(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG147(MACRO), MACRO(148)
#define KAGUYA_PP_REPEAT_ARG149(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG148(MACRO), MACRO(149)
#define KAGUYA_PP_REPEAT_ARG150(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG149(MACRO), MACRO(150)
#define KAGUYA_PP_REPEAT_ARG151(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG150(MACRO), MACRO(151)
#define KAGUYA_PP_REPEAT_ARG152(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG151(MACRO), MACRO(152)
#define KAGUYA_PP_REPEAT_ARG153(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG152(MACRO), MACRO(153)
#define KAGUYA_PP_REPEAT_ARG154(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG153(MACRO), MACRO(154)
#define KAGUYA_PP_REPEAT_ARG155(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG154(MACRO), MACRO(155)
#define KAGUYA_PP_REPEAT_ARG156(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG155(MACRO), MACRO(156)
#define KAGUYA_PP_REPEAT_ARG157(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG156(MACRO), MACRO(157)
#define KAGUYA_PP_REPEAT_ARG158(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG157(MACRO), MACRO(158)
#define KAGUYA_PP_REPEAT_ARG159(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG158(MACRO), MACRO(159)
#define KAGUYA_PP_REPEAT_ARG160(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG159(MACRO), MACRO(160)
#define KAGUYA_PP_REPEAT_ARG161(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG160(MACRO), MACRO(161)
#define KAGUYA_PP_REPEAT_ARG162(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG161(MACRO), MACRO(162)
#define KAGUYA_PP_REPEAT_ARG163(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG162(MACRO), MACRO(163)
#define KAGUYA_PP_REPEAT_ARG164(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG163(MACRO), MACRO(164)
#define KAGUYA_PP_REPEAT_ARG165(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG164(MACRO), MACRO(165)
#define KAGUYA_PP_REPEAT_ARG166(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG165(MACRO), MACRO(166)
#define KAGUYA_PP_REPEAT_ARG167(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG166(MACRO), MACRO(167)
#define KAGUYA_PP_REPEAT_ARG168(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG167(MACRO), MACRO(168)
#define KAGUYA_PP_REPEAT_ARG169(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG168(MACRO), MACRO(169)
#define KAGUYA_PP_REPEAT_ARG170(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG169(MACRO), MACRO(170)
#define KAGUYA_PP_REPEAT_ARG171(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG170(MACRO), MACRO(171)
#define KAGUYA_PP_REPEAT_ARG172(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG171(MACRO), MACRO(172)
#define KAGUYA_PP_REPEAT_ARG173(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG172(MACRO), MACRO(173)
#define KAGUYA_PP_REPEAT_ARG174(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG173(MACRO), MACRO(174)
#define KAGUYA_PP_REPEAT_ARG175(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG174(MACRO), MACRO(175)
#define KAGUYA_PP_REPEAT_ARG176(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG175(MACRO), MACRO(176)
#define KAGUYA_PP_REPEAT_ARG177(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG176(MACRO), MACRO(177)
#define KAGUYA_PP_REPEAT_ARG178(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG177(MACRO), MACRO(178)
#define KAGUYA_PP_REPEAT_ARG179(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG178(MACRO), MACRO(179)
#define KAGUYA_PP_REPEAT_ARG180(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG179(MACRO), MACRO(180)
#define KAGUYA_PP_REPEAT_ARG181(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG180(MACRO), MACRO(181)
#define KAGUYA_PP_REPEAT_ARG182(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG181(MACRO), MACRO(182)
#define KAGUYA_PP_REPEAT_ARG183(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG182(MACRO), MACRO(183)
#define KAGUYA_PP_REPEAT_ARG184(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG183(MACRO), MACRO(184)
#define KAGUYA_PP_REPEAT_ARG185(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG184(MACRO), MACRO(185)
#define KAGUYA_PP_REPEAT_ARG186(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG185(MACRO), MACRO(186)
#define KAGUYA_PP_REPEAT_ARG187(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG186(MACRO), MACRO(187)
#define KAGUYA_PP_REPEAT_ARG188(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG187(MACRO), MACRO(188)
#define KAGUYA_PP_REPEAT_ARG189(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG188(MACRO), MACRO(189)
#define KAGUYA_PP_REPEAT_ARG190(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG189(MACRO), MACRO(190)
#define KAGUYA_PP_REPEAT_ARG191(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG190(MACRO), MACRO(191)
#define KAGUYA_PP_REPEAT_ARG192(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG191(MACRO), MACRO(192)
#define KAGUYA_PP_REPEAT_ARG193(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG192(MACRO), MACRO(193)
#define KAGUYA_PP_REPEAT_ARG194(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG193(MACRO), MACRO(194)
#define KAGUYA_PP_REPEAT_ARG195(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG194(MACRO), MACRO(195)
#define KAGUYA_PP_REPEAT_ARG196(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG195(MACRO), MACRO(196)
#define KAGUYA_PP_REPEAT_ARG197(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG196(MACRO), MACRO(197)
#define KAGUYA_PP_REPEAT_ARG198(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG197(MACRO), MACRO(198)
#define KAGUYA_PP_REPEAT_ARG199(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG198(MACRO), MACRO(199)
#define KAGUYA_PP_REPEAT_ARG200(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG199(MACRO), MACRO(200)
#define KAGUYA_PP_REPEAT_ARG201(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG200(MACRO), MACRO(201)
#define KAGUYA_PP_REPEAT_ARG202(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG201(MACRO), MACRO(202)
#define KAGUYA_PP_REPEAT_ARG203(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG202(MACRO), MACRO(203)
#define KAGUYA_PP_REPEAT_ARG204(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG203(MACRO), MACRO(204)
#define KAGUYA_PP_REPEAT_ARG205(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG204(MACRO), MACRO(205)
#define KAGUYA_PP_REPEAT_ARG206(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG205(MACRO), MACRO(206)
#define KAGUYA_PP_REPEAT_ARG207(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG206(MACRO), MACRO(207)
#define KAGUYA_PP_REPEAT_ARG208(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG207(MACRO), MACRO(208)
#define KAGUYA_PP_REPEAT_ARG209(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG208(MACRO), MACRO(209)
#define KAGUYA_PP_REPEAT_ARG210(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG209(MACRO), MACRO(210)
#define KAGUYA_PP_REPEAT_ARG211(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG210(MACRO), MACRO(211)
#define KAGUYA_PP_REPEAT_ARG212(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG211(MACRO), MACRO(212)
#define KAGUYA_PP_REPEAT_ARG213(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG212(MACRO), MACRO(213)
#define KAGUYA_PP_REPEAT_ARG214(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG213(MACRO), MACRO(214)
#define KAGUYA_PP_REPEAT_ARG215(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG214(MACRO), MACRO(215)
#define KAGUYA_PP_REPEAT_ARG216(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG215(MACRO), MACRO(216)
#define KAGUYA_PP_REPEAT_ARG217(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG216(MACRO), MACRO(217)
#define KAGUYA_PP_REPEAT_ARG218(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG217(MACRO), MACRO(218)
#define KAGUYA_PP_REPEAT_ARG219(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG218(MACRO), MACRO(219)
#define KAGUYA_PP_REPEAT_ARG220(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG219(MACRO), MACRO(220)
#define KAGUYA_PP_REPEAT_ARG221(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG220(MACRO), MACRO(221)
#define KAGUYA_PP_REPEAT_ARG222(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG221(MACRO), MACRO(222)
#define KAGUYA_PP_REPEAT_ARG223(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG222(MACRO), MACRO(223)
#define KAGUYA_PP_REPEAT_ARG224(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG223(MACRO), MACRO(224)
#define KAGUYA_PP_REPEAT_ARG225(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG224(MACRO), MACRO(225)
#define KAGUYA_PP_REPEAT_ARG226(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG225(MACRO), MACRO(226)
#define KAGUYA_PP_REPEAT_ARG227(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG226(MACRO), MACRO(227)
#define KAGUYA_PP_REPEAT_ARG228(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG227(MACRO), MACRO(228)
#define KAGUYA_PP_REPEAT_ARG229(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG228(MACRO), MACRO(229)
#define KAGUYA_PP_REPEAT_ARG230(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG229(MACRO), MACRO(230)
#define KAGUYA_PP_REPEAT_ARG231(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG230(MACRO), MACRO(231)
#define KAGUYA_PP_REPEAT_ARG232(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG231(MACRO), MACRO(232)
#define KAGUYA_PP_REPEAT_ARG233(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG232(MACRO), MACRO(233)
#define KAGUYA_PP_REPEAT_ARG234(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG233(MACRO), MACRO(234)
#define KAGUYA_PP_REPEAT_ARG235(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG234(MACRO), MACRO(235)
#define KAGUYA_PP_REPEAT_ARG236(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG235(MACRO), MACRO(236)
#define KAGUYA_PP_REPEAT_ARG237(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG236(MACRO), MACRO(237)
#define KAGUYA_PP_REPEAT_ARG238(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG237(MACRO), MACRO(238)
#define KAGUYA_PP_REPEAT_ARG239(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG238(MACRO), MACRO(239)
#define KAGUYA_PP_REPEAT_ARG240(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG239(MACRO), MACRO(240)
#define KAGUYA_PP_REPEAT_ARG241(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG240(MACRO), MACRO(241)
#define KAGUYA_PP_REPEAT_ARG242(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG241(MACRO), MACRO(242)
#define KAGUYA_PP_REPEAT_ARG243(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG242(MACRO), MACRO(243)
#define KAGUYA_PP_REPEAT_ARG244(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG243(MACRO), MACRO(244)
#define KAGUYA_PP_REPEAT_ARG245(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG244(MACRO), MACRO(245)
#define KAGUYA_PP_REPEAT_ARG246(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG245(MACRO), MACRO(246)
#define KAGUYA_PP_REPEAT_ARG247(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG246(MACRO), MACRO(247)
#define KAGUYA_PP_REPEAT_ARG248(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG247(MACRO), MACRO(248)
#define KAGUYA_PP_REPEAT_ARG249(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG248(MACRO), MACRO(249)
#define KAGUYA_PP_REPEAT_ARG250(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG249(MACRO), MACRO(250)
#define KAGUYA_PP_REPEAT_ARG251(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG250(MACRO), MACRO(251)
#define KAGUYA_PP_REPEAT_ARG252(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG251(MACRO), MACRO(252)
#define KAGUYA_PP_REPEAT_ARG253(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG252(MACRO), MACRO(253)
#define KAGUYA_PP_REPEAT_ARG254(MACRO)                                         \
  KAGUYA_PP_REPEAT_ARG253(MACRO), MACRO(254)
#define KAGUYA_PP_REPEAT_ARG(COUNT, MACRO)                                     \
  KAGUYA_PP_CAT(KAGUYA_PP_REPEAT_ARG, COUNT)(MACRO)

#define KAGUYA_PP_REPEAT_DEF_VA_ARG0(MACRO, ...)
#define KAGUYA_PP_REPEAT_DEF_VA_ARG1(MACRO, ...)                               \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG0(MACRO, __VA_ARGS__)               \
                    MACRO(1, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG2(MACRO, ...)                               \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG1(MACRO, __VA_ARGS__)               \
                    MACRO(2, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG3(MACRO, ...)                               \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG2(MACRO, __VA_ARGS__)               \
                    MACRO(3, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG4(MACRO, ...)                               \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG3(MACRO, __VA_ARGS__)               \
                    MACRO(4, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG5(MACRO, ...)                               \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG4(MACRO, __VA_ARGS__)               \
                    MACRO(5, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG6(MACRO, ...)                               \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG5(MACRO, __VA_ARGS__)               \
                    MACRO(6, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG7(MACRO, ...)                               \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG6(MACRO, __VA_ARGS__)               \
                    MACRO(7, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG8(MACRO, ...)                               \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG7(MACRO, __VA_ARGS__)               \
                    MACRO(8, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG9(MACRO, ...)                               \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG8(MACRO, __VA_ARGS__)               \
                    MACRO(9, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG10(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG9(MACRO, __VA_ARGS__)               \
                    MACRO(10, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG11(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG10(MACRO, __VA_ARGS__)              \
                    MACRO(11, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG12(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG11(MACRO, __VA_ARGS__)              \
                    MACRO(12, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG13(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG12(MACRO, __VA_ARGS__)              \
                    MACRO(13, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG14(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG13(MACRO, __VA_ARGS__)              \
                    MACRO(14, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG15(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG14(MACRO, __VA_ARGS__)              \
                    MACRO(15, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG16(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG15(MACRO, __VA_ARGS__)              \
                    MACRO(16, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG17(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG16(MACRO, __VA_ARGS__)              \
                    MACRO(17, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG18(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG17(MACRO, __VA_ARGS__)              \
                    MACRO(18, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG19(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG18(MACRO, __VA_ARGS__)              \
                    MACRO(19, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG20(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG19(MACRO, __VA_ARGS__)              \
                    MACRO(20, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG21(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG20(MACRO, __VA_ARGS__)              \
                    MACRO(21, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG22(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG21(MACRO, __VA_ARGS__)              \
                    MACRO(22, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG23(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG22(MACRO, __VA_ARGS__)              \
                    MACRO(23, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG24(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG23(MACRO, __VA_ARGS__)              \
                    MACRO(24, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG25(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG24(MACRO, __VA_ARGS__)              \
                    MACRO(25, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG26(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG25(MACRO, __VA_ARGS__)              \
                    MACRO(26, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG27(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG26(MACRO, __VA_ARGS__)              \
                    MACRO(27, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG28(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG27(MACRO, __VA_ARGS__)              \
                    MACRO(28, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG29(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG28(MACRO, __VA_ARGS__)              \
                    MACRO(29, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG30(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG29(MACRO, __VA_ARGS__)              \
                    MACRO(30, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG31(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG30(MACRO, __VA_ARGS__)              \
                    MACRO(31, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG32(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG31(MACRO, __VA_ARGS__)              \
                    MACRO(32, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG33(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG32(MACRO, __VA_ARGS__)              \
                    MACRO(33, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG34(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG33(MACRO, __VA_ARGS__)              \
                    MACRO(34, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG35(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG34(MACRO, __VA_ARGS__)              \
                    MACRO(35, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG36(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG35(MACRO, __VA_ARGS__)              \
                    MACRO(36, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG37(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG36(MACRO, __VA_ARGS__)              \
                    MACRO(37, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG38(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG37(MACRO, __VA_ARGS__)              \
                    MACRO(38, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG39(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG38(MACRO, __VA_ARGS__)              \
                    MACRO(39, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG40(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG39(MACRO, __VA_ARGS__)              \
                    MACRO(40, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG41(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG40(MACRO, __VA_ARGS__)              \
                    MACRO(41, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG42(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG41(MACRO, __VA_ARGS__)              \
                    MACRO(42, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG43(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG42(MACRO, __VA_ARGS__)              \
                    MACRO(43, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG44(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG43(MACRO, __VA_ARGS__)              \
                    MACRO(44, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG45(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG44(MACRO, __VA_ARGS__)              \
                    MACRO(45, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG46(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG45(MACRO, __VA_ARGS__)              \
                    MACRO(46, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG47(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG46(MACRO, __VA_ARGS__)              \
                    MACRO(47, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG48(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG47(MACRO, __VA_ARGS__)              \
                    MACRO(48, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG49(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG48(MACRO, __VA_ARGS__)              \
                    MACRO(49, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG50(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG49(MACRO, __VA_ARGS__)              \
                    MACRO(50, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG51(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG50(MACRO, __VA_ARGS__)              \
                    MACRO(51, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG52(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG51(MACRO, __VA_ARGS__)              \
                    MACRO(52, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG53(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG52(MACRO, __VA_ARGS__)              \
                    MACRO(53, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG54(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG53(MACRO, __VA_ARGS__)              \
                    MACRO(54, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG55(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG54(MACRO, __VA_ARGS__)              \
                    MACRO(55, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG56(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG55(MACRO, __VA_ARGS__)              \
                    MACRO(56, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG57(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG56(MACRO, __VA_ARGS__)              \
                    MACRO(57, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG58(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG57(MACRO, __VA_ARGS__)              \
                    MACRO(58, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG59(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG58(MACRO, __VA_ARGS__)              \
                    MACRO(59, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG60(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG59(MACRO, __VA_ARGS__)              \
                    MACRO(60, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG61(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG60(MACRO, __VA_ARGS__)              \
                    MACRO(61, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG62(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG61(MACRO, __VA_ARGS__)              \
                    MACRO(62, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG63(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG62(MACRO, __VA_ARGS__)              \
                    MACRO(63, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG64(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG63(MACRO, __VA_ARGS__)              \
                    MACRO(64, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG65(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG64(MACRO, __VA_ARGS__)              \
                    MACRO(65, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG66(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG65(MACRO, __VA_ARGS__)              \
                    MACRO(66, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG67(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG66(MACRO, __VA_ARGS__)              \
                    MACRO(67, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG68(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG67(MACRO, __VA_ARGS__)              \
                    MACRO(68, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG69(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG68(MACRO, __VA_ARGS__)              \
                    MACRO(69, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG70(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG69(MACRO, __VA_ARGS__)              \
                    MACRO(70, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG71(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG70(MACRO, __VA_ARGS__)              \
                    MACRO(71, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG72(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG71(MACRO, __VA_ARGS__)              \
                    MACRO(72, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG73(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG72(MACRO, __VA_ARGS__)              \
                    MACRO(73, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG74(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG73(MACRO, __VA_ARGS__)              \
                    MACRO(74, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG75(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG74(MACRO, __VA_ARGS__)              \
                    MACRO(75, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG76(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG75(MACRO, __VA_ARGS__)              \
                    MACRO(76, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG77(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG76(MACRO, __VA_ARGS__)              \
                    MACRO(77, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG78(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG77(MACRO, __VA_ARGS__)              \
                    MACRO(78, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG79(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG78(MACRO, __VA_ARGS__)              \
                    MACRO(79, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG80(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG79(MACRO, __VA_ARGS__)              \
                    MACRO(80, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG81(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG80(MACRO, __VA_ARGS__)              \
                    MACRO(81, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG82(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG81(MACRO, __VA_ARGS__)              \
                    MACRO(82, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG83(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG82(MACRO, __VA_ARGS__)              \
                    MACRO(83, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG84(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG83(MACRO, __VA_ARGS__)              \
                    MACRO(84, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG85(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG84(MACRO, __VA_ARGS__)              \
                    MACRO(85, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG86(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG85(MACRO, __VA_ARGS__)              \
                    MACRO(86, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG87(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG86(MACRO, __VA_ARGS__)              \
                    MACRO(87, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG88(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG87(MACRO, __VA_ARGS__)              \
                    MACRO(88, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG89(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG88(MACRO, __VA_ARGS__)              \
                    MACRO(89, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG90(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG89(MACRO, __VA_ARGS__)              \
                    MACRO(90, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG91(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG90(MACRO, __VA_ARGS__)              \
                    MACRO(91, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG92(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG91(MACRO, __VA_ARGS__)              \
                    MACRO(92, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG93(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG92(MACRO, __VA_ARGS__)              \
                    MACRO(93, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG94(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG93(MACRO, __VA_ARGS__)              \
                    MACRO(94, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG95(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG94(MACRO, __VA_ARGS__)              \
                    MACRO(95, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG96(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG95(MACRO, __VA_ARGS__)              \
                    MACRO(96, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG97(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG96(MACRO, __VA_ARGS__)              \
                    MACRO(97, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG98(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG97(MACRO, __VA_ARGS__)              \
                    MACRO(98, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG99(MACRO, ...)                              \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG98(MACRO, __VA_ARGS__)              \
                    MACRO(99, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG100(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG99(MACRO, __VA_ARGS__)              \
                    MACRO(100, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG101(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG100(MACRO, __VA_ARGS__)             \
                    MACRO(101, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG102(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG101(MACRO, __VA_ARGS__)             \
                    MACRO(102, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG103(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG102(MACRO, __VA_ARGS__)             \
                    MACRO(103, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG104(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG103(MACRO, __VA_ARGS__)             \
                    MACRO(104, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG105(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG104(MACRO, __VA_ARGS__)             \
                    MACRO(105, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG106(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG105(MACRO, __VA_ARGS__)             \
                    MACRO(106, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG107(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG106(MACRO, __VA_ARGS__)             \
                    MACRO(107, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG108(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG107(MACRO, __VA_ARGS__)             \
                    MACRO(108, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG109(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG108(MACRO, __VA_ARGS__)             \
                    MACRO(109, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG110(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG109(MACRO, __VA_ARGS__)             \
                    MACRO(110, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG111(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG110(MACRO, __VA_ARGS__)             \
                    MACRO(111, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG112(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG111(MACRO, __VA_ARGS__)             \
                    MACRO(112, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG113(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG112(MACRO, __VA_ARGS__)             \
                    MACRO(113, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG114(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG113(MACRO, __VA_ARGS__)             \
                    MACRO(114, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG115(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG114(MACRO, __VA_ARGS__)             \
                    MACRO(115, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG116(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG115(MACRO, __VA_ARGS__)             \
                    MACRO(116, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG117(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG116(MACRO, __VA_ARGS__)             \
                    MACRO(117, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG118(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG117(MACRO, __VA_ARGS__)             \
                    MACRO(118, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG119(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG118(MACRO, __VA_ARGS__)             \
                    MACRO(119, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG120(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG119(MACRO, __VA_ARGS__)             \
                    MACRO(120, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG121(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG120(MACRO, __VA_ARGS__)             \
                    MACRO(121, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG122(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG121(MACRO, __VA_ARGS__)             \
                    MACRO(122, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG123(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG122(MACRO, __VA_ARGS__)             \
                    MACRO(123, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG124(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG123(MACRO, __VA_ARGS__)             \
                    MACRO(124, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG125(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG124(MACRO, __VA_ARGS__)             \
                    MACRO(125, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG126(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG125(MACRO, __VA_ARGS__)             \
                    MACRO(126, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG127(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG126(MACRO, __VA_ARGS__)             \
                    MACRO(127, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG128(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG127(MACRO, __VA_ARGS__)             \
                    MACRO(128, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG129(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG128(MACRO, __VA_ARGS__)             \
                    MACRO(129, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG130(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG129(MACRO, __VA_ARGS__)             \
                    MACRO(130, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG131(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG130(MACRO, __VA_ARGS__)             \
                    MACRO(131, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG132(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG131(MACRO, __VA_ARGS__)             \
                    MACRO(132, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG133(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG132(MACRO, __VA_ARGS__)             \
                    MACRO(133, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG134(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG133(MACRO, __VA_ARGS__)             \
                    MACRO(134, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG135(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG134(MACRO, __VA_ARGS__)             \
                    MACRO(135, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG136(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG135(MACRO, __VA_ARGS__)             \
                    MACRO(136, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG137(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG136(MACRO, __VA_ARGS__)             \
                    MACRO(137, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG138(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG137(MACRO, __VA_ARGS__)             \
                    MACRO(138, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG139(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG138(MACRO, __VA_ARGS__)             \
                    MACRO(139, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG140(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG139(MACRO, __VA_ARGS__)             \
                    MACRO(140, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG141(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG140(MACRO, __VA_ARGS__)             \
                    MACRO(141, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG142(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG141(MACRO, __VA_ARGS__)             \
                    MACRO(142, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG143(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG142(MACRO, __VA_ARGS__)             \
                    MACRO(143, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG144(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG143(MACRO, __VA_ARGS__)             \
                    MACRO(144, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG145(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG144(MACRO, __VA_ARGS__)             \
                    MACRO(145, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG146(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG145(MACRO, __VA_ARGS__)             \
                    MACRO(146, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG147(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG146(MACRO, __VA_ARGS__)             \
                    MACRO(147, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG148(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG147(MACRO, __VA_ARGS__)             \
                    MACRO(148, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG149(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG148(MACRO, __VA_ARGS__)             \
                    MACRO(149, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG150(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG149(MACRO, __VA_ARGS__)             \
                    MACRO(150, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG151(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG150(MACRO, __VA_ARGS__)             \
                    MACRO(151, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG152(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG151(MACRO, __VA_ARGS__)             \
                    MACRO(152, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG153(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG152(MACRO, __VA_ARGS__)             \
                    MACRO(153, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG154(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG153(MACRO, __VA_ARGS__)             \
                    MACRO(154, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG155(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG154(MACRO, __VA_ARGS__)             \
                    MACRO(155, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG156(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG155(MACRO, __VA_ARGS__)             \
                    MACRO(156, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG157(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG156(MACRO, __VA_ARGS__)             \
                    MACRO(157, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG158(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG157(MACRO, __VA_ARGS__)             \
                    MACRO(158, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG159(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG158(MACRO, __VA_ARGS__)             \
                    MACRO(159, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG160(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG159(MACRO, __VA_ARGS__)             \
                    MACRO(160, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG161(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG160(MACRO, __VA_ARGS__)             \
                    MACRO(161, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG162(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG161(MACRO, __VA_ARGS__)             \
                    MACRO(162, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG163(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG162(MACRO, __VA_ARGS__)             \
                    MACRO(163, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG164(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG163(MACRO, __VA_ARGS__)             \
                    MACRO(164, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG165(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG164(MACRO, __VA_ARGS__)             \
                    MACRO(165, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG166(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG165(MACRO, __VA_ARGS__)             \
                    MACRO(166, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG167(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG166(MACRO, __VA_ARGS__)             \
                    MACRO(167, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG168(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG167(MACRO, __VA_ARGS__)             \
                    MACRO(168, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG169(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG168(MACRO, __VA_ARGS__)             \
                    MACRO(169, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG170(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG169(MACRO, __VA_ARGS__)             \
                    MACRO(170, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG171(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG170(MACRO, __VA_ARGS__)             \
                    MACRO(171, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG172(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG171(MACRO, __VA_ARGS__)             \
                    MACRO(172, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG173(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG172(MACRO, __VA_ARGS__)             \
                    MACRO(173, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG174(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG173(MACRO, __VA_ARGS__)             \
                    MACRO(174, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG175(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG174(MACRO, __VA_ARGS__)             \
                    MACRO(175, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG176(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG175(MACRO, __VA_ARGS__)             \
                    MACRO(176, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG177(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG176(MACRO, __VA_ARGS__)             \
                    MACRO(177, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG178(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG177(MACRO, __VA_ARGS__)             \
                    MACRO(178, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG179(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG178(MACRO, __VA_ARGS__)             \
                    MACRO(179, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG180(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG179(MACRO, __VA_ARGS__)             \
                    MACRO(180, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG181(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG180(MACRO, __VA_ARGS__)             \
                    MACRO(181, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG182(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG181(MACRO, __VA_ARGS__)             \
                    MACRO(182, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG183(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG182(MACRO, __VA_ARGS__)             \
                    MACRO(183, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG184(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG183(MACRO, __VA_ARGS__)             \
                    MACRO(184, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG185(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG184(MACRO, __VA_ARGS__)             \
                    MACRO(185, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG186(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG185(MACRO, __VA_ARGS__)             \
                    MACRO(186, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG187(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG186(MACRO, __VA_ARGS__)             \
                    MACRO(187, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG188(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG187(MACRO, __VA_ARGS__)             \
                    MACRO(188, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG189(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG188(MACRO, __VA_ARGS__)             \
                    MACRO(189, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG190(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG189(MACRO, __VA_ARGS__)             \
                    MACRO(190, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG191(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG190(MACRO, __VA_ARGS__)             \
                    MACRO(191, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG192(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG191(MACRO, __VA_ARGS__)             \
                    MACRO(192, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG193(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG192(MACRO, __VA_ARGS__)             \
                    MACRO(193, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG194(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG193(MACRO, __VA_ARGS__)             \
                    MACRO(194, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG195(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG194(MACRO, __VA_ARGS__)             \
                    MACRO(195, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG196(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG195(MACRO, __VA_ARGS__)             \
                    MACRO(196, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG197(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG196(MACRO, __VA_ARGS__)             \
                    MACRO(197, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG198(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG197(MACRO, __VA_ARGS__)             \
                    MACRO(198, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG199(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG198(MACRO, __VA_ARGS__)             \
                    MACRO(199, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG200(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG199(MACRO, __VA_ARGS__)             \
                    MACRO(200, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG201(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG200(MACRO, __VA_ARGS__)             \
                    MACRO(201, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG202(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG201(MACRO, __VA_ARGS__)             \
                    MACRO(202, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG203(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG202(MACRO, __VA_ARGS__)             \
                    MACRO(203, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG204(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG203(MACRO, __VA_ARGS__)             \
                    MACRO(204, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG205(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG204(MACRO, __VA_ARGS__)             \
                    MACRO(205, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG206(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG205(MACRO, __VA_ARGS__)             \
                    MACRO(206, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG207(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG206(MACRO, __VA_ARGS__)             \
                    MACRO(207, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG208(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG207(MACRO, __VA_ARGS__)             \
                    MACRO(208, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG209(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG208(MACRO, __VA_ARGS__)             \
                    MACRO(209, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG210(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG209(MACRO, __VA_ARGS__)             \
                    MACRO(210, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG211(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG210(MACRO, __VA_ARGS__)             \
                    MACRO(211, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG212(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG211(MACRO, __VA_ARGS__)             \
                    MACRO(212, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG213(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG212(MACRO, __VA_ARGS__)             \
                    MACRO(213, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG214(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG213(MACRO, __VA_ARGS__)             \
                    MACRO(214, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG215(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG214(MACRO, __VA_ARGS__)             \
                    MACRO(215, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG216(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG215(MACRO, __VA_ARGS__)             \
                    MACRO(216, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG217(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG216(MACRO, __VA_ARGS__)             \
                    MACRO(217, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG218(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG217(MACRO, __VA_ARGS__)             \
                    MACRO(218, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG219(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG218(MACRO, __VA_ARGS__)             \
                    MACRO(219, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG220(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG219(MACRO, __VA_ARGS__)             \
                    MACRO(220, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG221(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG220(MACRO, __VA_ARGS__)             \
                    MACRO(221, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG222(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG221(MACRO, __VA_ARGS__)             \
                    MACRO(222, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG223(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG222(MACRO, __VA_ARGS__)             \
                    MACRO(223, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG224(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG223(MACRO, __VA_ARGS__)             \
                    MACRO(224, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG225(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG224(MACRO, __VA_ARGS__)             \
                    MACRO(225, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG226(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG225(MACRO, __VA_ARGS__)             \
                    MACRO(226, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG227(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG226(MACRO, __VA_ARGS__)             \
                    MACRO(227, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG228(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG227(MACRO, __VA_ARGS__)             \
                    MACRO(228, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG229(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG228(MACRO, __VA_ARGS__)             \
                    MACRO(229, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG230(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG229(MACRO, __VA_ARGS__)             \
                    MACRO(230, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG231(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG230(MACRO, __VA_ARGS__)             \
                    MACRO(231, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG232(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG231(MACRO, __VA_ARGS__)             \
                    MACRO(232, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG233(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG232(MACRO, __VA_ARGS__)             \
                    MACRO(233, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG234(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG233(MACRO, __VA_ARGS__)             \
                    MACRO(234, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG235(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG234(MACRO, __VA_ARGS__)             \
                    MACRO(235, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG236(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG235(MACRO, __VA_ARGS__)             \
                    MACRO(236, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG237(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG236(MACRO, __VA_ARGS__)             \
                    MACRO(237, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG238(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG237(MACRO, __VA_ARGS__)             \
                    MACRO(238, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG239(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG238(MACRO, __VA_ARGS__)             \
                    MACRO(239, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG240(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG239(MACRO, __VA_ARGS__)             \
                    MACRO(240, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG241(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG240(MACRO, __VA_ARGS__)             \
                    MACRO(241, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG242(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG241(MACRO, __VA_ARGS__)             \
                    MACRO(242, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG243(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG242(MACRO, __VA_ARGS__)             \
                    MACRO(243, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG244(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG243(MACRO, __VA_ARGS__)             \
                    MACRO(244, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG245(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG244(MACRO, __VA_ARGS__)             \
                    MACRO(245, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG246(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG245(MACRO, __VA_ARGS__)             \
                    MACRO(246, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG247(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG246(MACRO, __VA_ARGS__)             \
                    MACRO(247, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG248(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG247(MACRO, __VA_ARGS__)             \
                    MACRO(248, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG249(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG248(MACRO, __VA_ARGS__)             \
                    MACRO(249, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG250(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG249(MACRO, __VA_ARGS__)             \
                    MACRO(250, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG251(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG250(MACRO, __VA_ARGS__)             \
                    MACRO(251, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG252(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG251(MACRO, __VA_ARGS__)             \
                    MACRO(252, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG253(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG252(MACRO, __VA_ARGS__)             \
                    MACRO(253, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG254(MACRO, ...)                             \
  KAGUYA_VA_ARG(KAGUYA_PP_REPEAT_DEF_VA_ARG253(MACRO, __VA_ARGS__)             \
                    MACRO(254, __VA_ARGS__))
#define KAGUYA_PP_REPEAT_DEF_VA_ARG(COUNT, MACRO, ...)                         \
  KAGUYA_VA_ARG(                                                               \
      KAGUYA_PP_CAT(KAGUYA_PP_REPEAT_DEF_VA_ARG, COUNT)(MACRO, __VA_ARGS__))

#define KAGUYA_PP_WHILE0(MACRO, R) R
#define KAGUYA_PP_WHILE1(MACRO, R) MACRO(KAGUYA_PP_WHILE0(MACRO, R))
#define KAGUYA_PP_WHILE2(MACRO, R) MACRO(KAGUYA_PP_WHILE1(MACRO, R))
#define KAGUYA_PP_WHILE3(MACRO, R) MACRO(KAGUYA_PP_WHILE2(MACRO, R))
#define KAGUYA_PP_WHILE4(MACRO, R) MACRO(KAGUYA_PP_WHILE3(MACRO, R))
#define KAGUYA_PP_WHILE5(MACRO, R) MACRO(KAGUYA_PP_WHILE4(MACRO, R))
#define KAGUYA_PP_WHILE6(MACRO, R) MACRO(KAGUYA_PP_WHILE5(MACRO, R))
#define KAGUYA_PP_WHILE7(MACRO, R) MACRO(KAGUYA_PP_WHILE6(MACRO, R))
#define KAGUYA_PP_WHILE8(MACRO, R) MACRO(KAGUYA_PP_WHILE7(MACRO, R))
#define KAGUYA_PP_WHILE9(MACRO, R) MACRO(KAGUYA_PP_WHILE8(MACRO, R))
#define KAGUYA_PP_WHILE10(MACRO, R) MACRO(KAGUYA_PP_WHILE9(MACRO, R))
#define KAGUYA_PP_WHILE11(MACRO, R) MACRO(KAGUYA_PP_WHILE10(MACRO, R))
#define KAGUYA_PP_WHILE12(MACRO, R) MACRO(KAGUYA_PP_WHILE11(MACRO, R))
#define KAGUYA_PP_WHILE13(MACRO, R) MACRO(KAGUYA_PP_WHILE12(MACRO, R))
#define KAGUYA_PP_WHILE14(MACRO, R) MACRO(KAGUYA_PP_WHILE13(MACRO, R))
#define KAGUYA_PP_WHILE15(MACRO, R) MACRO(KAGUYA_PP_WHILE14(MACRO, R))
#define KAGUYA_PP_WHILE16(MACRO, R) MACRO(KAGUYA_PP_WHILE15(MACRO, R))
#define KAGUYA_PP_WHILE17(MACRO, R) MACRO(KAGUYA_PP_WHILE16(MACRO, R))
#define KAGUYA_PP_WHILE18(MACRO, R) MACRO(KAGUYA_PP_WHILE17(MACRO, R))
#define KAGUYA_PP_WHILE19(MACRO, R) MACRO(KAGUYA_PP_WHILE18(MACRO, R))
#define KAGUYA_PP_WHILE20(MACRO, R) MACRO(KAGUYA_PP_WHILE19(MACRO, R))
#define KAGUYA_PP_WHILE21(MACRO, R) MACRO(KAGUYA_PP_WHILE20(MACRO, R))
#define KAGUYA_PP_WHILE22(MACRO, R) MACRO(KAGUYA_PP_WHILE21(MACRO, R))
#define KAGUYA_PP_WHILE23(MACRO, R) MACRO(KAGUYA_PP_WHILE22(MACRO, R))
#define KAGUYA_PP_WHILE24(MACRO, R) MACRO(KAGUYA_PP_WHILE23(MACRO, R))
#define KAGUYA_PP_WHILE25(MACRO, R) MACRO(KAGUYA_PP_WHILE24(MACRO, R))
#define KAGUYA_PP_WHILE26(MACRO, R) MACRO(KAGUYA_PP_WHILE25(MACRO, R))
#define KAGUYA_PP_WHILE27(MACRO, R) MACRO(KAGUYA_PP_WHILE26(MACRO, R))
#define KAGUYA_PP_WHILE28(MACRO, R) MACRO(KAGUYA_PP_WHILE27(MACRO, R))
#define KAGUYA_PP_WHILE29(MACRO, R) MACRO(KAGUYA_PP_WHILE28(MACRO, R))
#define KAGUYA_PP_WHILE30(MACRO, R) MACRO(KAGUYA_PP_WHILE29(MACRO, R))
#define KAGUYA_PP_WHILE31(MACRO, R) MACRO(KAGUYA_PP_WHILE30(MACRO, R))
#define KAGUYA_PP_WHILE32(MACRO, R) MACRO(KAGUYA_PP_WHILE31(MACRO, R))
#define KAGUYA_PP_WHILE33(MACRO, R) MACRO(KAGUYA_PP_WHILE32(MACRO, R))
#define KAGUYA_PP_WHILE34(MACRO, R) MACRO(KAGUYA_PP_WHILE33(MACRO, R))
#define KAGUYA_PP_WHILE35(MACRO, R) MACRO(KAGUYA_PP_WHILE34(MACRO, R))
#define KAGUYA_PP_WHILE36(MACRO, R) MACRO(KAGUYA_PP_WHILE35(MACRO, R))
#define KAGUYA_PP_WHILE37(MACRO, R) MACRO(KAGUYA_PP_WHILE36(MACRO, R))
#define KAGUYA_PP_WHILE38(MACRO, R) MACRO(KAGUYA_PP_WHILE37(MACRO, R))
#define KAGUYA_PP_WHILE39(MACRO, R) MACRO(KAGUYA_PP_WHILE38(MACRO, R))
#define KAGUYA_PP_WHILE40(MACRO, R) MACRO(KAGUYA_PP_WHILE39(MACRO, R))
#define KAGUYA_PP_WHILE41(MACRO, R) MACRO(KAGUYA_PP_WHILE40(MACRO, R))
#define KAGUYA_PP_WHILE42(MACRO, R) MACRO(KAGUYA_PP_WHILE41(MACRO, R))
#define KAGUYA_PP_WHILE43(MACRO, R) MACRO(KAGUYA_PP_WHILE42(MACRO, R))
#define KAGUYA_PP_WHILE44(MACRO, R) MACRO(KAGUYA_PP_WHILE43(MACRO, R))
#define KAGUYA_PP_WHILE45(MACRO, R) MACRO(KAGUYA_PP_WHILE44(MACRO, R))
#define KAGUYA_PP_WHILE46(MACRO, R) MACRO(KAGUYA_PP_WHILE45(MACRO, R))
#define KAGUYA_PP_WHILE47(MACRO, R) MACRO(KAGUYA_PP_WHILE46(MACRO, R))
#define KAGUYA_PP_WHILE48(MACRO, R) MACRO(KAGUYA_PP_WHILE47(MACRO, R))
#define KAGUYA_PP_WHILE49(MACRO, R) MACRO(KAGUYA_PP_WHILE48(MACRO, R))
#define KAGUYA_PP_WHILE50(MACRO, R) MACRO(KAGUYA_PP_WHILE49(MACRO, R))
#define KAGUYA_PP_WHILE51(MACRO, R) MACRO(KAGUYA_PP_WHILE50(MACRO, R))
#define KAGUYA_PP_WHILE52(MACRO, R) MACRO(KAGUYA_PP_WHILE51(MACRO, R))
#define KAGUYA_PP_WHILE53(MACRO, R) MACRO(KAGUYA_PP_WHILE52(MACRO, R))
#define KAGUYA_PP_WHILE54(MACRO, R) MACRO(KAGUYA_PP_WHILE53(MACRO, R))
#define KAGUYA_PP_WHILE55(MACRO, R) MACRO(KAGUYA_PP_WHILE54(MACRO, R))
#define KAGUYA_PP_WHILE56(MACRO, R) MACRO(KAGUYA_PP_WHILE55(MACRO, R))
#define KAGUYA_PP_WHILE57(MACRO, R) MACRO(KAGUYA_PP_WHILE56(MACRO, R))
#define KAGUYA_PP_WHILE58(MACRO, R) MACRO(KAGUYA_PP_WHILE57(MACRO, R))
#define KAGUYA_PP_WHILE59(MACRO, R) MACRO(KAGUYA_PP_WHILE58(MACRO, R))
#define KAGUYA_PP_WHILE60(MACRO, R) MACRO(KAGUYA_PP_WHILE59(MACRO, R))
#define KAGUYA_PP_WHILE61(MACRO, R) MACRO(KAGUYA_PP_WHILE60(MACRO, R))
#define KAGUYA_PP_WHILE62(MACRO, R) MACRO(KAGUYA_PP_WHILE61(MACRO, R))
#define KAGUYA_PP_WHILE63(MACRO, R) MACRO(KAGUYA_PP_WHILE62(MACRO, R))
#define KAGUYA_PP_WHILE64(MACRO, R) MACRO(KAGUYA_PP_WHILE63(MACRO, R))
#define KAGUYA_PP_WHILE65(MACRO, R) MACRO(KAGUYA_PP_WHILE64(MACRO, R))
#define KAGUYA_PP_WHILE66(MACRO, R) MACRO(KAGUYA_PP_WHILE65(MACRO, R))
#define KAGUYA_PP_WHILE67(MACRO, R) MACRO(KAGUYA_PP_WHILE66(MACRO, R))
#define KAGUYA_PP_WHILE68(MACRO, R) MACRO(KAGUYA_PP_WHILE67(MACRO, R))
#define KAGUYA_PP_WHILE69(MACRO, R) MACRO(KAGUYA_PP_WHILE68(MACRO, R))
#define KAGUYA_PP_WHILE70(MACRO, R) MACRO(KAGUYA_PP_WHILE69(MACRO, R))
#define KAGUYA_PP_WHILE71(MACRO, R) MACRO(KAGUYA_PP_WHILE70(MACRO, R))
#define KAGUYA_PP_WHILE72(MACRO, R) MACRO(KAGUYA_PP_WHILE71(MACRO, R))
#define KAGUYA_PP_WHILE73(MACRO, R) MACRO(KAGUYA_PP_WHILE72(MACRO, R))
#define KAGUYA_PP_WHILE74(MACRO, R) MACRO(KAGUYA_PP_WHILE73(MACRO, R))
#define KAGUYA_PP_WHILE75(MACRO, R) MACRO(KAGUYA_PP_WHILE74(MACRO, R))
#define KAGUYA_PP_WHILE76(MACRO, R) MACRO(KAGUYA_PP_WHILE75(MACRO, R))
#define KAGUYA_PP_WHILE77(MACRO, R) MACRO(KAGUYA_PP_WHILE76(MACRO, R))
#define KAGUYA_PP_WHILE78(MACRO, R) MACRO(KAGUYA_PP_WHILE77(MACRO, R))
#define KAGUYA_PP_WHILE79(MACRO, R) MACRO(KAGUYA_PP_WHILE78(MACRO, R))
#define KAGUYA_PP_WHILE80(MACRO, R) MACRO(KAGUYA_PP_WHILE79(MACRO, R))
#define KAGUYA_PP_WHILE81(MACRO, R) MACRO(KAGUYA_PP_WHILE80(MACRO, R))
#define KAGUYA_PP_WHILE82(MACRO, R) MACRO(KAGUYA_PP_WHILE81(MACRO, R))
#define KAGUYA_PP_WHILE83(MACRO, R) MACRO(KAGUYA_PP_WHILE82(MACRO, R))
#define KAGUYA_PP_WHILE84(MACRO, R) MACRO(KAGUYA_PP_WHILE83(MACRO, R))
#define KAGUYA_PP_WHILE85(MACRO, R) MACRO(KAGUYA_PP_WHILE84(MACRO, R))
#define KAGUYA_PP_WHILE86(MACRO, R) MACRO(KAGUYA_PP_WHILE85(MACRO, R))
#define KAGUYA_PP_WHILE87(MACRO, R) MACRO(KAGUYA_PP_WHILE86(MACRO, R))
#define KAGUYA_PP_WHILE88(MACRO, R) MACRO(KAGUYA_PP_WHILE87(MACRO, R))
#define KAGUYA_PP_WHILE89(MACRO, R) MACRO(KAGUYA_PP_WHILE88(MACRO, R))
#define KAGUYA_PP_WHILE90(MACRO, R) MACRO(KAGUYA_PP_WHILE89(MACRO, R))
#define KAGUYA_PP_WHILE91(MACRO, R) MACRO(KAGUYA_PP_WHILE90(MACRO, R))
#define KAGUYA_PP_WHILE92(MACRO, R) MACRO(KAGUYA_PP_WHILE91(MACRO, R))
#define KAGUYA_PP_WHILE93(MACRO, R) MACRO(KAGUYA_PP_WHILE92(MACRO, R))
#define KAGUYA_PP_WHILE94(MACRO, R) MACRO(KAGUYA_PP_WHILE93(MACRO, R))
#define KAGUYA_PP_WHILE95(MACRO, R) MACRO(KAGUYA_PP_WHILE94(MACRO, R))
#define KAGUYA_PP_WHILE96(MACRO, R) MACRO(KAGUYA_PP_WHILE95(MACRO, R))
#define KAGUYA_PP_WHILE97(MACRO, R) MACRO(KAGUYA_PP_WHILE96(MACRO, R))
#define KAGUYA_PP_WHILE98(MACRO, R) MACRO(KAGUYA_PP_WHILE97(MACRO, R))
#define KAGUYA_PP_WHILE99(MACRO, R) MACRO(KAGUYA_PP_WHILE98(MACRO, R))
#define KAGUYA_PP_WHILE100(MACRO, R) MACRO(KAGUYA_PP_WHILE99(MACRO, R))
#define KAGUYA_PP_WHILE101(MACRO, R) MACRO(KAGUYA_PP_WHILE100(MACRO, R))
#define KAGUYA_PP_WHILE102(MACRO, R) MACRO(KAGUYA_PP_WHILE101(MACRO, R))
#define KAGUYA_PP_WHILE103(MACRO, R) MACRO(KAGUYA_PP_WHILE102(MACRO, R))
#define KAGUYA_PP_WHILE104(MACRO, R) MACRO(KAGUYA_PP_WHILE103(MACRO, R))
#define KAGUYA_PP_WHILE105(MACRO, R) MACRO(KAGUYA_PP_WHILE104(MACRO, R))
#define KAGUYA_PP_WHILE106(MACRO, R) MACRO(KAGUYA_PP_WHILE105(MACRO, R))
#define KAGUYA_PP_WHILE107(MACRO, R) MACRO(KAGUYA_PP_WHILE106(MACRO, R))
#define KAGUYA_PP_WHILE108(MACRO, R) MACRO(KAGUYA_PP_WHILE107(MACRO, R))
#define KAGUYA_PP_WHILE109(MACRO, R) MACRO(KAGUYA_PP_WHILE108(MACRO, R))
#define KAGUYA_PP_WHILE110(MACRO, R) MACRO(KAGUYA_PP_WHILE109(MACRO, R))
#define KAGUYA_PP_WHILE111(MACRO, R) MACRO(KAGUYA_PP_WHILE110(MACRO, R))
#define KAGUYA_PP_WHILE112(MACRO, R) MACRO(KAGUYA_PP_WHILE111(MACRO, R))
#define KAGUYA_PP_WHILE113(MACRO, R) MACRO(KAGUYA_PP_WHILE112(MACRO, R))
#define KAGUYA_PP_WHILE114(MACRO, R) MACRO(KAGUYA_PP_WHILE113(MACRO, R))
#define KAGUYA_PP_WHILE115(MACRO, R) MACRO(KAGUYA_PP_WHILE114(MACRO, R))
#define KAGUYA_PP_WHILE116(MACRO, R) MACRO(KAGUYA_PP_WHILE115(MACRO, R))
#define KAGUYA_PP_WHILE117(MACRO, R) MACRO(KAGUYA_PP_WHILE116(MACRO, R))
#define KAGUYA_PP_WHILE118(MACRO, R) MACRO(KAGUYA_PP_WHILE117(MACRO, R))
#define KAGUYA_PP_WHILE119(MACRO, R) MACRO(KAGUYA_PP_WHILE118(MACRO, R))
#define KAGUYA_PP_WHILE120(MACRO, R) MACRO(KAGUYA_PP_WHILE119(MACRO, R))
#define KAGUYA_PP_WHILE121(MACRO, R) MACRO(KAGUYA_PP_WHILE120(MACRO, R))
#define KAGUYA_PP_WHILE122(MACRO, R) MACRO(KAGUYA_PP_WHILE121(MACRO, R))
#define KAGUYA_PP_WHILE123(MACRO, R) MACRO(KAGUYA_PP_WHILE122(MACRO, R))
#define KAGUYA_PP_WHILE124(MACRO, R) MACRO(KAGUYA_PP_WHILE123(MACRO, R))
#define KAGUYA_PP_WHILE125(MACRO, R) MACRO(KAGUYA_PP_WHILE124(MACRO, R))
#define KAGUYA_PP_WHILE126(MACRO, R) MACRO(KAGUYA_PP_WHILE125(MACRO, R))
#define KAGUYA_PP_WHILE127(MACRO, R) MACRO(KAGUYA_PP_WHILE126(MACRO, R))
#define KAGUYA_PP_WHILE128(MACRO, R) MACRO(KAGUYA_PP_WHILE127(MACRO, R))
#define KAGUYA_PP_WHILE129(MACRO, R) MACRO(KAGUYA_PP_WHILE128(MACRO, R))
#define KAGUYA_PP_WHILE130(MACRO, R) MACRO(KAGUYA_PP_WHILE129(MACRO, R))
#define KAGUYA_PP_WHILE131(MACRO, R) MACRO(KAGUYA_PP_WHILE130(MACRO, R))
#define KAGUYA_PP_WHILE132(MACRO, R) MACRO(KAGUYA_PP_WHILE131(MACRO, R))
#define KAGUYA_PP_WHILE133(MACRO, R) MACRO(KAGUYA_PP_WHILE132(MACRO, R))
#define KAGUYA_PP_WHILE134(MACRO, R) MACRO(KAGUYA_PP_WHILE133(MACRO, R))
#define KAGUYA_PP_WHILE135(MACRO, R) MACRO(KAGUYA_PP_WHILE134(MACRO, R))
#define KAGUYA_PP_WHILE136(MACRO, R) MACRO(KAGUYA_PP_WHILE135(MACRO, R))
#define KAGUYA_PP_WHILE137(MACRO, R) MACRO(KAGUYA_PP_WHILE136(MACRO, R))
#define KAGUYA_PP_WHILE138(MACRO, R) MACRO(KAGUYA_PP_WHILE137(MACRO, R))
#define KAGUYA_PP_WHILE139(MACRO, R) MACRO(KAGUYA_PP_WHILE138(MACRO, R))
#define KAGUYA_PP_WHILE140(MACRO, R) MACRO(KAGUYA_PP_WHILE139(MACRO, R))
#define KAGUYA_PP_WHILE141(MACRO, R) MACRO(KAGUYA_PP_WHILE140(MACRO, R))
#define KAGUYA_PP_WHILE142(MACRO, R) MACRO(KAGUYA_PP_WHILE141(MACRO, R))
#define KAGUYA_PP_WHILE143(MACRO, R) MACRO(KAGUYA_PP_WHILE142(MACRO, R))
#define KAGUYA_PP_WHILE144(MACRO, R) MACRO(KAGUYA_PP_WHILE143(MACRO, R))
#define KAGUYA_PP_WHILE145(MACRO, R) MACRO(KAGUYA_PP_WHILE144(MACRO, R))
#define KAGUYA_PP_WHILE146(MACRO, R) MACRO(KAGUYA_PP_WHILE145(MACRO, R))
#define KAGUYA_PP_WHILE147(MACRO, R) MACRO(KAGUYA_PP_WHILE146(MACRO, R))
#define KAGUYA_PP_WHILE148(MACRO, R) MACRO(KAGUYA_PP_WHILE147(MACRO, R))
#define KAGUYA_PP_WHILE149(MACRO, R) MACRO(KAGUYA_PP_WHILE148(MACRO, R))
#define KAGUYA_PP_WHILE150(MACRO, R) MACRO(KAGUYA_PP_WHILE149(MACRO, R))
#define KAGUYA_PP_WHILE151(MACRO, R) MACRO(KAGUYA_PP_WHILE150(MACRO, R))
#define KAGUYA_PP_WHILE152(MACRO, R) MACRO(KAGUYA_PP_WHILE151(MACRO, R))
#define KAGUYA_PP_WHILE153(MACRO, R) MACRO(KAGUYA_PP_WHILE152(MACRO, R))
#define KAGUYA_PP_WHILE154(MACRO, R) MACRO(KAGUYA_PP_WHILE153(MACRO, R))
#define KAGUYA_PP_WHILE155(MACRO, R) MACRO(KAGUYA_PP_WHILE154(MACRO, R))
#define KAGUYA_PP_WHILE156(MACRO, R) MACRO(KAGUYA_PP_WHILE155(MACRO, R))
#define KAGUYA_PP_WHILE157(MACRO, R) MACRO(KAGUYA_PP_WHILE156(MACRO, R))
#define KAGUYA_PP_WHILE158(MACRO, R) MACRO(KAGUYA_PP_WHILE157(MACRO, R))
#define KAGUYA_PP_WHILE159(MACRO, R) MACRO(KAGUYA_PP_WHILE158(MACRO, R))
#define KAGUYA_PP_WHILE160(MACRO, R) MACRO(KAGUYA_PP_WHILE159(MACRO, R))
#define KAGUYA_PP_WHILE161(MACRO, R) MACRO(KAGUYA_PP_WHILE160(MACRO, R))
#define KAGUYA_PP_WHILE162(MACRO, R) MACRO(KAGUYA_PP_WHILE161(MACRO, R))
#define KAGUYA_PP_WHILE163(MACRO, R) MACRO(KAGUYA_PP_WHILE162(MACRO, R))
#define KAGUYA_PP_WHILE164(MACRO, R) MACRO(KAGUYA_PP_WHILE163(MACRO, R))
#define KAGUYA_PP_WHILE165(MACRO, R) MACRO(KAGUYA_PP_WHILE164(MACRO, R))
#define KAGUYA_PP_WHILE166(MACRO, R) MACRO(KAGUYA_PP_WHILE165(MACRO, R))
#define KAGUYA_PP_WHILE167(MACRO, R) MACRO(KAGUYA_PP_WHILE166(MACRO, R))
#define KAGUYA_PP_WHILE168(MACRO, R) MACRO(KAGUYA_PP_WHILE167(MACRO, R))
#define KAGUYA_PP_WHILE169(MACRO, R) MACRO(KAGUYA_PP_WHILE168(MACRO, R))
#define KAGUYA_PP_WHILE170(MACRO, R) MACRO(KAGUYA_PP_WHILE169(MACRO, R))
#define KAGUYA_PP_WHILE171(MACRO, R) MACRO(KAGUYA_PP_WHILE170(MACRO, R))
#define KAGUYA_PP_WHILE172(MACRO, R) MACRO(KAGUYA_PP_WHILE171(MACRO, R))
#define KAGUYA_PP_WHILE173(MACRO, R) MACRO(KAGUYA_PP_WHILE172(MACRO, R))
#define KAGUYA_PP_WHILE174(MACRO, R) MACRO(KAGUYA_PP_WHILE173(MACRO, R))
#define KAGUYA_PP_WHILE175(MACRO, R) MACRO(KAGUYA_PP_WHILE174(MACRO, R))
#define KAGUYA_PP_WHILE176(MACRO, R) MACRO(KAGUYA_PP_WHILE175(MACRO, R))
#define KAGUYA_PP_WHILE177(MACRO, R) MACRO(KAGUYA_PP_WHILE176(MACRO, R))
#define KAGUYA_PP_WHILE178(MACRO, R) MACRO(KAGUYA_PP_WHILE177(MACRO, R))
#define KAGUYA_PP_WHILE179(MACRO, R) MACRO(KAGUYA_PP_WHILE178(MACRO, R))
#define KAGUYA_PP_WHILE180(MACRO, R) MACRO(KAGUYA_PP_WHILE179(MACRO, R))
#define KAGUYA_PP_WHILE181(MACRO, R) MACRO(KAGUYA_PP_WHILE180(MACRO, R))
#define KAGUYA_PP_WHILE182(MACRO, R) MACRO(KAGUYA_PP_WHILE181(MACRO, R))
#define KAGUYA_PP_WHILE183(MACRO, R) MACRO(KAGUYA_PP_WHILE182(MACRO, R))
#define KAGUYA_PP_WHILE184(MACRO, R) MACRO(KAGUYA_PP_WHILE183(MACRO, R))
#define KAGUYA_PP_WHILE185(MACRO, R) MACRO(KAGUYA_PP_WHILE184(MACRO, R))
#define KAGUYA_PP_WHILE186(MACRO, R) MACRO(KAGUYA_PP_WHILE185(MACRO, R))
#define KAGUYA_PP_WHILE187(MACRO, R) MACRO(KAGUYA_PP_WHILE186(MACRO, R))
#define KAGUYA_PP_WHILE188(MACRO, R) MACRO(KAGUYA_PP_WHILE187(MACRO, R))
#define KAGUYA_PP_WHILE189(MACRO, R) MACRO(KAGUYA_PP_WHILE188(MACRO, R))
#define KAGUYA_PP_WHILE190(MACRO, R) MACRO(KAGUYA_PP_WHILE189(MACRO, R))
#define KAGUYA_PP_WHILE191(MACRO, R) MACRO(KAGUYA_PP_WHILE190(MACRO, R))
#define KAGUYA_PP_WHILE192(MACRO, R) MACRO(KAGUYA_PP_WHILE191(MACRO, R))
#define KAGUYA_PP_WHILE193(MACRO, R) MACRO(KAGUYA_PP_WHILE192(MACRO, R))
#define KAGUYA_PP_WHILE194(MACRO, R) MACRO(KAGUYA_PP_WHILE193(MACRO, R))
#define KAGUYA_PP_WHILE195(MACRO, R) MACRO(KAGUYA_PP_WHILE194(MACRO, R))
#define KAGUYA_PP_WHILE196(MACRO, R) MACRO(KAGUYA_PP_WHILE195(MACRO, R))
#define KAGUYA_PP_WHILE197(MACRO, R) MACRO(KAGUYA_PP_WHILE196(MACRO, R))
#define KAGUYA_PP_WHILE198(MACRO, R) MACRO(KAGUYA_PP_WHILE197(MACRO, R))
#define KAGUYA_PP_WHILE199(MACRO, R) MACRO(KAGUYA_PP_WHILE198(MACRO, R))
#define KAGUYA_PP_WHILE200(MACRO, R) MACRO(KAGUYA_PP_WHILE199(MACRO, R))
#define KAGUYA_PP_WHILE201(MACRO, R) MACRO(KAGUYA_PP_WHILE200(MACRO, R))
#define KAGUYA_PP_WHILE202(MACRO, R) MACRO(KAGUYA_PP_WHILE201(MACRO, R))
#define KAGUYA_PP_WHILE203(MACRO, R) MACRO(KAGUYA_PP_WHILE202(MACRO, R))
#define KAGUYA_PP_WHILE204(MACRO, R) MACRO(KAGUYA_PP_WHILE203(MACRO, R))
#define KAGUYA_PP_WHILE205(MACRO, R) MACRO(KAGUYA_PP_WHILE204(MACRO, R))
#define KAGUYA_PP_WHILE206(MACRO, R) MACRO(KAGUYA_PP_WHILE205(MACRO, R))
#define KAGUYA_PP_WHILE207(MACRO, R) MACRO(KAGUYA_PP_WHILE206(MACRO, R))
#define KAGUYA_PP_WHILE208(MACRO, R) MACRO(KAGUYA_PP_WHILE207(MACRO, R))
#define KAGUYA_PP_WHILE209(MACRO, R) MACRO(KAGUYA_PP_WHILE208(MACRO, R))
#define KAGUYA_PP_WHILE210(MACRO, R) MACRO(KAGUYA_PP_WHILE209(MACRO, R))
#define KAGUYA_PP_WHILE211(MACRO, R) MACRO(KAGUYA_PP_WHILE210(MACRO, R))
#define KAGUYA_PP_WHILE212(MACRO, R) MACRO(KAGUYA_PP_WHILE211(MACRO, R))
#define KAGUYA_PP_WHILE213(MACRO, R) MACRO(KAGUYA_PP_WHILE212(MACRO, R))
#define KAGUYA_PP_WHILE214(MACRO, R) MACRO(KAGUYA_PP_WHILE213(MACRO, R))
#define KAGUYA_PP_WHILE215(MACRO, R) MACRO(KAGUYA_PP_WHILE214(MACRO, R))
#define KAGUYA_PP_WHILE216(MACRO, R) MACRO(KAGUYA_PP_WHILE215(MACRO, R))
#define KAGUYA_PP_WHILE217(MACRO, R) MACRO(KAGUYA_PP_WHILE216(MACRO, R))
#define KAGUYA_PP_WHILE218(MACRO, R) MACRO(KAGUYA_PP_WHILE217(MACRO, R))
#define KAGUYA_PP_WHILE219(MACRO, R) MACRO(KAGUYA_PP_WHILE218(MACRO, R))
#define KAGUYA_PP_WHILE220(MACRO, R) MACRO(KAGUYA_PP_WHILE219(MACRO, R))
#define KAGUYA_PP_WHILE221(MACRO, R) MACRO(KAGUYA_PP_WHILE220(MACRO, R))
#define KAGUYA_PP_WHILE222(MACRO, R) MACRO(KAGUYA_PP_WHILE221(MACRO, R))
#define KAGUYA_PP_WHILE223(MACRO, R) MACRO(KAGUYA_PP_WHILE222(MACRO, R))
#define KAGUYA_PP_WHILE224(MACRO, R) MACRO(KAGUYA_PP_WHILE223(MACRO, R))
#define KAGUYA_PP_WHILE225(MACRO, R) MACRO(KAGUYA_PP_WHILE224(MACRO, R))
#define KAGUYA_PP_WHILE226(MACRO, R) MACRO(KAGUYA_PP_WHILE225(MACRO, R))
#define KAGUYA_PP_WHILE227(MACRO, R) MACRO(KAGUYA_PP_WHILE226(MACRO, R))
#define KAGUYA_PP_WHILE228(MACRO, R) MACRO(KAGUYA_PP_WHILE227(MACRO, R))
#define KAGUYA_PP_WHILE229(MACRO, R) MACRO(KAGUYA_PP_WHILE228(MACRO, R))
#define KAGUYA_PP_WHILE230(MACRO, R) MACRO(KAGUYA_PP_WHILE229(MACRO, R))
#define KAGUYA_PP_WHILE231(MACRO, R) MACRO(KAGUYA_PP_WHILE230(MACRO, R))
#define KAGUYA_PP_WHILE232(MACRO, R) MACRO(KAGUYA_PP_WHILE231(MACRO, R))
#define KAGUYA_PP_WHILE233(MACRO, R) MACRO(KAGUYA_PP_WHILE232(MACRO, R))
#define KAGUYA_PP_WHILE234(MACRO, R) MACRO(KAGUYA_PP_WHILE233(MACRO, R))
#define KAGUYA_PP_WHILE235(MACRO, R) MACRO(KAGUYA_PP_WHILE234(MACRO, R))
#define KAGUYA_PP_WHILE236(MACRO, R) MACRO(KAGUYA_PP_WHILE235(MACRO, R))
#define KAGUYA_PP_WHILE237(MACRO, R) MACRO(KAGUYA_PP_WHILE236(MACRO, R))
#define KAGUYA_PP_WHILE238(MACRO, R) MACRO(KAGUYA_PP_WHILE237(MACRO, R))
#define KAGUYA_PP_WHILE239(MACRO, R) MACRO(KAGUYA_PP_WHILE238(MACRO, R))
#define KAGUYA_PP_WHILE240(MACRO, R) MACRO(KAGUYA_PP_WHILE239(MACRO, R))
#define KAGUYA_PP_WHILE241(MACRO, R) MACRO(KAGUYA_PP_WHILE240(MACRO, R))
#define KAGUYA_PP_WHILE242(MACRO, R) MACRO(KAGUYA_PP_WHILE241(MACRO, R))
#define KAGUYA_PP_WHILE243(MACRO, R) MACRO(KAGUYA_PP_WHILE242(MACRO, R))
#define KAGUYA_PP_WHILE244(MACRO, R) MACRO(KAGUYA_PP_WHILE243(MACRO, R))
#define KAGUYA_PP_WHILE245(MACRO, R) MACRO(KAGUYA_PP_WHILE244(MACRO, R))
#define KAGUYA_PP_WHILE246(MACRO, R) MACRO(KAGUYA_PP_WHILE245(MACRO, R))
#define KAGUYA_PP_WHILE247(MACRO, R) MACRO(KAGUYA_PP_WHILE246(MACRO, R))
#define KAGUYA_PP_WHILE248(MACRO, R) MACRO(KAGUYA_PP_WHILE247(MACRO, R))
#define KAGUYA_PP_WHILE249(MACRO, R) MACRO(KAGUYA_PP_WHILE248(MACRO, R))
#define KAGUYA_PP_WHILE250(MACRO, R) MACRO(KAGUYA_PP_WHILE249(MACRO, R))
#define KAGUYA_PP_WHILE251(MACRO, R) MACRO(KAGUYA_PP_WHILE250(MACRO, R))
#define KAGUYA_PP_WHILE252(MACRO, R) MACRO(KAGUYA_PP_WHILE251(MACRO, R))
#define KAGUYA_PP_WHILE253(MACRO, R) MACRO(KAGUYA_PP_WHILE252(MACRO, R))
#define KAGUYA_PP_WHILE254(MACRO, R) MACRO(KAGUYA_PP_WHILE253(MACRO, R))
#define KAGUYA_PP_WHILE(COUNT, R, MACRO)                                       \
  KAGUYA_PP_CAT(KAGUYA_PP_WHILE, COUNT)(MACRO, R)

#define KAGUYA_PP_INC0 1
#define KAGUYA_PP_INC1 2
#define KAGUYA_PP_INC2 3
#define KAGUYA_PP_INC3 4
#define KAGUYA_PP_INC4 5
#define KAGUYA_PP_INC5 6
#define KAGUYA_PP_INC6 7
#define KAGUYA_PP_INC7 8
#define KAGUYA_PP_INC8 9
#define KAGUYA_PP_INC9 10
#define KAGUYA_PP_INC10 11
#define KAGUYA_PP_INC11 12
#define KAGUYA_PP_INC12 13
#define KAGUYA_PP_INC13 14
#define KAGUYA_PP_INC14 15
#define KAGUYA_PP_INC15 16
#define KAGUYA_PP_INC16 17
#define KAGUYA_PP_INC17 18
#define KAGUYA_PP_INC18 19
#define KAGUYA_PP_INC19 20
#define KAGUYA_PP_INC20 21
#define KAGUYA_PP_INC21 22
#define KAGUYA_PP_INC22 23
#define KAGUYA_PP_INC23 24
#define KAGUYA_PP_INC24 25
#define KAGUYA_PP_INC25 26
#define KAGUYA_PP_INC26 27
#define KAGUYA_PP_INC27 28
#define KAGUYA_PP_INC28 29
#define KAGUYA_PP_INC29 30
#define KAGUYA_PP_INC30 31
#define KAGUYA_PP_INC31 32
#define KAGUYA_PP_INC32 33
#define KAGUYA_PP_INC33 34
#define KAGUYA_PP_INC34 35
#define KAGUYA_PP_INC35 36
#define KAGUYA_PP_INC36 37
#define KAGUYA_PP_INC37 38
#define KAGUYA_PP_INC38 39
#define KAGUYA_PP_INC39 40
#define KAGUYA_PP_INC40 41
#define KAGUYA_PP_INC41 42
#define KAGUYA_PP_INC42 43
#define KAGUYA_PP_INC43 44
#define KAGUYA_PP_INC44 45
#define KAGUYA_PP_INC45 46
#define KAGUYA_PP_INC46 47
#define KAGUYA_PP_INC47 48
#define KAGUYA_PP_INC48 49
#define KAGUYA_PP_INC49 50
#define KAGUYA_PP_INC50 51
#define KAGUYA_PP_INC51 52
#define KAGUYA_PP_INC52 53
#define KAGUYA_PP_INC53 54
#define KAGUYA_PP_INC54 55
#define KAGUYA_PP_INC55 56
#define KAGUYA_PP_INC56 57
#define KAGUYA_PP_INC57 58
#define KAGUYA_PP_INC58 59
#define KAGUYA_PP_INC59 60
#define KAGUYA_PP_INC60 61
#define KAGUYA_PP_INC61 62
#define KAGUYA_PP_INC62 63
#define KAGUYA_PP_INC63 64
#define KAGUYA_PP_INC64 65
#define KAGUYA_PP_INC65 66
#define KAGUYA_PP_INC66 67
#define KAGUYA_PP_INC67 68
#define KAGUYA_PP_INC68 69
#define KAGUYA_PP_INC69 70
#define KAGUYA_PP_INC70 71
#define KAGUYA_PP_INC71 72
#define KAGUYA_PP_INC72 73
#define KAGUYA_PP_INC73 74
#define KAGUYA_PP_INC74 75
#define KAGUYA_PP_INC75 76
#define KAGUYA_PP_INC76 77
#define KAGUYA_PP_INC77 78
#define KAGUYA_PP_INC78 79
#define KAGUYA_PP_INC79 80
#define KAGUYA_PP_INC80 81
#define KAGUYA_PP_INC81 82
#define KAGUYA_PP_INC82 83
#define KAGUYA_PP_INC83 84
#define KAGUYA_PP_INC84 85
#define KAGUYA_PP_INC85 86
#define KAGUYA_PP_INC86 87
#define KAGUYA_PP_INC87 88
#define KAGUYA_PP_INC88 89
#define KAGUYA_PP_INC89 90
#define KAGUYA_PP_INC90 91
#define KAGUYA_PP_INC91 92
#define KAGUYA_PP_INC92 93
#define KAGUYA_PP_INC93 94
#define KAGUYA_PP_INC94 95
#define KAGUYA_PP_INC95 96
#define KAGUYA_PP_INC96 97
#define KAGUYA_PP_INC97 98
#define KAGUYA_PP_INC98 99
#define KAGUYA_PP_INC99 100
#define KAGUYA_PP_INC100 101
#define KAGUYA_PP_INC101 102
#define KAGUYA_PP_INC102 103
#define KAGUYA_PP_INC103 104
#define KAGUYA_PP_INC104 105
#define KAGUYA_PP_INC105 106
#define KAGUYA_PP_INC106 107
#define KAGUYA_PP_INC107 108
#define KAGUYA_PP_INC108 109
#define KAGUYA_PP_INC109 110
#define KAGUYA_PP_INC110 111
#define KAGUYA_PP_INC111 112
#define KAGUYA_PP_INC112 113
#define KAGUYA_PP_INC113 114
#define KAGUYA_PP_INC114 115
#define KAGUYA_PP_INC115 116
#define KAGUYA_PP_INC116 117
#define KAGUYA_PP_INC117 118
#define KAGUYA_PP_INC118 119
#define KAGUYA_PP_INC119 120
#define KAGUYA_PP_INC120 121
#define KAGUYA_PP_INC121 122
#define KAGUYA_PP_INC122 123
#define KAGUYA_PP_INC123 124
#define KAGUYA_PP_INC124 125
#define KAGUYA_PP_INC125 126
#define KAGUYA_PP_INC126 127
#define KAGUYA_PP_INC127 128
#define KAGUYA_PP_INC128 129
#define KAGUYA_PP_INC129 130
#define KAGUYA_PP_INC130 131
#define KAGUYA_PP_INC131 132
#define KAGUYA_PP_INC132 133
#define KAGUYA_PP_INC133 134
#define KAGUYA_PP_INC134 135
#define KAGUYA_PP_INC135 136
#define KAGUYA_PP_INC136 137
#define KAGUYA_PP_INC137 138
#define KAGUYA_PP_INC138 139
#define KAGUYA_PP_INC139 140
#define KAGUYA_PP_INC140 141
#define KAGUYA_PP_INC141 142
#define KAGUYA_PP_INC142 143
#define KAGUYA_PP_INC143 144
#define KAGUYA_PP_INC144 145
#define KAGUYA_PP_INC145 146
#define KAGUYA_PP_INC146 147
#define KAGUYA_PP_INC147 148
#define KAGUYA_PP_INC148 149
#define KAGUYA_PP_INC149 150
#define KAGUYA_PP_INC150 151
#define KAGUYA_PP_INC151 152
#define KAGUYA_PP_INC152 153
#define KAGUYA_PP_INC153 154
#define KAGUYA_PP_INC154 155
#define KAGUYA_PP_INC155 156
#define KAGUYA_PP_INC156 157
#define KAGUYA_PP_INC157 158
#define KAGUYA_PP_INC158 159
#define KAGUYA_PP_INC159 160
#define KAGUYA_PP_INC160 161
#define KAGUYA_PP_INC161 162
#define KAGUYA_PP_INC162 163
#define KAGUYA_PP_INC163 164
#define KAGUYA_PP_INC164 165
#define KAGUYA_PP_INC165 166
#define KAGUYA_PP_INC166 167
#define KAGUYA_PP_INC167 168
#define KAGUYA_PP_INC168 169
#define KAGUYA_PP_INC169 170
#define KAGUYA_PP_INC170 171
#define KAGUYA_PP_INC171 172
#define KAGUYA_PP_INC172 173
#define KAGUYA_PP_INC173 174
#define KAGUYA_PP_INC174 175
#define KAGUYA_PP_INC175 176
#define KAGUYA_PP_INC176 177
#define KAGUYA_PP_INC177 178
#define KAGUYA_PP_INC178 179
#define KAGUYA_PP_INC179 180
#define KAGUYA_PP_INC180 181
#define KAGUYA_PP_INC181 182
#define KAGUYA_PP_INC182 183
#define KAGUYA_PP_INC183 184
#define KAGUYA_PP_INC184 185
#define KAGUYA_PP_INC185 186
#define KAGUYA_PP_INC186 187
#define KAGUYA_PP_INC187 188
#define KAGUYA_PP_INC188 189
#define KAGUYA_PP_INC189 190
#define KAGUYA_PP_INC190 191
#define KAGUYA_PP_INC191 192
#define KAGUYA_PP_INC192 193
#define KAGUYA_PP_INC193 194
#define KAGUYA_PP_INC194 195
#define KAGUYA_PP_INC195 196
#define KAGUYA_PP_INC196 197
#define KAGUYA_PP_INC197 198
#define KAGUYA_PP_INC198 199
#define KAGUYA_PP_INC199 200
#define KAGUYA_PP_INC200 201
#define KAGUYA_PP_INC201 202
#define KAGUYA_PP_INC202 203
#define KAGUYA_PP_INC203 204
#define KAGUYA_PP_INC204 205
#define KAGUYA_PP_INC205 206
#define KAGUYA_PP_INC206 207
#define KAGUYA_PP_INC207 208
#define KAGUYA_PP_INC208 209
#define KAGUYA_PP_INC209 210
#define KAGUYA_PP_INC210 211
#define KAGUYA_PP_INC211 212
#define KAGUYA_PP_INC212 213
#define KAGUYA_PP_INC213 214
#define KAGUYA_PP_INC214 215
#define KAGUYA_PP_INC215 216
#define KAGUYA_PP_INC216 217
#define KAGUYA_PP_INC217 218
#define KAGUYA_PP_INC218 219
#define KAGUYA_PP_INC219 220
#define KAGUYA_PP_INC220 221
#define KAGUYA_PP_INC221 222
#define KAGUYA_PP_INC222 223
#define KAGUYA_PP_INC223 224
#define KAGUYA_PP_INC224 225
#define KAGUYA_PP_INC225 226
#define KAGUYA_PP_INC226 227
#define KAGUYA_PP_INC227 228
#define KAGUYA_PP_INC228 229
#define KAGUYA_PP_INC229 230
#define KAGUYA_PP_INC230 231
#define KAGUYA_PP_INC231 232
#define KAGUYA_PP_INC232 233
#define KAGUYA_PP_INC233 234
#define KAGUYA_PP_INC234 235
#define KAGUYA_PP_INC235 236
#define KAGUYA_PP_INC236 237
#define KAGUYA_PP_INC237 238
#define KAGUYA_PP_INC238 239
#define KAGUYA_PP_INC239 240
#define KAGUYA_PP_INC240 241
#define KAGUYA_PP_INC241 242
#define KAGUYA_PP_INC242 243
#define KAGUYA_PP_INC243 244
#define KAGUYA_PP_INC244 245
#define KAGUYA_PP_INC245 246
#define KAGUYA_PP_INC246 247
#define KAGUYA_PP_INC247 248
#define KAGUYA_PP_INC248 249
#define KAGUYA_PP_INC249 250
#define KAGUYA_PP_INC250 251
#define KAGUYA_PP_INC251 252
#define KAGUYA_PP_INC252 253
#define KAGUYA_PP_INC253 254
#define KAGUYA_PP_INC254 255
#define KAGUYA_PP_INC(N) KAGUYA_PP_CAT(KAGUYA_PP_INC, N)

#define KAGUYA_PP_DEC1 0
#define KAGUYA_PP_DEC2 1
#define KAGUYA_PP_DEC3 2
#define KAGUYA_PP_DEC4 3
#define KAGUYA_PP_DEC5 4
#define KAGUYA_PP_DEC6 5
#define KAGUYA_PP_DEC7 6
#define KAGUYA_PP_DEC8 7
#define KAGUYA_PP_DEC9 8
#define KAGUYA_PP_DEC10 9
#define KAGUYA_PP_DEC11 10
#define KAGUYA_PP_DEC12 11
#define KAGUYA_PP_DEC13 12
#define KAGUYA_PP_DEC14 13
#define KAGUYA_PP_DEC15 14
#define KAGUYA_PP_DEC16 15
#define KAGUYA_PP_DEC17 16
#define KAGUYA_PP_DEC18 17
#define KAGUYA_PP_DEC19 18
#define KAGUYA_PP_DEC20 19
#define KAGUYA_PP_DEC21 20
#define KAGUYA_PP_DEC22 21
#define KAGUYA_PP_DEC23 22
#define KAGUYA_PP_DEC24 23
#define KAGUYA_PP_DEC25 24
#define KAGUYA_PP_DEC26 25
#define KAGUYA_PP_DEC27 26
#define KAGUYA_PP_DEC28 27
#define KAGUYA_PP_DEC29 28
#define KAGUYA_PP_DEC30 29
#define KAGUYA_PP_DEC31 30
#define KAGUYA_PP_DEC32 31
#define KAGUYA_PP_DEC33 32
#define KAGUYA_PP_DEC34 33
#define KAGUYA_PP_DEC35 34
#define KAGUYA_PP_DEC36 35
#define KAGUYA_PP_DEC37 36
#define KAGUYA_PP_DEC38 37
#define KAGUYA_PP_DEC39 38
#define KAGUYA_PP_DEC40 39
#define KAGUYA_PP_DEC41 40
#define KAGUYA_PP_DEC42 41
#define KAGUYA_PP_DEC43 42
#define KAGUYA_PP_DEC44 43
#define KAGUYA_PP_DEC45 44
#define KAGUYA_PP_DEC46 45
#define KAGUYA_PP_DEC47 46
#define KAGUYA_PP_DEC48 47
#define KAGUYA_PP_DEC49 48
#define KAGUYA_PP_DEC50 49
#define KAGUYA_PP_DEC51 50
#define KAGUYA_PP_DEC52 51
#define KAGUYA_PP_DEC53 52
#define KAGUYA_PP_DEC54 53
#define KAGUYA_PP_DEC55 54
#define KAGUYA_PP_DEC56 55
#define KAGUYA_PP_DEC57 56
#define KAGUYA_PP_DEC58 57
#define KAGUYA_PP_DEC59 58
#define KAGUYA_PP_DEC60 59
#define KAGUYA_PP_DEC61 60
#define KAGUYA_PP_DEC62 61
#define KAGUYA_PP_DEC63 62
#define KAGUYA_PP_DEC64 63
#define KAGUYA_PP_DEC65 64
#define KAGUYA_PP_DEC66 65
#define KAGUYA_PP_DEC67 66
#define KAGUYA_PP_DEC68 67
#define KAGUYA_PP_DEC69 68
#define KAGUYA_PP_DEC70 69
#define KAGUYA_PP_DEC71 70
#define KAGUYA_PP_DEC72 71
#define KAGUYA_PP_DEC73 72
#define KAGUYA_PP_DEC74 73
#define KAGUYA_PP_DEC75 74
#define KAGUYA_PP_DEC76 75
#define KAGUYA_PP_DEC77 76
#define KAGUYA_PP_DEC78 77
#define KAGUYA_PP_DEC79 78
#define KAGUYA_PP_DEC80 79
#define KAGUYA_PP_DEC81 80
#define KAGUYA_PP_DEC82 81
#define KAGUYA_PP_DEC83 82
#define KAGUYA_PP_DEC84 83
#define KAGUYA_PP_DEC85 84
#define KAGUYA_PP_DEC86 85
#define KAGUYA_PP_DEC87 86
#define KAGUYA_PP_DEC88 87
#define KAGUYA_PP_DEC89 88
#define KAGUYA_PP_DEC90 89
#define KAGUYA_PP_DEC91 90
#define KAGUYA_PP_DEC92 91
#define KAGUYA_PP_DEC93 92
#define KAGUYA_PP_DEC94 93
#define KAGUYA_PP_DEC95 94
#define KAGUYA_PP_DEC96 95
#define KAGUYA_PP_DEC97 96
#define KAGUYA_PP_DEC98 97
#define KAGUYA_PP_DEC99 98
#define KAGUYA_PP_DEC100 99
#define KAGUYA_PP_DEC101 100
#define KAGUYA_PP_DEC102 101
#define KAGUYA_PP_DEC103 102
#define KAGUYA_PP_DEC104 103
#define KAGUYA_PP_DEC105 104
#define KAGUYA_PP_DEC106 105
#define KAGUYA_PP_DEC107 106
#define KAGUYA_PP_DEC108 107
#define KAGUYA_PP_DEC109 108
#define KAGUYA_PP_DEC110 109
#define KAGUYA_PP_DEC111 110
#define KAGUYA_PP_DEC112 111
#define KAGUYA_PP_DEC113 112
#define KAGUYA_PP_DEC114 113
#define KAGUYA_PP_DEC115 114
#define KAGUYA_PP_DEC116 115
#define KAGUYA_PP_DEC117 116
#define KAGUYA_PP_DEC118 117
#define KAGUYA_PP_DEC119 118
#define KAGUYA_PP_DEC120 119
#define KAGUYA_PP_DEC121 120
#define KAGUYA_PP_DEC122 121
#define KAGUYA_PP_DEC123 122
#define KAGUYA_PP_DEC124 123
#define KAGUYA_PP_DEC125 124
#define KAGUYA_PP_DEC126 125
#define KAGUYA_PP_DEC127 126
#define KAGUYA_PP_DEC128 127
#define KAGUYA_PP_DEC129 128
#define KAGUYA_PP_DEC130 129
#define KAGUYA_PP_DEC131 130
#define KAGUYA_PP_DEC132 131
#define KAGUYA_PP_DEC133 132
#define KAGUYA_PP_DEC134 133
#define KAGUYA_PP_DEC135 134
#define KAGUYA_PP_DEC136 135
#define KAGUYA_PP_DEC137 136
#define KAGUYA_PP_DEC138 137
#define KAGUYA_PP_DEC139 138
#define KAGUYA_PP_DEC140 139
#define KAGUYA_PP_DEC141 140
#define KAGUYA_PP_DEC142 141
#define KAGUYA_PP_DEC143 142
#define KAGUYA_PP_DEC144 143
#define KAGUYA_PP_DEC145 144
#define KAGUYA_PP_DEC146 145
#define KAGUYA_PP_DEC147 146
#define KAGUYA_PP_DEC148 147
#define KAGUYA_PP_DEC149 148
#define KAGUYA_PP_DEC150 149
#define KAGUYA_PP_DEC151 150
#define KAGUYA_PP_DEC152 151
#define KAGUYA_PP_DEC153 152
#define KAGUYA_PP_DEC154 153
#define KAGUYA_PP_DEC155 154
#define KAGUYA_PP_DEC156 155
#define KAGUYA_PP_DEC157 156
#define KAGUYA_PP_DEC158 157
#define KAGUYA_PP_DEC159 158
#define KAGUYA_PP_DEC160 159
#define KAGUYA_PP_DEC161 160
#define KAGUYA_PP_DEC162 161
#define KAGUYA_PP_DEC163 162
#define KAGUYA_PP_DEC164 163
#define KAGUYA_PP_DEC165 164
#define KAGUYA_PP_DEC166 165
#define KAGUYA_PP_DEC167 166
#define KAGUYA_PP_DEC168 167
#define KAGUYA_PP_DEC169 168
#define KAGUYA_PP_DEC170 169
#define KAGUYA_PP_DEC171 170
#define KAGUYA_PP_DEC172 171
#define KAGUYA_PP_DEC173 172
#define KAGUYA_PP_DEC174 173
#define KAGUYA_PP_DEC175 174
#define KAGUYA_PP_DEC176 175
#define KAGUYA_PP_DEC177 176
#define KAGUYA_PP_DEC178 177
#define KAGUYA_PP_DEC179 178
#define KAGUYA_PP_DEC180 179
#define KAGUYA_PP_DEC181 180
#define KAGUYA_PP_DEC182 181
#define KAGUYA_PP_DEC183 182
#define KAGUYA_PP_DEC184 183
#define KAGUYA_PP_DEC185 184
#define KAGUYA_PP_DEC186 185
#define KAGUYA_PP_DEC187 186
#define KAGUYA_PP_DEC188 187
#define KAGUYA_PP_DEC189 188
#define KAGUYA_PP_DEC190 189
#define KAGUYA_PP_DEC191 190
#define KAGUYA_PP_DEC192 191
#define KAGUYA_PP_DEC193 192
#define KAGUYA_PP_DEC194 193
#define KAGUYA_PP_DEC195 194
#define KAGUYA_PP_DEC196 195
#define KAGUYA_PP_DEC197 196
#define KAGUYA_PP_DEC198 197
#define KAGUYA_PP_DEC199 198
#define KAGUYA_PP_DEC200 199
#define KAGUYA_PP_DEC201 200
#define KAGUYA_PP_DEC202 201
#define KAGUYA_PP_DEC203 202
#define KAGUYA_PP_DEC204 203
#define KAGUYA_PP_DEC205 204
#define KAGUYA_PP_DEC206 205
#define KAGUYA_PP_DEC207 206
#define KAGUYA_PP_DEC208 207
#define KAGUYA_PP_DEC209 208
#define KAGUYA_PP_DEC210 209
#define KAGUYA_PP_DEC211 210
#define KAGUYA_PP_DEC212 211
#define KAGUYA_PP_DEC213 212
#define KAGUYA_PP_DEC214 213
#define KAGUYA_PP_DEC215 214
#define KAGUYA_PP_DEC216 215
#define KAGUYA_PP_DEC217 216
#define KAGUYA_PP_DEC218 217
#define KAGUYA_PP_DEC219 218
#define KAGUYA_PP_DEC220 219
#define KAGUYA_PP_DEC221 220
#define KAGUYA_PP_DEC222 221
#define KAGUYA_PP_DEC223 222
#define KAGUYA_PP_DEC224 223
#define KAGUYA_PP_DEC225 224
#define KAGUYA_PP_DEC226 225
#define KAGUYA_PP_DEC227 226
#define KAGUYA_PP_DEC228 227
#define KAGUYA_PP_DEC229 228
#define KAGUYA_PP_DEC230 229
#define KAGUYA_PP_DEC231 230
#define KAGUYA_PP_DEC232 231
#define KAGUYA_PP_DEC233 232
#define KAGUYA_PP_DEC234 233
#define KAGUYA_PP_DEC235 234
#define KAGUYA_PP_DEC236 235
#define KAGUYA_PP_DEC237 236
#define KAGUYA_PP_DEC238 237
#define KAGUYA_PP_DEC239 238
#define KAGUYA_PP_DEC240 239
#define KAGUYA_PP_DEC241 240
#define KAGUYA_PP_DEC242 241
#define KAGUYA_PP_DEC243 242
#define KAGUYA_PP_DEC244 243
#define KAGUYA_PP_DEC245 244
#define KAGUYA_PP_DEC246 245
#define KAGUYA_PP_DEC247 246
#define KAGUYA_PP_DEC248 247
#define KAGUYA_PP_DEC249 248
#define KAGUYA_PP_DEC250 249
#define KAGUYA_PP_DEC251 250
#define KAGUYA_PP_DEC252 251
#define KAGUYA_PP_DEC253 252
#define KAGUYA_PP_DEC254 253
#define KAGUYA_PP_DEC(N) KAGUYA_PP_CAT(KAGUYA_PP_DEC, N)

#define KAGUYA_PP_VARIADIC_TARG_CONCAT_REP(N) , KAGUYA_PP_CAT(A, N)
#define KAGUYA_PP_VARIADIC_TARG_REP(N) KAGUYA_PP_CAT(A, N)

#define KAGUYA_PP_TEMPLATE_ARG_REPEAT_CONCAT(N)                                \
  KAGUYA_PP_REPEAT(N, KAGUYA_PP_VARIADIC_TARG_CONCAT_REP)
#define KAGUYA_PP_TEMPLATE_ARG_REPEAT(N)                                       \
  KAGUYA_PP_REPEAT_ARG(N, KAGUYA_PP_VARIADIC_TARG_REP)

#define KAGUYA_PP_ARG_DEF_CONCAT_REP(N)                                        \
  , KAGUYA_PP_CAT(A, N) KAGUYA_PP_CAT(a, N)
#define KAGUYA_PP_ARG_DEF_REP(N) KAGUYA_PP_CAT(A, N) KAGUYA_PP_CAT(a, N)

#define KAGUYA_PP_ARG_DEF_REPEAT_CONCAT(N)                                     \
  KAGUYA_PP_REPEAT(N, KAGUYA_PP_ARG_DEF_CONCAT_REP)
#define KAGUYA_PP_ARG_DEF_REPEAT(N)                                            \
  KAGUYA_PP_REPEAT_ARG(N, KAGUYA_PP_ARG_DEF_REP)

#define KAGUYA_PP_ARG_CR_DEF_CONCAT_REP(N)                                     \
  , const KAGUYA_PP_CAT(A, N) & KAGUYA_PP_CAT(a, N)
#define KAGUYA_PP_ARG_CR_DEF_REP(N)                                            \
  const KAGUYA_PP_CAT(A, N) & KAGUYA_PP_CAT(a, N)

#define KAGUYA_PP_ARG_CR_DEF_REPEAT_CONCAT(N)                                  \
  KAGUYA_PP_REPEAT(N, KAGUYA_PP_ARG_CR_DEF_CONCAT_REP)
#define KAGUYA_PP_ARG_CR_DEF_REPEAT(N)                                         \
  KAGUYA_PP_REPEAT_ARG(N, KAGUYA_PP_ARG_CR_DEF_REP)

#define KAGUYA_PP_ARG_CONCAT_REP(N) , KAGUYA_PP_CAT(a, N)
#define KAGUYA_PP_ARG_REP(N) KAGUYA_PP_CAT(a, N)

#define KAGUYA_PP_ARG_REPEAT_CONCAT(N)                                         \
  KAGUYA_PP_REPEAT(N, KAGUYA_PP_ARG_CONCAT_REP)
#define KAGUYA_PP_ARG_REPEAT(N) KAGUYA_PP_REPEAT_ARG(N, KAGUYA_PP_ARG_REP)

#define KAGUYA_PP_VARIADIC_TDEF_CONCAT_REP(N) , KAGUYA_PP_CAT(typename A, N)
#define KAGUYA_PP_VARIADIC_TDEF_REP(N) KAGUYA_PP_CAT(typename A, N)

#define KAGUYA_PP_TEMPLATE_DEF_REPEAT_CONCAT(N)                                \
  KAGUYA_PP_REPEAT(N, KAGUYA_PP_VARIADIC_TDEF_CONCAT_REP)
#define KAGUYA_PP_TEMPLATE_DEF_REPEAT(N)                                       \
  KAGUYA_PP_REPEAT_ARG(N, KAGUYA_PP_VARIADIC_TDEF_REP)

#define KAGUYA_PP_ADD(X, Y) KAGUYA_PP_WHILE(Y, X, KAGUYA_PP_INC)
#define KAGUYA_PP_SUB(X, Y) KAGUYA_PP_WHILE(Y, X, KAGUYA_PP_DEC)
// Copyright satoren
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#include <exception>
#include <stdexcept>

namespace kaguya {
class LuaException : public std::exception {
  int status_;
  std::string what_;
  const char *what_c_;

public:
  LuaException(int status, const char *what) throw()
      : status_(status), what_c_(what) {}
  LuaException(int status, const std::string &what)
      : status_(status), what_(what), what_c_(0) {}
  int status() const throw() { return status_; }
  const char *what() const throw() { return what_c_ ? what_c_ : what_.c_str(); }

  ~LuaException() throw() {}
};
class KaguyaException : public std::exception {
  std::string what_;
  const char *what_c_;

public:
  KaguyaException(const char *what) throw() : what_c_(what) {}
  KaguyaException(const std::string &what) : what_(what), what_c_(0) {}
  const char *what() const throw() { return what_c_ ? what_c_ : what_.c_str(); }

  ~KaguyaException() throw() {}
};
class LuaTypeMismatch : public LuaException {
public:
  LuaTypeMismatch() throw() : LuaException(0, "type mismatch!!") {}
  LuaTypeMismatch(const char *what) throw() : LuaException(0, what) {}
  LuaTypeMismatch(const std::string &what) : LuaException(0, what) {}
};
class LuaMemoryError : public LuaException {
public:
  LuaMemoryError(int status, const char *what) throw()
      : LuaException(status, what) {}
  LuaMemoryError(int status, const std::string &what)
      : LuaException(status, what) {}
};
class LuaRuntimeError : public LuaException {
public:
  LuaRuntimeError(int status, const char *what) throw()
      : LuaException(status, what) {}
  LuaRuntimeError(int status, const std::string &what)
      : LuaException(status, what) {}
};
class LuaErrorRunningError : public LuaException {
public:
  LuaErrorRunningError(int status, const char *what) throw()
      : LuaException(status, what) {}
  LuaErrorRunningError(int status, const std::string &what)
      : LuaException(status, what) {}
};
class LuaGCError : public LuaException {
public:
  LuaGCError(int status, const char *what) throw()
      : LuaException(status, what) {}
  LuaGCError(int status, const std::string &what)
      : LuaException(status, what) {}
};
class LuaUnknownError : public LuaException {
public:
  LuaUnknownError(int status, const char *what) throw()
      : LuaException(status, what) {}
  LuaUnknownError(int status, const std::string &what)
      : LuaException(status, what) {}
};

class LuaSyntaxError : public LuaException {
public:
  LuaSyntaxError(int status, const std::string &what)
      : LuaException(status, what) {}
};

namespace except {
void OtherError(lua_State *state, const std::string &message);
void typeMismatchError(lua_State *state, const std::string &message);
void memoryError(lua_State *state, const char *message);
bool checkErrorAndThrow(int status, lua_State *state);
}
}

#if KAGUYA_USE_CPP11
// Copyright satoren
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <string>


namespace kaguya {
namespace util {
struct null_type {};

template <class... Args> struct TypeTuple {};
template <class Ret, class... Args> struct FunctionSignatureType {
  typedef Ret result_type;
  typedef TypeTuple<Args...> argument_type_tuple;
  static const size_t argument_count = sizeof...(Args);
  typedef Ret (*c_function_type)(Args...);
};
template <typename T> struct FunctorSignature {};

template <typename T, typename Ret, typename... Args>
struct FunctorSignature<Ret (T::*)(Args...) const> {
  typedef FunctionSignatureType<Ret, Args...> type;
};
template <typename T, typename Ret, typename... Args>
struct FunctorSignature<Ret (T::*)(Args...)> {
  typedef FunctionSignatureType<Ret, Args...> type;
};

#if defined(_MSC_VER) && _MSC_VER < 1900
template <typename T>
struct FunctionSignature : public FunctorSignature<decltype(&T::operator())> {};
#else

template <typename T, typename Enable = void> struct FunctionSignature;

template <typename T, typename = void>
struct has_operator_fn : std::false_type {};
template <typename T>
struct has_operator_fn<T, typename std::enable_if<!std::is_same<
                              void, decltype(&T::operator())>::value>::type>
    : std::true_type {};

template <typename T>
struct FunctionSignature<
    T, typename std::enable_if<has_operator_fn<T>::value>::type>
    : public FunctorSignature<decltype(&T::operator())> {};
#endif

template <typename T, typename Ret, typename... Args>
struct FunctionSignature<Ret (T::*)(Args...)> {
  typedef FunctionSignatureType<Ret, T &, Args...> type;
};
template <typename T, typename Ret, typename... Args>
struct FunctionSignature<Ret (T::*)(Args...) const> {
  typedef FunctionSignatureType<Ret, const T &, Args...> type;
};

#if defined(_MSC_VER) && _MSC_VER >= 1900 || defined(__cpp_ref_qualifiers)
template <typename T, typename Ret, typename... Args>
struct FunctionSignature<Ret (T::*)(Args...) const &> {
  typedef FunctionSignatureType<Ret, const T &, Args...> type;
};
template <typename T, typename Ret, typename... Args>
struct FunctionSignature<Ret (T::*)(Args...) const &&> {
  typedef FunctionSignatureType<Ret, const T &, Args...> type;
};
#endif

template <class Ret, class... Args> struct FunctionSignature<Ret (*)(Args...)> {
  typedef FunctionSignatureType<Ret, Args...> type;
};
template <class Ret, class... Args> struct FunctionSignature<Ret(Args...)> {
  typedef FunctionSignatureType<Ret, Args...> type;
};

template <typename F> struct FunctionResultType {
  typedef typename FunctionSignature<F>::type::result_type type;
};

template <std::size_t remain, class Arg, bool flag = remain <= 0>
struct TypeIndexGet;

template <std::size_t remain, class Arg, class... Args>
struct TypeIndexGet<remain, TypeTuple<Arg, Args...>, true> {
  typedef Arg type;
};

template <std::size_t remain, class Arg, class... Args>
struct TypeIndexGet<remain, TypeTuple<Arg, Args...>, false>
    : TypeIndexGet<remain - 1, TypeTuple<Args...> > {};
template <int N, typename F> struct ArgumentType {
  typedef typename TypeIndexGet<
      N, typename FunctionSignature<F>::type::argument_type_tuple>::type type;
};

namespace detail {
template <class F, class ThisType, class... Args>
auto invoke_helper(F &&f, ThisType &&this_, Args &&... args)
    -> decltype((std::forward<ThisType>(this_).*
                 f)(std::forward<Args>(args)...)) {
  return (std::forward<ThisType>(this_).*f)(std::forward<Args>(args)...);
}

template <class F, class... Args>
auto invoke_helper(F &&f, Args &&... args)
    -> decltype(f(std::forward<Args>(args)...)) {
  return f(std::forward<Args>(args)...);
}
}
template <class F, class... Args>
typename FunctionResultType<typename traits::decay<F>::type>::type
invoke(F &&f, Args &&... args) {
  return detail::invoke_helper(std::forward<F>(f), std::forward<Args>(args)...);
}
}
}
#else
// Copyright satoren
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <string>


namespace kaguya {
namespace util {
///!
struct null_type {};

#define KAGUYA_PP_STRUCT_TDEF_REP(N) KAGUYA_PP_CAT(typename A, N) = null_type
#define KAGUYA_PP_STRUCT_TEMPLATE_DEF_REPEAT(N)                                \
  KAGUYA_PP_REPEAT_ARG(N, KAGUYA_PP_STRUCT_TDEF_REP)

template <KAGUYA_PP_STRUCT_TEMPLATE_DEF_REPEAT(KAGUYA_FUNCTION_MAX_ARGS)>
struct TypeTuple {};

template <typename F> struct TypeTupleSize;

#define KAGUYA_TYPE_TUPLE_SIZE_DEF(N)                                          \
  template <KAGUYA_PP_TEMPLATE_DEF_REPEAT(N)>                                  \
  struct TypeTupleSize<TypeTuple<KAGUYA_PP_TEMPLATE_ARG_REPEAT(N)> > {         \
    static const size_t value = N;                                             \
  };

KAGUYA_TYPE_TUPLE_SIZE_DEF(0)
KAGUYA_PP_REPEAT_DEF(KAGUYA_FUNCTION_MAX_ARGS, KAGUYA_TYPE_TUPLE_SIZE_DEF)
#undef KAGUYA_TYPE_TUPLE_SIZE_DEF

template <typename Ret, typename typetuple> struct CFuntionType;
#define KAGUYA_CFUNCTION_TYPE_DEF(N)                                           \
  template <typename Ret KAGUYA_PP_TEMPLATE_DEF_REPEAT_CONCAT(N)>              \
  struct CFuntionType<Ret, TypeTuple<KAGUYA_PP_TEMPLATE_ARG_REPEAT(N)> > {     \
    typedef Ret (*type)(KAGUYA_PP_TEMPLATE_ARG_REPEAT(N));                     \
  };

KAGUYA_CFUNCTION_TYPE_DEF(0)
KAGUYA_PP_REPEAT_DEF(KAGUYA_FUNCTION_MAX_ARGS, KAGUYA_CFUNCTION_TYPE_DEF)
#undef KAGUYA_CFUNCTION_TYPE_DEF

template <typename Ret,
          KAGUYA_PP_STRUCT_TEMPLATE_DEF_REPEAT(KAGUYA_FUNCTION_MAX_ARGS)>
struct FunctionSignatureType {
  typedef Ret result_type;
  typedef TypeTuple<KAGUYA_PP_TEMPLATE_ARG_REPEAT(KAGUYA_FUNCTION_MAX_ARGS)>
      argument_type_tuple;
  static const size_t argument_count =
      TypeTupleSize<argument_type_tuple>::value;
  typedef typename CFuntionType<Ret, argument_type_tuple>::type c_function_type;
};

template <typename T, typename Enable = void> struct FunctionSignature;

#define KAGUYA_MEMBER_FUNCTION_SIGNATURE_DEF(N)                                \
  template <typename T, typename Ret KAGUYA_PP_TEMPLATE_DEF_REPEAT_CONCAT(N)>  \
  struct FunctionSignature<Ret (T::*)(KAGUYA_PP_TEMPLATE_ARG_REPEAT(N))> {     \
    typedef FunctionSignatureType<Ret,                                         \
                                  T & KAGUYA_PP_TEMPLATE_ARG_REPEAT_CONCAT(N)> \
        type;                                                                  \
  };                                                                           \
  template <typename T, typename Ret KAGUYA_PP_TEMPLATE_DEF_REPEAT_CONCAT(N)>  \
  struct FunctionSignature<Ret (T::*)(KAGUYA_PP_TEMPLATE_ARG_REPEAT(N))        \
                               const> {                                        \
    typedef FunctionSignatureType<                                             \
        Ret, const T & KAGUYA_PP_TEMPLATE_ARG_REPEAT_CONCAT(N)>                \
        type;                                                                  \
  };

#define KAGUYA_FUNCTION_SIGNATURE_DEF(N)                                       \
  template <class Ret KAGUYA_PP_TEMPLATE_DEF_REPEAT_CONCAT(N)>                 \
  struct FunctionSignature<Ret (*)(KAGUYA_PP_TEMPLATE_ARG_REPEAT(N))> {        \
    typedef FunctionSignatureType<Ret KAGUYA_PP_TEMPLATE_ARG_REPEAT_CONCAT(N)> \
        type;                                                                  \
  };                                                                           \
  template <class Ret KAGUYA_PP_TEMPLATE_DEF_REPEAT_CONCAT(N)>                 \
  struct FunctionSignature<Ret(KAGUYA_PP_TEMPLATE_ARG_REPEAT(N))> {            \
    typedef FunctionSignatureType<Ret KAGUYA_PP_TEMPLATE_ARG_REPEAT_CONCAT(N)> \
        type;                                                                  \
  };                                                                           \
  template <class Ret KAGUYA_PP_TEMPLATE_DEF_REPEAT_CONCAT(N)>                 \
  struct FunctionSignature<                                                    \
      standard::function<Ret(KAGUYA_PP_TEMPLATE_ARG_REPEAT(N))> > {            \
    typedef FunctionSignatureType<Ret KAGUYA_PP_TEMPLATE_ARG_REPEAT_CONCAT(N)> \
        type;                                                                  \
  };

KAGUYA_MEMBER_FUNCTION_SIGNATURE_DEF(0)
KAGUYA_PP_REPEAT_DEF(KAGUYA_PP_DEC(KAGUYA_FUNCTION_MAX_ARGS),
                     KAGUYA_MEMBER_FUNCTION_SIGNATURE_DEF)
#undef KAGUYA_MEMBER_FUNCTION_SIGNATURE_DEF
KAGUYA_FUNCTION_SIGNATURE_DEF(0)
KAGUYA_PP_REPEAT_DEF(KAGUYA_FUNCTION_MAX_ARGS, KAGUYA_FUNCTION_SIGNATURE_DEF)
#undef KAGUYA_FUNCTION_SIGNATURE_DEF

template <typename F> struct FunctionResultType {
  typedef typename FunctionSignature<F>::type::result_type type;
};

template <std::size_t remain, class result, bool flag = remain <= 0>
struct TypeIndexGet {};

#define KAGUYA_TYPE_INDEX_GET_DEF(N)                                           \
  template <std::size_t remain,                                                \
            class arg KAGUYA_PP_TEMPLATE_DEF_REPEAT_CONCAT(N)>                 \
  struct TypeIndexGet<                                                         \
      remain, TypeTuple<arg KAGUYA_PP_TEMPLATE_ARG_REPEAT_CONCAT(N)>, true> {  \
    typedef arg type;                                                          \
  };                                                                           \
  template <std::size_t remain,                                                \
            class arg KAGUYA_PP_TEMPLATE_DEF_REPEAT_CONCAT(N)>                 \
  struct TypeIndexGet<                                                         \
      remain, TypeTuple<arg KAGUYA_PP_TEMPLATE_ARG_REPEAT_CONCAT(N)>, false>   \
      : TypeIndexGet<remain - 1,                                               \
                     TypeTuple<KAGUYA_PP_TEMPLATE_ARG_REPEAT(N)> > {};

//		KAGUYA_TYPE_INDEX_GET_DEF(0);
KAGUYA_PP_REPEAT_DEF(KAGUYA_PP_DEC(KAGUYA_FUNCTION_MAX_ARGS),
                     KAGUYA_TYPE_INDEX_GET_DEF)
#undef KAGUYA_TYPE_INDEX_GET_DEF

template <std::size_t N, typename F> struct ArgumentType {
  typedef typename TypeIndexGet<
      N, typename FunctionSignature<F>::type::argument_type_tuple>::type type;
};

namespace detail {

#define KAGUYA_INVOKE_HELPER_DEF(N)                                            \
  template <class F, class ThisType KAGUYA_PP_TEMPLATE_DEF_REPEAT_CONCAT(N),   \
            typename T>                                                        \
  typename FunctionResultType<F>::type invoke_helper(                          \
      typename FunctionResultType<F>::type (T::*f)(                            \
          KAGUYA_PP_TEMPLATE_ARG_REPEAT(N)),                                   \
      ThisType this_ KAGUYA_PP_ARG_DEF_REPEAT_CONCAT(N)) {                     \
    return (this_.*f)(KAGUYA_PP_ARG_REPEAT(N));                                \
  }                                                                            \
  template <class F, class ThisType KAGUYA_PP_TEMPLATE_DEF_REPEAT_CONCAT(N),   \
            typename T>                                                        \
  typename FunctionResultType<F>::type invoke_helper(                          \
      typename FunctionResultType<F>::type (T::*f)(                            \
          KAGUYA_PP_TEMPLATE_ARG_REPEAT(N)) const,                             \
      ThisType this_ KAGUYA_PP_ARG_DEF_REPEAT_CONCAT(N)) {                     \
    return (this_.*f)(KAGUYA_PP_ARG_REPEAT(N));                                \
  }                                                                            \
  template <class F KAGUYA_PP_TEMPLATE_DEF_REPEAT_CONCAT(N)>                   \
  typename FunctionResultType<F>::type invoke_helper(                          \
      F f KAGUYA_PP_ARG_DEF_REPEAT_CONCAT(N)) {                                \
    return f(KAGUYA_PP_ARG_REPEAT(N));                                         \
  }

KAGUYA_INVOKE_HELPER_DEF(0)
KAGUYA_PP_REPEAT_DEF(KAGUYA_FUNCTION_MAX_ARGS, KAGUYA_INVOKE_HELPER_DEF)
#undef KAGUYA_INVOKE_HELPER_DEF
}

#define KAGUYA_INVOKE_DEF(N)                                                   \
  template <class F KAGUYA_PP_TEMPLATE_DEF_REPEAT_CONCAT(N)>                   \
  typename FunctionResultType<F>::type invoke(                                 \
      F f KAGUYA_PP_ARG_DEF_REPEAT_CONCAT(N)) {                                \
    return detail::invoke_helper<F KAGUYA_PP_TEMPLATE_ARG_REPEAT_CONCAT(N)>(   \
        f KAGUYA_PP_ARG_REPEAT_CONCAT(N));                                     \
  }

KAGUYA_INVOKE_DEF(0)
KAGUYA_PP_REPEAT_DEF(KAGUYA_FUNCTION_MAX_ARGS, KAGUYA_INVOKE_DEF)
#undef KAGUYA_INVOKE_DEF

#undef KAGUYA_PP_STRUCT_TDEF_REP
#undef KAGUYA_PP_STRUCT_TEMPLATE_DEF_REPEAT
}
}
#endif

namespace kaguya {
namespace util {
/// @brief save stack count and restore on destructor
class ScopedSavedStack {
  lua_State *state_;
  int saved_top_index_;

public:
  /// @brief save stack count
  /// @param state
  explicit ScopedSavedStack(lua_State *state)
      : state_(state), saved_top_index_(state_ ? lua_gettop(state_) : 0) {}

  /// @brief save stack count
  /// @param state
  /// @param count stack count
  explicit ScopedSavedStack(lua_State *state, int count)
      : state_(state), saved_top_index_(count) {}

  /// @brief restore stack count
  ~ScopedSavedStack() {
    if (state_) {
      lua_settop(state_, saved_top_index_);
    }
  }

private:
  ScopedSavedStack(ScopedSavedStack const &);
  ScopedSavedStack &operator=(ScopedSavedStack const &);
};
inline void traceBack(lua_State *state, const char *message, int level = 0) {
#if LUA_VERSION_NUM >= 502
  luaL_traceback(state, state, message, level);
#else
  KAGUYA_UNUSED(level);
  lua_pushstring(state, message);
#endif
}

inline void stackDump(lua_State *L) {
  int i;
  int top = lua_gettop(L);
  for (i = 1; i <= top; i++) { /* repeat for each level */
    int t = lua_type(L, i);
    switch (t) {

    case LUA_TSTRING: /* strings */
      printf("`%s'", lua_tostring(L, i));
      break;

    case LUA_TBOOLEAN: /* booleans */
      printf(lua_toboolean(L, i) ? "true" : "false");
      break;

    case LUA_TNUMBER: /* numbers */
      printf("%g", lua_tonumber(L, i));
      break;
    case LUA_TUSERDATA:
      if (luaL_getmetafield(L, i, "__name") == LUA_TSTRING) {
        printf("userdata:%s", lua_tostring(L, -1));
        lua_pop(L, 1);
        break;
      }
    default: /* other values */
      printf("%s", lua_typename(L, t));
      break;
    }
    printf("  "); /* put a separator */
  }
  printf("\n"); /* end the listing */
}

inline void stackValueDump(std::ostream &os, lua_State *state, int stackIndex,
                           int max_recursive = 2) {
  stackIndex = lua_absindex(state, stackIndex);
  util::ScopedSavedStack save(state);
  int type = lua_type(state, stackIndex);
  switch (type) {
  case LUA_TNONE:
    os << "none";
    break;
  case LUA_TNIL:
    os << "nil";
    break;
  case LUA_TBOOLEAN:
    os << ((lua_toboolean(state, stackIndex) != 0) ? "true" : "false");
    break;
  case LUA_TNUMBER:
    os << lua_tonumber(state, stackIndex);
    break;
  case LUA_TSTRING:
    os << "'" << lua_tostring(state, stackIndex) << "'";
    break;
  case LUA_TTABLE: {
    os << "{";
    if (max_recursive <= 1) {
      os << "...";
    } else {
      lua_pushnil(state);
      if ((lua_next(state, stackIndex) != 0)) {
        stackValueDump(os, state, -2, max_recursive - 1);
        os << "=";
        stackValueDump(os, state, -1, max_recursive - 1);
        lua_pop(state, 1); // pop value

        while (lua_next(state, stackIndex) != 0) {
          os << ",";
          stackValueDump(os, state, -2, max_recursive - 1);
          os << "=";
          stackValueDump(os, state, -1, max_recursive - 1);
          lua_pop(state, 1); // pop value
        }
      }
    }
    os << "}";
  } break;
  case LUA_TUSERDATA:
  case LUA_TLIGHTUSERDATA:
  case LUA_TTHREAD:
    os << lua_typename(state, type) << "(" << lua_topointer(state, stackIndex)
       << ")";
    break;
  case LUA_TFUNCTION:
    os << lua_typename(state, type);
    break;
  default:
    os << "unknown type value";
    break;
  }
}

#if LUA_VERSION_NUM >= 502
inline lua_State *toMainThread(lua_State *state) {
  if (state) {
    lua_rawgeti(state, LUA_REGISTRYINDEX, LUA_RIDX_MAINTHREAD);
    lua_State *mainthread = lua_tothread(state, -1);
    lua_pop(state, 1);
    if (mainthread) {
      return mainthread;
    }
  }
  return state;
}

#else
inline lua_State *toMainThread(lua_State *state) {
  if (state) {
    // lua_pushthread return 1 if state is main thread
    bool state_is_main = lua_pushthread(state) == 1;
	lua_pop(state, 1);
    if (state_is_main) {
      return state;
    }
    lua_getfield(state, LUA_REGISTRYINDEX, "KAGUYA_REG_MAINTHREAD");
    lua_State *mainthread = lua_tothread(state, -1);
    lua_pop(state, 1);
    if (mainthread) {
      return mainthread;
    }
  }
  return state;
}
inline bool registerMainThread(lua_State *state) {
  if (lua_pushthread(state)) {
    lua_setfield(state, LUA_REGISTRYINDEX, "KAGUYA_REG_MAINTHREAD");
    return true;
  } else {
    lua_pop(state, 1);
    return false;
  }
}
#endif

#if KAGUYA_USE_CPP11
inline int push_args(lua_State *) { return 0; }
template <class Arg, class... Args>
inline int push_args(lua_State *l, Arg &&arg, Args &&... args) {
  int c = lua_type_traits<typename traits::decay<Arg>::type>::push(
      l, std::forward<Arg>(arg));
  return c + push_args(l, std::forward<Args>(args)...);
}
template <class Arg, class... Args>
inline int push_args(lua_State *l, const Arg &arg, Args &&... args) {
  int c = lua_type_traits<Arg>::push(l, arg);
  return c + push_args(l, std::forward<Args>(args)...);
}
#else
inline int push_args(lua_State *) { return 0; }

#define KAGUYA_PUSH_DEF(N)                                                     \
  c += lua_type_traits<KAGUYA_PP_CAT(A, N)>::push(l, KAGUYA_PP_CAT(a, N));
#define KAGUYA_PUSH_ARG_DEF(N)                                                 \
  template <KAGUYA_PP_TEMPLATE_DEF_REPEAT(N)>                                  \
  inline int push_args(lua_State *l, KAGUYA_PP_ARG_CR_DEF_REPEAT(N)) {         \
    int c = 0;                                                                 \
    KAGUYA_PP_REPEAT(N, KAGUYA_PUSH_DEF)                                       \
    return c;                                                                  \
  }
KAGUYA_PP_REPEAT_DEF(KAGUYA_FUNCTION_MAX_ARGS, KAGUYA_PUSH_ARG_DEF)
#undef KAGUYA_PUSH_DEF
#undef KAGUYA_PUSH_ARG_DEF
#endif

#if KAGUYA_USE_CPP11
template <typename T> inline bool one_push(lua_State *state, T &&v) {
  int count = util::push_args(state, std::forward<T>(v));
  if (count > 1) {
    lua_pop(state, count - 1);
  }
  return count != 0;
}
#else
template <typename T> inline bool one_push(lua_State *state, const T &v) {
  int count = util::push_args(state, v);
  if (count > 1) {
    lua_pop(state, count - 1);
  }
  return count != 0;
}
#endif

inline std::string pretty_name(const std::type_info &t) {
#if KAGUYA_USE_CXX_ABI_DEMANGLE
  int status = 0;
  char *demangle_name = abi::__cxa_demangle(t.name(), 0, 0, &status);
  struct deleter {
    char *data;
    deleter(char *d) : data(d) {}
    ~deleter() { std::free(data); }
  } d(demangle_name);
  return demangle_name;
#else
  return t.name();
#endif
}
}
}

namespace kaguya {
namespace types {
template <typename T> struct typetag {};
}
#if KAGUYA_SUPPORT_MULTIPLE_SHARED_LIBRARY
inline const char *metatable_name_key() { return "\x80KAGUYA_N_KEY"; }
inline const char *metatable_type_table_key() { return "\x80KAGUYA_T_KEY"; }
#else
inline void *metatable_name_key() {
  static int key;
  return &key;
}
inline void *metatable_type_table_key() {
  static int key;
  return &key;
}
#endif

template <typename T> const std::type_info &metatableType() {
  return typeid(typename traits::decay<T>::type);
}
template <typename T> inline std::string metatableName() {
  return util::pretty_name(metatableType<T>());
}

struct ObjectWrapperBase {
  virtual const void *cget() = 0;
  virtual void *get() = 0;

  virtual const std::type_info &type() = 0;

  virtual const std::type_info &native_type() { return type(); }
  virtual void *native_get() { return get(); }

  ObjectWrapperBase() {}
  virtual ~ObjectWrapperBase() {}

private:
  // noncopyable
  ObjectWrapperBase(const ObjectWrapperBase &);
  ObjectWrapperBase &operator=(const ObjectWrapperBase &);
};

template <class T> struct ObjectWrapper : ObjectWrapperBase {
#if KAGUYA_USE_CPP11
  template <class... Args>
  ObjectWrapper(Args &&... args) : object_(std::forward<Args>(args)...) {}
#else

  ObjectWrapper() : object_() {}
#define KAGUYA_OBJECT_WRAPPER_CONSTRUCTOR_DEF(N)                               \
  template <KAGUYA_PP_TEMPLATE_DEF_REPEAT(N)>                                  \
  ObjectWrapper(KAGUYA_PP_ARG_DEF_REPEAT(N))                                   \
      : object_(KAGUYA_PP_ARG_REPEAT(N)) {}

  KAGUYA_PP_REPEAT_DEF(KAGUYA_FUNCTION_MAX_ARGS,
                       KAGUYA_OBJECT_WRAPPER_CONSTRUCTOR_DEF)
#undef KAGUYA_OBJECT_WRAPPER_CONSTRUCTOR_DEF
#endif

  virtual const std::type_info &type() { return metatableType<T>(); }

  virtual void *get() { return &object_; }
  virtual const void *cget() { return &object_; }

private:
  T object_;
};

struct ObjectSharedPointerWrapper : ObjectWrapperBase {
  template <typename T>
  ObjectSharedPointerWrapper(const standard::shared_ptr<T> &sptr)
      : object_(standard::const_pointer_cast<
                typename standard::remove_const<T>::type>(sptr)),
        type_(metatableType<T>()),
        shared_ptr_type_(
            metatableType<
                standard::shared_ptr<typename traits::decay<T>::type> >()),
        const_value_(traits::is_const<T>::value) {}
#if KAGUYA_USE_RVALUE_REFERENCE
  template <typename T>
  ObjectSharedPointerWrapper(standard::shared_ptr<T> &&sptr)
      : object_(std::move(standard::const_pointer_cast<
                          typename standard::remove_const<T>::type>(sptr))),
        type_(metatableType<T>()),
        shared_ptr_type_(
            metatableType<
                standard::shared_ptr<typename traits::decay<T>::type> >()),
        const_value_(traits::is_const<T>::value) {}
#endif
  virtual const std::type_info &type() { return type_; }
  virtual void *get() { return const_value_ ? 0 : object_.get(); }
  virtual const void *cget() { return object_.get(); }
  standard::shared_ptr<void> object() const {
    return const_value_ ? standard::shared_ptr<void>() : object_;
  }
  standard::shared_ptr<const void> const_object() const { return object_; }
  const std::type_info &shared_ptr_type() const { return shared_ptr_type_; }

  virtual const std::type_info &native_type() {
    return metatableType<standard::shared_ptr<void> >();
  }
  virtual void *native_get() { return &object_; }

private:
  standard::shared_ptr<void> object_;
  const std::type_info &type_;

  const std::type_info &shared_ptr_type_;
  bool const_value_;
};

template <typename T, typename ElementType = typename T::element_type>
struct ObjectSmartPointerWrapper : ObjectWrapperBase {
  ObjectSmartPointerWrapper(const T &sptr) : object_(sptr) {}
#if KAGUYA_USE_RVALUE_REFERENCE
  ObjectSmartPointerWrapper(T &&sptr) : object_(std::move(sptr)) {}
#endif
  virtual const std::type_info &type() { return metatableType<ElementType>(); }
  virtual void *get() { return object_ ? &(*object_) : 0; }
  virtual const void *cget() { return object_ ? &(*object_) : 0; }
  virtual const std::type_info &native_type() { return metatableType<T>(); }
  virtual void *native_get() { return &object_; }

private:
  T object_;
};

template <class T> struct ObjectPointerWrapper : ObjectWrapperBase {
  ObjectPointerWrapper(T *ptr) : object_(ptr) {}

  virtual const std::type_info &type() { return metatableType<T>(); }
  virtual void *get() {
    if (traits::is_const<T>::value) {
      return 0;
    }
    return const_cast<void *>(static_cast<const void *>(object_));
  }
  virtual const void *cget() { return object_; }
  ~ObjectPointerWrapper() {}

protected:
  T *object_;
};

// Customizable for ObjectPointerWrapper
template <class T, class Enable = void> struct ObjectPointerWrapperType {
  typedef ObjectPointerWrapper<T> type;
};

// for internal use
struct PointerConverter {
  template <typename T, typename F> static void *base_pointer_cast(void *from) {
    return static_cast<T *>(static_cast<F *>(from));
  }
  template <typename T, typename F>
  static standard::shared_ptr<void>
  base_shared_pointer_cast(const standard::shared_ptr<void> &from) {
    return standard::shared_ptr<T>(standard::static_pointer_cast<F>(from));
  }

  typedef void *(*convert_function_type)(void *);
  typedef standard::shared_ptr<void> (*shared_ptr_convert_function_type)(
      const standard::shared_ptr<void> &);
  typedef std::pair<std::string, std::string> convert_map_key;

  template <typename ToType, typename FromType> void add_type_conversion() {
    add_function(metatableType<ToType>(), metatableType<FromType>(),
                 &base_pointer_cast<ToType, FromType>);
    add_function(metatableType<standard::shared_ptr<ToType> >(),
                 metatableType<standard::shared_ptr<FromType> >(),
                 &base_shared_pointer_cast<ToType, FromType>);
  }

  template <typename TO> TO *get_pointer(ObjectWrapperBase *from) const {
    const std::type_info &to_type = metatableType<TO>();
    // unreachable
    // if (to_type == from->type())
    //{
    //	return static_cast<TO*>(from->get());
    //}
    std::map<convert_map_key,
             std::vector<convert_function_type> >::const_iterator match =
        function_map_.find(
            convert_map_key(to_type.name(), from->type().name()));
    if (match != function_map_.end()) {
      return static_cast<TO *>(pcvt_list_apply(from->get(), match->second));
    }
    return 0;
  }
  template <typename TO>
  const TO *get_const_pointer(ObjectWrapperBase *from) const {
    const std::type_info &to_type = metatableType<TO>();
    // unreachable
    // if (to_type == from->type())
    //{
    //	return static_cast<const TO*>(from->cget());
    //}
    std::map<convert_map_key,
             std::vector<convert_function_type> >::const_iterator match =
        function_map_.find(
            convert_map_key(to_type.name(), from->type().name()));
    if (match != function_map_.end()) {
      return static_cast<const TO *>(
          pcvt_list_apply(const_cast<void *>(from->cget()), match->second));
    }
    return 0;
  }

  template <typename TO>
  standard::shared_ptr<TO>
  get_shared_pointer(ObjectSharedPointerWrapper *from) const {
    const std::type_info &to_type = metatableType<
        standard::shared_ptr<typename traits::decay<TO>::type> >();
    // unreachable
    //			if (to_type == from->type())
    //			{
    //				return
    // standard::static_pointer_cast<TO>(from->object());
    //			}
    const std::type_info &from_type = from->shared_ptr_type();
    std::map<convert_map_key,
             std::vector<shared_ptr_convert_function_type> >::const_iterator
        match = shared_ptr_function_map_.find(
            convert_map_key(to_type.name(), from_type.name()));
    if (match != shared_ptr_function_map_.end()) {
      standard::shared_ptr<void> sptr = from->object();

      if (!sptr && standard::is_const<TO>::value) {
        sptr = standard::const_pointer_cast<void>(from->const_object());
      }

      return standard::static_pointer_cast<TO>(
          pcvt_list_apply(sptr, match->second));
    }
    return standard::shared_ptr<TO>();
  }

  template <class T>
  T *get_pointer(ObjectWrapperBase *from, types::typetag<T>) {
    return get_pointer<T>(from);
  }
  template <class T>
  standard::shared_ptr<T>
  get_pointer(ObjectWrapperBase *from,
              types::typetag<standard::shared_ptr<T> >) {
    ObjectSharedPointerWrapper *ptr =
        dynamic_cast<ObjectSharedPointerWrapper *>(from);
    if (ptr) {
      return get_shared_pointer<T>(ptr);
    }
    return standard::shared_ptr<T>();
  }

  static int deleter(lua_State *state) {
    PointerConverter *ptr = (PointerConverter *)lua_touserdata(state, 1);
    ptr->~PointerConverter();
    return 0;
  }

  static PointerConverter &get(lua_State *state) {
#if KAGUYA_SUPPORT_MULTIPLE_SHARED_LIBRARY
    const char *kaguya_ptrcvt_key_ptr = "\x80KAGUYA_CVT_KEY";
#else
    static char kaguya_ptrcvt_key_ptr;
#endif
    util::ScopedSavedStack save(state);
#if KAGUYA_SUPPORT_MULTIPLE_SHARED_LIBRARY
    lua_pushstring(state, kaguya_ptrcvt_key_ptr);
#else
    lua_pushlightuserdata(state, &kaguya_ptrcvt_key_ptr);
#endif
    lua_rawget(state, LUA_REGISTRYINDEX);
    if (lua_isuserdata(state, -1)) {
      return *static_cast<PointerConverter *>(lua_touserdata(state, -1));
    } else {
      void *ptr = lua_newuserdata(
          state, sizeof(PointerConverter)); // dummy data for gc call
      PointerConverter *converter = new (ptr) PointerConverter();

      lua_createtable(state, 0, 2);
      lua_pushcclosure(state, &deleter, 0);
      lua_setfield(state, -2, "__gc");
      lua_pushvalue(state, -1);
      lua_setfield(state, -2, "__index");
      lua_setmetatable(state, -2); // set to userdata
#if KAGUYA_SUPPORT_MULTIPLE_SHARED_LIBRARY
      lua_pushstring(state, kaguya_ptrcvt_key_ptr);
#else
      lua_pushlightuserdata(state, &kaguya_ptrcvt_key_ptr);
#endif
      lua_pushvalue(state, -2);
      lua_rawset(state, LUA_REGISTRYINDEX);
      return *converter;
    }
  }

private:
  void add_function(const std::type_info &to_type,
                    const std::type_info &from_type, convert_function_type f) {
    std::map<convert_map_key, std::vector<convert_function_type> > add_map;
    for (std::map<convert_map_key,
                  std::vector<convert_function_type> >::iterator it =
             function_map_.begin();
         it != function_map_.end(); ++it) {
      if (it->first.second == to_type.name()) {
        std::vector<convert_function_type> newlist;
        newlist.push_back(f);
        newlist.insert(newlist.end(), it->second.begin(), it->second.end());
        add_map[convert_map_key(it->first.first, from_type.name())] = newlist;
      }
    }
    function_map_.insert(add_map.begin(), add_map.end());

    std::vector<convert_function_type> flist;
    flist.push_back(f);
    function_map_[convert_map_key(to_type.name(), from_type.name())] = flist;
  }
  void add_function(const std::type_info &to_type,
                    const std::type_info &from_type,
                    shared_ptr_convert_function_type f) {
    std::map<convert_map_key, std::vector<shared_ptr_convert_function_type> >
        add_map;
    for (std::map<convert_map_key,
                  std::vector<shared_ptr_convert_function_type> >::iterator it =
             shared_ptr_function_map_.begin();
         it != shared_ptr_function_map_.end(); ++it) {
      if (it->first.second == to_type.name()) {
        std::vector<shared_ptr_convert_function_type> newlist;
        newlist.push_back(f);
        newlist.insert(newlist.end(), it->second.begin(), it->second.end());
        add_map[convert_map_key(it->first.first, from_type.name())] = newlist;
      }
    }
    shared_ptr_function_map_.insert(add_map.begin(), add_map.end());

    std::vector<shared_ptr_convert_function_type> flist;
    flist.push_back(f);
    shared_ptr_function_map_[convert_map_key(to_type.name(),
                                             from_type.name())] = flist;
  }

  void *pcvt_list_apply(void *ptr,
                        const std::vector<convert_function_type> &flist) const {
    for (std::vector<convert_function_type>::const_iterator i = flist.begin();
         i != flist.end(); ++i) {
      ptr = (*i)(ptr);
    }
    return ptr;
  }
  standard::shared_ptr<void> pcvt_list_apply(
      standard::shared_ptr<void> ptr,
      const std::vector<shared_ptr_convert_function_type> &flist) const {
    for (std::vector<shared_ptr_convert_function_type>::const_iterator i =
             flist.begin();
         i != flist.end(); ++i) {

#if KAGUYA_USE_CPP11
      ptr = (*i)(std::move(ptr));
#else
      ptr = (*i)(ptr);
#endif
    }
    return ptr;
  }

  PointerConverter() {}

  std::map<convert_map_key, std::vector<convert_function_type> > function_map_;
  std::map<convert_map_key, std::vector<shared_ptr_convert_function_type> >
      shared_ptr_function_map_;

  PointerConverter(PointerConverter &);
  PointerConverter &operator=(PointerConverter &);
};

namespace detail {
inline bool object_wrapper_type_check(lua_State *l, int index) {
#if KAGUYA_NO_USERDATA_TYPE_CHECK
  return lua_isuserdata(l, index) && !lua_islightuserdata(l, index);
#endif
  if (lua_getmetatable(l, index)) {
#if KAGUYA_SUPPORT_MULTIPLE_SHARED_LIBRARY
    lua_pushstring(l, metatable_name_key());
#else
    lua_pushlightuserdata(l, metatable_name_key());
#endif
    int type = lua_rawget_rtype(l, -2);
    lua_pop(l, 2);
    return type == LUA_TSTRING;
  }
  return false;
}
}

inline ObjectWrapperBase *object_wrapper(lua_State *l, int index) {
  if (detail::object_wrapper_type_check(l, index)) {
    ObjectWrapperBase *ptr =
        static_cast<ObjectWrapperBase *>(lua_touserdata(l, index));
    return ptr;
  }
  return 0;
}

template <typename RequireType>
inline ObjectWrapperBase *
object_wrapper(lua_State *l, int index, bool convert = true,
               types::typetag<RequireType> = types::typetag<RequireType>()) {
  if (detail::object_wrapper_type_check(l, index)) {
    ObjectWrapperBase *ptr =
        static_cast<ObjectWrapperBase *>(lua_touserdata(l, index));
#if KAGUYA_NAME_BASED_TYPE_CHECK
    if (strcmp(ptr->type().name(), metatableType<RequireType>().name()) == 0) {
#else
    if (ptr->type() == metatableType<RequireType>()) {
#endif
      return ptr;
    } else if (convert) {
      PointerConverter &pcvt = PointerConverter::get(l);
      return pcvt.get_pointer(ptr, types::typetag<RequireType>()) ? ptr : 0;
    }
    return 0;
  }
  return 0;
}

template <class T> T *get_pointer(lua_State *l, int index, types::typetag<T>) {
  int type = lua_type(l, index);

  if (type == LUA_TLIGHTUSERDATA) {
    return (T *)lua_topointer(l, index);
  } else if (type != LUA_TUSERDATA) {
    return 0;
  } else {
    ObjectWrapperBase *objwrapper = object_wrapper(l, index);
    if (objwrapper) {
      const std::type_info &to_type = metatableType<T>();
#if KAGUYA_NAME_BASED_TYPE_CHECK
      if (strcmp(objwrapper->type().name(), to_type.name()) == 0) {
#else
      if (objwrapper->type() == to_type) {
#endif
        return static_cast<T *>(objwrapper->get());
      }
      if (objwrapper->native_type() == to_type) {
        return static_cast<T *>(objwrapper->native_get());
      } else {
        PointerConverter &pcvt = PointerConverter::get(l);
        return pcvt.get_pointer<T>(objwrapper);
      }
    }
  }
  return 0;
}
template <class T>
const T *get_const_pointer(lua_State *l, int index, types::typetag<T>) {
  int type = lua_type(l, index);

  if (type == LUA_TLIGHTUSERDATA) {
    return (T *)lua_topointer(l, index);
  } else if (type != LUA_TUSERDATA) {
    return 0;
  } else {
    ObjectWrapperBase *objwrapper = object_wrapper(l, index);
    if (objwrapper) {
#if KAGUYA_NAME_BASED_TYPE_CHECK
      if (strcmp(objwrapper->type().name(), metatableType<T>().name()) == 0) {
#else
      if (objwrapper->type() == metatableType<T>()) {
#endif
        return static_cast<const T *>(objwrapper->cget());
      } else {
        PointerConverter &pcvt = PointerConverter::get(l);
        return pcvt.get_const_pointer<T>(objwrapper);
      }
    }
  }
  return 0;
}
template <class T>
const T *get_pointer(lua_State *l, int index, types::typetag<const T>) {
  return get_const_pointer<T>(l, index, types::typetag<T>());
}

template <class T>
standard::shared_ptr<T> get_shared_pointer(lua_State *l, int index,
                                           types::typetag<T>) {
  ObjectSharedPointerWrapper *ptr =
      dynamic_cast<ObjectSharedPointerWrapper *>(object_wrapper(l, index));
  if (ptr) {
    const std::type_info &from_type = ptr->shared_ptr_type();
    const std::type_info &to_type =
        metatableType<standard::shared_ptr<typename traits::decay<T>::type> >();
#if KAGUYA_NAME_BASED_TYPE_CHECK
    if (strcmp(from_type.name(), to_type.name()) == 0) {
#else
    if (from_type == to_type) {
#endif
      if (standard::is_const<T>::value) {
        return standard::static_pointer_cast<T>(
            standard::const_pointer_cast<void>(ptr->const_object()));
      } else {
        return standard::static_pointer_cast<T>(ptr->object());
      }
    }
    PointerConverter &pcvt = PointerConverter::get(l);
    return pcvt.get_shared_pointer<T>(ptr);
  }
  return standard::shared_ptr<T>();
}
inline standard::shared_ptr<void> get_shared_pointer(lua_State *l, int index,
                                                     types::typetag<void>) {
  ObjectSharedPointerWrapper *ptr =
      dynamic_cast<ObjectSharedPointerWrapper *>(object_wrapper(l, index));
  if (ptr) {
    return ptr->object();
  }
  return standard::shared_ptr<void>();
}
inline standard::shared_ptr<const void>
get_shared_pointer(lua_State *l, int index, types::typetag<const void>) {
  ObjectSharedPointerWrapper *ptr =
      dynamic_cast<ObjectSharedPointerWrapper *>(object_wrapper(l, index));
  if (ptr) {
    return ptr->const_object();
  }
  return standard::shared_ptr<const void>();
}

namespace class_userdata {
template <typename T> inline void destructor(T *pointer) {
  if (pointer) {
    pointer->~T();
  }
}
inline bool get_metatable(lua_State *l, const std::type_info &typeinfo) {
#if KAGUYA_SUPPORT_MULTIPLE_SHARED_LIBRARY
  lua_pushstring(l, metatable_type_table_key());
#else
  lua_pushlightuserdata(l, metatable_type_table_key());
#endif
  int ttype =
      lua_rawget_rtype(l, LUA_REGISTRYINDEX); // get metatable registry table
  if (ttype != LUA_TTABLE) {
#if KAGUYA_SUPPORT_MULTIPLE_SHARED_LIBRARY
    lua_pushstring(l, metatable_type_table_key());
#else
    lua_pushlightuserdata(l, metatable_type_table_key());
#endif
    lua_newtable(l);
    lua_rawset(l, LUA_REGISTRYINDEX);
    return false;
  }
#if KAGUYA_NAME_BASED_TYPE_CHECK
  lua_pushstring(l, typeinfo.name());
  int type = lua_rawget_rtype(l, -2);
#else
  int type = lua_rawgetp_rtype(l, -1, &typeinfo);
#endif
  lua_remove(l, -2); // remove metatable registry table
  return type != LUA_TNIL;
}
template <typename T> bool get_metatable(lua_State *l) {
  return get_metatable(l, metatableType<T>());
}
template <typename T> bool available_metatable(lua_State *l) {
  util::ScopedSavedStack save(l);
  return get_metatable<T>(l);
}

inline bool newmetatable(lua_State *l, const std::type_info &typeinfo,
                         const char *name) {
  if (get_metatable(l, typeinfo)) // already register
  {
    return false; //
  }
  lua_pop(l, 1);

// get metatable registry table
#if KAGUYA_SUPPORT_MULTIPLE_SHARED_LIBRARY
  lua_getfield(l, LUA_REGISTRYINDEX, metatable_type_table_key());
#else
  lua_rawgetp_rtype(l, LUA_REGISTRYINDEX, metatable_type_table_key());
#endif

  int metaregindex = lua_absindex(l, -1);

  lua_createtable(l, 0, 2);
  lua_pushstring(l, name);
  lua_setfield(l, -2, "__name"); // metatable.__name = name

#if KAGUYA_SUPPORT_MULTIPLE_SHARED_LIBRARY
  lua_pushstring(l, metatable_name_key());
#else
  lua_pushlightuserdata(l, metatable_name_key());
#endif
  lua_pushstring(l, name);
  lua_rawset(l, -3);
#if KAGUYA_NAME_BASED_TYPE_CHECK
  lua_pushstring(l, typeinfo.name());
  lua_pushvalue(l, -2);
  lua_rawset(l, metaregindex);
#else
  lua_pushvalue(l, -1);
  lua_rawsetp(l, metaregindex, &typeinfo);
#endif
  lua_remove(l, metaregindex); // remove metatable registry table

  return true;
}
template <typename T> bool newmetatable(lua_State *l) {
  return newmetatable(l, metatableType<T>(), metatableName<T>().c_str());
}

template <typename T> inline int deleter(lua_State *state) {
  T *ptr = (T *)lua_touserdata(state, 1);
  ptr->~T();
  return 0;
}
struct UnknownType {};
inline void setmetatable(lua_State *l, const std::type_info &typeinfo) {
  // if not available metatable, set unknown class metatable
  if (!get_metatable(l, typeinfo)) {
    lua_pop(l, 1);
    if (!get_metatable<UnknownType>(l)) {
      lua_pop(l, 1);
      newmetatable<UnknownType>(l);
      lua_pushcclosure(l, &deleter<ObjectWrapperBase>, 0);
      lua_setfield(l, -2, "__gc");
    }
  }
  lua_setmetatable(l, -2);
}
template <typename T> void setmetatable(lua_State *l) {
  setmetatable(l, metatableType<T>());
}
}
template <typename T>
bool available_metatable(lua_State *l,
                         types::typetag<T> = types::typetag<T>()) {
  return class_userdata::available_metatable<T>(l);
}

namespace util {
template <typename T> inline bool object_checkType(lua_State *l, int index) {
  return object_wrapper<T>(l, index) != 0;
}
template <typename T>
inline bool object_strictCheckType(lua_State *l, int index) {
  return object_wrapper<T>(l, index, false) != 0;
}

template <typename T>
inline optional<T> object_getopt(lua_State *l, int index) {
  const typename traits::remove_reference<T>::type *pointer = get_const_pointer(
      l, index, types::typetag<typename traits::remove_reference<T>::type>());
  if (!pointer) {
    return optional<T>();
  }
  return *pointer;
}

template <typename T> inline T object_get(lua_State *l, int index) {
  const typename traits::remove_reference<T>::type *pointer = get_const_pointer(
      l, index, types::typetag<typename traits::remove_reference<T>::type>());
  if (!pointer) {
    throw LuaTypeMismatch();
  }
  return *pointer;
}

template <typename To> struct ConvertibleRegisterHelperProxy {
  template <typename DataType>
  ConvertibleRegisterHelperProxy(DataType v)
      : holder_(new DataHolder<DataType>(v)) {}
  operator To() { return holder_->get(); }

private:
  struct DataHolderBase {
    virtual To get() const = 0;
    virtual ~DataHolderBase() {}
  };
  template <typename Type> class DataHolder : public DataHolderBase {
    typedef typename traits::decay<Type>::type DataType;

  public:
    explicit DataHolder(DataType v) : data_(v) {}
    virtual To get() const { return To(data_); }

  private:
    DataType data_;
  };
  standard::shared_ptr<DataHolderBase> holder_;
};

#if KAGUYA_USE_CPP11
template <typename T> inline int object_push(lua_State *l, T &&v) {
  typedef ObjectWrapper<typename traits::remove_const_and_reference<T>::type>
      wrapper_type;
  void *storage = lua_newuserdata(l, sizeof(wrapper_type));
  new (storage) wrapper_type(std::forward<T>(v));
  class_userdata::setmetatable<T>(l);
  return 1;
}

namespace conv_helper_detail {
template <class To> bool checkType(lua_State *, int) { return false; }
template <class To, class From, class... Remain>
bool checkType(lua_State *l, int index) {
  return lua_type_traits<From>::checkType(l, index) ||
         checkType<To, Remain...>(l, index);
}
template <class To> bool strictCheckType(lua_State *, int) { return false; }
template <class To, class From, class... Remain>
bool strictCheckType(lua_State *l, int index) {
  return lua_type_traits<From>::strictCheckType(l, index) ||
         strictCheckType<To, Remain...>(l, index);
  ;
}

template <class To> To get(lua_State *, int) { throw LuaTypeMismatch(); }
template <class To, class From, class... Remain>
To get(lua_State *l, int index) {
  typedef optional<typename lua_type_traits<From>::get_type> opt_type;
  if (auto opt = lua_type_traits<opt_type>::get(l, index)) {
    return *opt;
  }
  return get<To, Remain...>(l, index);
}
}

template <class To, class... From> struct ConvertibleRegisterHelper {
  typedef To get_type;

  static bool checkType(lua_State *l, int index) {
    if (object_checkType<To>(l, index)) {
      return true;
    }
    return conv_helper_detail::checkType<To, From...>(l, index);
  }
  static bool strictCheckType(lua_State *l, int index) {
    if (object_strictCheckType<To>(l, index)) {
      return true;
    }
    return conv_helper_detail::strictCheckType<To, From...>(l, index);
  }

  static get_type get(lua_State *l, int index) {
    if (auto opt = object_getopt<To>(l, index)) {
      return *opt;
    }
    return conv_helper_detail::get<get_type, From...>(l, index);
  }
};
#else
template <typename T> inline int object_push(lua_State *l, T v) {
  typedef ObjectWrapper<typename traits::remove_const_and_reference<T>::type>
      wrapper_type;
  void *storage = lua_newuserdata(l, sizeof(wrapper_type));
  new (storage) wrapper_type(v);
  class_userdata::setmetatable<T>(l);
  return 1;
}
namespace conv_helper_detail {
#define KAGUYA_CONVERTIBLE_REG_HELPER_CHECK_TYPE_REP(N)                        \
  || lua_type_traits<KAGUYA_PP_CAT(A, N)>::checkType(state, index)
#define KAGUYA_CONVERTIBLE_REG_HELPER_STRICT_CHECK_TYPE_REP(N)                 \
  || lua_type_traits<KAGUYA_PP_CAT(A, N)>::strictCheckType(state, index)
#define KAGUYA_CONVERTIBLE_REG_HELPER_GET_OPT_TYPEDEF(N)                       \
  typedef optional<typename lua_type_traits<KAGUYA_PP_CAT(A, N)>::get_type>    \
      KAGUYA_PP_CAT(opt_type, N);
#define KAGUYA_CONVERTIBLE_REG_HELPER_GET_REP(N)                               \
  if (KAGUYA_PP_CAT(opt_type, N) opt =                                         \
          lua_type_traits<KAGUYA_PP_CAT(opt_type, N)>::get(state, index)) {    \
    return *opt;                                                               \
  } else

template <typename To> bool checkType(lua_State *, int, TypeTuple<>) {
  return false;
}
#define KAGUYA_CONVERTIBLE_REG_HELPER_CHECK_TYPE_DEF(N)                        \
  template <typename To, KAGUYA_PP_TEMPLATE_DEF_REPEAT(N)>                     \
  bool checkType(lua_State *state, int index,                                  \
                 TypeTuple<KAGUYA_PP_TEMPLATE_ARG_REPEAT(N)>) {                \
    return false KAGUYA_PP_REPEAT(                                             \
        N, KAGUYA_CONVERTIBLE_REG_HELPER_CHECK_TYPE_REP);                      \
  }

KAGUYA_PP_REPEAT_DEF(KAGUYA_FUNCTION_MAX_ARGS,
                     KAGUYA_CONVERTIBLE_REG_HELPER_CHECK_TYPE_DEF)
#undef KAGUYA_CONVERTIBLE_REG_HELPER_CHECK_TYPE_DEF
template <typename To> bool strictCheckType(lua_State *, int, TypeTuple<>) {
  return false;
}
#define KAGUYA_CONVERTIBLE_REG_HELPER_ST_CHECK_TYPE_DEF(N)                     \
  template <typename To, KAGUYA_PP_TEMPLATE_DEF_REPEAT(N)>                     \
  bool strictCheckType(lua_State *state, int index,                            \
                       TypeTuple<KAGUYA_PP_TEMPLATE_ARG_REPEAT(N)>) {          \
    return false KAGUYA_PP_REPEAT(                                             \
        N, KAGUYA_CONVERTIBLE_REG_HELPER_STRICT_CHECK_TYPE_REP);               \
  }
KAGUYA_PP_REPEAT_DEF(KAGUYA_FUNCTION_MAX_ARGS,
                     KAGUYA_CONVERTIBLE_REG_HELPER_ST_CHECK_TYPE_DEF)
#undef KAGUYA_CONVERTIBLE_REG_HELPER_ST_CHECK_TYPE_DEF
#define KAGUYA_CONVERTIBLE_REG_HELPER_GET_DEF(N)                               \
  template <typename To, KAGUYA_PP_TEMPLATE_DEF_REPEAT(N)>                     \
  To get(lua_State *state, int index,                                          \
         TypeTuple<KAGUYA_PP_TEMPLATE_ARG_REPEAT(N)>) {                        \
    KAGUYA_PP_REPEAT(N, KAGUYA_CONVERTIBLE_REG_HELPER_GET_OPT_TYPEDEF)         \
    KAGUYA_PP_REPEAT(N, KAGUYA_CONVERTIBLE_REG_HELPER_GET_REP) {               \
      throw LuaTypeMismatch();                                                 \
    }                                                                          \
  }
KAGUYA_PP_REPEAT_DEF(KAGUYA_FUNCTION_MAX_ARGS,
                     KAGUYA_CONVERTIBLE_REG_HELPER_GET_DEF)
#undef KAGUYA_CONVERTIBLE_REG_HELPER_GET_DEF

#undef KAGUYA_CONVERTIBLE_REG_HELPER_CHECK_TYPE_REP
#undef KAGUYA_CONVERTIBLE_REG_HELPER_STRICT_CHECK_TYPE_REP
#undef KAGUYA_CONVERTIBLE_REG_HELPER_ST_GET_REP
#undef KAGUYA_CONVERTIBLE_REG_HELPER_GET_REP
}

#define KAGUYA_PP_CONVERTIBLE_REG_HELPER_DEF_REP(N)                            \
  KAGUYA_PP_CAT(typename A, N) = null_type
#define KAGUYA_PP_CONVERTIBLE_REG_HELPER_DEF_REPEAT(N)                         \
  KAGUYA_PP_REPEAT_ARG(N, KAGUYA_PP_CONVERTIBLE_REG_HELPER_DEF_REP)

template <typename To,
          KAGUYA_PP_CONVERTIBLE_REG_HELPER_DEF_REPEAT(KAGUYA_FUNCTION_MAX_ARGS)>
struct ConvertibleRegisterHelper {
  typedef To get_type;
  typedef TypeTuple<KAGUYA_PP_TEMPLATE_ARG_REPEAT(KAGUYA_FUNCTION_MAX_ARGS)>
      conv_types;

  static bool checkType(lua_State *l, int index) {
    if (object_checkType<To>(l, index)) {
      return true;
    }
    return conv_helper_detail::checkType<To>(l, index, conv_types());
  }
  static bool strictCheckType(lua_State *l, int index) {
    if (object_strictCheckType<To>(l, index)) {
      return true;
    }
    return conv_helper_detail::strictCheckType<To>(l, index, conv_types());
  }
  static get_type get(lua_State *l, int index) {
    if (optional<To> opt = object_getopt<To>(l, index)) {
      return *opt;
    }
    return conv_helper_detail::get<get_type>(l, index, conv_types());
  }
};
#undef KAGUYA_PP_CONVERTIBLE_REG_HELPER_DEF_REP
#undef KAGUYA_PP_CONVERTIBLE_REG_HELPER_DEF_REPEAT
#endif
}
}
// Copyright satoren
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)



namespace kaguya {
#if KAGUYA_USE_CPP11
namespace detail {
template <std::size_t... indexes> struct index_tuple {};
template <std::size_t first, std::size_t last, class result = index_tuple<>,
          bool flag = first >= last>
struct index_range {
  using type = result;
};
template <std::size_t step, std::size_t last, std::size_t... indexes>
struct index_range<step, last, index_tuple<indexes...>, false>
    : index_range<step + 1, last, index_tuple<indexes..., step> > {};

template <std::size_t... Indexes, class... Args>
int push_tuple(lua_State *l, index_tuple<Indexes...>, std::tuple<Args...> &&v) {
  return util::push_args(l, std::get<Indexes>(v)...);
}
}

/// @ingroup lua_type_traits
/// @brief lua_type_traits for std::tuple or boost::tuple
template <class... Args> struct lua_type_traits<standard::tuple<Args...> > {
  static int push(lua_State *l, std::tuple<Args...> &&v) {
    typename detail::index_range<0, sizeof...(Args)>::type index;
    return detail::push_tuple(l, index, std::forward<std::tuple<Args...> >(v));
  }
};
#else
#define KAGUYA_PP_GET_DATA(N) standard::get<N - 1>(v)
#define KAGUYA_PUSH_TUPLE_DEF(N)                                               \
  template <KAGUYA_PP_TEMPLATE_DEF_REPEAT(N)>                                  \
  struct lua_type_traits<standard::tuple<KAGUYA_PP_TEMPLATE_ARG_REPEAT(N)> > { \
    static int                                                                 \
    push(lua_State *l,                                                         \
         const standard::tuple<KAGUYA_PP_TEMPLATE_ARG_REPEAT(N)> &v) {         \
      return util::push_args(l, KAGUYA_PP_REPEAT_ARG(N, KAGUYA_PP_GET_DATA));  \
    }                                                                          \
  };
KAGUYA_PP_REPEAT_DEF(KAGUYA_FUNCTION_MAX_TUPLE_SIZE, KAGUYA_PUSH_TUPLE_DEF)
#undef KAGUYA_PP_GET_DATA
#undef KAGUYA_PUSH_TUPLE_DEF
#endif
}

namespace kaguya {

// default implements
template <typename T, typename Enable>
bool lua_type_traits<T, Enable>::checkType(lua_State *l, int index) {
  return object_wrapper<T>(l, index) != 0;
}
template <typename T, typename Enable>
bool lua_type_traits<T, Enable>::strictCheckType(lua_State *l, int index) {
  return object_wrapper<T>(l, index, false) != 0;
}
template <typename T, typename Enable>
typename lua_type_traits<T, Enable>::opt_type
lua_type_traits<T, Enable>::opt(lua_State *l, int index) KAGUYA_NOEXCEPT {
  const typename traits::remove_reference<T>::type *pointer = get_const_pointer(
      l, index, types::typetag<typename traits::remove_reference<T>::type>());
  if (!pointer) {
    return opt_type();
  }
  return *pointer;
}
template <typename T, typename Enable>
typename lua_type_traits<T, Enable>::get_type
lua_type_traits<T, Enable>::get(lua_State *l, int index) {
  const typename traits::remove_reference<T>::type *pointer = get_const_pointer(
      l, index, types::typetag<typename traits::remove_reference<T>::type>());
  if (!pointer) {
    throw LuaTypeMismatch();
  }
  return *pointer;
}
template <typename T, typename Enable>
int lua_type_traits<T, Enable>::push(lua_State *l, push_type v) {
  return util::object_push(l, v);
}

#if KAGUYA_USE_RVALUE_REFERENCE

template <typename T, typename Enable>
int lua_type_traits<T, Enable>::push(lua_State *l, NCRT &&v) {
  return util::object_push(l, std::forward<NCRT>(v));
}
#endif

/// @ingroup lua_type_traits
/// @brief lua_type_traits for const reference type
template <typename T>
struct lua_type_traits<
    T, typename traits::enable_if<traits::is_const_reference<T>::value>::type>
    : lua_type_traits<typename traits::remove_const_reference<T>::type> {};

/// @ingroup lua_type_traits
/// @brief lua_type_traits for lvalue reference type
template <typename REF>
struct lua_type_traits<
    REF, typename traits::enable_if<
             traits::is_lvalue_reference<REF>::value &&
             !traits::is_const<
                 typename traits::remove_reference<REF>::type>::value>::type> {
  typedef void Registerable;

  typedef REF get_type;
  typedef optional<get_type> opt_type;
  typedef REF push_type;
  typedef typename traits::remove_reference<REF>::type T;

  static bool strictCheckType(lua_State *l, int index) {
    return object_wrapper<T>(l, index, false) != 0;
  }
  static bool checkType(lua_State *l, int index) {
    if (lua_type(l, index) == LUA_TLIGHTUSERDATA) {
      return true;
    }
    return object_wrapper<T>(l, index) != 0;
  }
  static get_type get(lua_State *l, int index) {
    T *pointer = get_pointer(l, index, types::typetag<T>());
    if (!pointer) {
      throw LuaTypeMismatch();
    }
    return *pointer;
  }
  static opt_type opt(lua_State *l, int index) KAGUYA_NOEXCEPT {
    T *pointer = get_pointer(l, index, types::typetag<T>());
    if (!pointer) {
      return opt_type();
    }
    return opt_type(*pointer);
  }
  static int push(lua_State *l, push_type v) {
    if (!available_metatable<T>(l)) {
      lua_pushlightuserdata(
          l, const_cast<typename traits::remove_const<T>::type *>(&v));
    } else {
      typedef typename ObjectPointerWrapperType<T>::type wrapper_type;
      void *storage = lua_newuserdata(l, sizeof(wrapper_type));
      new (storage) wrapper_type(&v);
      class_userdata::setmetatable<T>(l);
    }
    return 1;
  }
};

/// @ingroup lua_type_traits
/// @brief lua_type_traits for pointer type
template <typename PTR>
struct lua_type_traits<
    PTR, typename traits::enable_if<
             traits::is_pointer<
                 typename traits::remove_const_reference<PTR>::type>::value &&
             !traits::is_function<
                 typename traits::remove_pointer<PTR>::type>::value>::type> {
  typedef void Registerable;

  typedef PTR get_type;
  typedef optional<get_type> opt_type;
  typedef PTR push_type;
  typedef typename traits::remove_pointer<PTR>::type T;

  static bool strictCheckType(lua_State *l, int index) {
    return object_wrapper<T>(l, index, false) != 0;
  }
  static bool checkType(lua_State *l, int index) {
    int type = lua_type(l, index);
    if (type == LUA_TLIGHTUSERDATA || type == LUA_TNIL || type == LUA_TNONE) {
      return true;
    }
    return object_wrapper<T>(l, index) != 0;
  }
  static get_type get(lua_State *l, int index) {
    int type = lua_type(l, index);
    if (type == LUA_TUSERDATA || type == LUA_TLIGHTUSERDATA) {
      return get_pointer(l, index, types::typetag<T>());
    }

    if (type == LUA_TNIL || type == LUA_TNONE) {
      return 0;
    }
    throw LuaTypeMismatch();
    return 0;
  }
  static opt_type opt(lua_State *l, int index) KAGUYA_NOEXCEPT {
    int type = lua_type(l, index);
    if (type == LUA_TUSERDATA || type == LUA_TLIGHTUSERDATA) {
      return get_pointer(l, index, types::typetag<T>());
    }
    if (type == LUA_TNIL || type == LUA_TNONE) {
      return opt_type(0);
    }
    return opt_type();
  }
  static int push(lua_State *l, push_type v) {
    if (!v) {
      lua_pushnil(l);
    } else if (!available_metatable<T>(l)) {
      lua_pushlightuserdata(
          l, const_cast<typename traits::remove_const<T>::type *>(v));
    } else {
      typedef typename ObjectPointerWrapperType<T>::type wrapper_type;
      void *storage = lua_newuserdata(l, sizeof(wrapper_type));
      new (storage) wrapper_type(v);
      class_userdata::setmetatable<T>(l);
    }
    return 1;
  }
};

/// @ingroup lua_type_traits
/// @brief lua_type_traits for bool
template <> struct lua_type_traits<bool> {
  typedef bool get_type;
  typedef optional<get_type> opt_type;
  typedef bool push_type;

  static bool strictCheckType(lua_State *l, int index) {
    return lua_type(l, index) == LUA_TBOOLEAN;
  }
  static bool checkType(lua_State *l, int index) {
    KAGUYA_UNUSED(l);
    KAGUYA_UNUSED(index);
    return true;
  }
  static bool get(lua_State *l, int index) {
    return l && lua_toboolean(l, index) != 0;
  }
  static opt_type opt(lua_State *l, int index) KAGUYA_NOEXCEPT {
    if (l) {
      return opt_type(lua_toboolean(l, index) != 0);
    } else {
      return opt_type();
    }
  }
  static int push(lua_State *l, bool s) {
    lua_pushboolean(l, s);
    return 1;
  }
};

/// @ingroup lua_type_traits
/// @brief lua_type_traits for void
template <> struct lua_type_traits<void> {
  typedef void *get_type;
  typedef void *push_type;

  static bool strictCheckType(lua_State *, int) { return true; }
  static bool checkType(lua_State *, int) { return true; }
  static get_type get(lua_State *, int) { return 0; }
  static int push(lua_State *, push_type) { return 0; }
};

/// @ingroup lua_type_traits
/// @brief lua_type_traits for reference_wrapper
template <typename T> struct lua_type_traits<standard::reference_wrapper<T> > {
  typedef const standard::reference_wrapper<T> &push_type;

  static int push(lua_State *l, push_type v) {
    return util::push_args(l, &v.get());
  }
};

namespace detail {

template <typename T, typename Enable = void>
struct has_optional_get : traits::false_type {};
template <typename T>
struct has_optional_get<
    T, typename traits::enable_if<!traits::is_same<
           void, typename lua_type_traits<T>::opt_type>::value>::type>
    : traits::true_type {};

template <typename T>
typename traits::enable_if<has_optional_get<T>::value, optional<T> >::type
opt_helper(lua_State *state, int index) KAGUYA_NOEXCEPT {
  return lua_type_traits<T>::opt(state, index);
}
template <typename T>
typename traits::enable_if<!has_optional_get<T>::value, optional<T> >::type
opt_helper(lua_State *state, int index) {
  try {
    return lua_type_traits<T>::get(state, index);
  } catch (...) {
    return optional<T>();
  }
}
}

/// @ingroup lua_type_traits
/// @brief lua_type_traits for optional
template <typename T> struct lua_type_traits<optional<T> > {
  typedef const optional<T> &push_type;
  typedef optional<T> get_type;

  static bool strictCheckType(lua_State *l, int index) {
    return lua_type_traits<T>::strictCheckType(l, index);
  }
  static bool checkType(lua_State *l, int index) {
    KAGUYA_UNUSED(l);
    KAGUYA_UNUSED(index);
    return true;
  }
  static get_type get(lua_State *l, int index) KAGUYA_NOEXCEPT {
    return detail::opt_helper<T>(l, index);
  }

  static int push(lua_State *l, push_type v) KAGUYA_NOEXCEPT {
    if (v) {
      return util::push_args(l, v.value());
    } else {
      lua_pushnil(l);
    }
    return 1;
  }
};

/// @ingroup lua_type_traits
/// @brief lua_type_traits for shared_ptr
template <typename T> struct lua_type_traits<standard::shared_ptr<T> > {
  typedef const standard::shared_ptr<T> &push_type;
  typedef standard::shared_ptr<T> get_type;

  static bool strictCheckType(lua_State *l, int index) {
    ObjectSharedPointerWrapper *wrapper =
        dynamic_cast<ObjectSharedPointerWrapper *>(object_wrapper(l, index));
    if (!wrapper) {
      return false;
    }
    const std::type_info &type =
        metatableType<standard::shared_ptr<typename traits::decay<T>::type> >();
#if KAGUYA_NAME_BASED_TYPE_CHECK
    return strcmp(wrapper->shared_ptr_type().name(), type.name()) == 0;
#else
    return wrapper->shared_ptr_type() == type;
#endif
  }
  static bool checkType(lua_State *l, int index) {
    return get_shared_pointer(l, index, types::typetag<T>()) ||
           lua_isnil(l, index);
  }
  static get_type get(lua_State *l, int index) {
    if (lua_isnil(l, index)) {
      return get_type();
    }
    return get_shared_pointer(l, index, types::typetag<T>());
  }

  static int push(lua_State *l, push_type v) {
    if (v) {
      typedef ObjectSharedPointerWrapper wrapper_type;
      void *storage = lua_newuserdata(l, sizeof(wrapper_type));
      new (storage) wrapper_type(v);
      class_userdata::setmetatable<T>(l);
    } else {
      lua_pushnil(l);
    }
    return 1;
  }
};
#if KAGUYA_USE_CPP11
/// @ingroup lua_type_traits
/// @brief lua_type_traits for unique_ptr
template <typename T, typename Deleter>
struct lua_type_traits<std::unique_ptr<T, Deleter> > {
  typedef std::unique_ptr<T, Deleter> &&push_type;
  typedef std::unique_ptr<T, Deleter> &get_type;
  typedef std::unique_ptr<T, Deleter> type;

  static bool strictCheckType(lua_State *l, int index) {
    return object_wrapper<type>(l, index, false) != 0;
  }
  static bool checkType(lua_State *l, int index) {
    return object_wrapper<type>(l, index) != 0 || lua_isnil(l, index);
  }
  static get_type get(lua_State *l, int index) {
    type *pointer = get_pointer(l, index, types::typetag<type>());
    if (!pointer) {
      throw LuaTypeMismatch();
    }
    return *pointer;
  }

  static int push(lua_State *l, push_type v) {
    if (v) {
      typedef ObjectSmartPointerWrapper<type> wrapper_type;
      void *storage = lua_newuserdata(l, sizeof(wrapper_type));
      new (storage) wrapper_type(std::forward<push_type>(v));
      class_userdata::setmetatable<T>(l);
    } else {
      lua_pushnil(l);
    }
    return 1;
  }
};
/// @ingroup lua_type_traits
/// @brief lua_type_traits for nullptr
template <> struct lua_type_traits<std::nullptr_t> {
  typedef const std::nullptr_t &push_type;
  typedef std::nullptr_t get_type;
  typedef optional<get_type> opt_type;

  static bool checkType(lua_State *l, int index) {
    return lua_isnoneornil(l, index);
  }
  static bool strictCheckType(lua_State *l, int index) {
    return lua_isnil(l, index);
  }
  static opt_type opt(lua_State *l, int index) {
    if (!lua_isnoneornil(l, index)) {
      return opt_type();
    }
    return nullptr;
  }
  static get_type get(lua_State *l, int index) {
    if (!lua_isnoneornil(l, index)) {
      throw LuaTypeMismatch();
    }
    return nullptr;
  }

  static int push(lua_State *l, const std::nullptr_t &) {
    lua_pushnil(l);
    return 1;
  }
};
#endif

/// @ingroup lua_type_traits
/// @brief lua_type_traits for ObjectWrapperBase*
template <> struct lua_type_traits<ObjectWrapperBase *> {
  typedef ObjectWrapperBase *get_type;
  typedef ObjectWrapperBase *push_type;

  static bool strictCheckType(lua_State *l, int index) {
    return object_wrapper(l, index) != 0;
  }
  static bool checkType(lua_State *l, int index) {
    return object_wrapper(l, index) != 0;
  }
  static get_type get(lua_State *l, int index) {
    return object_wrapper(l, index);
  }
};

/// @ingroup lua_type_traits
/// @brief lua_type_traits for native type of luathread(lua_State*)
template <> struct lua_type_traits<lua_State *> {
  typedef lua_State *get_type;
  typedef lua_State *push_type;

  static bool strictCheckType(lua_State *l, int index) {
    return lua_isthread(l, index);
  }
  static bool checkType(lua_State *l, int index) {
    return lua_isthread(l, index);
  }
  static lua_State *get(lua_State *l, int index) {
    return lua_tothread(l, index);
  }
};

/// @ingroup lua_type_traits
/// @brief lua_type_traits for floating point number value
template <typename T>
struct lua_type_traits<
    T, typename traits::enable_if<traits::is_floating_point<T>::value>::type> {
  typedef typename traits::remove_const_reference<T>::type get_type;
  typedef optional<get_type> opt_type;
  typedef lua_Number push_type;

  static bool strictCheckType(lua_State *l, int index) {
    return lua_type(l, index) == LUA_TNUMBER;
  }
  static bool checkType(lua_State *l, int index) {
    return lua_isnumber(l, index) != 0;
  }
  static opt_type opt(lua_State *l, int index) KAGUYA_NOEXCEPT {
    int isnum = 0;
    get_type num = static_cast<T>(lua_tonumberx(l, index, &isnum));
    if (!isnum) {
      return opt_type();
    }
    return num;
  }
  static get_type get(lua_State *l, int index) {
    int isnum = 0;
    get_type num = static_cast<T>(lua_tonumberx(l, index, &isnum));
    if (!isnum) {
      throw LuaTypeMismatch();
    }
    return num;
  }
  static int push(lua_State *l, lua_Number s) {
    lua_pushnumber(l, s);
    return 1;
  }
};

/// @ingroup lua_type_traits
/// @brief lua_type_traits for integral number value
template <typename T>
struct lua_type_traits<
    T, typename traits::enable_if<traits::is_integral<T>::value>::type> {
  typedef typename traits::remove_const_reference<T>::type get_type;
  typedef optional<get_type> opt_type;
#if LUA_VERSION_NUM >= 503
  typedef lua_Integer push_type;

  static bool strictCheckType(lua_State *l, int index) {
    return lua_isinteger(l, index) != 0;
  }
  static bool checkType(lua_State *l, int index) {
    return lua_isnumber(l, index) != 0;
  }
  static opt_type opt(lua_State *l, int index) KAGUYA_NOEXCEPT {
    int isnum = 0;
    get_type num = static_cast<T>(lua_tointegerx(l, index, &isnum));
    if (!isnum) {
      return opt_type();
    }
    return num;
  }
  static get_type get(lua_State *l, int index) {
    int isnum = 0;
    get_type num = static_cast<T>(lua_tointegerx(l, index, &isnum));
    if (!isnum) {
      throw LuaTypeMismatch();
    }
    return num;
  }
  static int push(lua_State *l, lua_Integer s) {
    lua_pushinteger(l, s);
    return 1;
  }
#else
  typedef typename lua_type_traits<lua_Number>::push_type push_type;

  static bool strictCheckType(lua_State *l, int index) {
    return lua_type_traits<lua_Number>::strictCheckType(l, index);
  }
  static bool checkType(lua_State *l, int index) {
    return lua_type_traits<lua_Number>::checkType(l, index);
  }
  static get_type get(lua_State *l, int index) {
    return static_cast<get_type>(lua_type_traits<lua_Number>::get(l, index));
  }
  static opt_type opt(lua_State *l, int index) KAGUYA_NOEXCEPT {
    lua_type_traits<lua_Number>::opt_type v =
        lua_type_traits<lua_Number>::opt(l, index);
    if (!v) {
      return opt_type();
    }
    return static_cast<get_type>(*v);
  }
  static int push(lua_State *l, push_type s) { return util::push_args(l, s); }
#endif
};

/// @ingroup lua_type_traits
/// @brief lua_type_traits for enum
template <typename T>
struct lua_type_traits<
    T, typename traits::enable_if<traits::is_enum<T>::value>::type> {
  typedef typename traits::remove_const_reference<T>::type get_type;
  typedef optional<get_type> opt_type;
  typedef typename traits::remove_const_reference<T>::type push_type;

  static bool strictCheckType(lua_State *l, int index) {
    return lua_type_traits<luaInt>::strictCheckType(l, index);
  }
  static bool checkType(lua_State *l, int index) {
    return lua_type_traits<luaInt>::checkType(l, index);
  }
  static opt_type opt(lua_State *l, int index) KAGUYA_NOEXCEPT {
    if (lua_type_traits<luaInt>::opt_type t =
            lua_type_traits<luaInt>::opt(l, index)) {
      return opt_type(static_cast<get_type>(*t));
    }
    return opt_type();
  }
  static get_type get(lua_State *l, int index) {
    return static_cast<get_type>(lua_type_traits<luaInt>::get(l, index));
  }
  static int push(lua_State *l, push_type s) {
    return util::push_args(
        l, static_cast<typename lua_type_traits<int64_t>::push_type>(s));
  }
};

/// @ingroup lua_type_traits
/// @brief lua_type_traits for cstring
template <> struct lua_type_traits<const char *> {
  typedef const char *get_type;
  typedef optional<get_type> opt_type;
  typedef const char *push_type;

  static bool strictCheckType(lua_State *l, int index) {
    return lua_type(l, index) == LUA_TSTRING;
  }
  static bool checkType(lua_State *l, int index) {
    return lua_isstring(l, index) != 0;
  }
  static get_type get(lua_State *l, int index) {
    const char *buffer = lua_tostring(l, index);
    if (!buffer) {
      throw LuaTypeMismatch();
    }
    return buffer;
  }
  static opt_type opt(lua_State *l, int index) {
    const char *buffer = lua_tostring(l, index);
    if (!buffer) {
      return opt_type();
    }
    return buffer;
  }
  static int push(lua_State *l, const char *s) {
    lua_pushstring(l, s);
    return 1;
  }
};

/// @ingroup lua_type_traits
/// @brief lua_type_traits for cstring
template <int N> struct lua_type_traits<char[N]> {
  typedef std::string get_type;
  typedef const char *push_type;

  static bool strictCheckType(lua_State *l, int index) {
    return lua_type(l, index) == LUA_TSTRING;
  }
  static bool checkType(lua_State *l, int index) {
    return lua_isstring(l, index) != 0;
  }
  static const char *get(lua_State *l, int index) {
    const char *buffer = lua_tostring(l, index);
    if (!buffer) {
      throw LuaTypeMismatch();
    }
    return buffer;
  }
  static int push(lua_State *l, const char s[N]) {
    lua_pushlstring(l, s, s[N - 1] != '\0' ? N : N - 1);
    return 1;
  }
};
/// @ingroup lua_type_traits
/// @brief lua_type_traits for cstring
template <int N>
struct lua_type_traits<const char[N]> : lua_type_traits<char[N]> {};

/// @ingroup lua_type_traits
/// @brief lua_type_traits for std::string
template <> struct lua_type_traits<std::string> {
  typedef std::string get_type;
  typedef optional<get_type> opt_type;
  typedef const std::string &push_type;

  static bool strictCheckType(lua_State *l, int index) {
    return lua_type(l, index) == LUA_TSTRING;
  }
  static bool checkType(lua_State *l, int index) {
    return lua_isstring(l, index) != 0;
  }
  static opt_type opt(lua_State *l, int index) KAGUYA_NOEXCEPT {
    size_t size = 0;
    const char *buffer = lua_tolstring(l, index, &size);
    if (!buffer) {
      return opt_type();
    }
    return std::string(buffer, size);
  }
  static get_type get(lua_State *l, int index) {
    if (opt_type o = opt(l, index)) {
      return *o;
    }
    throw LuaTypeMismatch();
  }
  static int push(lua_State *l, const std::string &s) {
    lua_pushlstring(l, s.c_str(), s.size());
    return 1;
  }
};

struct NewTable {
  NewTable() : reserve_array_(0), reserve_record_(0) {}
  NewTable(int reserve_array, int reserve_record_)
      : reserve_array_(reserve_array), reserve_record_(reserve_record_) {}
  int reserve_array_;
  int reserve_record_;
};
struct NewThread {};
struct GlobalTable {};
struct NilValue {};

struct NoTypeCheck {};

/// @ingroup lua_type_traits
/// @brief lua_type_traits for NewTable, push only
template <> struct lua_type_traits<NewTable> {
  static int push(lua_State *l, const NewTable &table) {
    lua_createtable(l, table.reserve_array_, table.reserve_record_);
    return 1;
  }
};

/// @ingroup lua_type_traits
/// @brief lua_type_traits for NewThread, push only
template <> struct lua_type_traits<NewThread> {
  static int push(lua_State *l, const NewThread &) {
    lua_newthread(l);
    return 1;
  }
};
/// @ingroup lua_type_traits
/// @brief lua_type_traits for NilValue, similar to nullptr_t
/// If you using C++11, recommend use nullptr instead.
template <> struct lua_type_traits<NilValue> {
  typedef NilValue get_type;
  typedef optional<get_type> opt_type;
  typedef NilValue push_type;

  static bool checkType(lua_State *l, int index) {
    return lua_isnoneornil(l, index);
  }
  static bool strictCheckType(lua_State *l, int index) {
    return lua_isnil(l, index);
  }

  static opt_type opt(lua_State *l, int index) {
    if (!checkType(l, index)) {
      return opt_type();
    }
    return NilValue();
  }
  static get_type get(lua_State *l, int index) {
    if (!checkType(l, index)) {
      throw LuaTypeMismatch();
    }
    return NilValue();
  }
  static int push(lua_State *l, const NilValue &) {
    lua_pushnil(l);
    return 1;
  }
};
inline std::ostream &operator<<(std::ostream &os, const NilValue &) {
  return os << "nil";
}
inline bool operator==(const NilValue &, const NilValue &) { return true; }
inline bool operator!=(const NilValue &, const NilValue &) { return false; }

/// @ingroup lua_type_traits
/// @brief lua_type_traits for GlobalTable, push only
template <> struct lua_type_traits<GlobalTable> {
  static int push(lua_State *l, const GlobalTable &) {
    lua_pushglobaltable(l);
    return 1;
  }
};

namespace detail {
template <typename Derived> class LuaBasicTypeFunctions {
  template <class Other> friend class LuaBasicTypeFunctions;
  typedef void (LuaBasicTypeFunctions::*bool_type)() const;
  void this_type_does_not_support_comparisons() const {}

public:
  enum value_type {
    TYPE_NONE = LUA_TNONE,                   //!< none type
    TYPE_NIL = LUA_TNIL,                     //!< nil type
    TYPE_BOOLEAN = LUA_TBOOLEAN,             //!< boolean type
    TYPE_LIGHTUSERDATA = LUA_TLIGHTUSERDATA, //!< light userdata type
    TYPE_NUMBER = LUA_TNUMBER,               //!< number type
    TYPE_STRING = LUA_TSTRING,               //!< string type
    TYPE_TABLE = LUA_TTABLE,                 //!< table type
    TYPE_FUNCTION = LUA_TFUNCTION,           //!< function type
    TYPE_USERDATA = LUA_TUSERDATA,           //!< userdata type
    TYPE_THREAD = LUA_TTHREAD                //!< thread(coroutine) type
  };

  /// @brief If reference value is none or nil return true. Otherwise false.
  bool isNilref_() const {
    int t = type();
    return t == LUA_TNIL || t == LUA_TNONE;
  }

  /// @brief Equivalent to `#` operator for strings and tables with no
  /// metamethods.
  /// Follows Lua's reference manual documentation of `lua_rawlen`, ie. types
  /// other
  /// than tables, strings or userdatas return 0.
  /// @return Size of table, string length or userdata memory block size.
  size_t size() const {
    lua_State *state = state_();
    if (!state) {
      return 0;
    }
    util::ScopedSavedStack save(state);
    int index = pushStackIndex_(state);

    return lua_rawlen(state, index);
  }

  // return type
  int type() const {
    lua_State *state = state_();
    if (!state) {
      return LUA_TNONE;
    }
    util::ScopedSavedStack save(state);
    return lua_type(state, pushStackIndex_(state));
  }

  // return type name
  const char *typeName() const { return lua_typename(state_(), type()); }

  operator bool_type() const {
    lua_State *state = state_();
    if (!state) {
      return 0; // hasn't lua_State
    }
    util::ScopedSavedStack save(state);
    int stackindex = pushStackIndex_(state);
    int t = lua_type(state, stackindex);
    if (t == LUA_TNONE) {
      return 0; // none
    }
    return lua_toboolean(state, stackindex)
               ? &LuaBasicTypeFunctions::this_type_does_not_support_comparisons
               : 0;
  }

  /**
  * @name relational operators
  * @brief
  */
  //@{
  template <typename OtherDrived>
  inline bool operator==(const LuaBasicTypeFunctions<OtherDrived> &rhs) const {
    if (isNilref_() || rhs.isNilref_()) {
      return !isNilref_() == !rhs.isNilref_();
    }
    lua_State *state = state_();
    util::ScopedSavedStack save(state);
    int index = pushStackIndex_(state);
    int rhsindex = rhs.pushStackIndex_(state);
    return lua_compare(state, index, rhsindex, LUA_OPEQ) != 0;
  }
  template <typename OtherDrived>
  inline bool operator<(const LuaBasicTypeFunctions<OtherDrived> &rhs) const {
    if (isNilref_() || rhs.isNilref_()) {
      return !isNilref_() != !rhs.isNilref_();
    }
    lua_State *state = state_();
    util::ScopedSavedStack save(state);
    int index = pushStackIndex_(state);
    int rhsindex = rhs.pushStackIndex_(state);
    return lua_compare(state, index, rhsindex, LUA_OPLT) != 0;
  }
  template <typename OtherDrived>
  inline bool operator<=(const LuaBasicTypeFunctions<OtherDrived> &rhs) const {
    if (isNilref_() || rhs.isNilref_()) {
      return !isNilref_() == !rhs.isNilref_();
    }
    lua_State *state = state_();
    util::ScopedSavedStack save(state);
    int index = pushStackIndex_(state);
    int rhsindex = rhs.pushStackIndex_(state);
    return lua_compare(state, index, rhsindex, LUA_OPLE) != 0;
  }
  template <typename OtherDrived>
  inline bool operator>=(const LuaBasicTypeFunctions<OtherDrived> &rhs) const {
    return rhs <= (*this);
  }
  template <typename OtherDrived>
  inline bool operator>(const LuaBasicTypeFunctions<OtherDrived> &rhs) const {
    return rhs < (*this);
  }
  template <typename OtherDrived>
  inline bool operator!=(const LuaBasicTypeFunctions<OtherDrived> &rhs) const {
    return !this->operator==(rhs);
  }

  template <typename T>
  inline typename traits::enable_if<
      !traits::is_convertible<T *, LuaBasicTypeFunctions<T> *>::value,
      bool>::type
  operator==(const T &rhs) const {
    if (optional<typename lua_type_traits<T>::get_type> d = checkGet_<T>()) {
      return *d == rhs;
    }
    return false;
  }
  template <typename T>
  inline typename traits::enable_if<
      !traits::is_convertible<T *, LuaBasicTypeFunctions<T> *>::value,
      bool>::type
  operator!=(const T &rhs) const {
    return !((*this) == rhs);
  }
  //@}

  void dump(std::ostream &os) const {
    lua_State *state = state_();
    util::ScopedSavedStack save(state);
    int stackIndex = pushStackIndex_(state);
    util::stackValueDump(os, state, stackIndex);
  }

private:
  lua_State *state_() const {
    return static_cast<const Derived *>(this)->state();
  }
  int pushStackIndex_(lua_State *state) const {
    return static_cast<const Derived *>(this)->pushStackIndex(state);
  }
  template <typename T>
  optional<typename lua_type_traits<T>::get_type> checkGet_() const {
    lua_State *state = state_();
    util::ScopedSavedStack save(state);
    int stackindex = pushStackIndex_(state);
    return lua_type_traits<
        optional<typename lua_type_traits<T>::get_type> >::get(state,
                                                               stackindex);
  }
};
template <typename D>
inline std::ostream &operator<<(std::ostream &os,
                                const LuaBasicTypeFunctions<D> &ref) {
  ref.dump(os);
  return os;
}
/**
* @name relational operators
* @brief
*/
//@{

#define KAGUYA_ENABLE_IF_NOT_LUAREF(RETTYPE)                                   \
  typename traits::enable_if<                                                  \
      !traits::is_convertible<T *, LuaBasicTypeFunctions<T> *>::value,         \
      RETTYPE>::type
template <typename D, typename T>
inline KAGUYA_ENABLE_IF_NOT_LUAREF(bool)
operator==(const T &lhs, const LuaBasicTypeFunctions<D> &rhs) {
  return rhs == lhs;
}
template <typename D, typename T>
inline KAGUYA_ENABLE_IF_NOT_LUAREF(bool)
operator!=(const T &lhs, const LuaBasicTypeFunctions<D> &rhs) {
  return !(rhs == lhs);
}
#undef KAGUYA_ENABLE_IF_NOT_LUAREF
//@}
}
}

namespace kaguya {
inline const char *get_error_message(lua_State *state) {
  if (lua_type(state, -1) == LUA_TSTRING) {
    const char *message = lua_tostring(state, -1);
    return message ? message : "unknown error";
  } else {
    return "unknown error";
  }
}
inline int lua_pcall_wrap(lua_State *state, int argnum, int retnum) {
  int result = lua_pcall(state, argnum, retnum, 0);
  return result;
}

struct ErrorHandler {
  typedef standard::function<void(int, const char *)> function_type;

  static bool handle(const char *message, lua_State *state) {
	  function_type *handler = getFunctionPointer(state);
    if (handler) {
		(*handler)(0, message);
      return true;
    }
    return false;
  }
  static bool handle(int status_code, const char *message, lua_State *state) {
	  function_type *handler = getFunctionPointer(state);
    if (handler) {
		(*handler)(status_code, message);
      return true;
    }
    return false;
  }
  static bool handle(int status_code, lua_State *state) {
    function_type *handler = getFunctionPointer(state);
    if (handler) {
      (*handler)(status_code, get_error_message(state));
      return true;
    }
    return false;
  }

  static function_type getHandler(lua_State *state) {
    function_type *funptr = getFunctionPointer(state);
    if (funptr) {
      return *funptr;
    }
    return function_type();
  }

  static void unregisterHandler(lua_State *state) {
    if (state) {
      function_type *funptr = getFunctionPointer(state);
      if (funptr) {
        *funptr = function_type();
      }
    }
  }
  static void registerHandler(lua_State *state, function_type f) {
    if (state) {
      function_type *funptr = getFunctionPointer(state);
      if (!funptr) {
        util::ScopedSavedStack save(state);
#if KAGUYA_SUPPORT_MULTIPLE_SHARED_LIBRARY
        lua_pushstring(state, handlerRegistryKey());
#else
        lua_pushlightuserdata(state, handlerRegistryKey());
#endif
        void *ptr = lua_newuserdata(
            state, sizeof(function_type)); // dummy data for gc call
        funptr = new (ptr) function_type();

        // create function_type metatable
        lua_newtable(state);
        lua_pushcclosure(state, &error_handler_cleanner, 0);
        lua_setfield(state, -2, "__gc");
        lua_pushvalue(state, -1);
        lua_setfield(state, -1, "__index");
        lua_setmetatable(state, -2);

        lua_rawset(state, LUA_REGISTRYINDEX);
      }
      *funptr = f;
    }
  }

  static void throwDefaultError(int status, const char *message = 0) {
    switch (status) {
    case LUA_ERRSYNTAX:
      throw LuaSyntaxError(
          status, message ? std::string(message) : "unknown syntax error");
    case LUA_ERRRUN:
      throw LuaRuntimeError(
          status, message ? std::string(message) : "unknown runtime error");
    case LUA_ERRMEM:
      throw LuaMemoryError(status,
                           message ? std::string(message)
                                   : "lua memory allocation error");
    case LUA_ERRERR:
      throw LuaErrorRunningError(status,
                                 message ? std::string(message)
                                         : "unknown error running error");
#if LUA_VERSION_NUM >= 502
    case LUA_ERRGCMM:
      throw LuaGCError(status,
                       message ? std::string(message) : "unknown gc error");
#endif
    default:
      throw LuaUnknownError(
          status, message ? std::string(message) : "lua unknown error");
    }
  }

private:
#if KAGUYA_SUPPORT_MULTIPLE_SHARED_LIBRARY
  static const char *handlerRegistryKey() {
    return "\x80KAGUYA_ERROR_HANDLER_REGISTRY_KEY";
  }
#else
  static void *handlerRegistryKey() {
    static void *key;
    return key;
  }
#endif
  static function_type *getFunctionPointer(lua_State *state) {
    if (state) {
      util::ScopedSavedStack save(state);
#if KAGUYA_SUPPORT_MULTIPLE_SHARED_LIBRARY
      lua_pushstring(state, handlerRegistryKey());
#else
      lua_pushlightuserdata(state, handlerRegistryKey());
#endif
      lua_rawget(state, LUA_REGISTRYINDEX);
      function_type *ptr = (function_type *)lua_touserdata(state, -1);
      return ptr;
    }
    return 0;
  }

  ErrorHandler() {}

  ErrorHandler(const ErrorHandler &);
  ErrorHandler &operator=(const ErrorHandler &);

  static int error_handler_cleanner(lua_State *state) {
    function_type *ptr = (function_type *)lua_touserdata(state, 1);
    ptr->~function_type();
    return 0;
  }
};

namespace except {
inline void OtherError(lua_State *state, const std::string &message) {
  if (ErrorHandler::handle(message.c_str(), state)) {
    return;
  }
}
inline void typeMismatchError(lua_State *state, const std::string &message) {
  if (ErrorHandler::handle(message.c_str(), state)) {
    return;
  }
}
inline void memoryError(lua_State *state, const char *message) {
  if (ErrorHandler::handle(message, state)) {
    return;
  }
}
inline bool checkErrorAndThrow(int status, lua_State *state) {
  if (status != 0 && status != LUA_YIELD) {
    ErrorHandler::handle(status, state);

    return false;
  }
  return true;
}
}
}
// Copyright satoren
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#include <vector>
#include <map>
#include <cassert>
#include <algorithm>
#include <ostream>

namespace kaguya {
/// @brief StackTop tag type
struct StackTop {};

namespace Ref {
/// @brief NoMainCheck tag type
struct NoMainCheck {};

/// @brief reference to Lua stack value
class StackRef {
protected:
  lua_State *state_;
  int stack_index_;
  mutable bool pop_;
#if KAGUYA_USE_CPP11
  StackRef(StackRef &&src)
      : state_(src.state_), stack_index_(src.stack_index_), pop_(src.pop_) {
    src.pop_ = false;
  }
  StackRef &operator=(StackRef &&src) {
    state_ = src.state_;
    stack_index_ = src.stack_index_;
    pop_ = src.pop_;

    src.pop_ = false;
    return *this;
  }

  StackRef(const StackRef &src) = delete;
  StackRef &operator=(const StackRef &src) = delete;
#else
  StackRef(const StackRef &src)
      : state_(src.state_), stack_index_(src.stack_index_), pop_(src.pop_) {
    src.pop_ = false;
  }
  StackRef &operator=(const StackRef &src) {
    if (this != &src) {
      state_ = src.state_;
      stack_index_ = src.stack_index_;
      pop_ = src.pop_;

      src.pop_ = false;
    }
    return *this;
  }
#endif
  StackRef(lua_State *s, int index)
      : state_(s), stack_index_(lua_absindex(s, index)), pop_(true) {}
  StackRef(lua_State *s, int index, bool pop)
      : state_(s), stack_index_(lua_absindex(s, index)), pop_(pop) {}
  StackRef() : state_(0), stack_index_(0), pop_(false) {}
  ~StackRef() {
    if (state_ && pop_) {
      if (lua_gettop(state_) >= stack_index_) {
        lua_settop(state_, stack_index_ - 1);
      }
    }
  }

public:
  bool isNilref() const {
    return state_ == 0 || lua_type(state_, stack_index_) == LUA_TNIL;
  }

  int push() const {
    lua_pushvalue(state_, stack_index_);
    return 1;
  }
  int push(lua_State *state) const {
    lua_pushvalue(state_, stack_index_);
    if (state_ != state) {
      lua_pushvalue(state_, stack_index_);
      lua_xmove(state_, state, 1);
    }
    return 1;
  }

  int pushStackIndex(lua_State *state) const {
    if (state_ != state) {
      lua_pushvalue(state_, stack_index_);
      lua_xmove(state_, state, 1);
      return lua_gettop(state);
    } else {
      return stack_index_;
    }
  }
  lua_State *state() const { return state_; }
};

/// @brief Reference to Lua value. Retain reference by LUA_REGISTRYINDEX
class RegistoryRef {
public:
#if KAGUYA_USE_SHARED_LUAREF
  struct RefHolder {
    struct RefDeleter {
      RefDeleter(lua_State *L) : state_(L) {}
      void operator()(int *ref) {
        luaL_unref(state_, LUA_REGISTRYINDEX, *ref);
        delete ref;
      }
      lua_State *state_;
    };
    RefHolder(lua_State *L, int ref)
        : state_(L), ref_(new int(ref), RefDeleter(L)) {}

    RefHolder(const RefHolder &src) : state_(src.state_), ref_(src.ref_) {}
    RefHolder &operator=(const RefHolder &src) {
      state_ = src.state_;
      ref_ = src.ref_;
      return *this;
    }
#if KAGUYA_USE_RVALUE_REFERENCE
    RefHolder(RefHolder &&src) : state_(0), ref_() { swap(src); }
    RefHolder &operator=(RefHolder &&src) throw() {
      swap(src);
      return *this;
    }
#endif
    void swap(RefHolder &other) throw() {
      std::swap(state_, other.state_);
      std::swap(ref_, other.ref_);
    }
    int ref() const {
      if (state_ && ref_) {
        return *ref_;
      }
      return LUA_REFNIL;
    }
    void reset() { ref_.reset(); }
    lua_State *state() const { return state_; }

  private:
    lua_State *state_;
    standard::shared_ptr<int> ref_;
  };
#else
  struct RefHolder {
    RefHolder(lua_State *L, int ref) : state_(L), ref_(ref) {}
    RefHolder(const RefHolder &src) : state_(src.state_), ref_(LUA_REFNIL) {
      if (state_) {
        lua_rawgeti(state_, LUA_REGISTRYINDEX, src.ref_);
        ref_ = luaL_ref(state_, LUA_REGISTRYINDEX);
      }
    }
    RefHolder &operator=(const RefHolder &src) {
      reset();
      state_ = src.state_;
      if (state_) {
        lua_rawgeti(state_, LUA_REGISTRYINDEX, src.ref_);
        ref_ = luaL_ref(state_, LUA_REGISTRYINDEX);
      } else {
        ref_ = LUA_REFNIL;
      }
      return *this;
    }
#if KAGUYA_USE_RVALUE_REFERENCE
    RefHolder(RefHolder &&src) throw() : state_(src.state_), ref_(src.ref_) {
      src.ref_ = LUA_REFNIL;
    }
    RefHolder &operator=(RefHolder &&src) throw() {
      swap(src);
      return *this;
    }
#endif
    void swap(RefHolder &other) throw() {
      std::swap(state_, other.state_);
      std::swap(ref_, other.ref_);
    }
    int ref() const {
      if (state_) {
        return ref_;
      }
      return LUA_REFNIL;
    }
    void reset() {
      if (ref_ != LUA_REFNIL && state_) {
        luaL_unref(state_, LUA_REGISTRYINDEX, ref_);
        ref_ = LUA_REFNIL;
      }
    }
    ~RefHolder() { reset(); }

    lua_State *state() const { return state_; }

  private:
    lua_State *state_;
    int ref_;
  };
#endif
  RegistoryRef(const RegistoryRef &src) : ref_(src.ref_) {}
  RegistoryRef &operator=(const RegistoryRef &src) {
    if (this != &src) {
      ref_ = src.ref_;
    }
    return *this;
  }

  static int ref_from_stacktop(lua_State *state) {
    if (state) {
      return luaL_ref(state, LUA_REGISTRYINDEX);
    } else {
      return LUA_REFNIL;
    }
  }
#if KAGUYA_USE_RVALUE_REFERENCE
  RegistoryRef(RegistoryRef &&src) throw() : ref_(0, LUA_REFNIL) { swap(src); }
  RegistoryRef &operator=(RegistoryRef &&src) throw() {
    swap(src);
    return *this;
  }
#endif

  RegistoryRef() : ref_(0, LUA_REFNIL) {}
  RegistoryRef(lua_State *state) : ref_(state, LUA_REFNIL) {}

  RegistoryRef(lua_State *state, StackTop, NoMainCheck)
      : ref_(state, ref_from_stacktop(state)) {}

  RegistoryRef(lua_State *state, StackTop)
      : ref_(util::toMainThread(state), ref_from_stacktop(state)) {}

  void swap(RegistoryRef &other) throw() { ref_.swap(other.ref_); }

  template <typename T>
  RegistoryRef(lua_State *state, const T &v, NoMainCheck)
      : ref_(0, LUA_REFNIL) {
    if (!state) {
      return;
    }
    util::ScopedSavedStack save(state);
    util::one_push(state, v);
    ref_ = RefHolder(state, ref_from_stacktop(state));
  }
  template <typename T>
  RegistoryRef(lua_State *state, const T &v) : ref_(0, LUA_REFNIL) {
    if (!state) {
      return;
    }
    util::ScopedSavedStack save(state);
    util::one_push(state, v);
    ref_ = RefHolder(util::toMainThread(state), ref_from_stacktop(state));
  }
#if KAGUYA_USE_CPP11
  template <typename T>
  RegistoryRef(lua_State *state, T &&v, NoMainCheck) : ref_(0, LUA_REFNIL) {
    if (!state) {
      return;
    }
    util::ScopedSavedStack save(state);
    util::one_push(state, standard::forward<T>(v));
    ref_ = RefHolder(state, ref_from_stacktop(state));
  }
  template <typename T>
  RegistoryRef(lua_State *state, T &&v) : ref_(0, LUA_REFNIL) {
    if (!state) {
      return;
    }
    util::ScopedSavedStack save(state);
    util::one_push(state, standard::forward<T>(v));
    ref_ = RefHolder(util::toMainThread(state), ref_from_stacktop(state));
  }
#endif
  ~RegistoryRef() {
    try {
      unref();
    } catch (...) {
    } // can't throw at Destructor
  }

  /// @brief push to Lua stack
  int push() const { return push(ref_.state()); }
  /// @brief push to Lua stack
  int push(lua_State *state) const {
    if (isNilref()) {
      lua_pushnil(state);
      return 1;
    }
#if LUA_VERSION_NUM >= 502
    if (state != ref_.state()) { // state check
      assert(util::toMainThread(state) == util::toMainThread(ref_.state()));
    }
#endif
    lua_rawgeti(state, LUA_REGISTRYINDEX, ref_.ref());
    return 1;
  }

  int pushStackIndex(lua_State *state) const {
    push(state);
    return lua_gettop(state);
  }
  lua_State *state() const { return ref_.state(); }

  bool isNilref() const { return ref_.ref() == LUA_REFNIL; }

  void unref() { ref_.reset(); }

private:
  RefHolder ref_;
};
}
}
// Copyright satoren
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


// Copyright satoren
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#include <vector>
#include <map>
#include <cassert>
#include <algorithm>
#include <ostream>

namespace kaguya {
class LuaTable;
class LuaFunction;

class FunctionResults;

/**
* status of coroutine
*/
enum coroutine_status {
  COSTAT_RUNNING,   //!< coroutine is running
  COSTAT_SUSPENDED, //!< coroutine is suspended
  COSTAT_NORMAL,    //!<
  COSTAT_DEAD       //!< coroutine is dead
};

namespace detail {
class FunctionResultProxy {
public:
  template <typename RetType>
  static RetType ReturnValue(lua_State *state, int restatus, int retindex,
                             types::typetag<RetType> tag);
  static FunctionResults ReturnValue(lua_State *state, int restatus,
                                     int retindex,
                                     types::typetag<FunctionResults> tag);
  static void ReturnValue(lua_State *state, int restatus, int retindex,
                          types::typetag<void> tag);
};

template <typename Derived> class LuaFunctionImpl {
private:
  lua_State *state_() const {
    return static_cast<const Derived *>(this)->state();
  }
  int pushStackIndex_(lua_State *state) const {
    return static_cast<const Derived *>(this)->pushStackIndex(state);
  }
  int push_(lua_State *state) const {
    return static_cast<const Derived *>(this)->push(state);
  }

public:
  /**
  * set function environment table
  */
  bool setFunctionEnv(const LuaTable &env);
  /**
  * set function environment to new table
  */
  bool setFunctionEnv(NewTable env);
  /**
  * get function environment table
  */
  LuaTable getFunctionEnv() const;

#if KAGUYA_USE_CPP11
  template <class Result, class... Args> Result call(Args &&... args) {
    lua_State *state = state_();
    if (!state) {
      except::typeMismatchError(state, "nil");
      return Result();
    }
    int argstart = lua_gettop(state) + 1;
    push_(state);
    int argnum = util::push_args(state, std::forward<Args>(args)...);
    int result = lua_pcall_wrap(state, argnum, LUA_MULTRET);
    except::checkErrorAndThrow(result, state);
    return detail::FunctionResultProxy::ReturnValue(state, result, argstart,
                                                    types::typetag<Result>());
  }

  template <class... Args> FunctionResults operator()(Args &&... args);
#else

#define KAGUYA_CALL_DEF(N)                                                     \
  template <class Result KAGUYA_PP_TEMPLATE_DEF_REPEAT_CONCAT(N)>              \
  Result call(KAGUYA_PP_ARG_CR_DEF_REPEAT(N)) {                                \
    lua_State *state = state_();                                               \
    if (!state) {                                                              \
      except::typeMismatchError(state, "attempt to call nil value");           \
      return Result();                                                         \
    }                                                                          \
    int argstart = lua_gettop(state) + 1;                                      \
    push_(state);                                                              \
    int argnum = util::push_args(state KAGUYA_PP_ARG_REPEAT_CONCAT(N));        \
    int result = lua_pcall_wrap(state, argnum, LUA_MULTRET);                   \
    except::checkErrorAndThrow(result, state);                                 \
    return detail::FunctionResultProxy::ReturnValue(state, result, argstart,   \
                                                    types::typetag<Result>()); \
  }

  KAGUYA_CALL_DEF(0)
  KAGUYA_PP_REPEAT_DEF(KAGUYA_FUNCTION_MAX_ARGS, KAGUYA_CALL_DEF)

#undef KAGUYA_RESUME_DEF

  inline FunctionResults operator()();

#define KAGUYA_OP_FN_DEF(N)                                                    \
  template <KAGUYA_PP_TEMPLATE_DEF_REPEAT(N)>                                  \
  inline FunctionResults operator()(KAGUYA_PP_ARG_CR_DEF_REPEAT(N));

#define KAGUYA_FUNCTION_ARGS_DEF(N)
#define KAGUYA_CALL_ARGS(N) KAGUYA_PP_ARG_REPEAT(N)

  KAGUYA_PP_REPEAT_DEF(KAGUYA_FUNCTION_MAX_ARGS, KAGUYA_OP_FN_DEF)
#undef KAGUYA_OP_FN_DEF

#undef KAGUYA_CALL_ARGS
#undef KAGUYA_FUNCTION_ARGS_DEF
#undef KAGUYA_CALL_DEF
#endif
};

template <typename Derived> class LuaThreadImpl {
private:
  lua_State *state_() const {
    return static_cast<const Derived *>(this)->state();
  }
  int pushStackIndex_(lua_State *state) const {
    return static_cast<const Derived *>(this)->pushStackIndex(state);
  }

public:
#if KAGUYA_USE_CPP11
  template <class Result, class... Args> Result resume(Args &&... args) {
    lua_State *state = state_();
    if (!state) {
      except::typeMismatchError(state, "attempt to call nil value");
      return Result();
    }
    util::ScopedSavedStack save(state);
    int corStackIndex = pushStackIndex_(state);
    lua_State *thread = lua_tothread(state, corStackIndex);
    if (!thread) {
      except::typeMismatchError(state, "not thread");
      return Result();
    }
    int argstart = 1; // exist function in stack at first resume.
    if (lua_status(thread) == LUA_YIELD) {
      argstart = 0;
    }
    util::push_args(thread, std::forward<Args>(args)...);
    int argnum = lua_gettop(thread) - argstart;
    if (argnum < 0) {
      argnum = 0;
    }
    int result = lua_resume(thread, state, argnum);
    except::checkErrorAndThrow(result, thread);
    return detail::FunctionResultProxy::ReturnValue(thread, result, 1,
                                                    types::typetag<Result>());
  }
  template <class... Args> FunctionResults operator()(Args &&... args);
#else

#define KAGUYA_RESUME_DEF(N)                                                   \
  template <class Result KAGUYA_PP_TEMPLATE_DEF_REPEAT_CONCAT(N)>              \
  Result resume(KAGUYA_PP_ARG_CR_DEF_REPEAT(N)) {                              \
    lua_State *state = state_();                                               \
    if (!state) {                                                              \
      except::typeMismatchError(state, "attempt to call nil value");           \
      return Result();                                                         \
    }                                                                          \
    util::ScopedSavedStack save(state);                                        \
    int corStackIndex = pushStackIndex_(state);                                \
    lua_State *thread = lua_tothread(state, corStackIndex);                    \
    if (!thread) {                                                             \
      except::typeMismatchError(state, "not thread");                          \
      return Result();                                                         \
    }                                                                          \
    int argstart = 1;                                                          \
    if (lua_status(thread) == LUA_YIELD) {                                     \
      argstart = 0;                                                            \
    }                                                                          \
    util::push_args(thread KAGUYA_PP_ARG_REPEAT_CONCAT(N));                    \
    int argnum = lua_gettop(thread) - argstart;                                \
    if (argnum < 0) {                                                          \
      argnum = 0;                                                              \
    }                                                                          \
    int result = lua_resume(thread, state, argnum);                            \
    except::checkErrorAndThrow(result, thread);                                \
    return detail::FunctionResultProxy::ReturnValue(thread, result, 1,         \
                                                    types::typetag<Result>()); \
  }

  KAGUYA_RESUME_DEF(0)
  KAGUYA_PP_REPEAT_DEF(KAGUYA_FUNCTION_MAX_ARGS, KAGUYA_RESUME_DEF)

#undef KAGUYA_RESUME_DEF

  inline FunctionResults operator()();

#define KAGUYA_OP_FN_DEF(N)                                                    \
  template <KAGUYA_PP_TEMPLATE_DEF_REPEAT(N)>                                  \
  inline FunctionResults operator()(KAGUYA_PP_ARG_CR_DEF_REPEAT(N));

  KAGUYA_PP_REPEAT_DEF(KAGUYA_FUNCTION_MAX_ARGS, KAGUYA_OP_FN_DEF)

#undef KAGUYA_OP_FN_DEF
#endif

  //!
  //! @return state status
  //!
  int threadStatus() const {
    lua_State *state = state_();
    if (!state) {
      except::typeMismatchError(state, "attempt to call nil value");
      return LUA_ERRRUN;
    }
    util::ScopedSavedStack save(state);
    int corStackIndex = pushStackIndex_(state);
    lua_State *thread = lua_tothread(state, corStackIndex);

    if (!thread) {
      except::typeMismatchError(state, "not thread");
      return LUA_ERRRUN;
    }
    return lua_status(thread);
  }

  //! deprecate
  int thread_status() const { return threadStatus(); }

  //!
  //! @return coroutine status
  //!
  coroutine_status costatus(lua_State *l = 0) const {
    lua_State *state = state_();
    if (!state) {
      return COSTAT_DEAD;
    }
    util::ScopedSavedStack save(state);
    int corStackIndex = pushStackIndex_(state);
    lua_State *thread = lua_tothread(state, corStackIndex);

    if (!thread) {
      except::typeMismatchError(state, "not thread");
      return COSTAT_DEAD;
    } else if (thread == l) {
      return COSTAT_RUNNING;
    } else {
      switch (lua_status(thread)) {
      case LUA_YIELD:
        return COSTAT_SUSPENDED;
      case 0: // LUA_OK
      {
        if (lua_gettop(thread) == 0) {
          return COSTAT_DEAD;
        } else {
          return COSTAT_SUSPENDED;
        }
      }
      default:
        break;
      }
    }
    return COSTAT_DEAD;
  }

  //!
  //! @return if coroutine status is dead, return true. Otherwise return false
  //!
  bool isThreadDead() const { return costatus() == COSTAT_DEAD; }

  /// @brief set function for thread running.
  void setFunction(const LuaFunction &f);

  /// @brief get lua thread
  lua_State *getthread() {
    lua_State *state = state_();
    util::ScopedSavedStack save(state);
    int corStackIndex = pushStackIndex_(state);
    return lua_tothread(state, corStackIndex);
  }
};
}
}
// Copyright satoren
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#include <vector>
#include <map>
#include <algorithm>

namespace kaguya {
class LuaRef;
class LuaStackRef;
class LuaTable;
template <typename KEY> class TableKeyReferenceProxy;
class MemberFunctionBinder;

namespace detail {

struct table_proxy {

#if KAGUYA_USE_CPP11
  template <typename V, typename KEY>
  static void set(lua_State *state, int table_index, KEY &&key, V &&value) {
    util::one_push(state, std::forward<KEY>(key));
    util::one_push(state, std::forward<V>(value));
    lua_settable(state, table_index);
  }
  template <typename V>
  static void set(lua_State *state, int table_index, const char *key,
                  V &&value) {
    util::one_push(state, std::forward<V>(value));
    lua_setfield(state, table_index, key);
  }
  template <typename V>
  static void set(lua_State *state, int table_index, const std::string &key,
                  V &&value) {
    set(state, table_index, key.c_str(), std::forward<V>(value));
  }

  template <typename V>
  static void set(lua_State *state, int table_index, luaInt key, V &&value) {
    util::one_push(state, std::forward<V>(value));
    lua_seti(state, table_index, key);
  }
  template <typename V, typename KEY>
  static void rawset(lua_State *state, int table_index, KEY &&key, V &&value) {
    util::one_push(state, std::forward<KEY>(key));
    util::one_push(state, std::forward<V>(value));
    lua_rawset(state, table_index);
  }
  template <typename V>
  static void rawset(lua_State *state, int table_index, luaInt key, V &&value) {
    util::one_push(state, std::forward<V>(value));
    lua_rawseti(state, table_index, key);
  }
#else
  template <typename V, typename KEY>
  static void set(lua_State *state, int table_index, const KEY &key,
                  const V &value) {
    util::one_push(state, key);
    util::one_push(state, value);
    lua_settable(state, table_index);
  }
  template <typename V>
  static void set(lua_State *state, int table_index, const char *key,
                  const V &value) {
    util::one_push(state, value);
    lua_setfield(state, table_index, key);
  }
  template <typename V>
  static void set(lua_State *state, int table_index, const std::string &key,
                  const V &value) {
    util::one_push(state, value);
    lua_setfield(state, table_index, key.c_str());
  }

  template <typename V>
  static void set(lua_State *state, int table_index, luaInt key,
                  const V &value) {
    util::one_push(state, value);
    lua_seti(state, table_index, key);
  }

  template <typename V, typename KEY>
  static void rawset(lua_State *state, int table_index, const KEY &key,
                     const V &value) {
    util::one_push(state, key);
    util::one_push(state, value);
    lua_rawset(state, table_index);
  }
  template <typename V>
  static void rawset(lua_State *state, int table_index, luaInt key,
                     const V &value) {
    util::one_push(state, value);
    lua_rawseti(state, table_index, key);
  }
#endif

#if KAGUYA_USE_CPP11
  template <typename KEY>
  static void get(lua_State *state, int table_index, KEY &&key) {
    util::one_push(state, std::forward<KEY>(key));
    lua_gettable(state, table_index);
  }
#endif
  template <typename KEY>
  static void get(lua_State *state, int table_index, const KEY &key) {
    util::one_push(state, key);
    lua_gettable(state, table_index);
  }
  static void get(lua_State *state, int table_index, const char *key) {
    lua_getfield(state, table_index, key);
  }
  static void get(lua_State *state, int table_index, const std::string &key) {
    lua_getfield(state, table_index, key.c_str());
  }
  static void get(lua_State *state, int table_index, luaInt key) {
    lua_geti(state, table_index, key);
  }
#if KAGUYA_USE_CPP11
  template <typename KEY>
  static void rawget(lua_State *state, int table_index, KEY &&key) {
    util::one_push(state, std::forward<KEY>(key));
    lua_rawget(state, table_index);
  }
#endif
  template <typename KEY>
  static void rawget(lua_State *state, int table_index, const KEY &key) {
    util::one_push(state, key);
    lua_rawget(state, table_index);
  }
  static void rawget(lua_State *state, int table_index, luaInt key) {
    lua_rawgeti(state, table_index, key);
  }
};

template <typename Derived> class LuaTableOrUserDataImpl {
private:
  lua_State *state_() const {
    return static_cast<const Derived *>(this)->state();
  }
  int pushStackIndex_(lua_State *state) const {
    return static_cast<const Derived *>(this)->pushStackIndex(state);
  }
  int push_(lua_State *state) const {
    return static_cast<const Derived *>(this)->push(state);
  }

public:
  /// @brief set metatable
  /// @param table metatable
  bool setMetatable(const LuaTable &table);

  /// @brief get metatable
  LuaTable getMetatable() const;

  /// @brief table->*"function_name"() in c++ and table:function_name(); in lua
  /// is same
  /// @param function_name function_name in table
  MemberFunctionBinder operator->*(const char *function_name);

  /// @brief value = table[key];
  /// @param key key of table
  /// @return reference of field value
  template <typename T, typename KEY>
  typename lua_type_traits<T>::get_type getField(const KEY &key) const {
    lua_State *state = state_();
    typedef typename lua_type_traits<T>::get_type get_type;
    if (!state) {
      except::typeMismatchError(state, "is nil");
      return get_type();
    }
    util::ScopedSavedStack save(state);
    int stackIndex = pushStackIndex_(state);

    table_proxy::get(state, stackIndex, key);

    return lua_type_traits<T>::get(state, -1);
  }

  /// @brief value = table[key];
  /// @param key key of table
  /// @return reference of field value
  template <typename KEY> LuaStackRef getField(const KEY &key) const;

#if KAGUYA_USE_CPP11
  /// @brief table[key] = value;
  template <typename K, typename V> bool setField(K &&key, V &&value) {
    lua_State *state = state_();
    if (!state) {
      except::typeMismatchError(state, "is nil");
      return false;
    }
    util::ScopedSavedStack save(state);
    int stackIndex = pushStackIndex_(state);

    table_proxy::set(state, stackIndex, std::forward<K>(key),
                     std::forward<V>(value));
    return true;
  }
#else
  /// @brief table[key] = value;
  template <typename K, typename V>
  bool setField(const K &key, const V &value) {
    lua_State *state = state_();
    if (!state) {
      except::typeMismatchError(state, "is nil");
      return false;
    }
    util::ScopedSavedStack save(state);
    int stackIndex = pushStackIndex_(state);

    table_proxy::set(state, stackIndex, key, value);

    return true;
  }
#endif

  /// @brief value = table[key];
  /// @param key key of table
  /// @return reference of field value
  template <typename K> LuaStackRef operator[](K key) const;

  /// @brief value = table[key];or table[key] = value;
  /// @param key key of table
  /// @return reference of field value
  template <typename K> TableKeyReferenceProxy<K> operator[](K key);
};

template <typename Derived> class LuaTableImpl {
private:
  lua_State *state_() const {
    return static_cast<const Derived *>(this)->state();
  }
  int pushStackIndex_(lua_State *state) const {
    return static_cast<const Derived *>(this)->pushStackIndex(state);
  }
  int push_(lua_State *state) const {
    return static_cast<const Derived *>(this)->push(state);
  }

  template <typename K, typename A> struct gettablekey {
    typedef K key_type;
    typedef void value_type;
    std::vector<K, A> &v_;
    gettablekey(std::vector<K, A> &v) : v_(v) {}
    void operator()(K key, const void *) { v_.push_back(key); }
  };
  template <typename V, typename A> struct gettablevalue {
    typedef void key_type;
    typedef V value_type;
    std::vector<V, A> &v_;
    gettablevalue(std::vector<V, A> &v) : v_(v) {}
    void operator()(const void *, V value) { v_.push_back(value); }
  };
  template <typename K, typename V, typename C, typename A> struct gettablemap {
    typedef K key_type;
    typedef V value_type;
    std::map<K, V, C, A> &m_;
    gettablemap(std::map<K, V, C, A> &m) : m_(m) {}
    void operator()(K key, V value) { m_[key] = value; }
  };

public:
#if KAGUYA_USE_CPP11
  /// @brief rawset(table,key,value)
  template <typename K, typename V> bool setRawField(K &&key, V &&value) {
    lua_State *state = state_();
    if (!state) {
      except::typeMismatchError(state, "is nil");
      return false;
    }
    util::ScopedSavedStack save(state);
    int stackIndex = pushStackIndex_(state);

    table_proxy::rawset(state, stackIndex, std::forward<K>(key),
                        std::forward<V>(value));

    return true;
  }
#else
  /// @brief rawset(table,key,value)
  template <typename K, typename V>
  bool setRawField(const K &key, const V &value) {
    lua_State *state = state_();
    if (!state) {
      except::typeMismatchError(state, "is nil");
      return false;
    }
    util::ScopedSavedStack save(state);
    int stackIndex = pushStackIndex_(state);
    table_proxy::rawset(state, stackIndex, key, value);
    return true;
  }
#endif
  /// @brief value = rawget(table,key);
  /// @param key key of table
  /// @return reference of field value
  template <typename T, typename KEY>
  typename lua_type_traits<T>::get_type getRawField(const KEY &key) const {
    lua_State *state = state_();
    typedef typename lua_type_traits<T>::get_type get_type;
    if (!state) {
      except::typeMismatchError(state, "is nil");
      return get_type();
    }
    util::ScopedSavedStack save(state);
    int stackIndex = pushStackIndex_(state);

    table_proxy::rawget(state, stackIndex, key);

    return lua_type_traits<T>::get(state, -1);
  }
  /// @brief value = rawget(table,key);
  /// @param key key of table
  /// @return reference of field value
  template <typename KEY> LuaStackRef getRawField(const KEY &key) const;

  /// @brief foreach table fields
  template <class K, class V, class Fun> void foreach_table(Fun f) const {
    lua_State *state = state_();
    if (!state) {
      except::typeMismatchError(state, "is nil");
      return;
    }
    util::ScopedSavedStack save(state);
    int stackIndex = pushStackIndex_(state);
    lua_pushnil(state);
    while (lua_next(state, stackIndex) != 0) {
      // backup key
      lua_pushvalue(state, -2);

      f(lua_type_traits<K>::get(state, -1), lua_type_traits<V>::get(state, -2));
      lua_pop(state, 2); // pop key and value
    }
  }

  /// @brief foreach table fields
  template <class K, class V, class Fun>
  void foreach_table_breakable(Fun f) const {
    lua_State *state = state_();
    if (!state) {
      except::typeMismatchError(state, "is nil");
      return;
    }
    util::ScopedSavedStack save(state);
    int stackIndex = pushStackIndex_(state);
    lua_pushnil(state);
    while (lua_next(state, stackIndex) != 0) {
      lua_pushvalue(state, -2); // backup key

      bool cont = f(lua_type_traits<K>::get(state, -1),
                    lua_type_traits<V>::get(state, -2));
      lua_pop(state, 2); // pop key and value
      if (!cont) {
        break;
      }
    }
  }

  /// @brief If type is table or userdata, return keys.
  /// @return field keys
  template <typename K, typename A> std::vector<K, A> keys() const {
    std::vector<K, A> res;
    util::ScopedSavedStack save(state_());
    int stackIndex = pushStackIndex_(state_());
    size_t size = lua_rawlen(state_(), stackIndex);
    res.reserve(size);
    foreach_table<K, void>(gettablekey<K, A>(res));
    return res;
  }

  /// @brief If type is table or userdata, return keys.
  /// @return field keys
  template <typename K> std::vector<K> keys() const {
    return keys<K, std::allocator<K> >();
  }
  std::vector<LuaRef> keys() const;

  /// @brief If type is table or userdata, return values.
  /// @return field value
  template <typename V, typename A> std::vector<V, A> values() const {
    std::vector<V, A> res;
    util::ScopedSavedStack save(state_());
    int stackIndex = pushStackIndex_(state_());
    size_t size = lua_rawlen(state_(), stackIndex);
    res.reserve(size);
    foreach_table<void, V>(gettablevalue<V, A>(res));
    return res;
  }

  /// @brief If type is table or userdata, return values.
  /// @return field value
  template <typename V> std::vector<V> values() const {
    return values<V, std::allocator<V> >();
  }
  std::vector<LuaRef> values() const;

  /// @brief If type is table or userdata, return key value pair.
  /// @return key value pair
  template <typename K, typename V, typename C, typename A>
  std::map<K, V, C, A> map() const {
    std::map<K, V, C, A> res;
    foreach_table<K, V>(gettablemap<K, V, C, A>(res));
    return res;
  }

  /// @brief If type is table or userdata, return key value pair.
  /// @return key value pair
  template <typename K, typename V, typename C> std::map<K, V, C> map() const {
    return map<K, V, C, std::allocator<std::pair<const K, V> > >();
  }

  /// @brief If type is table or userdata, return key value pair.
  /// @return key value pair
  template <typename K, typename V> std::map<K, V> map() const {
    return map<K, V, std::less<K> >();
  }
  std::map<LuaRef, LuaRef> map() const;
};

template <typename Derived> class LuaUserDataImpl {
private:
  lua_State *state_() const {
    return static_cast<const Derived *>(this)->state();
  }
  int pushStackIndex_(lua_State *state) const {
    return static_cast<const Derived *>(this)->pushStackIndex(state);
  }
  int push_(lua_State *state) const {
    return static_cast<const Derived *>(this)->push(state);
  }

public:
  /// @brief is type test
  template <typename T> bool isType() const {
    lua_State *state = state_();
    util::ScopedSavedStack save(state);
    return lua_type_traits<T>::strictCheckType(state, pushStackIndex_(state));
  }

  template <typename T> bool isConvertible() const {
    lua_State *state = state_();
    util::ScopedSavedStack save(state);
    return lua_type_traits<T>::checkType(state, pushStackIndex_(state));
  }

  template <typename T> bool typeTest() const { return isType<T>(); }
  template <typename T> bool weakTypeTest() const { return isConvertible<T>(); }

  template <typename T> typename lua_type_traits<T>::get_type get() const {
    lua_State *state = state_();
    util::ScopedSavedStack save(state);
    return lua_type_traits<T>::get(state, state ? pushStackIndex_(state) : 0);
  }

  template <typename T> operator T() const { return get<T>(); }
};
}
}

namespace kaguya {
class LuaRef;
class LuaTable;
template <typename KEY> class TableKeyReferenceProxy;
class MemberFunctionBinder;

namespace detail {

template <typename Derived>
class LuaVariantImpl : public LuaTableImpl<Derived>,
                       public LuaTableOrUserDataImpl<Derived>,
                       public detail::LuaFunctionImpl<Derived>,
                       public detail::LuaThreadImpl<Derived>,
                       public LuaBasicTypeFunctions<Derived> {
private:
  lua_State *state_() const {
    return static_cast<const Derived *>(this)->state();
  }
  int pushStackIndex_(lua_State *state) const {
    return static_cast<const Derived *>(this)->pushStackIndex(state);
  }

public:
  using LuaBasicTypeFunctions<Derived>::type;
  using LuaBasicTypeFunctions<Derived>::typeName;

  /// @brief deprecated, use isType instead.
  template <typename T> bool typeTest() const { return isType<T>(); }

  /// @brief deprecated, use isConvertible instead.
  template <typename T> bool weakTypeTest() const { return isConvertible<T>(); }

  /// @brief is type test
  template <typename T> bool isType() const {
    lua_State *state = state_();
    util::ScopedSavedStack save(state);
    return lua_type_traits<T>::strictCheckType(state, pushStackIndex_(state));
  }

  template <typename T> bool isConvertible() const {
    lua_State *state = state_();
    util::ScopedSavedStack save(state);
    return lua_type_traits<T>::checkType(state, pushStackIndex_(state));
  }

  template <typename T> typename lua_type_traits<T>::get_type get() const {
    lua_State *state = state_();
    util::ScopedSavedStack save(state);
    return lua_type_traits<T>::get(state, state ? pushStackIndex_(state) : 0);
  }
  template <typename T, typename U>
  typename lua_type_traits<T>::get_type value_or(U v) const {
    lua_State *state = state_();
    util::ScopedSavedStack save(state);
    return lua_type_traits<optional<T> >::get(
               state, state ? pushStackIndex_(state) : 0)
        .value_or(v);
  }

  // deprecated. use get<kaguya::optional<T> >() instead;
  template <typename T>
  typename lua_type_traits<T>::get_type
  get(bool &was_valid, bool allow_convertible = true) const {
    lua_State *state = state_();
    util::ScopedSavedStack save(state);
    int stackindex = pushStackIndex_(state);
    if (allow_convertible) {
      was_valid = lua_type_traits<T>::checkType(state, stackindex);
    } else {
      was_valid = lua_type_traits<T>::strictCheckType(state, stackindex);
    }
    if (was_valid) {
      return lua_type_traits<T>::get(state, stackindex);
    } else {
      return T();
    }
  }
  template <typename T> operator T() const { return get<T>(); }

#if KAGUYA_USE_CPP11
  template <class... Args> FunctionResults operator()(Args &&... args);
#else
  inline FunctionResults operator()();

#define KAGUYA_OP_FN_DEF(N)                                                    \
  template <KAGUYA_PP_TEMPLATE_DEF_REPEAT(N)>                                  \
  inline FunctionResults operator()(KAGUYA_PP_ARG_CR_DEF_REPEAT(N));

  KAGUYA_PP_REPEAT_DEF(KAGUYA_FUNCTION_MAX_ARGS, KAGUYA_OP_FN_DEF)
#undef KAGUYA_OP_FN_DEF
#endif
};
}
}

namespace kaguya {
namespace util {
template <class Result>
inline Result get_result_impl(lua_State *l, int startindex,
                              types::typetag<Result>) {
  return lua_type_traits<Result>::get(l, startindex);
}
#if KAGUYA_USE_CPP11
inline standard::tuple<>
get_result_tuple_impl(lua_State *, int, types::typetag<standard::tuple<> >) {
  return standard::tuple<>();
}
template <typename T, typename... TYPES>
inline standard::tuple<T, TYPES...>
get_result_tuple_impl(lua_State *l, int index,
                      types::typetag<standard::tuple<T, TYPES...> >) {
  return standard::tuple_cat(
      standard::tuple<T>(lua_type_traits<T>::get(l, index)),
      get_result_tuple_impl(l, index + 1,
                            types::typetag<standard::tuple<TYPES...> >()));
}
template <typename... TYPES>
inline standard::tuple<TYPES...>
get_result_impl(lua_State *l, int startindex,
                types::typetag<standard::tuple<TYPES...> > tag) {
  return get_result_tuple_impl<TYPES...>(l, startindex, tag);
}
#else

inline standard::tuple<> get_result_impl(lua_State *l, int startindex,
                                         types::typetag<standard::tuple<> >) {
  KAGUYA_UNUSED(l);
  KAGUYA_UNUSED(startindex);
  return standard::tuple<>();
}

#define KAGUYA_GET_DEF(N)                                                      \
  lua_type_traits<KAGUYA_PP_CAT(A, N)>::get(l, N + startindex - 1)
#define KAGUYA_GET_TUPLE_DEF(N)                                                \
  template <KAGUYA_PP_TEMPLATE_DEF_REPEAT(N)>                                  \
  inline standard::tuple<KAGUYA_PP_TEMPLATE_ARG_REPEAT(N)> get_result_impl(    \
      lua_State *l, int startindex,                                            \
      types::typetag<standard::tuple<KAGUYA_PP_TEMPLATE_ARG_REPEAT(N)> >) {    \
    return standard::tuple<KAGUYA_PP_TEMPLATE_ARG_REPEAT(N)>(                  \
        KAGUYA_PP_REPEAT_ARG(N, KAGUYA_GET_DEF));                              \
  }
KAGUYA_PP_REPEAT_DEF(KAGUYA_FUNCTION_MAX_TUPLE_SIZE, KAGUYA_GET_TUPLE_DEF)
#undef KAGUYA_GET_DEF
#undef KAGUYA_GET_TUPLE_DEF
#endif

template <class Result> inline Result get_result(lua_State *l, int startindex) {
  return get_result_impl(l, startindex, types::typetag<Result>());
}
template <> inline void get_result<void>(lua_State *, int) {}
}

/// @addtogroup Lua_reference_types

/// @ingroup Lua_reference_types
/// @brief Reference to any Lua data.
class LuaRef : public Ref::RegistoryRef, public detail::LuaVariantImpl<LuaRef> {
private:
public:
  LuaRef(const Ref::RegistoryRef &src) : Ref::RegistoryRef(src) {}
  LuaRef(const LuaRef &src) : Ref::RegistoryRef(src) {}
  LuaRef &operator=(const LuaRef &src) {
    static_cast<RegistoryRef &>(*this) = src;
    return *this;
  }

#if KAGUYA_USE_CPP11

  LuaRef(LuaRef &&src) : Ref::RegistoryRef(std::move(src)) {}

  LuaRef &operator=(LuaRef &&src) throw() {
    swap(src);
    return *this;
  }

  LuaRef(RegistoryRef &&src) throw() : Ref::RegistoryRef(std::move(src)) {}
  template <typename T>
  LuaRef(lua_State *state, T &&v, Ref::NoMainCheck)
      : Ref::RegistoryRef(state, std::move(v), Ref::NoMainCheck()) {}
  template <typename T>
  LuaRef(lua_State *state, T &&v) : Ref::RegistoryRef(state, std::move(v)) {}
#endif

  LuaRef() {}
  LuaRef(lua_State *state) : Ref::RegistoryRef(state) {}

  LuaRef(lua_State *state, StackTop, Ref::NoMainCheck)
      : Ref::RegistoryRef(state, StackTop(), Ref::NoMainCheck()) {}

  LuaRef(lua_State *state, StackTop) : Ref::RegistoryRef(state, StackTop()) {}

  template <typename T>
  LuaRef(lua_State *state, const T &v, Ref::NoMainCheck)
      : Ref::RegistoryRef(state, v, Ref::NoMainCheck()) {}
  template <typename T>
  LuaRef(lua_State *state, const T &v) : Ref::RegistoryRef(state, v) {}

  const void *native_pointer() const {
    util::ScopedSavedStack save(state());
    push(state());
    return lua_topointer(state(), -1);
  }
  static void putindent(std::ostream &os, int indent) {
    while (indent-- > 0) {
      os << "  ";
    }
  }
};

/// @ingroup lua_type_traits
/// @brief lua_type_traits for LuaRef
template <> struct lua_type_traits<LuaRef> {
  typedef LuaRef get_type;
  typedef const LuaRef &push_type;

  static bool checkType(lua_State *l, int index) {
    KAGUYA_UNUSED(l);
    KAGUYA_UNUSED(index);
    return true;
  }
  static bool strictCheckType(lua_State *l, int index) {
    KAGUYA_UNUSED(l);
    KAGUYA_UNUSED(index);
    return false;
  }

  static get_type get(lua_State *l, int index) {
    lua_pushvalue(l, index);
    return LuaRef(l, StackTop());
  }
  static int push(lua_State *l, push_type v) { return v.push(l); }
};
/// @ingroup lua_type_traits
/// @brief lua_type_traits for LuaRef
template <> struct lua_type_traits<const LuaRef &> : lua_type_traits<LuaRef> {};

class LuaStackRef : public Ref::StackRef,
                    public detail::LuaVariantImpl<LuaStackRef> {
public:
  LuaStackRef() : Ref::StackRef() {}
  LuaStackRef(lua_State *s, int index) : Ref::StackRef(s, index, false) {}
  LuaStackRef(lua_State *s, int index, bool popAtDestruct)
      : Ref::StackRef(s, index, popAtDestruct) {}
#if KAGUYA_USE_CPP11
  LuaStackRef(LuaStackRef &&src) : Ref::StackRef(std::move(src)) {
    src.pop_ = false;
  }
  LuaStackRef &operator=(LuaStackRef &&src) {
    if (this != &src) {
      Ref::StackRef::operator=(std::move(src));
      src.pop_ = false;
    }
    return *this;
  }
  LuaStackRef(const LuaStackRef &src) = delete;
#else
  LuaStackRef(const LuaStackRef &src) : Ref::StackRef(src) { src.pop_ = false; }
  LuaStackRef &operator=(const LuaStackRef &src) {
    if (this != &src) {
      Ref::StackRef::operator=(src);
      src.pop_ = false;
    }
    return *this;
  }
#endif
};

/// @ingroup lua_type_traits
/// @brief lua_type_traits for LuaStackRef
template <> struct lua_type_traits<LuaStackRef> {
  typedef LuaStackRef get_type;
  typedef const LuaStackRef &push_type;

  static bool checkType(lua_State *l, int index) {
    KAGUYA_UNUSED(l);
    KAGUYA_UNUSED(index);
    return true;
  }
  static bool strictCheckType(lua_State *l, int index) {
    KAGUYA_UNUSED(l);
    KAGUYA_UNUSED(index);
    return false;
  }

  static get_type get(lua_State *l, int index) { return LuaStackRef(l, index); }
  static int push(lua_State *l, push_type v) { return v.push(l); }
};
/// @ingroup lua_type_traits
/// @brief lua_type_traits for LuaStackRef
template <>
struct lua_type_traits<const LuaStackRef &> : lua_type_traits<LuaStackRef> {};

/// @ingroup Lua_reference_types
/// @brief Reference to Lua userdata.
class LuaUserData : public Ref::RegistoryRef,
                    public detail::LuaUserDataImpl<LuaUserData>,
                    public detail::LuaTableOrUserDataImpl<LuaUserData>,
                    public detail::LuaBasicTypeFunctions<LuaUserData> {

  void typecheck() {
    int t = type();
    if (t != TYPE_USERDATA && t != TYPE_LIGHTUSERDATA && t != TYPE_NIL &&
        t != TYPE_NONE) {
      except::typeMismatchError(state(), "not user data");
      unref();
    }
  }

public:
  operator LuaRef() {
    push(state());
    return LuaRef(state(), StackTop());
  }
  LuaUserData(lua_State *state, StackTop)
      : Ref::RegistoryRef(state, StackTop()) {
    typecheck();
  }
  template <typename TYPE>
  LuaUserData(lua_State *state, const TYPE &table)
      : Ref::RegistoryRef(state, table) {
    typecheck();
  }
  explicit LuaUserData(lua_State *state)
      : Ref::RegistoryRef(state, NilValue()) {
    typecheck();
  }
  LuaUserData() { typecheck(); }
};

/// @ingroup lua_type_traits
/// @brief lua_type_traits for LuaUserData
template <> struct lua_type_traits<LuaUserData> {
  typedef LuaUserData get_type;
  typedef LuaUserData push_type;

  static bool strictCheckType(lua_State *l, int index) {
    return lua_type(l, index) == LUA_TUSERDATA;
  }
  static bool checkType(lua_State *l, int index) {
    return lua_type(l, index) == LUA_TUSERDATA || lua_isnil(l, index);
  }
  static LuaUserData get(lua_State *l, int index) {
    lua_pushvalue(l, index);
    return LuaUserData(l, StackTop());
  }
  static int push(lua_State *l, const LuaUserData &ref) { return ref.push(l); }
};
/// @ingroup lua_type_traits
/// @brief lua_type_traits for LuaUserData
template <>
struct lua_type_traits<const LuaUserData &> : lua_type_traits<LuaUserData> {};

/// @ingroup Lua_reference_types
/// @brief Reference to Lua table.
class LuaTable : public Ref::RegistoryRef,
                 public detail::LuaTableImpl<LuaTable>,
                 public detail::LuaTableOrUserDataImpl<LuaTable>,
                 public detail::LuaBasicTypeFunctions<LuaTable> {

  void typecheck() {
    int t = type();
    if (t != TYPE_TABLE && t != TYPE_NIL && t != TYPE_NONE) {
      except::typeMismatchError(state(), "not table");
      unref();
    }
  }

public:
  operator LuaRef() {
    push(state());
    return LuaRef(state(), StackTop());
  }
  LuaTable(lua_State *state, StackTop) : Ref::RegistoryRef(state, StackTop()) {
    typecheck();
  }
  LuaTable(lua_State *state, const NewTable &table)
      : Ref::RegistoryRef(state, table) {
    typecheck();
  }
  explicit LuaTable(lua_State *state) : Ref::RegistoryRef(state, NewTable()) {
    typecheck();
  }
  LuaTable() { typecheck(); }
};

/// @ingroup lua_type_traits
/// @brief lua_type_traits for LuaTable
template <> struct lua_type_traits<LuaTable> {
  typedef LuaTable get_type;
  typedef LuaTable push_type;

  static bool strictCheckType(lua_State *l, int index) {
    return lua_istable(l, index);
  }
  static bool checkType(lua_State *l, int index) {
    return lua_istable(l, index) || lua_isnil(l, index);
  }
  static LuaTable get(lua_State *l, int index) {
    lua_pushvalue(l, index);
    return LuaTable(l, StackTop());
  }
  static int push(lua_State *l, const LuaTable &ref) { return ref.push(l); }
};
/// @ingroup lua_type_traits
/// @brief lua_type_traits for LuaTable
template <>
struct lua_type_traits<const LuaTable &> : lua_type_traits<LuaTable> {};

/// @ingroup Lua_reference_types
/// @brief Reference to Lua function.
class LuaFunction : public Ref::RegistoryRef,
                    public detail::LuaFunctionImpl<LuaFunction>,
                    public detail::LuaBasicTypeFunctions<LuaFunction> {
  void typecheck() {
    int t = type();
    if (t != TYPE_FUNCTION && t != TYPE_NIL && t != TYPE_NONE) {
      except::typeMismatchError(state(), "not function");
      RegistoryRef::unref();
    }
  }

  struct LuaLoadStreamWrapper {
    LuaLoadStreamWrapper(std::istream &stream)
        : preloaded_(false), stream_(stream) {
      buffer_.reserve(512);
      skipComment();
      preloaded_ = !buffer_.empty();
    }

    void skipComment() {
      // skip bom
      const char *bom = "\xEF\xBB\xBF";
      const char *bomseq = bom;
      char c;
      while (stream_.get(c)) {
        if (c != *bomseq) // not bom sequence
        {
          buffer_.assign(bom, bomseq);
          buffer_.push_back(c);
          break;
        }
        bomseq++;
        if ('\0' == *bomseq) {
          return;
        }
      }

      // skip comment
      if (!buffer_.empty() && buffer_.front() == '#') {
        buffer_.clear();
        std::string comment;
        std::getline(stream_, comment);
      }
    }

    static const char *getdata(lua_State *, void *ud, size_t *size) {
      LuaLoadStreamWrapper *loader = static_cast<LuaLoadStreamWrapper *>(ud);

      if (loader->preloaded_) {
        loader->preloaded_ = false;
      } else {
        loader->buffer_.clear();
      }

      char c = 0;
      while (loader->buffer_.size() < loader->buffer_.capacity() &&
             loader->stream_.get(c)) {
        loader->buffer_.push_back(c);
      }
      *size = loader->buffer_.size();
      return loader->buffer_.empty() ? 0 : &loader->buffer_[0];
    }

  private:
    bool preloaded_;
    std::vector<char> buffer_;
    std::istream &stream_;
  };

public:
  /// @brief construct with state and function .
  /// @param state pointer to lua_State
  /// @param f execute function for lua thread. e.g.
  /// kaguya::function(function_ptr),kaguya::overload(function_ptr)
  template <typename F>
  LuaFunction(lua_State *state, F f) : Ref::RegistoryRef(state, f) {
    typecheck();
  }

  /// @brief construct with stack top value.
  /// @param state pointer to lua_State
  LuaFunction(lua_State *state, StackTop)
      : Ref::RegistoryRef(state, StackTop()) {
    typecheck();
  }

  /// @brief construct with nil reference.
  LuaFunction() {}

  /// @brief load lua code .
  /// @param state pointer to lua_State
  /// @param luacode string
  static LuaFunction loadstring(lua_State *state, const std::string &luacode) {
    return loadstring(state, luacode.c_str());
  }
  /// @brief load lua code .
  /// @param state pointer to lua_State
  /// @param luacode string
  static LuaFunction loadstring(lua_State *state, const char *luacode) {
    util::ScopedSavedStack save(state);
    int status = luaL_loadstring(state, luacode);

    if (status) {
      ErrorHandler::handle(status, state);
      lua_pushnil(state);
    }
    return LuaFunction(state, StackTop());
  }

  /// @brief If there are no errors,compiled file as a Lua function and return.
  ///  Otherwise send error message to error handler and return nil reference
  /// @param state pointer to lua_State
  /// @param file  file path of lua script
  /// @return reference of lua function
  static LuaFunction loadfile(lua_State *state, const std::string &file) {
    return loadfile(state, file.c_str());
  }

  /// @brief If there are no errors,compiled file as a Lua function and return.
  ///  Otherwise send error message to error handler and return nil reference
  /// @param state pointer to lua_State
  /// @param file  file path of lua script
  /// @return reference of lua function
  static LuaFunction loadfile(lua_State *state, const char *file) {
    util::ScopedSavedStack save(state);

    int status = luaL_loadfile(state, file);

    if (status) {
      ErrorHandler::handle(status, state);
      lua_pushnil(state);
    }
    return LuaFunction(state, StackTop());
  }

  /// @brief If there are no errors,compiled stream as a Lua function and
  /// return.
  ///  Otherwise send error message to error handler and return nil reference
  /// @param state pointer to lua_State
  /// @param stream  stream of lua script data
  /// @param chunkname  use for error message.
  /// @return reference of lua function
  static LuaStackRef loadstreamtostack(lua_State *state, std::istream &stream,
                                       const char *chunkname = 0) {
    LuaLoadStreamWrapper wrapper(stream);
#if LUA_VERSION_NUM >= 502
    int status =
        lua_load(state, &LuaLoadStreamWrapper::getdata, &wrapper, chunkname, 0);
#else
    int status =
        lua_load(state, &LuaLoadStreamWrapper::getdata, &wrapper, chunkname);
#endif
    if (status) {
      ErrorHandler::handle(status, state);
      lua_pushnil(state);
    }
    return LuaStackRef(state, -1, true);
  }

  /// @brief If there are no errors,compiled stream as a Lua function and
  /// return.
  ///  Otherwise send error message to error handler and return nil reference
  /// @param state pointer to lua_State
  /// @param stream  stream of lua script data
  /// @param chunkname  use for error message.
  /// @return reference of lua function
  static LuaFunction loadstream(lua_State *state, std::istream &stream,
                                const char *chunkname = 0) {
    util::ScopedSavedStack save(state);
    LuaLoadStreamWrapper wrapper(stream);
#if LUA_VERSION_NUM >= 502
    int status =
        lua_load(state, &LuaLoadStreamWrapper::getdata, &wrapper, chunkname, 0);
#else
    int status =
        lua_load(state, &LuaLoadStreamWrapper::getdata, &wrapper, chunkname);
#endif
    if (status) {
      ErrorHandler::handle(status, state);
      lua_pushnil(state);
    }
    return LuaFunction(state, StackTop());
  }
};

/// @ingroup Lua_reference_types
/// @brief Reference to Lua thread(coroutine).
class LuaThread : public Ref::RegistoryRef,
                  public detail::LuaThreadImpl<LuaThread>,
                  public detail::LuaBasicTypeFunctions<LuaThread> {
  void typecheck() {
    int t = type();
    if (t != TYPE_THREAD && t != TYPE_NIL && t != TYPE_NONE) {
      except::typeMismatchError(state(), "not lua thread");
      RegistoryRef::unref();
    }
  }

public:
  /// @brief construct with stack top value.
  LuaThread(lua_State *state, StackTop) : Ref::RegistoryRef(state, StackTop()) {
    typecheck();
  }
  /// @brief construct with new thread.
  LuaThread(lua_State *state, const NewThread &t)
      : Ref::RegistoryRef(state, t) {}
  /// @brief construct with nil reference.
  LuaThread(lua_State *state) : Ref::RegistoryRef(state, NewThread()) {}
  /// @brief construct with nil reference.
  LuaThread() {}
  /// @brief get lua thread
  operator lua_State *() { return getthread(); }
};
}

#if KAGUYA_USE_CPP11
#else
namespace std {
template <> inline void swap(kaguya::LuaUserData &a, kaguya::LuaUserData &b) {
  a.swap(b);
}
template <> inline void swap(kaguya::LuaTable &a, kaguya::LuaTable &b) {
  a.swap(b);
}
template <> inline void swap(kaguya::LuaFunction &a, kaguya::LuaFunction &b) {
  a.swap(b);
}
template <> inline void swap(kaguya::LuaThread &a, kaguya::LuaThread &b) {
  a.swap(b);
}
template <> inline void swap(kaguya::LuaRef &a, kaguya::LuaRef &b) {
  a.swap(b);
}
}
#endif
// Copyright satoren
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#include <string>
#include <vector>
#include <iterator>


#if KAGUYA_USE_CPP11
// Copyright satoren
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <string>


namespace kaguya {
namespace nativefunction {
template <std::size_t... indexes> struct index_tuple {};

template <std::size_t first, std::size_t last, class result = index_tuple<>,
          bool flag = first >= last>
struct index_range {
  using type = result;
};

template <std::size_t step, std::size_t last, std::size_t... indexes>
struct index_range<step, last, index_tuple<indexes...>, false>
    : index_range<step + 1, last, index_tuple<indexes..., step> > {};

template <class F, class Ret, class... Args, size_t... Indexes>
int _call_apply(lua_State *state, F &&f, index_tuple<Indexes...>,
                util::FunctionSignatureType<Ret, Args...>) {
  return util::push_args(
      state, util::invoke(f, lua_type_traits<Args>::get(state, Indexes)...));
}
template <class F, class... Args, size_t... Indexes>
int _call_apply(lua_State *state, F &&f, index_tuple<Indexes...>,
                util::FunctionSignatureType<void, Args...>) {
  KAGUYA_UNUSED(state);
  util::invoke(f, lua_type_traits<Args>::get(state, Indexes)...);
  return 0;
}

inline bool all_true() { return true; }
template <class Arg, class... Args>
bool all_true(Arg f, Args... args) { // check from backward and lazy evaluation
  return all_true(args...) && bool(f);
}

inline void join(std::string &, const char *) {}
template <class Arg, class... Args>
void join(std::string &result, const char *delim, const Arg &str,
          const Args &... args) {
  result += str;
  result += delim;
  join(result, delim, args...);
}

template <typename T> struct _wcheckeval {
  _wcheckeval(lua_State *s, int i, bool opt)
      : state(s), index(i), opt_arg(opt) {}
  lua_State *state;
  int index;
  bool opt_arg;
  operator bool() {
    return (opt_arg && lua_isnoneornil(state, index)) ||
           lua_type_traits<T>::checkType(state, index);
  }
};

template <typename T> struct _scheckeval {
  _scheckeval(lua_State *s, int i, bool opt)
      : state(s), index(i), opt_arg(opt) {}
  lua_State *state;
  int index;
  bool opt_arg;
  operator bool() {
    return (opt_arg && lua_isnoneornil(state, index)) ||
           lua_type_traits<T>::strictCheckType(state, index);
  }
};

template <class... Args, size_t... Indexes>
bool _ctype_apply(lua_State *state, index_tuple<Indexes...>,
                  util::TypeTuple<Args...>, int opt_count) {
  KAGUYA_UNUSED(state);
  KAGUYA_UNUSED(opt_count);
  return all_true(_wcheckeval<Args>(
      state, Indexes, sizeof...(Indexes) - opt_count < Indexes)...);
}
template <class... Args, size_t... Indexes>
bool _sctype_apply(lua_State *state, index_tuple<Indexes...>,
                   util::TypeTuple<Args...>, int opt_count) {
  KAGUYA_UNUSED(state);
  KAGUYA_UNUSED(opt_count);
  return all_true(_scheckeval<Args>(
      state, Indexes, sizeof...(Indexes) - opt_count < Indexes)...);
}

template <class... Args, size_t... Indexes>
std::string _type_name_apply(index_tuple<Indexes...>, util::TypeTuple<Args...>,
                             int opt_count) {
  KAGUYA_UNUSED(opt_count);
  std::string result;
  const int max_arg = sizeof...(Args);
  join(result, ",",
       (((max_arg - opt_count < int(Indexes)) ? std::string("[OPT]")
                                              : std::string("")) +
        util::pretty_name(typeid(Args)))...);
  return result;
}

template <class F> int call(lua_State *state, F &&f) {
  typedef typename traits::decay<F>::type ftype;
  typedef typename util::FunctionSignature<ftype>::type fsigtype;
  typedef typename index_range<1, fsigtype::argument_count + 1>::type index;
  return _call_apply(state, f, index(), fsigtype());
}
template <class F>
bool checkArgTypes(lua_State *state, const F &, int opt_count = 0) {
  typedef typename traits::decay<F>::type ftype;
  typedef typename util::FunctionSignature<ftype>::type fsigtype;
  typedef typename index_range<1, fsigtype::argument_count + 1>::type index;
  typedef typename fsigtype::argument_type_tuple argument_type_tuple;
  return _ctype_apply(state, index(), argument_type_tuple(), opt_count);
}
template <class F>
bool strictCheckArgTypes(lua_State *state, const F &, int opt_count = 0) {
  typedef typename traits::decay<F>::type ftype;
  typedef typename util::FunctionSignature<ftype>::type fsigtype;
  typedef typename index_range<1, fsigtype::argument_count + 1>::type index;
  typedef typename fsigtype::argument_type_tuple argument_type_tuple;
  return _sctype_apply(state, index(), argument_type_tuple(), opt_count);
}

template <class F> std::string argTypesName(const F &, int opt_count = 0) {
  typedef typename traits::decay<F>::type ftype;
  typedef typename util::FunctionSignature<ftype>::type fsigtype;
  typedef typename index_range<1, fsigtype::argument_count + 1>::type index;
  typedef typename fsigtype::argument_type_tuple argument_type_tuple;
  return _type_name_apply(index(), argument_type_tuple(), opt_count);
}
template <class F> int minArgCount(const F &) {
  typedef typename traits::decay<F>::type ftype;
  typedef typename util::FunctionSignature<ftype>::type fsigtype;
  return fsigtype::argument_count;
}
template <class F> int maxArgCount(const F &) {
  typedef typename traits::decay<F>::type ftype;
  typedef typename util::FunctionSignature<ftype>::type fsigtype;
  return fsigtype::argument_count;
}

// for constructor
template <typename T> struct ConstructorFunctor;

template <typename ClassType, typename... Args>
struct ConstructorFunctor<util::FunctionSignatureType<ClassType, Args...> > {
  typedef util::FunctionSignatureType<ClassType, Args...> signature_type;
  typedef typename index_range<1, sizeof...(Args) + 1>::type get_index;

  template <size_t... Indexes>
  int invoke(lua_State *L, index_tuple<Indexes...>) const {
    typedef ObjectWrapper<ClassType> wrapper_type;
    void *storage = lua_newuserdata(L, sizeof(wrapper_type));
    try {
      new (storage) wrapper_type(lua_type_traits<Args>::get(L, Indexes)...);
    } catch (...) {
      lua_pop(L, 1);
      throw;
    }

    class_userdata::setmetatable<ClassType>(L);
    return 1;
  }

  int operator()(lua_State *L) const { return invoke(L, get_index()); }

  bool checkArgTypes(lua_State *L, int opt_count = 0) const {
    return _ctype_apply(L, get_index(),
                        typename signature_type::argument_type_tuple(),
                        opt_count);
  }
  bool strictCheckArgTypes(lua_State *L, int opt_count = 0) const {
    return _sctype_apply(L, get_index(),
                         typename signature_type::argument_type_tuple(),
                         opt_count);
  }
  std::string argTypesName(int opt_count = 0) const {
    return _type_name_apply(
        get_index(), typename signature_type::argument_type_tuple(), opt_count);
  }
};

template <typename ClassType, typename... Args> struct ConstructorFunction;
template <typename ClassType, typename... Args>
struct ConstructorFunction<ClassType(Args...)> {
  typedef ConstructorFunctor<util::FunctionSignatureType<ClassType, Args...> >
      type;
};
template <typename ClassType, typename RetType, typename... Args>
struct ConstructorFunction<ClassType,
                           RetType(Args...)> // class type check version
{
  typedef ConstructorFunctor<util::FunctionSignatureType<ClassType, Args...> >
      type;
};
}
using nativefunction::ConstructorFunction;
}
#else
// Copyright satoren
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <string>


namespace kaguya {
namespace nativefunction {

#define KAGUYA_INVOKE_SIG_TARG_DEF_CONCAT_REP(N)                               \
  , typename util::ArgumentType<N - 1, F>::type
#define KAGUYA_INVOKE_SIG_TARG_DEF_REPEAT_CONCAT(N)                            \
  KAGUYA_PP_REPEAT(N, KAGUYA_INVOKE_SIG_TARG_DEF_CONCAT_REP)

#define KAGUYA_GET_REP(N) , lua_type_traits<KAGUYA_PP_CAT(A, N)>::get(state, N)
#define KAGUYA_FUNC_DEF(N)                                                     \
  const util::FunctionSignatureType<Ret KAGUYA_PP_TEMPLATE_ARG_REPEAT_CONCAT(  \
      N)> &fsig
#define KAGUYA_TYPENAME_REP(N)                                                 \
  +((MAX_ARG - opt_count < N) ? "[OPT]" : "") +                                \
      util::pretty_name(typeid(KAGUYA_PP_CAT(A, N))) + ","
#define KAGUYA_TYPECHECK_REP(N)                                                \
  &&(((MAX_ARG - opt_count < N) && lua_isnoneornil(state, N)) ||               \
     lua_type_traits<KAGUYA_PP_CAT(A, N)>::checkType(state, N))
#define KAGUYA_STRICT_TYPECHECK_REP(N)                                         \
  &&(((MAX_ARG - opt_count < N) && lua_isnoneornil(state, N)) ||               \
     lua_type_traits<KAGUYA_PP_CAT(A, N)>::strictCheckType(state, N))
#define KAGUYA_CALL_FN_DEF(N)                                                  \
  template <typename F, typename Ret KAGUYA_PP_TEMPLATE_DEF_REPEAT_CONCAT(N)>  \
  inline typename traits::enable_if<!traits::is_same<void, Ret>::value,        \
                                    int>::type                                 \
  _call_apply(lua_State *state, F &f, KAGUYA_FUNC_DEF(N)) {                    \
    KAGUYA_UNUSED(fsig);                                                       \
    return util::push_args(                                                    \
        state, util::invoke<F KAGUYA_INVOKE_SIG_TARG_DEF_REPEAT_CONCAT(N)>(    \
                   f KAGUYA_PP_REPEAT(N, KAGUYA_GET_REP)));                    \
  }                                                                            \
  template <typename F, typename Ret KAGUYA_PP_TEMPLATE_DEF_REPEAT_CONCAT(N)>  \
  inline                                                                       \
      typename traits::enable_if<traits::is_same<void, Ret>::value, int>::type \
      _call_apply(lua_State *state, F &f, KAGUYA_FUNC_DEF(N)) {                \
    KAGUYA_UNUSED(state);                                                      \
    KAGUYA_UNUSED(fsig);                                                       \
    util::invoke<F KAGUYA_INVOKE_SIG_TARG_DEF_REPEAT_CONCAT(N)>(               \
        f KAGUYA_PP_REPEAT(N, KAGUYA_GET_REP));                                \
    return 0;                                                                  \
  }                                                                            \
  template <typename Ret KAGUYA_PP_TEMPLATE_DEF_REPEAT_CONCAT(N)>              \
  bool _ctype_apply(lua_State *state, KAGUYA_FUNC_DEF(N), int opt_count = 0) { \
    KAGUYA_UNUSED(state);                                                      \
    KAGUYA_UNUSED(opt_count);                                                  \
    KAGUYA_UNUSED(fsig);                                                       \
    const int MAX_ARG = N;                                                     \
    (void)MAX_ARG;                                                             \
    return true KAGUYA_PP_REVERSE_REPEAT(N, KAGUYA_TYPECHECK_REP);             \
  }                                                                            \
  template <typename Ret KAGUYA_PP_TEMPLATE_DEF_REPEAT_CONCAT(N)>              \
  bool _sctype_apply(lua_State *state, KAGUYA_FUNC_DEF(N),                     \
                     int opt_count = 0) {                                      \
    KAGUYA_UNUSED(state);                                                      \
    KAGUYA_UNUSED(opt_count);                                                  \
    KAGUYA_UNUSED(fsig);                                                       \
    const int MAX_ARG = N;                                                     \
    (void)MAX_ARG;                                                             \
    return true KAGUYA_PP_REVERSE_REPEAT(N, KAGUYA_STRICT_TYPECHECK_REP);      \
  }                                                                            \
  template <typename Ret KAGUYA_PP_TEMPLATE_DEF_REPEAT_CONCAT(N)>              \
  std::string _type_name_apply(KAGUYA_FUNC_DEF(N), int opt_count) {            \
    KAGUYA_UNUSED(fsig);                                                       \
    KAGUYA_UNUSED(opt_count);                                                  \
    const int MAX_ARG = N;                                                     \
    (void)MAX_ARG;                                                             \
    return std::string() KAGUYA_PP_REPEAT(N, KAGUYA_TYPENAME_REP);             \
  }

KAGUYA_CALL_FN_DEF(0)
KAGUYA_PP_REPEAT_DEF(KAGUYA_FUNCTION_MAX_ARGS, KAGUYA_CALL_FN_DEF)
#undef KAGUYA_CALL_FN_DEF

#undef KAGUYA_CALL_FN_DEF
#undef KAGUYA_FUNC_DEF

template <class F> int call(lua_State *state, F f) {
  typedef typename traits::decay<F>::type ftype;
  typedef typename util::FunctionSignature<ftype>::type fsigtype;
  return _call_apply(state, f, fsigtype());
}
template <class F>
bool checkArgTypes(lua_State *state, const F &, int opt_count = 0) {
  typedef typename traits::decay<F>::type ftype;
  typedef typename util::FunctionSignature<ftype>::type fsigtype;
  return _ctype_apply(state, fsigtype(), opt_count);
}
template <class F>
bool strictCheckArgTypes(lua_State *state, const F &, int opt_count = 0) {
  typedef typename traits::decay<F>::type ftype;
  typedef typename util::FunctionSignature<ftype>::type fsigtype;
  return _sctype_apply(state, fsigtype(), opt_count);
}

template <class F> std::string argTypesName(const F &f, int opt_count = 0) {
  KAGUYA_UNUSED(f);
  typedef typename traits::decay<F>::type ftype;
  typedef typename util::FunctionSignature<ftype>::type fsigtype;
  return _type_name_apply(fsigtype(), opt_count);
}
template <class F> int minArgCount(const F &f) {
  KAGUYA_UNUSED(f);
  typedef typename traits::decay<F>::type ftype;
  typedef typename util::FunctionSignature<ftype>::type fsigtype;
  return fsigtype::argument_count;
}
template <class F> int maxArgCount(const F &f) {
  KAGUYA_UNUSED(f);
  typedef typename traits::decay<F>::type ftype;
  typedef typename util::FunctionSignature<ftype>::type fsigtype;
  return fsigtype::argument_count;
}

///! for constructor
template <typename T> struct ConstructorFunctor;

#define KAGUYA_CONSTRUCTOR_GET_REP(N)                                          \
  lua_type_traits<KAGUYA_PP_CAT(A, N)>::get(L, N)
#define KAGUYA_CONSTRUCTOR_CALL_FN_DEF(N)                                      \
  template <typename ClassType KAGUYA_PP_TEMPLATE_DEF_REPEAT_CONCAT(N)>        \
  struct ConstructorFunctor<util::FunctionSignatureType<                       \
      ClassType KAGUYA_PP_TEMPLATE_ARG_REPEAT_CONCAT(N)> > {                   \
    typedef util::FunctionSignatureType<                                       \
        ClassType KAGUYA_PP_TEMPLATE_ARG_REPEAT_CONCAT(N)>                     \
        signature_type;                                                        \
    int operator()(lua_State *L) const {                                       \
      typedef ObjectWrapper<ClassType> wrapper_type;                           \
      void *storage = lua_newuserdata(L, sizeof(wrapper_type));                \
      try {                                                                    \
        new (storage)                                                          \
            wrapper_type(KAGUYA_PP_REPEAT_ARG(N, KAGUYA_CONSTRUCTOR_GET_REP)); \
      } catch (...) {                                                          \
        lua_pop(L, 1);                                                         \
        throw;                                                                 \
      }                                                                        \
      class_userdata::setmetatable<ClassType>(L);                              \
      return 1;                                                                \
    }                                                                          \
    bool checkArgTypes(lua_State *L, int opt_count = 0) const {                \
      return _ctype_apply(L, signature_type(), opt_count);                     \
    }                                                                          \
    bool strictCheckArgTypes(lua_State *L, int opt_count = 0) const {          \
      return _sctype_apply(L, signature_type(), opt_count);                    \
    }                                                                          \
    std::string argTypesName(int opt_count = 0) const {                        \
      KAGUYA_UNUSED(opt_count);                                                \
      return _type_name_apply(signature_type(), 0);                            \
    }                                                                          \
  };

KAGUYA_CONSTRUCTOR_CALL_FN_DEF(0)
KAGUYA_PP_REPEAT_DEF(KAGUYA_FUNCTION_MAX_ARGS, KAGUYA_CONSTRUCTOR_CALL_FN_DEF)
#undef KAGUYA_CONSTRUCTOR_CALL_FN_DEF

template <class ClassType, class FunType = void> struct ConstructorFunction;

#define KAGUYA_F_TO_CONSIG_TYPE_DEF(N)                                         \
  ConstructorFunctor<util::FunctionSignatureType<                              \
      ClassType KAGUYA_PP_TEMPLATE_ARG_REPEAT_CONCAT(N)> >
#define KAGUYA_F_TO_CONSIG_DEF(N)                                              \
  template <typename ClassType KAGUYA_PP_TEMPLATE_DEF_REPEAT_CONCAT(N)>        \
  struct ConstructorFunction<ClassType(KAGUYA_PP_TEMPLATE_ARG_REPEAT(N))> {    \
    typedef KAGUYA_F_TO_CONSIG_TYPE_DEF(N) type;                               \
  };                                                                           \
  template <typename ClassType,                                                \
            typename RetType KAGUYA_PP_TEMPLATE_DEF_REPEAT_CONCAT(N)>          \
  struct ConstructorFunction<ClassType,                                        \
                             RetType(KAGUYA_PP_TEMPLATE_ARG_REPEAT(N))> {      \
    typedef KAGUYA_F_TO_CONSIG_TYPE_DEF(N) type;                               \
  };

KAGUYA_F_TO_CONSIG_DEF(0)
KAGUYA_PP_REPEAT_DEF(KAGUYA_FUNCTION_MAX_ARGS, KAGUYA_F_TO_CONSIG_DEF)
#undef KAGUYA_F_TO_CONSIG_DEF
}
using nativefunction::ConstructorFunction;
}
#endif

// Copyright satoren
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <string>


namespace kaguya {
namespace fntuple {

#if KAGUYA_USE_CPP11 && !defined(KAGUYA_FUNCTION_MAX_OVERLOADS)
// In Clang with libstdc++.
// std::tuple elements is limited to 16 for template depth limit
using std::tuple;
using std::get;
using std::tuple_element;
using std::tuple_size;
#else
using util::null_type;
// boost::tuple is max
#define KAGUYA_PP_STRUCT_TDEF_REP(N) KAGUYA_PP_CAT(typename A, N) = null_type
#define KAGUYA_PP_STRUCT_TEMPLATE_DEF_REPEAT(N)                                \
  KAGUYA_PP_REPEAT_ARG(N, KAGUYA_PP_STRUCT_TDEF_REP)

template <KAGUYA_PP_STRUCT_TEMPLATE_DEF_REPEAT(
    KAGUYA_PP_INC(KAGUYA_FUNCTION_MAX_OVERLOADS))>
struct tuple {};
#undef KAGUYA_PP_STRUCT_TDEF_REP
#undef KAGUYA_PP_STRUCT_TEMPLATE_DEF_REPEAT

#define KAGUYA_FUNCTION_TUPLE_ELEMENT(N)                                       \
  KAGUYA_PP_CAT(A, N) KAGUYA_PP_CAT(elem, N);
#define KAGUYA_FUNCTION_TUPLE_ELEMENT_INIT(N)                                  \
  KAGUYA_PP_CAT(elem, N)(KAGUYA_PP_CAT(a, N))
#define KAGUYA_FUNCTION_TUPLE_IMPL_DEF(N)                                      \
  template <KAGUYA_PP_TEMPLATE_DEF_REPEAT(N)>                                  \
  struct tuple<KAGUYA_PP_TEMPLATE_ARG_REPEAT(N)> {                             \
    KAGUYA_PP_REPEAT(N, KAGUYA_FUNCTION_TUPLE_ELEMENT)                         \
    tuple(KAGUYA_PP_ARG_DEF_REPEAT(N))                                         \
        : KAGUYA_PP_REPEAT_ARG(N, KAGUYA_FUNCTION_TUPLE_ELEMENT_INIT) {}       \
  };

KAGUYA_PP_REPEAT_DEF(KAGUYA_FUNCTION_MAX_OVERLOADS,
                     KAGUYA_FUNCTION_TUPLE_IMPL_DEF)

template <typename Tuple> struct tuple_size;

#define KAGUYA_TUPLE_SIZE_DEF(N)                                               \
  template <KAGUYA_PP_TEMPLATE_DEF_REPEAT(N)>                                  \
  struct tuple_size<tuple<KAGUYA_PP_TEMPLATE_ARG_REPEAT(N)> > {                \
    static const size_t value = N;                                             \
  };

KAGUYA_TUPLE_SIZE_DEF(0)
KAGUYA_PP_REPEAT_DEF(KAGUYA_FUNCTION_MAX_OVERLOADS, KAGUYA_TUPLE_SIZE_DEF)
#undef KAGUYA_TUPLE_SIZE_DEF

template <std::size_t remain, class result, bool flag = remain <= 0>
struct tuple_element {};
#define KAGUYA_TUPLE_ELEMENT_DEF(N)                                            \
  template <std::size_t remain,                                                \
            class arg KAGUYA_PP_TEMPLATE_DEF_REPEAT_CONCAT(N)>                 \
  struct tuple_element<                                                        \
      remain, tuple<arg KAGUYA_PP_TEMPLATE_ARG_REPEAT_CONCAT(N)>, true> {      \
    typedef arg type;                                                          \
  };                                                                           \
  template <std::size_t remain,                                                \
            class arg KAGUYA_PP_TEMPLATE_DEF_REPEAT_CONCAT(N)>                 \
  struct tuple_element<                                                        \
      remain, tuple<arg KAGUYA_PP_TEMPLATE_ARG_REPEAT_CONCAT(N)>, false>       \
      : tuple_element<remain - 1, tuple<KAGUYA_PP_TEMPLATE_ARG_REPEAT(N)> > {  \
  };

KAGUYA_PP_REPEAT_DEF(KAGUYA_FUNCTION_MAX_OVERLOADS, KAGUYA_TUPLE_ELEMENT_DEF)

#undef KAGUYA_TUPLE_SIZE_DEF

template <size_t S, typename T> struct tuple_get_helper;
#define KAGUYA_TUPLE_GET_DEF(N)                                                \
  template <typename T> struct tuple_get_helper<N, T> {                        \
    static typename tuple_element<N - 1, T>::type &get(T &t) {                 \
      return t.KAGUYA_PP_CAT(elem, N);                                         \
    }                                                                          \
    static const typename tuple_element<N - 1, T>::type &cget(const T &t) {    \
      return t.KAGUYA_PP_CAT(elem, N);                                         \
    }                                                                          \
  };
KAGUYA_PP_REPEAT_DEF(KAGUYA_FUNCTION_MAX_OVERLOADS, KAGUYA_TUPLE_GET_DEF)

template <size_t S, typename T> typename tuple_element<S, T>::type &get(T &t) {
  return tuple_get_helper<S + 1, T>::get(t);
}
template <size_t S, typename T>
const typename tuple_element<S, T>::type &get(const T &t) {
  return tuple_get_helper<S + 1, T>::cget(t);
}
#endif
}
}

namespace kaguya {
struct FunctionImpl {
  virtual int invoke(lua_State *state) = 0;
  virtual std::string argTypesName() const = 0;
  virtual bool checkArgTypes(lua_State *state) const = 0;
  virtual bool strictCheckArgTypes(lua_State *state) const = 0;
  virtual int minArgCount() const = 0;
  virtual int maxArgCount() const = 0;
  virtual ~FunctionImpl() {}
};
struct PolymorphicInvoker {
  typedef standard::shared_ptr<FunctionImpl> holder_type;
  PolymorphicInvoker(const holder_type &fptr) : fnc(fptr) {}
  int invoke(lua_State *state) const { return fnc->invoke(state); }
  std::string argTypesName() const { return fnc->argTypesName(); }
  bool checkArgTypes(lua_State *state) const {
    return fnc->checkArgTypes(state);
  }
  bool strictCheckArgTypes(lua_State *state) const {
    return fnc->strictCheckArgTypes(state);
  }
  int minArgCount() const { return fnc->minArgCount(); }
  int maxArgCount() const { return fnc->maxArgCount(); }
  ~PolymorphicInvoker() {}

private:
  holder_type fnc;
};
struct PolymorphicMemberInvoker : PolymorphicInvoker {
  PolymorphicMemberInvoker(const holder_type &fptr)
      : PolymorphicInvoker(fptr) {}
};

namespace nativefunction {
template <size_t INDEX, typename F>
typename lua_type_traits<typename util::ArgumentType<INDEX, F>::type>::get_type
getArgument(lua_State *state) {
  return lua_type_traits<typename util::ArgumentType<INDEX, F>::type>::get(
      state, INDEX + 1);
}

template <typename T, typename Enable = void>
struct is_callable
    : traits::integral_constant<
          bool, !traits::is_same<
                    void, typename util::FunctionSignature<T>::type>::value> {};

template <class MemType, class T>
struct is_callable<MemType T::*> : traits::integral_constant<bool, true> {};

template <typename T>
struct is_callable<ConstructorFunctor<T> >
    : traits::integral_constant<bool, true> {};

// for constructors
template <class T> int call(lua_State *state, ConstructorFunctor<T> &con) {
  return con(state);
}
template <class T>
int call(lua_State *state, const ConstructorFunctor<T> &con) {
  return con(state);
}
template <class T>
bool checkArgTypes(lua_State *state, const ConstructorFunctor<T> &con,
                   int opt_count = 0) {
  return con.checkArgTypes(state, opt_count);
}
template <class T>
bool strictCheckArgTypes(lua_State *state, const ConstructorFunctor<T> &con,
                         int opt_count = 0) {
  return con.strictCheckArgTypes(state, opt_count);
}
template <class T> std::string argTypesName(const ConstructorFunctor<T> &con) {
  return con.argTypesName();
}
template <class T> int minArgCount(const ConstructorFunctor<T> &) {
  return ConstructorFunctor<T>::signature_type::argument_count;
}
template <class T> int maxArgCount(const ConstructorFunctor<T> &) {
  return ConstructorFunctor<T>::signature_type::argument_count;
}

// for data member
// using is_member_function_pointer in MSVC2010 : fatal error LNK1179: invalid
// or corrupt file: duplicate COMDAT
// '?value@?$result_@P8ABC@test_02_classreg@@AE?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ@?$is_mem_fun_pointer_select@$0A@@detail@boost@@2_NB'
template <class MemType, class T>
typename traits::enable_if<traits::is_object<MemType>::value, int>::type
call(lua_State *state, MemType T::*mptr) {
  T *this_ = lua_type_traits<T *>::get(state, 1);
  if (lua_gettop(state) == 1) {
    if (!this_) {
      const T &this_ = lua_type_traits<const T &>::get(state, 1);
      if (is_usertype<MemType>::value && !traits::is_pointer<MemType>::value) {
        return util::push_args(
            state, standard::reference_wrapper<const MemType>(this_.*mptr));
      } else {
        return util::push_args(state, this_.*mptr);
      }
    } else {
      if (is_usertype<MemType>::value && !traits::is_pointer<MemType>::value) {
        return util::push_args(
            state, standard::reference_wrapper<MemType>(this_->*mptr));
      } else {
        return util::push_args(state, this_->*mptr);
      }
    }
  } else {
    if (!this_) {
      throw LuaTypeMismatch();
    }
    this_->*mptr = lua_type_traits<MemType>::get(state, 2);
    return 0;
  }
}
template <class MemType, class T>
typename traits::enable_if<traits::is_object<MemType>::value, bool>::type
checkArgTypes(lua_State *state, MemType T::*, int opt_count = 0) {
  KAGUYA_UNUSED(opt_count);
  if (lua_gettop(state) >= 2) {
    // setter typecheck
    return lua_type_traits<MemType>::checkType(state, 2) &&
           lua_type_traits<T>::checkType(state, 1);
  }
  // getter typecheck
  return lua_type_traits<T>::checkType(state, 1);
}
template <class MemType, class T>
typename traits::enable_if<traits::is_object<MemType>::value, bool>::type
strictCheckArgTypes(lua_State *state, MemType T::*, int opt_count = 0) {
  KAGUYA_UNUSED(opt_count);
  if (lua_gettop(state) == 2) {
    // setter typecheck
    return lua_type_traits<MemType>::strictCheckType(state, 2) &&
           lua_type_traits<T>::strictCheckType(state, 1);
  }
  // getter typecheck
  return lua_type_traits<T>::strictCheckType(state, 1);
}
template <class MemType, class T>
typename traits::enable_if<traits::is_object<MemType>::value, std::string>::type
argTypesName(MemType T::*) {
  return util::pretty_name(typeid(T *)) + ",[OPT] " +
         util::pretty_name(typeid(MemType));
}
template <class MemType, class T>
typename traits::enable_if<traits::is_object<MemType>::value, int>::type
minArgCount(MemType T::*) {
  return 1;
}
template <class MemType, class T>
typename traits::enable_if<traits::is_object<MemType>::value, int>::type
maxArgCount(MemType T::*) {
  return 2;
}

inline int call(lua_State *state, const PolymorphicInvoker &f) {
  return f.invoke(state);
}
inline int call(lua_State *state, PolymorphicInvoker &f) {
  return f.invoke(state);
}
inline bool checkArgTypes(lua_State *state, const PolymorphicInvoker &f) {
  return f.checkArgTypes(state);
}
inline bool strictCheckArgTypes(lua_State *state, const PolymorphicInvoker &f) {
  return f.strictCheckArgTypes(state);
}
inline std::string argTypesName(const PolymorphicInvoker &f) {
  return f.argTypesName();
}
inline int minArgCount(const PolymorphicInvoker &f) { return f.minArgCount(); }
inline int maxArgCount(const PolymorphicInvoker &f) { return f.maxArgCount(); }

template <>
struct is_callable<PolymorphicInvoker> : traits::integral_constant<bool, true> {
};

inline int call(lua_State *state, const PolymorphicMemberInvoker &f) {
  return f.invoke(state);
}
inline int call(lua_State *state, PolymorphicMemberInvoker &f) {
  return f.invoke(state);
}
inline bool checkArgTypes(lua_State *state, const PolymorphicMemberInvoker &f) {
  return f.checkArgTypes(state);
}
inline bool strictCheckArgTypes(lua_State *state,
                                const PolymorphicMemberInvoker &f) {
  return f.strictCheckArgTypes(state);
}
inline std::string argTypesName(const PolymorphicMemberInvoker &f) {
  return f.argTypesName();
}
inline int minArgCount(const PolymorphicMemberInvoker &f) {
  return f.minArgCount();
}
inline int maxArgCount(const PolymorphicMemberInvoker &f) {
  return f.maxArgCount();
}

template <>
struct is_callable<PolymorphicMemberInvoker>
    : traits::integral_constant<bool, true> {};
}

class VariadicArgType {
public:
  VariadicArgType(lua_State *state, int startIndex)
      : state_(state), startIndex_(startIndex),
        endIndex_(lua_gettop(state) + 1) {
    if (startIndex_ > endIndex_) {
      endIndex_ = startIndex_;
    }
  }

  template <typename T> operator std::vector<T>() const {
    if (startIndex_ >= endIndex_) {
      return std::vector<T>();
    }
    std::vector<T> result;
    result.reserve(endIndex_ - startIndex_);
    for (int index = startIndex_; index < endIndex_; ++index) {
      result.push_back(lua_type_traits<T>::get(state_, index));
    }
    return result;
  }

  struct reference : public Ref::StackRef,
                     public detail::LuaVariantImpl<reference> {
    reference(lua_State *s, int index) : Ref::StackRef(s, index, false) {}

    const reference *operator->() const { return this; }
  };

  struct iterator {
    typedef std::random_access_iterator_tag iterator_category;
    typedef VariadicArgType::reference reference;
    typedef int difference_type;
    typedef reference value_type;
    typedef reference *pointer;

    iterator() : state_(0), stack_index_(0) {}
    iterator(lua_State *state, int index)
        : state_(state), stack_index_(index) {}
    reference operator*() const { return reference(state_, stack_index_); }
    reference operator->() const { return reference(state_, stack_index_); }
    iterator &operator++() {
      stack_index_++;
      return *this;
    }
    iterator operator++(int) { return iterator(state_, stack_index_++); }

    iterator &operator+=(int n) {
      stack_index_ += n;
      return *this;
    }
    iterator operator+(int n) const {
      return iterator(state_, stack_index_ + n);
    }
    iterator &operator--() {
      stack_index_--;
      return *this;
    }
    iterator operator--(int) { return iterator(state_, stack_index_--); }
    iterator &operator-=(int n) {
      stack_index_ -= n;
      return *this;
    }
    iterator operator-(int n) const {
      return iterator(state_, stack_index_ - n);
    }
    difference_type operator-(const iterator &n) {
      return stack_index_ - n.stack_index_;
    }

    reference operator[](difference_type offset) const {
      return reference(state_, stack_index_ + offset);
    }
    /**
    * @name relational operators
    * @brief
    */
    //@{
    bool operator==(const iterator &other) const {
      return state_ == other.state_ && stack_index_ == other.stack_index_;
    }
    bool operator!=(const iterator &other) const { return !(*this == other); }
    bool operator<(const iterator &other) const {
      return stack_index_ < other.stack_index_;
    }
    bool operator>(const iterator &other) const { return other < *this; }
    bool operator<=(const iterator &other) const { return other >= *this; }
    bool operator>=(const iterator &other) const { return !(*this < other); }
    //@}
  private:
    lua_State *state_;
    int stack_index_;
  };
  typedef iterator const_iterator;
  typedef reference const_reference;
  typedef reference value_type;

  iterator begin() { return iterator(state_, startIndex_); }
  iterator end() { return iterator(state_, endIndex_); }
  const_iterator cbegin() { return const_iterator(state_, startIndex_); }
  const_iterator cend() { return const_iterator(state_, endIndex_); }

  template <typename T>
  typename lua_type_traits<T>::get_type at(size_t index) const {
    if (index >= size()) {
      throw std::out_of_range("variadic arguments out of range");
    }
    return lua_type_traits<T>::get(state_,
                                   startIndex_ + static_cast<int>(index));
  }

  reference at(size_t index) const {
    if (index >= size()) {
      throw std::out_of_range("variadic arguments out of range");
    }
    return reference(state_, startIndex_ + static_cast<int>(index));
  }

  reference operator[](size_t index) const {
    return reference(state_, startIndex_ + static_cast<int>(index));
  }
  size_t size() const { return endIndex_ - startIndex_; }
  size_t empty() const { return endIndex_ == startIndex_; }

private:
  lua_State *state_;
  int startIndex_;
  int endIndex_;
};

inline VariadicArgType::iterator operator+(int n,
                                           const VariadicArgType::iterator &i) {
  return i + n;
}

/// @ingroup lua_type_traits
/// @brief lua_type_traits for VariadicArgType
template <> struct lua_type_traits<VariadicArgType> {
  typedef VariadicArgType get_type;

  static bool strictCheckType(lua_State *l, int index) {
    KAGUYA_UNUSED(l);
    KAGUYA_UNUSED(index);
    return true;
  }
  static bool checkType(lua_State *l, int index) {
    KAGUYA_UNUSED(l);
    KAGUYA_UNUSED(index);
    return true;
  }
  static get_type get(lua_State *l, int index) {
    return VariadicArgType(l, index);
  }
};

namespace nativefunction {
static const int MAX_OVERLOAD_SCORE = 255;
template <typename Fn>
uint8_t compute_function_matching_score(lua_State *state, const Fn &fn) {
  int argcount = lua_gettop(state);

  if (strictCheckArgTypes(state, fn)) {
    const int minargcount = minArgCount(fn);
    const int maxargcount = maxArgCount(fn);
    if (minargcount <= argcount && maxargcount >= argcount) {
      return MAX_OVERLOAD_SCORE;
    } else {
      int diff = std::min(std::abs(argcount - minargcount),
                          std::abs(argcount - maxargcount));
      return std::max(100 - diff, 51);
    }
  } else if (checkArgTypes(state, fn)) {
    const int minargcount = minArgCount(fn);
    const int maxargcount = maxArgCount(fn);
    if (minargcount <= argcount && maxargcount >= argcount) {
      return 200;
    } else {
      int diff = std::min(std::abs(argcount - minargcount),
                          std::abs(argcount - maxargcount));
      return std::max(50 - diff, 1);
    }
  } else {
    return 0;
  }
}
inline int pushArgmentTypeNames(lua_State *state, int top) {
  for (int i = 1; i <= top; i++) {
    if (i != 1) {
      lua_pushliteral(state, ",");
    }

    int type = lua_type(state, i);
    if (type == LUA_TUSERDATA) {
      int nametype = luaL_getmetafield(state, i, "__name");
      if (nametype != LUA_TSTRING) {
        lua_pop(state, 1);
        lua_pushstring(state, lua_typename(state, type));
      }
    } else {
      lua_pushstring(state, lua_typename(state, type));
    }
  }
  return lua_gettop(state) - top;
}
}

#if KAGUYA_USE_CPP11

namespace detail {
template <typename Fn, typename... Functions>
void function_match_scoring(lua_State *state, uint8_t *score_array,
                            int current_index, const Fn &fn) {
  score_array[current_index] =
      nativefunction::compute_function_matching_score(state, fn);
}
template <typename Fn, typename... Functions>
void function_match_scoring(lua_State *state, uint8_t *score_array,
                            int current_index, const Fn &fn,
                            const Functions &... fns) {
  score_array[current_index] =
      nativefunction::compute_function_matching_score(state, fn);
  if (score_array[current_index] < nativefunction::MAX_OVERLOAD_SCORE) {
    function_match_scoring(state, score_array, current_index + 1, fns...);
  }
}
template <typename... Functions>
int best_function_index(lua_State *state, const Functions &... fns) {
  static const int fncount = sizeof...(fns);
  uint8_t score[fncount] = {};
  function_match_scoring(state, score, 0, fns...);
  uint8_t best_score = 0;
  int best_score_index = -1;
  for (int i = 0; i < fncount; ++i) {
    if (best_score < score[i]) {
      best_score = score[i];
      best_score_index = i;
      if (best_score == nativefunction::MAX_OVERLOAD_SCORE) {
        break;
      }
    }
  }
  return best_score_index;
}
template <typename Fn>
int invoke_index(lua_State *state, int index, int current_index, Fn &&fn) {
  KAGUYA_UNUSED(index);
  KAGUYA_UNUSED(current_index);
  return nativefunction::call(state, fn);
}
template <typename Fn, typename... Functions>
int invoke_index(lua_State *state, int index, int current_index, Fn &&fn,
                 Functions &&... fns) {
  if (index == current_index) {
    return nativefunction::call(state, fn);
  } else {
    return invoke_index(state, index, current_index + 1, fns...);
  }
}

template <typename Fun> int best_match_invoke(lua_State *state, Fun &&fn) {
  return nativefunction::call(state, fn);
}

template <typename Fun, typename... Functions>
int best_match_invoke(lua_State *state, Fun &&fn, Functions &&... fns) {
  int index = best_function_index(state, fn, fns...);
  if (index >= 0) {
    assert(size_t(index) <= sizeof...(fns));
    return invoke_index(state, index, 0, fn, fns...);
  } else {
    throw LuaTypeMismatch();
  }
  return 0;
}

template <typename TupleType, std::size_t... S>
int invoke_tuple_impl(lua_State *state, TupleType &&tuple,
                      nativefunction::index_tuple<S...>) {
  return best_match_invoke(state, fntuple::get<S>(tuple)...);
}
template <typename TupleType>
int invoke_tuple(lua_State *state, TupleType &&tuple) {
  typedef typename std::decay<TupleType>::type ttype;

  typedef typename nativefunction::index_range<
      0, fntuple::tuple_size<ttype>::value>::type indexrange;

  return invoke_tuple_impl(state, tuple, indexrange());
}

template <typename Fun>
void push_arg_typename(lua_State *state, const Fun &fn) {
  lua_pushliteral(state, "\t\t");
  lua_pushstring(state, nativefunction::argTypesName(fn).c_str());
  lua_pushliteral(state, "\n");
}

template <typename Fun, typename... Functions>
void push_arg_typename(lua_State *state, const Fun &fn,
                       const Functions &... fns) {
  lua_pushliteral(state, "\t\t");
  lua_pushstring(state, nativefunction::argTypesName(fn).c_str());
  lua_pushliteral(state, "\n");
  push_arg_typename(state, fns...);
}
template <typename TupleType, std::size_t... S>
void push_arg_typename_tuple_impl(lua_State *state, TupleType &&tuple,
                                  nativefunction::index_tuple<S...>) {
  return push_arg_typename(state, fntuple::get<S>(tuple)...);
}
template <typename TupleType>
void push_arg_typename_tuple(lua_State *state, TupleType &&tuple) {
  typedef typename std::decay<TupleType>::type ttype;
  typedef typename nativefunction::index_range<
      0, fntuple::tuple_size<ttype>::value>::type indexrange;

  return push_arg_typename_tuple_impl(state, tuple, indexrange());
}
}

#else

namespace detail {
#define KAGUYA_FUNCTION_SCOREING(N)                                            \
  if (currentbestscore < nativefunction::MAX_OVERLOAD_SCORE) {                 \
    int score = nativefunction::compute_function_matching_score(               \
        state, fntuple::get<N - 1>(tuple));                                    \
    if (currentbestscore < score) {                                            \
      currentbestscore = score;                                                \
      currentbestindex = N;                                                    \
    }                                                                          \
  }
#define KAGUYA_FUNCTION_INVOKE(N)                                              \
  if (currentbestindex == N) {                                                 \
    return nativefunction::call(state, fntuple::get<N - 1>(tuple));            \
  }

#define KAGUYA_ARG_PUSH_TYPENAMES(N)                                           \
  lua_pushliteral(state, "\t\t");                                              \
  lua_pushstring(                                                              \
      state,                                                                   \
      nativefunction::argTypesName(fntuple::get<N - 1>(tuple)).c_str());       \
  lua_pushliteral(state, "\n");
#define KAGUYA_TEMPLATE_PARAMETER(N) template <KAGUYA_PP_TEMPLATE_DEF_REPEAT(N)>
#define KAGUYA_TUPLE_INVOKE_DEF(N)                                             \
  KAGUYA_TEMPLATE_PARAMETER(N)                                                 \
  int invoke_tuple(lua_State *state,                                           \
                   fntuple::tuple<KAGUYA_PP_TEMPLATE_ARG_REPEAT(N)> &tuple) {  \
    if (N == 1) {                                                              \
      return nativefunction::call(state, fntuple::get<0>(tuple));              \
    }                                                                          \
    int32_t currentbestscore = 0;                                              \
    int32_t currentbestindex = -1;                                             \
    KAGUYA_PP_REPEAT(N, KAGUYA_FUNCTION_SCOREING);                             \
    KAGUYA_PP_REPEAT(N, KAGUYA_FUNCTION_INVOKE);                               \
    throw LuaTypeMismatch();                                                   \
  }                                                                            \
  KAGUYA_TEMPLATE_PARAMETER(N)                                                 \
  void push_arg_typename_tuple(                                                \
      lua_State *state,                                                        \
      fntuple::tuple<KAGUYA_PP_TEMPLATE_ARG_REPEAT(N)> &tuple) {               \
    KAGUYA_PP_REPEAT(N, KAGUYA_ARG_PUSH_TYPENAMES);                            \
  }

KAGUYA_PP_REPEAT_DEF(KAGUYA_FUNCTION_MAX_OVERLOADS, KAGUYA_TUPLE_INVOKE_DEF)
#undef KAGUYA_TEMPLATE_PARAMETER
#undef KAGUYA_TUPLE_INVOKE_DEF
#undef KAGUYA_ARG_TYPENAMES
#undef KAGUYA_FUNCTION_INVOKE
#undef KAGUYA_FUNCTION_SCOREING

template <typename TupleType>
int invoke_tuple(lua_State *state, TupleType &tuple) {
  KAGUYA_UNUSED(state);
  KAGUYA_UNUSED(tuple);
  return 0;
}
}
#endif

template <typename FunctionTuple> struct FunctionInvokerType {
  FunctionTuple functions;
  FunctionInvokerType(const FunctionTuple &t) : functions(t) {}
};

template <typename T>
inline FunctionInvokerType<fntuple::tuple<T> > function(T f) {
  KAGUYA_STATIC_ASSERT(
      nativefunction::is_callable<typename traits::decay<T>::type>::value,
      "argument need callable");
  return FunctionInvokerType<fntuple::tuple<T> >(fntuple::tuple<T>(f));
}

template <typename FTYPE, typename T>
inline FunctionInvokerType<fntuple::tuple<standard::function<FTYPE> > >
function(T f) {
  return FunctionInvokerType<fntuple::tuple<standard::function<FTYPE> > >(
      fntuple::tuple<standard::function<FTYPE> >(standard::function<FTYPE>(f)));
}
#if KAGUYA_USE_CPP11

template <typename... Functions>
FunctionInvokerType<fntuple::tuple<Functions...> > overload(Functions... fns) {
  return FunctionInvokerType<fntuple::tuple<Functions...> >(
      fntuple::tuple<Functions...>(fns...));
}
#else
#define KAGUYA_FOVERLOAD_DEF(N)                                                \
  template <KAGUYA_PP_TEMPLATE_DEF_REPEAT(N)>                                  \
  FunctionInvokerType<fntuple::tuple<KAGUYA_PP_TEMPLATE_ARG_REPEAT(N)> >       \
  overload(KAGUYA_PP_ARG_DEF_REPEAT(N)) {                                      \
    typedef typename fntuple::tuple<KAGUYA_PP_TEMPLATE_ARG_REPEAT(N)> ttype;   \
    return FunctionInvokerType<ttype>(ttype(KAGUYA_PP_ARG_REPEAT(N)));         \
  }
KAGUYA_PP_REPEAT_DEF(KAGUYA_FUNCTION_MAX_OVERLOADS, KAGUYA_FOVERLOAD_DEF)
#undef KAGUYA_FOVERLOAD_DEF
#endif

struct luacfunction {
  lua_CFunction ptr;

  luacfunction(lua_CFunction f) : ptr(f) {}
  operator lua_CFunction() { return ptr; }
};

/// @ingroup lua_type_traits
/// @brief lua_type_traits for FunctionInvokerType
template <typename FunctionTuple>
struct lua_type_traits<FunctionInvokerType<FunctionTuple> > {
  typedef FunctionInvokerType<FunctionTuple> userdatatype;
  typedef const FunctionInvokerType<FunctionTuple> &push_type;

  static const char *build_arg_error_message(lua_State *state, const char *msg,
                                             FunctionTuple *tuple) {
    int stack_top = lua_gettop(state);
    if (msg) {
      lua_pushstring(state, msg);
    }
    lua_pushliteral(state, "Argument mismatch:");
    nativefunction::pushArgmentTypeNames(state, stack_top);

    lua_pushliteral(state, "\t candidate is:\n");
    detail::push_arg_typename_tuple(state, *tuple);

    lua_concat(state, lua_gettop(state) - stack_top);
    return lua_tostring(state, -1);
  }

  static int invoke(lua_State *state) {
    FunctionTuple *t = static_cast<FunctionTuple *>(
        lua_touserdata(state, lua_upvalueindex(1)));

    if (t) {
      try {
        return detail::invoke_tuple(state, *t);
      } catch (LuaTypeMismatch &e) {
        if (strcmp(e.what(), "type mismatch!!") == 0) {
          util::traceBack(state, build_arg_error_message(state, "maybe...", t));
        } else {
          util::traceBack(state, e.what());
        }
      } catch (std::exception &e) {
        util::traceBack(state, e.what());
      } catch (...) {
        util::traceBack(state, "Unknown exception");
      }
    }
    return lua_error(state);
  }

  inline static int tuple_destructor(lua_State *state) {
    FunctionTuple *f = static_cast<FunctionTuple *>(lua_touserdata(state, 1));
    if (f) {
      f->~FunctionTuple();
    }
    return 0;
  }

  static int push(lua_State *state, push_type fns) {
    void *ptr = lua_newuserdata(state, sizeof(FunctionTuple));
    new (ptr) FunctionTuple(fns.functions);
    lua_createtable(state, 0, 2);
    lua_pushcclosure(state, &tuple_destructor, 0);
    lua_setfield(state, -2, "__gc");
    lua_pushvalue(state, -1);
    lua_setfield(state, -1, "__index");
    lua_setmetatable(state, -2);
    lua_pushcclosure(state, &invoke, 1);

    return 1;
  }
};

/// @ingroup lua_type_traits
/// @brief lua_type_traits for c function
template <typename T>
struct lua_type_traits<
    T, typename traits::enable_if<traits::is_function<
           typename traits::remove_pointer<T>::type>::value>::type> {
  static int push(lua_State *l, T f) {
    return util::one_push(l, kaguya::function(f));
  }
};
/// @ingroup lua_type_traits
/// @brief lua_type_traits for luacfunction
template <> struct lua_type_traits<luacfunction> {
  typedef luacfunction push_type;
  typedef luacfunction get_type;
  static bool strictCheckType(lua_State *l, int index) {
    return lua_iscfunction(l, index) != 0;
  }
  static bool checkType(lua_State *l, int index) {
    return lua_iscfunction(l, index) != 0;
  }
  static get_type get(lua_State *l, int index) {
    return lua_tocfunction(l, index);
  }
  static int push(lua_State *l, push_type f) {
    lua_pushcfunction(l, f);
    return 1;
  }
};

/// @ingroup lua_type_traits
/// @brief lua_type_traits for std::function or boost::function
template <typename T> struct lua_type_traits<standard::function<T> > {
  typedef const standard::function<T> &push_type;
  typedef standard::function<T> get_type;

  static bool strictCheckType(lua_State *l, int index) {
    return lua_type(l, index) == LUA_TFUNCTION;
  }
  static bool checkType(lua_State *l, int index) {
    return lua_type(l, index) == LUA_TFUNCTION;
  }
  static get_type get(lua_State *l, int index) {
    if (!l || lua_type(l, index) != LUA_TFUNCTION) {
      return get_type();
    }
    lua_pushvalue(l, index);
    return get_type(LuaFunction(l, StackTop()));
  }

  static int push(lua_State *l, push_type v) {
    return util::one_push(l, kaguya::function(v));
  }
};

template <typename F,
          typename Ret = typename util::FunctionSignature<F>::type::result_type>
struct OverloadFunctionImpl : kaguya::FunctionImpl {
  typedef Ret result_type;
  typedef typename util::FunctionSignature<F>::type::c_function_type
      c_function_type;

  virtual result_type invoke_type(lua_State *state) = 0;

  virtual int invoke(lua_State *state) {
    return util::push_args(state, invoke_type(state));
  }
  virtual std::string argTypesName() const {
    return nativefunction::argTypesName(c_function_type(0),
                                        maxArgCount() - minArgCount());
  }
  virtual bool checkArgTypes(lua_State *state) const {
    return kaguya::nativefunction::checkArgTypes(state, c_function_type(0),
                                                 maxArgCount() - minArgCount());
  }
  virtual bool strictCheckArgTypes(lua_State *state) const {
    return kaguya::nativefunction::strictCheckArgTypes(
        state, c_function_type(0), maxArgCount() - minArgCount());
  }
};

template <typename F>
struct OverloadFunctionImpl<F, void> : kaguya::FunctionImpl {
  typedef void result_type;
  typedef typename util::FunctionSignature<F>::type::c_function_type
      c_function_type;

  virtual result_type invoke_type(lua_State *state) = 0;

  virtual int invoke(lua_State *state) {
    invoke_type(state);
    return 0;
  }
  virtual std::string argTypesName() const {
    return nativefunction::argTypesName(c_function_type(0),
                                        maxArgCount() - minArgCount());
  }
  virtual bool checkArgTypes(lua_State *state) const {
    return kaguya::nativefunction::checkArgTypes(state, c_function_type(0),
                                                 maxArgCount() - minArgCount());
  }
  virtual bool strictCheckArgTypes(lua_State *state) const {
    return kaguya::nativefunction::strictCheckArgTypes(
        state, c_function_type(0), maxArgCount() - minArgCount());
  }
};
}

#define KAGUYA_INTERNAL_OVERLOAD_FUNCTION_GET_REP(N)                           \
  getArgument<N - 1, F>(state)
#define KAGUYA_INTERNAL_OVERLOAD_FUNCTION_GET_REPEAT(N)                        \
  KAGUYA_PP_REPEAT_ARG(N, KAGUYA_INTERNAL_OVERLOAD_FUNCTION_GET_REP)
#define KAGUYA_INTERNAL_OVERLOAD_FUNCTION_INVOKE(N, FNAME, MINARG, MAXARG)     \
  if (argcount == KAGUYA_PP_ADD(MINARG, KAGUYA_PP_DEC(N))) {                   \
    return FNAME(KAGUYA_INTERNAL_OVERLOAD_FUNCTION_GET_REPEAT(                 \
        KAGUYA_PP_ADD(MINARG, KAGUYA_PP_DEC(N))));                             \
  }

#define KAGUYA_FUNCTION_OVERLOADS_INTERNAL(GENERATE_NAME, FNAME, MINARG,       \
                                           MAXARG, CREATE_FN)                  \
  \
struct GENERATE_NAME \
{                                                      \
    template <typename F> struct Function : kaguya::OverloadFunctionImpl<F> {  \
      typedef                                                                  \
          typename kaguya::OverloadFunctionImpl<F>::result_type result_type;   \
      virtual result_type invoke_type(lua_State *state) {                      \
        using namespace kaguya::nativefunction;                                \
        int argcount = lua_gettop(state);                                      \
        KAGUYA_PP_REPEAT_DEF_VA_ARG(                                           \
            KAGUYA_PP_INC(KAGUYA_PP_SUB(MAXARG, MINARG)),                      \
            KAGUYA_INTERNAL_OVERLOAD_FUNCTION_INVOKE, FNAME, MINARG, MAXARG)   \
        throw kaguya::LuaTypeMismatch("argument count mismatch");              \
      }                                                                        \
      virtual int minArgCount() const { return MINARG; }                       \
      virtual int maxArgCount() const { return MAXARG; }                       \
    };                                                                         \
    template <typename F> kaguya::PolymorphicInvoker::holder_type create(F) {  \
      kaguya::OverloadFunctionImpl<F> *ptr = new Function<F>();                \
      return kaguya::PolymorphicInvoker::holder_type(ptr);                     \
    }                                                                          \
    template <typename F> kaguya::PolymorphicInvoker::holder_type create() {   \
      kaguya::OverloadFunctionImpl<F> *ptr = new Function<F>();                \
      return kaguya::PolymorphicInvoker::holder_type(ptr);                     \
    }                                                                          \
    kaguya::PolymorphicInvoker operator()() { return CREATE_FN; }              \
  \
}                                                                         \
  GENERATE_NAME;

#define KAGUYA_INTERNAL_OVERLOAD_MEMBER_FUNCTION_GET_REP(N)                    \
  getArgument<N, F>(state)
#define KAGUYA_INTERNAL_OVERLOAD_MEMBER_FUNCTION_GET_REPEAT(N)                 \
  KAGUYA_PP_REPEAT_ARG(N, KAGUYA_INTERNAL_OVERLOAD_MEMBER_FUNCTION_GET_REP)
#define KAGUYA_INTERNAL_OVERLOAD_MEMBER_FUNCTION_INVOKE(N, FNAME, MINARG,      \
                                                        MAXARG)                \
  if (argcount == 1 + KAGUYA_PP_ADD(MINARG, KAGUYA_PP_DEC(N))) {               \
    return (getArgument<0, F>(state))                                          \
        .FNAME(KAGUYA_INTERNAL_OVERLOAD_MEMBER_FUNCTION_GET_REPEAT(            \
            KAGUYA_PP_ADD(MINARG, KAGUYA_PP_DEC(N))));                         \
  }

#define KAGUYA_MEMBER_FUNCTION_OVERLOADS_INTERNAL(GENERATE_NAME, CLASS, FNAME, \
                                                  MINARG, MAXARG, CREATE_FN)   \
  \
struct GENERATE_NAME \
{                                                      \
    template <typename F> struct Function : kaguya::OverloadFunctionImpl<F> {  \
      typedef                                                                  \
          typename kaguya::OverloadFunctionImpl<F>::result_type result_type;   \
      virtual result_type invoke_type(lua_State *state) {                      \
        using namespace kaguya::nativefunction;                                \
        int argcount = lua_gettop(state);                                      \
        KAGUYA_PP_REPEAT_DEF_VA_ARG(                                           \
            KAGUYA_PP_INC(KAGUYA_PP_SUB(MAXARG, MINARG)),                      \
            KAGUYA_INTERNAL_OVERLOAD_MEMBER_FUNCTION_INVOKE, FNAME, MINARG,    \
            MAXARG)                                                            \
        throw kaguya::LuaTypeMismatch("argument count mismatch");              \
      }                                                                        \
      virtual int minArgCount() const { return MINARG + 1; }                   \
      virtual int maxArgCount() const { return MAXARG + 1; }                   \
    };                                                                         \
    template <typename F>                                                      \
    kaguya::PolymorphicMemberInvoker::holder_type create(F f) {                \
      KAGUYA_UNUSED(f);                                                        \
      kaguya::OverloadFunctionImpl<F> *ptr = new Function<F>();                \
      return kaguya::PolymorphicMemberInvoker::holder_type(ptr);               \
    }                                                                          \
    template <typename F>                                                      \
    kaguya::PolymorphicMemberInvoker::holder_type create() {                   \
      kaguya::OverloadFunctionImpl<F> *ptr = new Function<F>();                \
      return kaguya::PolymorphicMemberInvoker::holder_type(ptr);               \
    }                                                                          \
    kaguya::PolymorphicMemberInvoker operator()() { return CREATE_FN; }        \
  \
}                                                                         \
  GENERATE_NAME;

/// @brief Generate wrapper function object for count based overloads with
/// nonvoid return function. Include default arguments parameter function
/// @param GENERATE_NAME generate function object name
/// @param FNAME target function name
/// @param MINARG minimum arguments count
/// @param MAXARG maximum arguments count
#define KAGUYA_FUNCTION_OVERLOADS(GENERATE_NAME, FNAME, MINARG, MAXARG)        \
  KAGUYA_FUNCTION_OVERLOADS_INTERNAL(GENERATE_NAME, FNAME, MINARG, MAXARG,     \
                                     create(FNAME))

/// @brief Generate wrapper function object for count based overloads with
/// nonvoid return function. Include default arguments parameter function
/// @param GENERATE_NAME generate function object name
/// @param FNAME target function name
/// @param MINARG minimum arguments count
/// @param MAXARG maximum arguments count
/// @param SIGNATURE function signature. e,g, int(int)
#define KAGUYA_FUNCTION_OVERLOADS_WITH_SIGNATURE(GENERATE_NAME, FNAME, MINARG, \
                                                 MAXARG, SIGNATURE)            \
  KAGUYA_FUNCTION_OVERLOADS_INTERNAL(GENERATE_NAME, FNAME, MINARG, MAXARG,     \
                                     create<SIGNATURE>())

/// @brief Generate wrapper function object for count based overloads with
/// nonvoid return function. Include default arguments parameter function
/// @param GENERATE_NAME generate function object name
/// @param CLASS target class name
/// @param FNAME target function name
/// @param MINARG minimum arguments count
/// @param MAXARG maximum arguments count
#define KAGUYA_MEMBER_FUNCTION_OVERLOADS(GENERATE_NAME, CLASS, FNAME, MINARG,  \
                                         MAXARG)                               \
  KAGUYA_MEMBER_FUNCTION_OVERLOADS_INTERNAL(                                   \
      GENERATE_NAME, CLASS, FNAME, MINARG, MAXARG, create(&CLASS::FNAME))

/// @brief Generate wrapper function object for count based overloads with
/// nonvoid return function. Include default arguments parameter function
/// @param GENERATE_NAME generate function object name
/// @param CLASS target class name
/// @param FNAME target function name
/// @param MINARG minimum arguments count
/// @param MAXARG maximum arguments count
/// @param SIGNATURE function signature. e,g, int(Test::*)(int)
#define KAGUYA_MEMBER_FUNCTION_OVERLOADS_WITH_SIGNATURE(                       \
    GENERATE_NAME, CLASS, FNAME, MINARG, MAXARG, SIGNATURE)                    \
  KAGUYA_MEMBER_FUNCTION_OVERLOADS_INTERNAL(                                   \
      GENERATE_NAME, CLASS, FNAME, MINARG, MAXARG, create<SIGNATURE>())
// Copyright satoren
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#include <string>
#include <iostream>


// Copyright satoren
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#include <string>
#include <vector>
#include <map>
#include <typeinfo>

// Copyright satoren
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#include <memory>


namespace kaguya {
/// @brief any data holder class for push to lua
class AnyDataPusher {
public:
  int pushToLua(lua_State *state) const {
    if (empty()) {
      lua_pushnil(state);
      return 1;
    } else {
      return holder_->pushToLua(state);
    }
  }

  AnyDataPusher() : holder_() {}

  template <typename DataType>
  AnyDataPusher(const DataType &v) : holder_(new DataHolder<DataType>(v)) {}

#if KAGUYA_USE_CPP11
  AnyDataPusher(AnyDataPusher &&other) : holder_(std::move(other.holder_)) {}
  AnyDataPusher &operator=(AnyDataPusher &&rhs) {
    holder_ = std::move(rhs.holder_);
    return *this;
  }
  template <typename DataType>
  AnyDataPusher(DataType &&v)
      : holder_(new DataHolder<DataType>(std::move(v))) {}
#endif
  AnyDataPusher(const AnyDataPusher &other) : holder_(other.holder_) {}
  AnyDataPusher &operator=(const AnyDataPusher &other) {
    holder_ = other.holder_;
    return *this;
  }

  bool empty() const { return !holder_.get(); }

private:
  struct DataHolderBase {
    virtual int pushToLua(lua_State *data) const = 0;
    //			virtual DataHolderBase * clone(void) = 0;
    virtual ~DataHolderBase() {}
  };
  template <typename Type> class DataHolder : public DataHolderBase {
    typedef typename traits::decay<Type>::type DataType;

  public:
#if KAGUYA_USE_CPP11
    explicit DataHolder(DataType &&v) : data_(std::forward<DataType>(v)) {}
#else
    explicit DataHolder(const DataType &v) : data_(v) {}
#endif
    virtual int pushToLua(lua_State *state) const {
      return util::push_args(state, data_);
    }

  private:
    DataType data_;
  };
  // specialize for string literal
  template <int N> struct DataHolder<const char[N]> : DataHolder<std::string> {
    explicit DataHolder(const char *v)
        : DataHolder<std::string>(
              std::string(v, v[N - 1] != '\0' ? v + N : v + N - 1)) {}
  };
  template <int N> struct DataHolder<char[N]> : DataHolder<std::string> {
    explicit DataHolder(const char *v)
        : DataHolder<std::string>(
              std::string(v, v[N - 1] != '\0' ? v + N : v + N - 1)) {}
  };
  standard::shared_ptr<DataHolderBase> holder_;
};

/// @ingroup lua_type_traits
/// @brief lua_type_traits for AnyDataPusher
template <> struct lua_type_traits<AnyDataPusher> {
  static int push(lua_State *l, const AnyDataPusher &data) {
    return data.pushToLua(l);
  }
};
}
// Copyright satoren
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#include <vector>
#include <map>
#include <cassert>

namespace kaguya {
/// @brief function result value proxy class.
/// don't direct use.
class FunctionResults : public Ref::StackRef,
                        public detail::LuaVariantImpl<FunctionResults> {
  FunctionResults(lua_State *state, int return_status, int startIndex)
      : Ref::StackRef(state, startIndex, true), state_(state),
        resultStatus_(return_status),
        resultCount_(lua_gettop(state) + 1 - startIndex) {}
  FunctionResults(lua_State *state, int return_status, int startIndex,
                  int endIndex)
      : Ref::StackRef(state, startIndex, true), state_(state),
        resultStatus_(return_status), resultCount_(endIndex - startIndex) {}
  friend class detail::FunctionResultProxy;

public:
  FunctionResults()
      : Ref::StackRef(), state_(0), resultStatus_(0), resultCount_(0) {}
  FunctionResults(lua_State *state)
      : Ref::StackRef(state, 0, true), state_(state), resultStatus_(0),
        resultCount_(0) {}
#if KAGUYA_USE_CPP11
  FunctionResults(FunctionResults &&src)
      : Ref::StackRef(std::move(src)), state_(src.state_),
        resultStatus_(src.resultStatus_), resultCount_(src.resultCount_) {
    src.state_ = 0;
  }
#else
  FunctionResults(const FunctionResults &src)
      : Ref::StackRef(src), state_(src.state_),
        resultStatus_(src.resultStatus_), resultCount_(src.resultCount_) {
    src.state_ = 0;
  }
#endif

  ~FunctionResults() {}

  struct reference : public Ref::StackRef,
                     public detail::LuaVariantImpl<reference> {
    reference(lua_State *s, int index) : Ref::StackRef(s, index, false) {}

    reference *operator->() { return this; }
    const reference *operator->() const { return this; }
  };
  template <typename T> struct iterator_base {
    iterator_base(lua_State *s, int i) : state(s), stack_index(i) {}
    template <typename U>
    iterator_base(const iterator_base<U> &other)
        : state(other.state), stack_index(other.stack_index) {}
    T operator*() const { return reference(state, stack_index); }
    T operator->() const { return reference(state, stack_index); }
    const iterator_base &operator++() {
      stack_index++;
      return *this;
    }
    iterator_base operator++(int) {
      return iterator_base(state, stack_index++);
    }

    iterator_base operator+=(int n) {
      stack_index += n;
      return iterator_base(state, stack_index);
    }
    /**
    * @name relational operators
    * @brief
    */
    //@{
    bool operator==(const iterator_base &other) const {
      return state == other.state && stack_index == other.stack_index;
    }
    bool operator!=(const iterator_base &other) const {
      return !(*this == other);
    }
    //@}
    int index() const { return stack_index; }

  private:
    template <typename U> friend struct iterator_base;
    lua_State *state;
    int stack_index;
  };
  typedef iterator_base<reference> iterator;
  typedef iterator_base<const reference> const_iterator;
  typedef reference const_reference;

  iterator begin() { return iterator(state_, stack_index_); }
  iterator end() { return iterator(state_, stack_index_ + resultCount_); }
  const_iterator begin() const { return const_iterator(state_, stack_index_); }
  const_iterator end() const {
    return const_iterator(state_, stack_index_ + resultCount_);
  }
  const_iterator cbegin() const { return const_iterator(state_, stack_index_); }
  const_iterator cend() const {
    return const_iterator(state_, stack_index_ + resultCount_);
  }

  template <class Result>
  Result get_result(types::typetag<Result> = types::typetag<Result>()) const {
    return util::get_result<Result>(state_, stack_index_);
  }
  LuaStackRef get_result(types::typetag<LuaStackRef>) const {
    pop_ = 0;
    return LuaStackRef(state_, stack_index_, true);
  }
  lua_State *state() const { return state_; }

  template <typename T>
  typename lua_type_traits<T>::get_type result_at(size_t index) const {
    if (index >= result_size()) {
      throw std::out_of_range("function result out of range");
    }
    return lua_type_traits<T>::get(state_,
                                   stack_index_ + static_cast<int>(index));
  }
  reference result_at(size_t index) const {
    if (index >= result_size()) {
      throw std::out_of_range("function result out of range");
    }
    return reference(state_, stack_index_ + static_cast<int>(index));
  }

  size_t result_size() const { return resultCount_; }

  size_t resultStatus() const { return resultStatus_; }

  operator LuaStackRef() {
    pop_ = 0;
    return LuaStackRef(state_, stack_index_, true);
  }

private:
  mutable lua_State *state_;
  int resultStatus_;
  int resultCount_;
};

namespace detail {
template <typename RetType>
inline RetType FunctionResultProxy::ReturnValue(lua_State *state,
                                                int return_status, int retindex,
                                                types::typetag<RetType>) {
  return FunctionResults(state, return_status, retindex)
      .get_result(types::typetag<RetType>());
}
inline FunctionResults
FunctionResultProxy::ReturnValue(lua_State *state, int return_status,
                                 int retindex,
                                 types::typetag<FunctionResults>) {
  return FunctionResults(state, return_status, retindex);
}
inline void FunctionResultProxy::ReturnValue(lua_State *state,
                                             int return_status, int retindex,
                                             types::typetag<void>) {
  KAGUYA_UNUSED(return_status);
  lua_settop(state, retindex - 1);
}

#if KAGUYA_USE_CPP11
template <typename Derived>
template <class... Args>
FunctionResults LuaFunctionImpl<Derived>::operator()(Args &&... args) {
  return this->template call<FunctionResults>(std::forward<Args>(args)...);
}

template <typename Derived>
template <class... Args>
FunctionResults LuaThreadImpl<Derived>::operator()(Args &&... args) {
  return this->template resume<FunctionResults>(std::forward<Args>(args)...);
}
template <typename Derived>
template <class... Args>
FunctionResults LuaVariantImpl<Derived>::operator()(Args &&... args) {
  int t = type();
  if (t == LUA_TTHREAD) {
    return this->template resume<FunctionResults>(std::forward<Args>(args)...);
  } else if (t == LUA_TFUNCTION) {
    return this->template call<FunctionResults>(std::forward<Args>(args)...);
  } else {
    except::typeMismatchError(state_(), " is not function or thread");
    return FunctionResults(state_());
  }
}
#else
#define KAGUYA_TEMPLATE_PARAMETER(N) template <typename Derived>
#define KAGUYA_FUNCTION_ARGS_DEF(N)
#define KAGUYA_CALL_ARGS(N)

#define KAGUYA_OP_FN_DEF(N)                                                    \
  KAGUYA_TEMPLATE_PARAMETER(N)                                                 \
  inline FunctionResults LuaFunctionImpl<Derived>::operator()(                 \
      KAGUYA_FUNCTION_ARGS_DEF(N)) {                                           \
    return this->template call<FunctionResults>(KAGUYA_CALL_ARGS(N));          \
  }                                                                            \
  KAGUYA_TEMPLATE_PARAMETER(N)                                                 \
  inline FunctionResults LuaThreadImpl<Derived>::operator()(                   \
      KAGUYA_FUNCTION_ARGS_DEF(N)) {                                           \
    return this->template resume<FunctionResults>(KAGUYA_CALL_ARGS(N));        \
  }                                                                            \
  KAGUYA_TEMPLATE_PARAMETER(N)                                                 \
  inline FunctionResults LuaVariantImpl<Derived>::operator()(                  \
      KAGUYA_FUNCTION_ARGS_DEF(N)) {                                           \
    int t = type();                                                            \
    if (t == LUA_TTHREAD) {                                                    \
      return this->template resume<FunctionResults>(KAGUYA_CALL_ARGS(N));      \
    } else if (t == LUA_TFUNCTION) {                                           \
      return this->template call<FunctionResults>(KAGUYA_CALL_ARGS(N));        \
    } else {                                                                   \
      except::typeMismatchError(state_(), " is not function or thread");       \
      return FunctionResults(state_());                                        \
    }                                                                          \
  }

KAGUYA_OP_FN_DEF(0)

#undef KAGUYA_TEMPLATE_PARAMETER
#undef KAGUYA_FUNCTION_ARGS_DEF
#undef KAGUYA_CALL_ARGS
#define KAGUYA_TEMPLATE_PARAMETER(N)                                           \
  template <typename Derived> template <KAGUYA_PP_TEMPLATE_DEF_REPEAT(N)>
#define KAGUYA_FUNCTION_ARGS_DEF(N) KAGUYA_PP_ARG_CR_DEF_REPEAT(N)
#define KAGUYA_CALL_ARGS(N) KAGUYA_PP_ARG_REPEAT(N)

KAGUYA_PP_REPEAT_DEF(KAGUYA_FUNCTION_MAX_ARGS, KAGUYA_OP_FN_DEF)
#undef KAGUYA_OP_FN_DEF
#undef KAGUYA_TEMPLATE_PARAMETER

#undef KAGUYA_CALL_ARGS
#undef KAGUYA_FUNCTION_ARGS_DEF
#undef KAGUYA_CALL_DEF
#undef KAGUYA_OP_FN_DEF
#endif
}

inline std::ostream &operator<<(std::ostream &os, const FunctionResults &res) {
  for (FunctionResults::const_iterator it = res.begin(); it != res.end();
       ++it) {
    if (it != res.begin()) {
      os << ",";
    }
    util::stackValueDump(os, res.state(), it.index());
  }

  return os;
}

/// @ingroup lua_type_traits
/// @brief lua_type_traits for FunctionResults
template <> struct lua_type_traits<FunctionResults> {
  static int push(lua_State *l, const FunctionResults &ref) {
    int size = 0;
    for (FunctionResults::const_iterator it = ref.cbegin(); it != ref.cend();
         ++it) {
      size += it->push(l);
    }
    return size;
  }
};

/// @ingroup lua_type_traits
/// @brief lua_type_traits for FunctionResults::reference
template <> struct lua_type_traits<FunctionResults::reference> {
  static int push(lua_State *l, const FunctionResults::reference &ref) {
    return ref.push(l);
  }
};
template <unsigned int I>
FunctionResults::reference get(const FunctionResults &res) {
  return res.result_at(I);
}

/// @ingroup lua_type_traits
/// @brief lua_type_traits for LuaFunction
template <> struct lua_type_traits<LuaFunction> {
  typedef LuaFunction get_type;
  typedef LuaFunction push_type;

  static bool strictCheckType(lua_State *l, int index) {
    return lua_isfunction(l, index);
  }
  static bool checkType(lua_State *l, int index) {
    return lua_isfunction(l, index) || lua_isnil(l, index);
  }
  static LuaFunction get(lua_State *l, int index) {
    lua_pushvalue(l, index);
    return LuaFunction(l, StackTop());
  }
  static int push(lua_State *l, const LuaFunction &ref) { return ref.push(l); }
};
/// @ingroup lua_type_traits
/// @brief lua_type_traits for LuaFunction
template <>
struct lua_type_traits<const LuaFunction &> : lua_type_traits<LuaFunction> {};

/// @ingroup lua_type_traits
/// @brief lua_type_traits for LuaThread
template <> struct lua_type_traits<LuaThread> {
  typedef LuaThread get_type;
  typedef LuaThread push_type;

  static bool strictCheckType(lua_State *l, int index) {
    return lua_isthread(l, index);
  }
  static bool checkType(lua_State *l, int index) {
    return lua_isthread(l, index) || lua_isnil(l, index);
  }
  static LuaThread get(lua_State *l, int index) {
    lua_pushvalue(l, index);
    return LuaThread(l, StackTop());
  }
  static int push(lua_State *l, const LuaThread &ref) { return ref.push(l); }
};
/// @ingroup lua_type_traits
/// @brief lua_type_traits for LuaThread
template <>
struct lua_type_traits<const LuaThread &> : lua_type_traits<LuaThread> {};

/**
* @brief table and function binder.
* state["table"]->*"fun"() is table:fun() in Lua
* @param arg... function args
*/
class MemberFunctionBinder {
public:
  template <class T>
  MemberFunctionBinder(LuaRef self, T key)
      : self_(self), fun_(self_.getField(key)) {}

#define KAGUYA_DELEGATE_LUAREF fun_
#define KAGUYA_DELEGATE_FIRST_ARG self_
#define KAGUYA_DELEGATE_FIRST_ARG_C KAGUYA_DELEGATE_FIRST_ARG,

#if KAGUYA_USE_CPP11
  template <class... Args> FunctionResults operator()(Args &&... args) {
    return KAGUYA_DELEGATE_LUAREF(
        KAGUYA_DELEGATE_FIRST_ARG_C std::forward<Args>(args)...);
  }

  template <class Result, class... Args> Result call(Args &&... args) {
    return KAGUYA_DELEGATE_LUAREF.call<Result>(
        KAGUYA_DELEGATE_FIRST_ARG_C std::forward<Args>(args)...);
  }
#else

#define KAGUYA_OP_FN_DEF(N)                                                    \
  template <KAGUYA_PP_TEMPLATE_DEF_REPEAT(N)>                                  \
  FunctionResults operator()(KAGUYA_PP_ARG_CR_DEF_REPEAT(N)) {                 \
    return KAGUYA_DELEGATE_LUAREF(                                             \
        KAGUYA_DELEGATE_FIRST_ARG_C KAGUYA_PP_ARG_REPEAT(N));                  \
  }

  /**
  * @brief If type is function, call lua function.
  * If type is lua thread,start or resume lua thread.
  * Otherwise send error message to error handler
  */
  FunctionResults operator()() {
    return KAGUYA_DELEGATE_LUAREF(KAGUYA_DELEGATE_FIRST_ARG);
  }
  KAGUYA_PP_REPEAT_DEF(KAGUYA_FUNCTION_MAX_ARGS, KAGUYA_OP_FN_DEF)

#undef KAGUYA_OP_FN_DEF

#define KAGUYA_CALL_DEF(N)                                                     \
  template <class Result, KAGUYA_PP_TEMPLATE_DEF_REPEAT(N)>                    \
  Result call(KAGUYA_PP_ARG_CR_DEF_REPEAT(N)) {                                \
    return KAGUYA_DELEGATE_LUAREF.call<Result>(                                \
        KAGUYA_DELEGATE_FIRST_ARG_C KAGUYA_PP_ARG_REPEAT(N));                  \
  }

  template <class Result> Result call() {
    return KAGUYA_DELEGATE_LUAREF.call<Result>(KAGUYA_DELEGATE_FIRST_ARG);
  }
  KAGUYA_PP_REPEAT_DEF(KAGUYA_FUNCTION_MAX_ARGS, KAGUYA_CALL_DEF)

#undef KAGUYA_CALL_DEF
#endif

#undef KAGUYA_DELEGATE_FIRST_ARG_C
#undef KAGUYA_DELEGATE_FIRST_ARG
#undef KAGUYA_DELEGATE_LUAREF

private:
  LuaRef self_; // Table or Userdata
  LuaFunction fun_;
};

typedef MemberFunctionBinder mem_fun_binder; // for backward conpatible
}

#define KAGUYA_PROPERTY_PREFIX "_prop_"

namespace kaguya {

#define KAGUYA_PP_STRUCT_TDEF_REP(N) KAGUYA_PP_CAT(class A, N) = void
#define KAGUYA_PP_STRUCT_TEMPLATE_DEF_REPEAT(N)                                \
  KAGUYA_PP_REPEAT_ARG(N, KAGUYA_PP_STRUCT_TDEF_REP)

template <KAGUYA_PP_STRUCT_TEMPLATE_DEF_REPEAT(KAGUYA_CLASS_MAX_BASE_CLASSES)>
struct MultipleBase {};
#undef KAGUYA_PP_STRUCT_TDEF_REP
#undef KAGUYA_PP_STRUCT_TEMPLATE_DEF_REPEAT
}

namespace kaguya {
struct LuaCodeChunk {
  LuaCodeChunk(const std::string &src, const std::string &name = "")
      : code_(src), chunk_name_(name) {}
  LuaCodeChunk(const char *src, const char *name = "")
      : code_(src), chunk_name_(name ? name : "") {}
  std::string code_;
  std::string chunk_name_;
};

/// @ingroup lua_type_traits
/// @brief lua_type_traits for LuaCodeChunk
template <> struct lua_type_traits<LuaCodeChunk> {
  static int push(lua_State *state, const LuaCodeChunk &ref) {
    int status = luaL_loadbuffer(
        state, ref.code_.c_str(), ref.code_.size(),
        ref.chunk_name_.empty() ? ref.code_.c_str() : ref.chunk_name_.c_str());
    if (!except::checkErrorAndThrow(status, state)) {
      return 0;
    }
    return 1;
  }
};
struct LuaCodeChunkExecute : LuaCodeChunk {
  LuaCodeChunkExecute(const std::string &src, const std::string &name = "")
      : LuaCodeChunk(src, name) {}
  LuaCodeChunkExecute(const char *src, const char *name = "")
      : LuaCodeChunk(src, name) {}
};
typedef LuaCodeChunkExecute LuaCodeChunkResult;
/// @ingroup lua_type_traits
/// @brief lua_type_traits for LuaCodeChunkResult
template <> struct lua_type_traits<LuaCodeChunkExecute> {
  static int push(lua_State *state, const LuaCodeChunkExecute &ref) {
    int status = luaL_loadbuffer(
        state, ref.code_.c_str(), ref.code_.size(),
        ref.chunk_name_.empty() ? ref.code_.c_str() : ref.chunk_name_.c_str());
    if (!except::checkErrorAndThrow(status, state)) {
      return 0;
    }
    status = lua_pcall_wrap(state, 0, 1);
    if (!except::checkErrorAndThrow(status, state)) {
      return 0;
    }
    return 1;
  }
};

namespace Metatable {
typedef std::map<std::string, AnyDataPusher> PropMapType;
typedef std::map<std::string, AnyDataPusher> MemberMapType;

inline bool is_property_key(const char *keyname) {
  return keyname &&
         strncmp(keyname, KAGUYA_PROPERTY_PREFIX,
                 sizeof(KAGUYA_PROPERTY_PREFIX) - 1) != 0;
}
inline int property_index_function(lua_State *L) {
  // Lua
  // local arg = {...};local metatable = arg[1];
  // return function(table, index)
  // if string.find(index,KAGUYA_PROPERTY_PREFIX)~=0 then
  // local propfun = metatable[KAGUYA_PROPERTY_PREFIX ..index];
  // if propfun then return propfun(table) end
  // end
  // return metatable[index]
  // end
  static const int table = 1;
  static const int key = 2;
  static const int metatable = lua_upvalueindex(1);
  const char *strkey = lua_tostring(L, key);

  if (lua_type(L, 1) == LUA_TUSERDATA && is_property_key(strkey)) {
    int type = lua_getfield_rtype(
        L, metatable, (KAGUYA_PROPERTY_PREFIX + std::string(strkey)).c_str());
    if (type == LUA_TFUNCTION) {
      lua_pushvalue(L, table);
      lua_call(L, 1, 1);
      return 1;
    }
  }
  lua_pushvalue(L, key);
  lua_gettable(L, metatable);
  return 1;
}
inline int property_newindex_function(lua_State *L) {
  // Lua
  // local arg = {...};local metatable = arg[1];
  // return function(table, index, value)
  // if type(table) == 'userdata' then
  // if string.find(index,KAGUYA_PROPERTY_PREFIX)~=0 then
  // local propfun = metatable[KAGUYA_PROPERTY_PREFIX..index];
  // if propfun then return propfun(table,value) end
  // end
  // end
  // rawset(table,index,value)
  // end
  static const int table = 1;
  static const int key = 2;
  static const int value = 3;
  static const int metatable = lua_upvalueindex(1);
  const char *strkey = lua_tostring(L, 2);

  if (lua_type(L, 1) == LUA_TUSERDATA && is_property_key(strkey)) {
    int type = lua_getfield_rtype(
        L, metatable, (KAGUYA_PROPERTY_PREFIX + std::string(strkey)).c_str());
    if (type == LUA_TFUNCTION) {
      lua_pushvalue(L, table);
      lua_pushvalue(L, value);
      lua_call(L, 2, 0);
      return 0;
    }
  }
  lua_pushvalue(L, key);
  lua_pushvalue(L, value);
  lua_rawset(L, table);
  return 0;
}

inline int multiple_base_index_function(lua_State *L) {
  // Lua
  // local arg = {...};local metabases = arg[1];
  // return function(t, k)
  // for i = 1,#metabases do
  // local v = metabases[i][k]
  // if v then
  // t[k] = v
  // return v end
  // end
  // end
  static const int table = 1;
  static const int key = 2;
  static const int metabases = lua_upvalueindex(1);

  lua_pushnil(L);
  while (lua_next(L, metabases) != 0) {
    if (lua_type(L, -1) == LUA_TTABLE) {
      lua_pushvalue(L, key);
      int type = lua_gettable_rtype(L, -2);
      if (type != LUA_TNIL) {
        lua_pushvalue(L, key);
        lua_pushvalue(L, -2);
        lua_settable(L, table);
        return 1;
      }
    }
    lua_settop(L, 3); // pop value
  }
  return 0;
}

inline int call_constructor_function(lua_State *L) {
  // function(t,...) return t.new(...) end
  lua_getfield(L, 1, "new");
  lua_replace(L, 1);
  lua_call(L, lua_gettop(L) - 1, LUA_MULTRET);
  return lua_gettop(L);
}
inline void get_call_constructor_metatable(lua_State *L) {
#if KAGUYA_SUPPORT_MULTIPLE_SHARED_LIBRARY
  static const char *key = "KAGUYA_CALL_CONSTRUCTOR_METATABLE_KEY";
  lua_pushstring(L, key);
#else
  static int key = 0;
  lua_pushlightuserdata(L, &key);
#endif
  int ttype = lua_rawget_rtype(L, LUA_REGISTRYINDEX);
  if (ttype != LUA_TTABLE) {
    lua_pop(L, 1);
    lua_createtable(L, 0, 1);
    lua_pushstring(L, "__call");
    lua_pushcfunction(L, &call_constructor_function);
    lua_rawset(L, -3);
#if KAGUYA_SUPPORT_MULTIPLE_SHARED_LIBRARY
    lua_pushstring(L, key);
#else
    lua_pushlightuserdata(L, &key);
#endif
    lua_pushvalue(L, -2);
    lua_rawset(L, LUA_REGISTRYINDEX);
  }
}

inline void setMembers(lua_State *state, int metatable_index,
                       const MemberMapType &member_map,
                       const PropMapType &property_map) {
  for (MemberMapType::const_iterator it = member_map.begin();
       it != member_map.end(); ++it) {
    util::one_push(state, it->first);
    util::one_push(state, it->second);
    lua_rawset(state, metatable_index);
  }
  for (PropMapType::const_iterator it = property_map.begin();
       it != property_map.end(); ++it) {
    util::one_push(state, KAGUYA_PROPERTY_PREFIX + it->first);
    util::one_push(state, it->second);
    lua_rawset(state, metatable_index);
  }
}

inline void setPropertyIndexMetamethod(lua_State *state, int metatable_index) {
  lua_pushstring(state, "__index");
  lua_pushvalue(state, metatable_index);
  lua_pushcclosure(state, &property_index_function, 1);
  lua_rawset(state, metatable_index);
}

inline void setPropertyNewIndexMetamethod(lua_State *state,
                                          int metatable_index) {
  lua_pushstring(state, "__newindex");
  lua_pushvalue(state, metatable_index);
  lua_pushcclosure(state, &property_newindex_function, 1);
  lua_rawset(state, metatable_index);
}
inline void setMultipleBase(lua_State *state, int metatable_index,
                            int metabase_array_index) {
  lua_createtable(state, 0, 1);
  int newmetaindex = lua_gettop(state);
  lua_pushstring(state, "__index");
  lua_pushvalue(state, metabase_array_index); // bind metabase_array to
                                              // multiple_base_index_function
  lua_pushcclosure(state, &multiple_base_index_function, 1);
  lua_rawset(state,
             newmetaindex); // newmeta["__index"] = multiple_base_index_function
  lua_setmetatable(state, metatable_index); // metatable.setMetatable(newmeta);
}
}

/// class binding interface.
template <typename class_type, typename base_class_type = void>
class UserdataMetatable {
public:
  UserdataMetatable() {
    addStaticFunction("__gc", &class_userdata::destructor<ObjectWrapperBase>);

    KAGUYA_STATIC_ASSERT(is_registerable<class_type>::value ||
                             !traits::is_std_vector<class_type>::value,
                         "std::vector is binding to lua-table by default.If "
                         "you wants register for std::vector yourself,"
                         "please define KAGUYA_NO_STD_VECTOR_TO_TABLE");

    KAGUYA_STATIC_ASSERT(is_registerable<class_type>::value ||
                             !traits::is_std_map<class_type>::value,
                         "std::map is binding to lua-table by default.If you "
                         "wants register for std::map yourself,"
                         "please define KAGUYA_NO_STD_MAP_TO_TABLE");

    // can not register push specialized class
    KAGUYA_STATIC_ASSERT(is_registerable<class_type>::value,
                         "Can not register specialized of type conversion "
                         "class. e.g. std::tuple");
  }

  bool pushCreateMatatable(lua_State *state) const {
    if (!class_userdata::newmetatable<class_type>(state)) {
      except::OtherError(state,
                         typeid(class_type *).name() +
                             std::string(" is already registered"));
      return false;
    }
    int metatable_index = lua_gettop(state);
    Metatable::setMembers(state, metatable_index, member_map_, property_map_);

    if (!traits::is_same<base_class_type, void>::value ||
        !property_map_.empty()) // if base class has property and derived class
                                // hasnt property. need property access
                                // metamethod
    {
      if (member_map_.count("__index") == 0) {
        Metatable::setPropertyIndexMetamethod(state, metatable_index);
      }

      if (member_map_.count("__newindex") == 0) {
        Metatable::setPropertyNewIndexMetamethod(state, metatable_index);
      }
    } else {
      if (member_map_.count("__index") == 0) {
        lua_pushstring(state, "__index");
        lua_pushvalue(state, metatable_index);
        lua_rawset(state, metatable_index);
      }
    }

    set_base_metatable(state, metatable_index,
                       types::typetag<base_class_type>());

    if (lua_getmetatable(state, metatable_index)) // get base_metatable
    {
      lua_pushstring(state, "__call");
      lua_pushcfunction(state, &Metatable::call_constructor_function);
      lua_rawset(state, -3); // base_metatable["__call"] =
                             // Metatable::call_constructor_function
    } else {
      Metatable::get_call_constructor_metatable(state);
      lua_setmetatable(state, metatable_index);
    }
    lua_settop(state, metatable_index);
    return true;
  }
  LuaTable createMatatable(lua_State *state) const {
    util::ScopedSavedStack save(state);
    if (!pushCreateMatatable(state)) {
      return LuaTable();
    }
    return LuaStackRef(state, -1);
  }

#if KAGUYA_USE_CPP11
  template <typename... ArgTypes> UserdataMetatable &setConstructors() {
    addOverloadedFunctions(
        "new", typename ConstructorFunction<class_type, ArgTypes>::type()...);
    return *this;
  }
#else
#define KAGUYA_SET_CON_TYPE_DEF(N)                                             \
  typename ConstructorFunction<class_type, KAGUYA_PP_CAT(A, N)>::type()
#define KAGUYA_SET_CON_FN_DEF(N)                                               \
  template <KAGUYA_PP_TEMPLATE_DEF_REPEAT(N)>                                  \
  inline UserdataMetatable &setConstructors() {                                \
    addOverloadedFunctions("new",                                              \
                           KAGUYA_PP_REPEAT_ARG(N, KAGUYA_SET_CON_TYPE_DEF));  \
    return *this;                                                              \
  }

  KAGUYA_PP_REPEAT_DEF(KAGUYA_FUNCTION_MAX_OVERLOADS, KAGUYA_SET_CON_FN_DEF)
#undef KAGUYA_SET_CON_FN_DEF
#undef KAGUYA_SET_CON_TYPE_DEF

#endif

  /// @brief add member property with getter function.(experimental)
  /// @param name function name for lua
  /// @param mem bind member data
  template <typename Ret>
  UserdataMetatable &addProperty(const char *name, Ret class_type::*mem) {
    if (has_key(name)) {
      throw KaguyaException(std::string(name) + " is already registered.");
      return *this;
    }
    property_map_[name] = AnyDataPusher(kaguya::function(mem));
    return *this;
  }

  /// @brief add member property with getter function.(experimental)
  /// @param name function name for lua
  /// @param getter getter function
  template <typename GetType>
  UserdataMetatable &addProperty(const char *name,
                                 GetType (class_type::*getter)() const) {
    if (has_key(name)) {
      throw KaguyaException(std::string(name) + " is already registered.");
      return *this;
    }
    property_map_[name] = AnyDataPusher(kaguya::function(getter));
    return *this;
  }

  /// @brief add member property with setter, getter functions.(experimental)
  /// @param name function name for lua
  /// @param getter getter function
  /// @param setter setter function
  template <typename GetType>
  UserdataMetatable &addProperty(const char *name,
                                 GetType (*getter)(const class_type *)) {
    if (has_key(name)) {
      throw KaguyaException(std::string(name) + " is already registered.");
      return *this;
    }
    property_map_[name] = AnyDataPusher(function(getter));
    return *this;
  }
  /// @brief add member property with setter, getter functions.(experimental)
  /// @param name function name for lua
  /// @param getter getter function
  /// @param setter setter function
  template <typename GetType>
  UserdataMetatable &addProperty(const char *name,
                                 GetType (*getter)(const class_type &)) {
    if (has_key(name)) {
      throw KaguyaException(std::string(name) + " is already registered.");
      return *this;
    }
    property_map_[name] = AnyDataPusher(function(getter));
    return *this;
  }

  /// @brief add member property with setter, getter functions.(experimental)
  /// @param name function name for lua
  /// @param getter getter function
  /// @param setter setter function
  template <typename GetType, typename SetType>
  UserdataMetatable &addProperty(const char *name,
                                 GetType (class_type::*getter)() const,
                                 void (class_type::*setter)(SetType)) {
    if (has_key(name)) {
      throw KaguyaException(std::string(name) + " is already registered.");
      return *this;
    }
    property_map_[name] = AnyDataPusher(overload(getter, setter));
    return *this;
  }

  /// @brief add member property with external setter, getter
  /// functions.(experimental)
  /// @param name function name for lua
  /// @param getter getter function
  /// @param setter setter function
  template <typename GetType, typename SetType>
  UserdataMetatable &addProperty(const char *name,
                                 GetType (*getter)(const class_type *),
                                 void (*setter)(class_type *, SetType)) {
    if (has_key(name)) {
      throw KaguyaException(std::string(name) + " is already registered.");
      return *this;
    }
    property_map_[name] = AnyDataPusher(overload(getter, setter));
    return *this;
  }

  /// @brief add member property with external setter, getter
  /// functions.(experimental)
  /// @param name function name for lua
  /// @param getter getter function
  /// @param setter setter function
  template <typename GetType, typename SetType>
  UserdataMetatable &addProperty(const char *name,
                                 GetType (*getter)(const class_type &),
                                 void (*setter)(class_type &, SetType)) {
    if (has_key(name)) {
      throw KaguyaException(std::string(name) + " is already registered.");
      return *this;
    }
    property_map_[name] = AnyDataPusher(overload(getter, setter));
    return *this;
  }

  /// @brief add member property with getter function.(experimental)
  /// @param name function name for lua
  /// @param getter getter function
  template <typename GetterType>
  UserdataMetatable &addPropertyAny(const char *name, GetterType getter) {
    if (has_key(name)) {
      throw KaguyaException(std::string(name) + " is already registered.");
      return *this;
    }
    property_map_[name] = AnyDataPusher(function(getter));
    return *this;
  }
  /// @brief add member property with setter, getter functions.(experimental)
  /// @param name function name for lua
  /// @param getter getter function
  /// @param setter setter function
  template <typename GetterType, typename SetterType>
  UserdataMetatable &addPropertyAny(const char *name, GetterType getter,
                                    SetterType setter) {
    if (has_key(name)) {
      throw KaguyaException(std::string(name) + " is already registered.");
      return *this;
    }
    property_map_[name] = AnyDataPusher(overload(getter, setter));
    return *this;
  }

  /// @brief add non member function
  /// @param name function name for lua
  /// @param f function
  template <typename Fun>
  UserdataMetatable &addStaticFunction(const char *name, Fun f) {
    if (has_key(name)) {
      throw KaguyaException(std::string(name) + " is already registered.");
      return *this;
    }
    member_map_[name] = AnyDataPusher(kaguya::function(f));
    return *this;
  }

#if KAGUYA_USE_CPP11
  /// @brief assign overloaded from functions.
  /// @param name name for lua
  /// @param f functions
  template <typename... Funcs>
  UserdataMetatable &addOverloadedFunctions(const char *name, Funcs... f) {
    if (has_key(name)) {
      throw KaguyaException(std::string(name) + " is already registered.");
      return *this;
    }

    member_map_[name] = AnyDataPusher(overload(f...));

    return *this;
  }

  /// @brief assign data by argument value.
  /// @param name name for lua
  /// @param d data
  template <typename Data>
  UserdataMetatable &addStaticField(const char *name, Data &&d) {
    if (has_key(name)) {
      throw KaguyaException(std::string(name) + " is already registered.");
      return *this;
    }
    member_map_[name] = AnyDataPusher(std::forward<Data>(d));
    return *this;
  }
#else

#define KAGUYA_ADD_OVERLOAD_FUNCTION_DEF(N)                                    \
  template <KAGUYA_PP_TEMPLATE_DEF_REPEAT(N)>                                  \
  inline UserdataMetatable &addOverloadedFunctions(                            \
      const char *name, KAGUYA_PP_ARG_CR_DEF_REPEAT(N)) {                      \
    if (has_key(name)) {                                                       \
      throw KaguyaException(std::string(name) + " is already registered.");    \
      return *this;                                                            \
    }                                                                          \
    member_map_[name] =                                                        \
        AnyDataPusher(kaguya::overload(KAGUYA_PP_ARG_REPEAT(N)));              \
    return *this;                                                              \
  }

  KAGUYA_PP_REPEAT_DEF(KAGUYA_FUNCTION_MAX_OVERLOADS,
                       KAGUYA_ADD_OVERLOAD_FUNCTION_DEF)
#undef KAGUYA_ADD_OVERLOAD_FUNCTION_DEF

  /// @brief assign data by argument value.
  /// @param name name for lua
  /// @param d data
  template <typename Data>
  UserdataMetatable &addStaticField(const char *name, const Data &d) {
    if (has_key(name)) {
      throw KaguyaException(std::string(name) + " is already registered.");
      return *this;
    }
    member_map_[name] = AnyDataPusher(d);
    return *this;
  }
#endif

#if defined(_MSC_VER) && _MSC_VER <= 1800
  // can not write  Ret class_type::* f on MSC++2013
  template <typename Fun>
  UserdataMetatable &addFunction(const char *name, Fun f) {
    if (has_key(name)) {
      throw KaguyaException(std::string(name) + " is already registered. To "
                                                "overload a function, use "
                                                "addOverloadedFunctions");
      return *this;
    }
    member_map_[name] = AnyDataPusher(kaguya::function(f));
    return *this;
  }
#else
  /// @brief assign function
  /// @param name name for lua
  /// @param f pointer to member function.
  template <typename Ret>
  UserdataMetatable &addFunction(const char *name, Ret class_type::*f) {
    if (has_key(name)) {
      throw KaguyaException(std::string(name) + " is already registered. To "
                                                "overload a function, use "
                                                "addOverloadedFunctions");
      return *this;
    }
    member_map_[name] = AnyDataPusher(kaguya::function(f));
    return *this;
  }
#endif
  /// @brief assign function
  /// @param name name for lua
  /// @param f member function object.
  UserdataMetatable &addFunction(const char *name, PolymorphicMemberInvoker f) {
    if (has_key(name)) {
      throw KaguyaException(std::string(name) + " is already registered. To "
                                                "overload a function, use "
                                                "addOverloadedFunctions");
      return *this;
    }
    member_map_[name] = AnyDataPusher(kaguya::function(f));
    return *this;
  }

private:
  void set_base_metatable(lua_State *, int, types::typetag<void>) const {}
  template <class Base>
  void set_base_metatable(lua_State *state, int metatable_index,
                          types::typetag<Base>) const {
    class_userdata::get_metatable<Base>(state);
    lua_setmetatable(state,
                     metatable_index); // metatable.setMetatable(newmeta);
    PointerConverter &pconverter = PointerConverter::get(state);
    pconverter.add_type_conversion<Base, class_type>();
  }

#if KAGUYA_USE_CPP11

  template <typename Base>
  void metatables(lua_State *state, int metabase_array_index,
                  PointerConverter &pvtreg,
                  types::typetag<MultipleBase<Base> >) const {
    class_userdata::get_metatable<Base>(state);
    lua_rawseti(state, metabase_array_index,
                lua_rawlen(state, metabase_array_index) + 1);
    pvtreg.add_type_conversion<Base, class_type>();
  }
  template <typename Base, typename... Remain>
  void metatables(lua_State *state, int metabase_array_index,
                  PointerConverter &pvtreg,
                  types::typetag<MultipleBase<Base, Remain...> >) const {
    class_userdata::get_metatable<Base>(state);
    lua_rawseti(state, metabase_array_index,
                lua_rawlen(state, metabase_array_index) + 1);
    pvtreg.add_type_conversion<Base, class_type>();
    metatables(state, metabase_array_index, pvtreg,
               types::typetag<MultipleBase<Remain...> >());
  }

  template <typename... Bases>
  void
  set_base_metatable(lua_State *state, int metatable_index,
                     types::typetag<MultipleBase<Bases...> > metatypes) const {
    PointerConverter &pconverter = PointerConverter::get(state);

    lua_createtable(state, sizeof...(Bases), 0);
    int metabase_array_index = lua_gettop(state);
    metatables(state, metabase_array_index, pconverter, metatypes);
    Metatable::setMultipleBase(state, metatable_index, metabase_array_index);
  }

#else
#define KAGUYA_GET_BASE_METATABLE(N)                                           \
  class_userdata::get_metatable<KAGUYA_PP_CAT(A, N)>(state);                   \
  lua_rawseti(state, metabase_array_index,                                     \
              lua_rawlen(state, metabase_array_index) + 1);                    \
  pconverter.add_type_conversion<KAGUYA_PP_CAT(A, N), class_type>();
#define KAGUYA_MULTIPLE_INHERITANCE_SETBASE_DEF(N)                             \
  template <KAGUYA_PP_TEMPLATE_DEF_REPEAT(N)>                                  \
  void set_base_metatable(                                                     \
      lua_State *state, int metatable_index,                                   \
      types::typetag<MultipleBase<KAGUYA_PP_TEMPLATE_ARG_REPEAT(N)> >) const { \
    PointerConverter &pconverter = PointerConverter::get(state);               \
    lua_createtable(state, N, 0);                                              \
    int metabase_array_index = lua_gettop(state);                              \
    KAGUYA_PP_REPEAT(N, KAGUYA_GET_BASE_METATABLE);                            \
    Metatable::setMultipleBase(state, metatable_index, metabase_array_index);  \
  }

  KAGUYA_PP_REPEAT_DEF(KAGUYA_CLASS_MAX_BASE_CLASSES,
                       KAGUYA_MULTIPLE_INHERITANCE_SETBASE_DEF)
#undef KAGUYA_MULTIPLE_INHERITANCE_SETBASE_DEF
#undef KAGUYA_GET_BASE_METATABLE
#endif

  bool has_key(const std::string &key) {
    if (member_map_.count(key) > 0) {
      return true;
    }
    if (property_map_.count(key) > 0) {
      return true;
    }
    std::string propkey = KAGUYA_PROPERTY_PREFIX + key;
    if (member_map_.count(propkey) > 0) //_prop_keyname is reserved for property
    {
      return true;
    }
    return false;
  }

  Metatable::PropMapType property_map_;
  Metatable::MemberMapType member_map_;
};

/// @ingroup lua_type_traits
/// @brief lua_type_traits for UserdataMetatable
template <typename T, typename Base>
struct lua_type_traits<UserdataMetatable<T, Base> > {
  typedef const UserdataMetatable<T, Base> &push_type;

  static int push(lua_State *l, push_type ref) {
    ref.pushCreateMatatable(l);
    return 1;
  }
};
}

// Copyright satoren
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#include <vector>
#include <map>

namespace kaguya {
class State;

/**
* This class is the type returned by members of non-const LuaRef(Table) when
* directly accessing its elements.
*/
template <typename KEY>
class TableKeyReferenceProxy
    : public detail::LuaVariantImpl<TableKeyReferenceProxy<KEY> > {
public:
  int pushStackIndex(lua_State *state) const {
    push(state);
    return lua_gettop(state);
  }
  lua_State *state() const { return state_; }

  friend class LuaRef;
  friend class State;

  //! this is not copy.same assign from referenced value.
  TableKeyReferenceProxy &operator=(const TableKeyReferenceProxy &src) {
    detail::table_proxy::set(state_, table_index_, key_, src);
    return *this;
  }

  //! assign from T
  template <typename T> TableKeyReferenceProxy &operator=(const T &src) {
    detail::table_proxy::set(state_, table_index_, key_, src);
    return *this;
  }
#if KAGUYA_USE_CPP11
  template <typename T> TableKeyReferenceProxy &operator=(T &&src) {
    detail::table_proxy::set(state_, table_index_, key_, std::forward<T>(src));
    return *this;
  }
#endif

  bool isNilref() const {
    if (!state_) {
      return false;
    }
    util::ScopedSavedStack save(state_);
    push(state_);
    return lua_isnoneornil(state_, -1);
  }

  //! register class metatable to lua and set to table
  template <typename T, typename P>
  void setClass(const UserdataMetatable<T, P> &reg) {
    set_class(reg);
  }

  //! set function
  template <typename T> void setFunction(T f) {
    detail::table_proxy::set(state_, table_index_, key_, kaguya::function(f));
  }

  int push(lua_State *state) const {
    int type = lua_type(state_, table_index_);
    if (type != LUA_TTABLE && type != LUA_TUSERDATA) {
      lua_pushnil(state);
      return 1;
    }

    detail::table_proxy::get(state_, table_index_, key_);

    if (state_ != state) {
      lua_xmove(state_, state, 1);
    }
    return 1;
  }

  int push() const { return push(state_); }

  int type() const {
    util::ScopedSavedStack save(state_);
    push();
    return lua_type(state_, -1);
  }

  ~TableKeyReferenceProxy() {
    if (state_) {
      lua_settop(state_, stack_top_);
    }
  }

  ///!constructs the reference. Accessible only to kaguya::LuaRef itself
  TableKeyReferenceProxy(const TableKeyReferenceProxy &src)
      : state_(src.state_), stack_top_(src.stack_top_),
        table_index_(src.table_index_), key_(src.key_) {
    src.state_ = 0;
  }

  ///!constructs the reference. Accessible only to kaguya::LuaRef itself
  TableKeyReferenceProxy(lua_State *state, int table_index, KEY key,
                         int revstacktop)
      : state_(state), stack_top_(revstacktop), table_index_(table_index),
        key_(key) {}

private:
  template <typename T, typename P>
  void set_class(const UserdataMetatable<T, P> &reg) {
    detail::table_proxy::set(state_, table_index_, key_,reg);
  }

  ///!constructs the reference. Accessible only to kaguya::LuaRef itself
  TableKeyReferenceProxy(lua_State *state, int table_index, const KEY &key,
                         int revstacktop, const NoTypeCheck &)
      : state_(state), stack_top_(revstacktop), table_index_(table_index),
        key_(key) {}

  TableKeyReferenceProxy(const LuaTable &table, const KEY &key)
      : state_(table.state()), stack_top_(lua_gettop(state_)), key_(key) {
    util::one_push(state_, table);
    table_index_ = stack_top_ + 1;
  }
  TableKeyReferenceProxy(const LuaRef &table, const KEY &key)
      : state_(table.state()), stack_top_(lua_gettop(state_)), key_(key) {
    util::one_push(state_, table);
    table_index_ = stack_top_ + 1;
    int t = lua_type(state_, table_index_);
    if (t != LUA_TTABLE) {
      except::typeMismatchError(state_, lua_typename(state_, t) +
                                            std::string(" is not table"));
    }
  }

  mutable lua_State *state_; // mutable for RVO unsupported compiler
  int stack_top_;
  int table_index_;
  KEY key_;
};

template <typename KEY>
inline std::ostream &operator<<(std::ostream &os,
                                const TableKeyReferenceProxy<KEY> &ref) {
  lua_State *state = ref.state();
  util::ScopedSavedStack save(state);
  int stackIndex = ref.pushStackIndex(state);
  util::stackValueDump(os, state, stackIndex);
  return os;
}

namespace detail {
template <typename T>
inline bool LuaFunctionImpl<T>::setFunctionEnv(const LuaTable &env) {
  lua_State *state = state_();
  if (!state) {
    return false;
  }
  util::ScopedSavedStack save(state);
  int stackIndex = pushStackIndex_(state);
  int t = lua_type(state, stackIndex);
  if (t != LUA_TFUNCTION) {
    except::typeMismatchError(state, lua_typename(state, t) +
                                         std::string(" is not function"));
    return false;
  }
  env.push(state);
#if LUA_VERSION_NUM >= 502
  lua_setupvalue(state, stackIndex, 1);
#else
  lua_setfenv(state, stackIndex);
#endif
  return true;
}
template <typename T> inline bool LuaFunctionImpl<T>::setFunctionEnv(NewTable) {
  return setFunctionEnv(LuaTable(state_()));
}

template <typename T>
inline LuaTable LuaFunctionImpl<T>::getFunctionEnv() const {
  lua_State *state = state_();
  util::ScopedSavedStack save(state);
  if (!state) {
    except::typeMismatchError(state, "is nil");
    return LuaTable();
  }
  int stackIndex = pushStackIndex_(state);
  int t = lua_type(state, stackIndex);
  if (t != LUA_TFUNCTION) {
    except::typeMismatchError(state, lua_typename(state, t) +
                                         std::string(" is not function"));
    return LuaTable();
  }
#if LUA_VERSION_NUM >= 502
  lua_getupvalue(state, stackIndex, 1);
#else
  lua_getfenv(state, stackIndex);
#endif
  return LuaTable(state, StackTop());
}

template <typename T> void LuaThreadImpl<T>::setFunction(const LuaFunction &f) {
  lua_State *corstate = getthread();
  if (corstate) {
    lua_settop(corstate, 0);
    f.push(corstate);
  }
}

template <typename T>
bool LuaTableOrUserDataImpl<T>::setMetatable(const LuaTable &table) {
  lua_State *state = state_();
  if (!state) {
    except::typeMismatchError(state, "is nil");
    return false;
  }
  util::ScopedSavedStack save(state);
  int stackindex = pushStackIndex_(state);
  int t = lua_type(state, stackindex);
  if (t != LUA_TTABLE && t != LUA_TUSERDATA) {
    except::typeMismatchError(state, lua_typename(state, t) +
                                         std::string(" is not table"));
    return false;
  }
  table.push();
  return lua_setmetatable(state, stackindex) != 0;
}
template <typename T> LuaTable LuaTableOrUserDataImpl<T>::getMetatable() const {
  lua_State *state = state_();
  if (!state) {
    except::typeMismatchError(state, "is nil");
    return LuaTable();
  }
  util::ScopedSavedStack save(state);
  int stackindex = pushStackIndex_(state);
  int t = lua_type(state, stackindex);
  if (t != LUA_TTABLE && t != LUA_TUSERDATA) {
    except::typeMismatchError(state, lua_typename(state, t) +
                                         std::string(" is not table"));
    return LuaTable();
  }
  if (!lua_getmetatable(state, stackindex)) {
    lua_pushnil(state);
  }
  return LuaTable(state, StackTop());
}
template <typename T>
MemberFunctionBinder LuaTableOrUserDataImpl<T>::
operator->*(const char *function_name) {
  push_(state_());
  return MemberFunctionBinder(LuaRef(state_(), StackTop()), function_name);
}

template <typename T>
template <typename KEY>
LuaStackRef LuaTableOrUserDataImpl<T>::getField(const KEY &key) const {
  lua_State *state = state_();
  if (!state) {
    except::typeMismatchError(state, "is nil");
    return LuaStackRef();
  }
  push_(state);
  detail::table_proxy::get(state, lua_gettop(state), key);
  lua_remove(state, -2); // remove table
  return LuaStackRef(state, -1, true);
}
template <typename T>
template <typename KEY>
LuaStackRef LuaTableImpl<T>::getRawField(const KEY &key) const {
  lua_State *state = state_();
  if (!state) {
    except::typeMismatchError(state, "is nil");
    return LuaStackRef();
  }
  push_(state);
  detail::table_proxy::rawget(state, lua_gettop(state), key);
  lua_remove(state, -2); // remove table
  return LuaStackRef(state, -1, true);
}

template <typename T>
template <typename KEY>
LuaStackRef LuaTableOrUserDataImpl<T>::operator[](KEY key) const {
  return getField(key);
}

template <typename T> std::vector<LuaRef> LuaTableImpl<T>::values() const {
  return values<LuaRef>();
}
template <typename T> std::vector<LuaRef> LuaTableImpl<T>::keys() const {
  return keys<LuaRef>();
}
template <typename T> std::map<LuaRef, LuaRef> LuaTableImpl<T>::map() const {
  return map<LuaRef, LuaRef>();
}

template <typename T>
template <typename K>
TableKeyReferenceProxy<K> LuaTableOrUserDataImpl<T>::operator[](K key) {
  lua_State *state = state_();
  int stack_top = lua_gettop(state);
  int stackindex = pushStackIndex_(state);
  return TableKeyReferenceProxy<K>(state, stackindex, key, stack_top);
}
}

/// @ingroup lua_type_traits
/// @brief lua_type_traits for TableKeyReferenceProxy<KEY>
template <typename KEY> struct lua_type_traits<TableKeyReferenceProxy<KEY> > {
  static int push(lua_State *l, const TableKeyReferenceProxy<KEY> &ref) {
    return ref.push(l);
  }
};

#if KAGUYA_USE_CPP11
/// @ingroup lua_type_traits
/// @brief lua_type_traits for std::array<T, A>
template <typename T, size_t S> struct lua_type_traits<std::array<T, S> > {
  typedef std::array<T, S> get_type;
  typedef const std::array<T, S> &push_type;

  static bool checkType(lua_State *l, int index) {
    if (lua_type(l, index) != LUA_TTABLE) {
      return false;
    }

    LuaStackRef table(l, index);
    if (table.size() != S) {
      return false;
    } // TODO
    bool valid = true;
    table.foreach_table_breakable<LuaStackRef, LuaStackRef>(
        [&](const LuaStackRef &k, const LuaStackRef &v) {
          valid = valid && k.typeTest<size_t>() && v.typeTest<T>();
          return valid;
        });
    return valid;
  }
  static bool strictCheckType(lua_State *l, int index) {
    if (lua_type(l, index) != LUA_TTABLE) {
      return false;
    }

    LuaStackRef table(l, index);
    if (table.size() != S) {
      return false;
    } // TODO
    bool valid = true;
    table.foreach_table_breakable<LuaStackRef, LuaStackRef>(
        [&](const LuaStackRef &k, const LuaStackRef &v) {
          valid = valid && k.typeTest<size_t>() && v.typeTest<T>();
          return valid;
        });
    return valid;
  }
  static get_type get(lua_State *l, int index) {
    if (lua_type(l, index) != LUA_TTABLE) {
      except::typeMismatchError(l, std::string("type mismatch"));
      return get_type();
    }
    LuaStackRef t(l, index);
    if (t.size() != S) // TODO
    {
      except::typeMismatchError(l, std::string("type mismatch"));
    }
    get_type res;
    t.foreach_table<size_t, const T &>([&](size_t k, const T &v) {
      if (k > 0 && k <= S) {
        res[k - 1] = v;
      }
    });
    return res;
  }
  static int push(lua_State *l, push_type v) {
    lua_createtable(l, int(S), 0);
    for (size_t i = 0; i < S; ++i) {
      util::one_push(l, v[i]);
      lua_rawseti(l, -2, i + 1);
    }
    return 1;
  }
};
#endif
#ifndef KAGUYA_NO_STD_VECTOR_TO_TABLE

/// @ingroup lua_type_traits
/// @brief lua_type_traits for std::vector<T, A>
template <typename T, typename A> struct lua_type_traits<std::vector<T, A> > {
  typedef std::vector<T, A> get_type;
  typedef const std::vector<T, A> &push_type;
  struct checkTypeForEach {
    checkTypeForEach(bool &valid) : valid_(valid) {}
    bool &valid_;
    bool operator()(const LuaStackRef &k, const LuaStackRef &v) {
      valid_ = k.typeTest<size_t>() && v.weakTypeTest<T>();
      return valid_;
    }
  };
  struct strictCheckTypeForEach {
    strictCheckTypeForEach(bool &valid) : valid_(valid) {}
    bool &valid_;
    bool operator()(const LuaStackRef &k, const LuaStackRef &v) {
      valid_ = k.typeTest<size_t>() && v.typeTest<T>();
      return valid_;
    }
  };

  static bool checkType(lua_State *l, int index) {
    LuaStackRef table(l, index);
    if (table.type() != LuaRef::TYPE_TABLE) {
      return false;
    }

    bool valid = true;
    table.foreach_table_breakable<LuaStackRef, LuaStackRef>(
        checkTypeForEach(valid));
    return valid;
  }
  static bool strictCheckType(lua_State *l, int index) {
    LuaStackRef table(l, index);
    if (table.type() != LuaRef::TYPE_TABLE) {
      return false;
    }

    bool valid = true;
    table.foreach_table_breakable<LuaStackRef, LuaStackRef>(
        strictCheckTypeForEach(valid));
    return valid;
  }

  static get_type get(lua_State *l, int index) {
    if (lua_type(l, index) != LUA_TTABLE) {
      except::typeMismatchError(l, std::string("type mismatch"));
      return get_type();
    }
    return LuaStackRef(l, index).values<T>();
  }
#if KAGUYA_USE_CPP11
  typedef std::vector<T, A> &&move_push_type;
  static int push(lua_State *l, move_push_type v) {
    lua_createtable(l, int(v.size()), 0);
    int count = 1; // array is 1 origin in Lua
    for (typename std::vector<T, A>::iterator it = v.begin(); it != v.end();
         ++it) {
      util::one_push(l, static_cast<T &&>(*it));
      lua_rawseti(l, -2, count++);
    }
    return 1;
  }
#endif
  static int push(lua_State *l, push_type v) {
    lua_createtable(l, int(v.size()), 0);
    int count = 1; // array is 1 origin in Lua
    for (typename std::vector<T, A>::const_iterator it = v.begin();
         it != v.end(); ++it) {
      util::one_push(l, static_cast<const T &>(*it));
      lua_rawseti(l, -2, count++);
    }
    return 1;
  }
};
#endif

#ifndef KAGUYA_NO_STD_MAP_TO_TABLE
/// @ingroup lua_type_traits
/// @brief lua_type_traits for std::map<K, V, C, A>
template <typename K, typename V, typename C, typename A>
struct lua_type_traits<std::map<K, V, C, A> > {
  typedef std::map<K, V, C, A> get_type;
  typedef const std::map<K, V, C, A> &push_type;

  struct checkTypeForEach {
    checkTypeForEach(bool &valid) : valid_(valid) {}
    bool &valid_;
    bool operator()(const LuaStackRef &k, const LuaStackRef &v) {
      valid_ = k.weakTypeTest<K>() && v.weakTypeTest<V>();
      return valid_;
    }
  };
  struct strictCheckTypeForEach {
    strictCheckTypeForEach(bool &valid) : valid_(valid) {}
    bool &valid_;
    bool operator()(const LuaStackRef &k, const LuaStackRef &v) {
      valid_ = k.typeTest<K>() && v.typeTest<V>();
      return valid_;
    }
  };
  static bool checkType(lua_State *l, int index) {
    LuaStackRef table(l, index);
    if (table.type() != LuaRef::TYPE_TABLE) {
      return false;
    }

    bool valid = true;
    table.foreach_table_breakable<LuaStackRef, LuaStackRef>(
        checkTypeForEach(valid));
    return valid;
  }
  static bool strictCheckType(lua_State *l, int index) {
    LuaStackRef table(l, index);
    if (table.type() != LuaRef::TYPE_TABLE) {
      return false;
    }

    bool valid = true;
    table.foreach_table_breakable<LuaStackRef, LuaStackRef>(
        strictCheckTypeForEach(valid));
    return valid;
  }

  static get_type get(lua_State *l, int index) {
    if (lua_type(l, index) != LUA_TTABLE) {
      except::typeMismatchError(l, std::string("type mismatch"));
      return get_type();
    }
    return LuaStackRef(l, index).map<K, V>();
  }
  static int push(lua_State *l, push_type v) {
    lua_createtable(l, 0, int(v.size()));
    for (typename std::map<K, V, C, A>::const_iterator it = v.begin();
         it != v.end(); ++it) {
      util::one_push(l, it->first);
      util::one_push(l, it->second);
      lua_rawset(l, -3);
    }
    return 1;
  }
};
#endif

struct TableDataElement {
  typedef std::pair<AnyDataPusher, AnyDataPusher> keyvalue_type;

  template <typename Value>
  TableDataElement(Value value)
      : keyvalue(keyvalue_type(AnyDataPusher(), value)) {}

  template <typename Key, typename Value>
  TableDataElement(Key key, Value value)
      : keyvalue(keyvalue_type(key, value)) {}
  std::pair<AnyDataPusher, AnyDataPusher> keyvalue;
};

struct TableData {
  typedef std::pair<AnyDataPusher, AnyDataPusher> data_type;

#if KAGUYA_USE_CPP11
  TableData(std::initializer_list<TableDataElement> list)
      : elements(list.begin(), list.end()) {}
#endif
  template <typename IT> TableData(IT beg, IT end) : elements(beg, end) {}

  TableData() {}
  std::vector<TableDataElement> elements;
};

/// @ingroup lua_type_traits
/// @brief lua_type_traits for TableData
template <> struct lua_type_traits<TableData> {
  static int push(lua_State *l, const TableData &list) {
    lua_createtable(l, int(list.elements.size()), int(list.elements.size()));
    int count = 1; // array is 1 origin in Lua
    for (std::vector<TableDataElement>::const_iterator it =
             list.elements.begin();
         it != list.elements.end(); ++it) {
      const TableDataElement &v = *it;
      if (v.keyvalue.first.empty()) {
        util::one_push(l, v.keyvalue.second);
        lua_rawseti(l, -2, count++);
      } else {
        util::one_push(l, v.keyvalue.first);
        util::one_push(l, v.keyvalue.second);
        lua_rawset(l, -3);
      }
    }
    return 1;
  }
};
}

namespace kaguya {
/// @addtogroup State
/// @{

/// @brief Load library info type @see State::openlibs @see State::State(const
/// LoadLibs &libs)
typedef std::pair<std::string, lua_CFunction> LoadLib;

/// @brief Load libraries info @see State::openlibs @see State::State(const
/// LoadLibs &libs)
typedef std::vector<LoadLib> LoadLibs;

/// @brief return no load library type @see State::State(const LoadLibs &libs)
inline LoadLibs NoLoadLib() { return LoadLibs(); }

/// @brief All load standard libraries type @see State::openlibs
struct AllLoadLibs {};

template <typename Allocator>
void *AllocatorFunction(void *ud, void *ptr, size_t osize, size_t nsize) {
  Allocator *allocator = static_cast<Allocator *>(ud);
  if (nsize == 0) {
    allocator->deallocate(ptr, osize);
  } else if (ptr) {
    return allocator->reallocate(ptr, nsize);
  } else {
    return allocator->allocate(nsize);
  }
  return 0;
}

struct DefaultAllocator {
  typedef void *pointer;
  typedef size_t size_type;
  pointer allocate(size_type n) { return std::malloc(n); }
  pointer reallocate(pointer p, size_type n) { return std::realloc(p, n); }
  void deallocate(pointer p, size_type n) {
    KAGUYA_UNUSED(n);
    std::free(p);
  }
};

/// lua_State wrap class
class State {
  standard::shared_ptr<void> allocator_holder_;
  lua_State *state_;
  bool created_;

  // non copyable
  State(const State &);
  State &operator=(const State &);

  static int initializing_panic(lua_State *L) {
    ErrorHandler::throwDefaultError(lua_status(L), lua_tostring(L, -1));
    return 0; // return to Lua to abort
  }
  static int default_panic(lua_State *L) {
    if (ErrorHandler::handle(lua_status(L), L)) {
      return 0;
    }
    fprintf(stderr, "PANIC: unprotected error in call to Lua API (%s)\n",
            lua_tostring(L, -1));
    fflush(stderr);
    return 0; // return to Lua to abort
  }
  static void stderror_out(int status, const char *message) {
    KAGUYA_UNUSED(status);
    std::cerr << message << std::endl;
  }

  template <typename Libs> void init(const Libs &lib) {
    if (state_) {
      lua_atpanic(state_, &initializing_panic);
      try {
        if (!ErrorHandler::getHandler(state_)) {
          setErrorHandler(&stderror_out);
        }
        registerMainThreadIfNeeded();
        openlibs(lib);
        lua_atpanic(state_, &default_panic);
      } catch (const LuaException &) {
        lua_close(state_);
        state_ = 0;
      }
    }
  }

  void registerMainThreadIfNeeded() {
#if LUA_VERSION_NUM < 502
    if (state_) {
      util::registerMainThread(state_);
    }
#endif
  }

public:
  /// @brief create Lua state with lua standard library
  State() : allocator_holder_(), state_(luaL_newstate()), created_(true) {
    init(AllLoadLibs());
  }

  /// @brief create Lua state with lua standard library. Can not use this
  /// constructor at luajit. error message is 'Must use luaL_newstate() for 64
  /// bit target'
  /// @param allocator allocator for memory allocation @see DefaultAllocator
  template <typename Allocator>
  State(standard::shared_ptr<Allocator> allocator)
      : allocator_holder_(allocator),
        state_(lua_newstate(&AllocatorFunction<Allocator>,
                            allocator_holder_.get())),
        created_(true) {
    init(AllLoadLibs());
  }

  /// @brief create Lua state with (or without) libraries.
  /// @param libs load libraries
  /// e.g. LoadLibs libs;libs.push_back(LoadLib("libname",libfunction));State
  /// state(libs);
  /// e.g. State state({{"libname",libfunction}}); for c++ 11
  State(const LoadLibs &libs)
      : allocator_holder_(), state_(luaL_newstate()), created_(true) {
    init(libs);
  }

  /// @brief create Lua state with (or without) libraries. Can not use this
  /// constructor at luajit. error message is 'Must use luaL_newstate() for 64
  /// bit target'
  /// @param libs load libraries
  /// @param allocator allocator for memory allocation @see DefaultAllocator
  template <typename Allocator>
  State(const LoadLibs &libs, standard::shared_ptr<Allocator> allocator)
      : allocator_holder_(allocator),
        state_(lua_newstate(&AllocatorFunction<Allocator>,
                            allocator_holder_.get())),
        created_(true) {
    init(libs);
  }

  /// @brief construct using created lua_State.
  /// @param lua created lua_State. It is not call lua_close() in this class
  State(lua_State *lua) : state_(lua), created_(false) {
    if (state_) {
      registerMainThreadIfNeeded();
      if (!ErrorHandler::getHandler(state_)) {
        setErrorHandler(&stderror_out);
      }
    }
  }
  ~State() {
    if (created_ && state_) {
      lua_close(state_);
    }
  }

  /// @brief set error handler for lua error.
  void
  setErrorHandler(standard::function<void(int statuscode, const char *message)>
                      errorfunction) {
    if (!state_) {
      return;
    }
    util::ScopedSavedStack save(state_);
    ErrorHandler::registerHandler(state_, errorfunction);
  }

  /// @brief load all lua standard library
  void openlibs(AllLoadLibs = AllLoadLibs()) {
    if (!state_) {
      return;
    }
    luaL_openlibs(state_);
  }

  /// @brief load lua library
  LuaStackRef openlib(const LoadLib &lib) {
    if (!state_) {
      return LuaStackRef();
    }
    luaL_requiref(state_, lib.first.c_str(), lib.second, 1);
    return LuaStackRef(state_, -1, true);
  }
  /// @brief load lua library
  LuaStackRef openlib(std::string name, lua_CFunction f) {
    return openlib(LoadLib(name, f));
  }

  /// @brief load lua libraries
  void openlibs(const LoadLibs &libs) {
    for (LoadLibs::const_iterator it = libs.begin(); it != libs.end(); ++it) {
      openlib(*it);
    }
  }

  /// @brief If there are no errors,compiled file as a Lua function and return.
  ///  Otherwise send error message to error handler and return nil reference
  /// @param file  file path of lua script
  /// @return reference of lua function
  LuaFunction loadfile(const std::string &file) {
    return LuaFunction::loadfile(state_, file);
  }
  /// @brief If there are no errors,compiled file as a Lua function and return.
  ///  Otherwise send error message to error handler and return nil reference
  /// @param file  file path of lua script
  /// @return reference of lua function
  LuaFunction loadfile(const char *file) {
    return LuaFunction::loadfile(state_, file);
  }

  /// @brief If there are no errors,compiled stream as a Lua function and
  /// return.
  ///  Otherwise send error message to error handler and return nil reference
  /// @param stream stream of lua script
  /// @param chunkname chunkname of lua script
  /// @return reference of lua function
  LuaFunction loadstream(std::istream &stream, const char *chunkname = 0) {
    return LuaFunction::loadstream(state_, stream, chunkname);
  }
  /// @brief Loads and runs the given stream.
  /// @param stream stream of lua script
  /// @param chunkname chunkname of lua script
  /// @param env execute env table
  /// @return If there are no errors, returns true.Otherwise return false
  bool dostream(std::istream &stream, const char *chunkname = 0,
                const LuaTable &env = LuaTable()) {
    util::ScopedSavedStack save(state_);
    LuaStackRef f = LuaFunction::loadstreamtostack(state_, stream, chunkname);
    if (!f) { // load failed
      return false;
    }
    if (!env.isNilref()) {
      f.setFunctionEnv(env);
    }

    FunctionResults ret = f.call<FunctionResults>();
    return !ret.resultStatus();
  }

  /// @brief If there are no errors,compiled string as a Lua function and
  /// return.
  ///  Otherwise send error message to error handler and return nil reference
  /// @param str lua code
  /// @return reference of lua function
  LuaFunction loadstring(const std::string &str) {
    return LuaFunction::loadstring(state_, str);
  }
  /// @brief If there are no errors,compiled string as a Lua function and
  /// return.
  ///  Otherwise send error message to error handler and return nil reference
  /// @param str lua code
  /// @return reference of lua function
  LuaFunction loadstring(const char *str) {
    return LuaFunction::loadstring(state_, str);
  }

  /// @brief Loads and runs the given file.
  /// @param file file path of lua script
  /// @param env execute env table
  /// @return If there are no errors, returns true.Otherwise return false
  bool dofile(const std::string &file, const LuaTable &env = LuaTable()) {
    return dofile(file.c_str(), env);
  }

  /// @brief Loads and runs the given file.
  /// @param file file path of lua script
  /// @param env execute env table
  /// @return If there are no errors, returns true.Otherwise return false
  bool dofile(const char *file, const LuaTable &env = LuaTable()) {
    util::ScopedSavedStack save(state_);

    int status = luaL_loadfile(state_, file);

    if (status) {
      ErrorHandler::handle(status, state_);
      return false;
    }

    if (!env.isNilref()) { // register _ENV
      env.push();
#if LUA_VERSION_NUM >= 502
      lua_setupvalue(state_, -2, 1);
#else
      lua_setfenv(state_, -2);
#endif
    }

    status = lua_pcall_wrap(state_, 0, LUA_MULTRET);
    if (status) {
      ErrorHandler::handle(status, state_);
      return false;
    }
    return true;
  }

  /// @brief Loads and runs the given string.
  /// @param str lua script cpde
  /// @param env execute env table
  /// @return If there are no errors, returns true.Otherwise return false
  bool dostring(const char *str, const LuaTable &env = LuaTable()) {
    util::ScopedSavedStack save(state_);

    int status = luaL_loadstring(state_, str);
    if (status) {
      ErrorHandler::handle(status, state_);
      return false;
    }
    if (!env.isNilref()) { // register _ENV
      env.push();
#if LUA_VERSION_NUM >= 502
      lua_setupvalue(state_, -2, 1);
#else
      lua_setfenv(state_, -2);
#endif
    }
    status = lua_pcall_wrap(state_, 0, LUA_MULTRET);
    if (status) {
      ErrorHandler::handle(status, state_);
      return false;
    }
    return true;
  }
  /// @brief Loads and runs the given string.
  /// @param str lua script cpde
  /// @param env execute env table
  /// @return If there are no errors, returns true.Otherwise return false
  bool dostring(const std::string &str, const LuaTable &env = LuaTable()) {
    return dostring(str.c_str(), env);
  }

  /// @brief Loads and runs the given string.
  /// @param str lua script cpde
  /// @return If there are no errors, returns true.Otherwise return false
  bool operator()(const std::string &str) { return dostring(str); }

  /// @brief Loads and runs the given string.
  /// @param str lua script cpde
  /// @return If there are no errors, returns true.Otherwise return false
  bool operator()(const char *str) { return dostring(str); }

  /// @brief return element reference from global table
  /// @param str table key
  /// @return proxy class for reference to table.
  TableKeyReferenceProxy<std::string> operator[](const std::string &str) {
    int stack_top = lua_gettop(state_);
    util::push_args(state_, GlobalTable());
    int table_index = stack_top + 1;
    return TableKeyReferenceProxy<std::string>(state_, table_index, str,
                                               stack_top, NoTypeCheck());
  }

  /// @brief return element reference from global table
  /// @param str table key
  /// @return proxy class for reference to table.

  TableKeyReferenceProxy<const char *> operator[](const char *str) {
    int stack_top = lua_gettop(state_);
    util::push_args(state_, GlobalTable());
    int table_index = stack_top + 1;
    return TableKeyReferenceProxy<const char *>(state_, table_index, str,
                                                stack_top, NoTypeCheck());
  }

  /// @brief return global table
  /// @return global table.
  LuaTable globalTable() { return newRef(GlobalTable()); }

  /// @brief create new Lua reference from argument value
  /// @return Lua reference.
  template <typename T> LuaRef newRef(const T &value) {
    return LuaRef(state_, value);
  }
#if KAGUYA_USE_CPP11

  /// @brief create new Lua reference from argument value
  /// @return Lua reference.
  template <typename T> LuaRef newRef(T &&value) {
    return LuaRef(state_, std::forward<T>(value));
  }
#endif

  /// @brief create new Lua table
  /// @return Lua table reference.
  LuaTable newTable() { return LuaTable(state_); }

  /// @brief create new Lua table
  /// @param reserve_array reserved array count
  /// @param reserve_record reserved record count
  /// @return Lua table reference.
  LuaTable newTable(int reserve_array, int reserve_record) {
    return LuaTable(state_, NewTable(reserve_array, reserve_record));
  }

  /// @brief create new Lua thread
  /// @return Lua thread reference.
  LuaThread newThread() { return LuaThread(state_); }

  /// @brief create new Lua thread with lua function
  /// @param f function
  /// @return Lua thread reference.
  LuaThread newThread(const LuaFunction &f) {
    LuaThread cor(state_);
    cor.setFunction(f);
    return cor;
  }

  /// @brief argument value push to stack.
  /// @param value value
  template <typename T> void pushToStack(T value) {
    util::push_args(state_, value);
  }

  /// @brief pop from stack.
  /// @return reference to pop value.
  LuaRef popFromStack() { return LuaRef(state_, StackTop()); }

  /// @brief Garbage Collection of Lua
  struct GCType {
    GCType(lua_State *state) : state_(state) {}

    /// @brief Performs a full garbage-collection cycle.
    void collect() { lua_gc(state_, LUA_GCCOLLECT, 0); }
    /// @brief Performs an incremental step of garbage collection.
    /// @return If returns true,the step finished a collection cycle.
    bool step() { return lua_gc(state_, LUA_GCSTEP, 0) == 1; }

    /// @brief Performs an incremental step of garbage collection.
    /// @param size the collector will perform as if that amount of memory (in
    /// KBytes) had been allocated by Lua.
    bool step(int size) { return lua_gc(state_, LUA_GCSTEP, size) == 1; }

    /// @brief enable gc
    void restart() { enable(); }

    /// @brief disable gc
    void stop() { disable(); }

    /// @brief returns the total memory in use by Lua in Kbytes.
    int count() const { return lua_gc(state_, LUA_GCCOUNT, 0); }

    /// @brief sets arg as the new value for the pause of the collector. Returns
    /// the previous value for pause.
    int steppause(int value) { return lua_gc(state_, LUA_GCSETPAUSE, value); }

    ///  @brief sets arg as the new value for the step multiplier of the
    ///  collector. Returns the previous value for step.
    int setstepmul(int value) {
      return lua_gc(state_, LUA_GCSETSTEPMUL, value);
    }

    /// @brief enable gc
    void enable() { lua_gc(state_, LUA_GCRESTART, 0); }

    /// @brief disable gc
    void disable() { lua_gc(state_, LUA_GCSTOP, 0); }
#if LUA_VERSION_NUM >= 502

    /// @brief returns a boolean that tells whether the collector is running
    bool isrunning() const { return isenabled(); }

    /// @brief returns a boolean that tells whether the collector is running
    bool isenabled() const { return lua_gc(state_, LUA_GCISRUNNING, 0) != 0; }
#endif

  private:
    lua_State *state_;
  };

  // /@brief  return Garbage collection interface.
  GCType gc() const { return GCType(state_); }
  /// @brief performs a full garbage-collection cycle.
  void garbageCollect() { gc().collect(); }

  /// @brief returns the current amount of memory (in Kbytes) in use by Lua.
  size_t useKBytes() const { return size_t(gc().count()); }

  /// @brief create Table and push to stack.
  /// using for Lua module
  /// @return return Lua Table Reference
  LuaTable newLib() {
    LuaTable newtable = newTable();
    newtable.push(state_);
    return newtable;
  }

  /// @brief return lua_State*.
  /// @return lua_State*
  lua_State *state() { return state_; };

  /// @brief check valid lua_State.
  bool isInvalid() const { return !state_; }
};

/// @}
}
// Copyright satoren
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)



namespace kaguya {

template <typename RefTuple, typename GetTuple> struct ref_tuple {
  RefTuple tref;
  ref_tuple(const RefTuple &va) : tref(va) {}
  void operator=(const FunctionResults &fres) {
    tref = fres.get_result(types::typetag<GetTuple>());
  }
  template <class T> void operator=(const T &fres) { tref = fres; }
};
#if KAGUYA_USE_CPP11
template <class... Args>
ref_tuple<standard::tuple<Args &...>, standard::tuple<Args...> >
tie(Args &... va) {
  typedef standard::tuple<Args &...> RefTuple;
  typedef standard::tuple<Args...> GetTuple;
  return ref_tuple<RefTuple, GetTuple>(RefTuple(va...));
}
#else
#define KAGUYA_VARIADIC_REFARG_REP(N) KAGUYA_PP_CAT(A, N) & KAGUYA_PP_CAT(a, N)
#define KAGUYA_VARIADIC_TREFARG_REP(N) KAGUYA_PP_CAT(A, N) &
#define KAGUYA_TEMPLATE_REFARG_REPEAT(N)                                       \
  KAGUYA_PP_REPEAT_ARG(N, KAGUYA_VARIADIC_TREFARG_REP)
#define KAGUYA_REF_TUPLE(N) standard::tuple<KAGUYA_TEMPLATE_REFARG_REPEAT(N)>
#define KAGUYA_GET_TUPLE(N) standard::tuple<KAGUYA_PP_TEMPLATE_ARG_REPEAT(N)>
#define KAGUYA_REF_TUPLE_DEF(N)                                                \
  template <KAGUYA_PP_TEMPLATE_DEF_REPEAT(N)>                                  \
  ref_tuple<KAGUYA_REF_TUPLE(N), KAGUYA_GET_TUPLE(N)> tie(                     \
      KAGUYA_PP_REPEAT_ARG(N, KAGUYA_VARIADIC_REFARG_REP)) {                   \
    return ref_tuple<KAGUYA_REF_TUPLE(N), KAGUYA_GET_TUPLE(N)>(                \
        KAGUYA_REF_TUPLE(N)(KAGUYA_PP_ARG_REPEAT(N)));                         \
  }

KAGUYA_PP_REPEAT_DEF(KAGUYA_FUNCTION_MAX_TUPLE_SIZE, KAGUYA_REF_TUPLE_DEF)
#undef KAGUYA_VARIADIC_REFARG_REP
#undef KAGUYA_TEMPLATE_REFARG_REPEAT
#undef KAGUYA_REF_TUPLE
#undef KAGUYA_GET_TUPLE
#undef KAGUYA_REF_TUPLE_DEF
#endif
}
#endif// KAGUYA_LUABINDING_HPP_INCLUDED
