#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include "compat-5.3.h"

/* don't compile it again if it already is included via compat53.h */
#ifndef COMPAT53_C_
#define COMPAT53_C_



/* definitions for Lua 5.1 only */
#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM == 501

#ifndef COMPAT53_HAVE_STRERROR_R
#  if defined(__GLIBC__) || defined(_POSIX_VERSION) || defined(__APPLE__) || (!defined (__MINGW32__) && defined(__GNUC__) && (__GNUC__ < 6))
#    define COMPAT53_HAVE_STRERROR_R 1
#    if ((defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 200112L) || (defined(_XOPEN_SOURCE) || _XOPEN_SOURCE >= 600)) && (!defined(_GNU_SOURCE) || !_GNU_SOURCE)
#    ifndef COMPAT53_HAVE_STRERROR_R_XSI
#      define COMPAT53_HAVE_STRERROR_R_XSI 1
#    endif /* XSI-Compliant strerror_r */
#    else /* XSI/Posix vs. GNU strerror_r */
#      ifndef COMPAT53_HAVE_STRERROR_R_GNU
#        define COMPAT53_HAVE_STRERROR_R_GNU 1
#      endif /* GNU variant strerror_r */
#    endif /* XSI/Posix vs. GNU strerror_r */
#  else /* none of the defines matched: define to 0 */
#    define COMPAT53_HAVE_STRERROR_R 0
#  endif /* have strerror_r of some form */
#endif /* strerror_r */

#ifndef COMPAT53_HAVE_STRERROR_S
#  if defined(_MSC_VER) || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L) || (defined(__STDC_LIB_EXT1__) && __STDC_LIB_EXT1__)
#    define COMPAT53_HAVE_STRERROR_S 1
#  else /* not VC++ or C11 */
#    define COMPAT53_HAVE_STRERROR_S 0
#  endif /* strerror_s from VC++ or C11 */
#endif /* strerror_s */

#ifndef COMPAT53_LUA_FILE_BUFFER_SIZE
#define COMPAT53_LUA_FILE_BUFFER_SIZE 4096
#endif /* Lua File Buffer Size */

struct compat53_f_parser_buffer {
  FILE* file;
  size_t start;
  char buffer[COMPAT53_LUA_FILE_BUFFER_SIZE];
};


COMPAT53_API int lua_absindex (lua_State *L, int i) {
  if (i < 0 && i > LUA_REGISTRYINDEX)
    i += lua_gettop(L) + 1;
  return i;
}


static void compat53_call_lua (lua_State *L, char const code[], size_t len,
                               int nargs, int nret) {
  lua_rawgetp(L, LUA_REGISTRYINDEX, (void*)code);
  if (lua_type(L, -1) != LUA_TFUNCTION) {
    lua_pop(L, 1);
    if (luaL_loadbuffer(L, code, len, "=none"))
      lua_error(L);
    lua_pushvalue(L, -1);
    lua_rawsetp(L, LUA_REGISTRYINDEX, (void*)code);
  }
  lua_insert(L, -nargs-1);
  lua_call(L, nargs, nret);
}


static const char compat53_arith_code[] =
  "local op,a,b=...\n"
  "if op==0 then return a+b\n"
  "elseif op==1 then return a-b\n"
  "elseif op==2 then return a*b\n"
  "elseif op==3 then return a/b\n"
  "elseif op==4 then return a%b\n"
  "elseif op==5 then return a^b\n"
  "elseif op==6 then return -a\n"
  "end\n";

COMPAT53_API void lua_arith (lua_State *L, int op) {
  if (op < LUA_OPADD || op > LUA_OPUNM)
    luaL_error(L, "invalid 'op' argument for lua_arith");
  luaL_checkstack(L, 5, "not enough stack slots");
  if (op == LUA_OPUNM)
    lua_pushvalue(L, -1);
  lua_pushnumber(L, op);
  lua_insert(L, -3);
  compat53_call_lua(L, compat53_arith_code,
                    sizeof(compat53_arith_code)-1, 3, 1);
}


static const char compat53_compare_code[] =
  "local a,b=...\n"
  "return a<=b\n";

COMPAT53_API int lua_compare (lua_State *L, int idx1, int idx2, int op) {
  int result = 0;
  switch (op) {
    case LUA_OPEQ:
      return lua_equal(L, idx1, idx2);
    case LUA_OPLT:
      return lua_lessthan(L, idx1, idx2);
    case LUA_OPLE:
      luaL_checkstack(L, 5, "not enough stack slots");
      idx1 = lua_absindex(L, idx1);
      idx2 = lua_absindex(L, idx2);
      lua_pushvalue(L, idx1);
      lua_pushvalue(L, idx2);
      compat53_call_lua(L, compat53_compare_code,
                        sizeof(compat53_compare_code)-1, 2, 1);
      result = lua_toboolean(L, -1);
      lua_pop(L, 1);
      return result;
    default:
      luaL_error(L, "invalid 'op' argument for lua_compare");
  }
  return 0;
}


COMPAT53_API void lua_copy (lua_State *L, int from, int to) {
  int abs_to = lua_absindex(L, to);
  luaL_checkstack(L, 1, "not enough stack slots");
  lua_pushvalue(L, from);
  lua_replace(L, abs_to);
}


COMPAT53_API void lua_len (lua_State *L, int i) {
  switch (lua_type(L, i)) {
    case LUA_TSTRING:
      lua_pushnumber(L, (lua_Number)lua_objlen(L, i));
      break;
    case LUA_TTABLE:
      if (!luaL_callmeta(L, i, "__len"))
        lua_pushnumber(L, (lua_Number)lua_objlen(L, i));
      break;
    case LUA_TUSERDATA:
      if (luaL_callmeta(L, i, "__len"))
        break;
      /* FALLTHROUGH */
    default:
      luaL_error(L, "attempt to get length of a %s value",
                 lua_typename(L, lua_type(L, i)));
  }
}


COMPAT53_API int lua_rawgetp (lua_State *L, int i, const void *p) {
  int abs_i = lua_absindex(L, i);
  lua_pushlightuserdata(L, (void*)p);
  lua_rawget(L, abs_i);
  return lua_type(L, -1);
}

COMPAT53_API void lua_rawsetp (lua_State *L, int i, const void *p) {
  int abs_i = lua_absindex(L, i);
  luaL_checkstack(L, 1, "not enough stack slots");
  lua_pushlightuserdata(L, (void*)p);
  lua_insert(L, -2);
  lua_rawset(L, abs_i);
}


COMPAT53_API lua_Integer lua_tointegerx (lua_State *L, int i, int *isnum) {
  lua_Integer n = lua_tointeger(L, i);
  if (isnum != NULL) {
    *isnum = (n != 0 || lua_isnumber(L, i));
  }
  return n;
}


COMPAT53_API lua_Number lua_tonumberx (lua_State *L, int i, int *isnum) {
  lua_Number n = lua_tonumber(L, i);
  if (isnum != NULL) {
    *isnum = (n != 0 || lua_isnumber(L, i));
  }
  return n;
}


COMPAT53_API void luaL_checkversion (lua_State *L) {
  (void)L;
}


COMPAT53_API void luaL_checkstack (lua_State *L, int sp, const char *msg) {
  if (!lua_checkstack(L, sp+LUA_MINSTACK)) {
    if (msg != NULL)
      luaL_error(L, "stack overflow (%s)", msg);
    else {
      lua_pushliteral(L, "stack overflow");
      lua_error(L);
    }
  }
}


COMPAT53_API int luaL_getsubtable (lua_State *L, int i, const char *name) {
  int abs_i = lua_absindex(L, i);
  luaL_checkstack(L, 3, "not enough stack slots");
  lua_pushstring(L, name);
  lua_gettable(L, abs_i);
  if (lua_istable(L, -1))
    return 1;
  lua_pop(L, 1);
  lua_newtable(L);
  lua_pushstring(L, name);
  lua_pushvalue(L, -2);
  lua_settable(L, abs_i);
  return 0;
}


COMPAT53_API lua_Integer luaL_len (lua_State *L, int i) {
  lua_Integer res = 0;
  int isnum = 0;
  luaL_checkstack(L, 1, "not enough stack slots");
  lua_len(L, i);
  res = lua_tointegerx(L, -1, &isnum);
  lua_pop(L, 1);
  if (!isnum)
    luaL_error(L, "object length is not an integer");
  return res;
}


COMPAT53_API void luaL_setfuncs (lua_State *L, const luaL_Reg *l, int nup) {
  luaL_checkstack(L, nup+1, "too many upvalues");
  for (; l->name != NULL; l++) {  /* fill the table with given functions */
    int i;
    lua_pushstring(L, l->name);
    for (i = 0; i < nup; i++)  /* copy upvalues to the top */
      lua_pushvalue(L, -(nup + 1));
    lua_pushcclosure(L, l->func, nup);  /* closure with those upvalues */
    lua_settable(L, -(nup + 3)); /* table must be below the upvalues, the name and the closure */
  }
  lua_pop(L, nup);  /* remove upvalues */
}


COMPAT53_API void luaL_setmetatable (lua_State *L, const char *tname) {
  luaL_checkstack(L, 1, "not enough stack slots");
  luaL_getmetatable(L, tname);
  lua_setmetatable(L, -2);
}


COMPAT53_API void *luaL_testudata (lua_State *L, int i, const char *tname) {
  void *p = lua_touserdata(L, i);
  luaL_checkstack(L, 2, "not enough stack slots");
  if (p == NULL || !lua_getmetatable(L, i))
    return NULL;
  else {
    int res = 0;
    luaL_getmetatable(L, tname);
    res = lua_rawequal(L, -1, -2);
    lua_pop(L, 2);
    if (!res)
      p = NULL;
  }
  return p;
}


static int compat53_countlevels (lua_State *L) {
  lua_Debug ar;
  int li = 1, le = 1;
  /* find an upper bound */
  while (lua_getstack(L, le, &ar)) { li = le; le *= 2; }
  /* do a binary search */
  while (li < le) {
    int m = (li + le)/2;
    if (lua_getstack(L, m, &ar)) li = m + 1;
    else le = m;
  }
  return le - 1;
}

static int compat53_findfield (lua_State *L, int objidx, int level) {
  if (level == 0 || !lua_istable(L, -1))
    return 0;  /* not found */
  lua_pushnil(L);  /* start 'next' loop */
  while (lua_next(L, -2)) {  /* for each pair in table */
    if (lua_type(L, -2) == LUA_TSTRING) {  /* ignore non-string keys */
      if (lua_rawequal(L, objidx, -1)) {  /* found object? */
        lua_pop(L, 1);  /* remove value (but keep name) */
        return 1;
      }
      else if (compat53_findfield(L, objidx, level - 1)) {  /* try recursively */
        lua_remove(L, -2);  /* remove table (but keep name) */
        lua_pushliteral(L, ".");
        lua_insert(L, -2);  /* place '.' between the two names */
        lua_concat(L, 3);
        return 1;
      }
    }
    lua_pop(L, 1);  /* remove value */
  }
  return 0;  /* not found */
}

static int compat53_pushglobalfuncname (lua_State *L, lua_Debug *ar) {
  int top = lua_gettop(L);
  lua_getinfo(L, "f", ar);  /* push function */
  lua_pushvalue(L, LUA_GLOBALSINDEX);
  if (compat53_findfield(L, top + 1, 2)) {
    lua_copy(L, -1, top + 1);  /* move name to proper place */
    lua_pop(L, 2);  /* remove pushed values */
    return 1;
  }
  else {
    lua_settop(L, top);  /* remove function and global table */
    return 0;
  }
}

static void compat53_pushfuncname (lua_State *L, lua_Debug *ar) {
  if (*ar->namewhat != '\0')  /* is there a name? */
    lua_pushfstring(L, "function " LUA_QS, ar->name);
  else if (*ar->what == 'm')  /* main? */
      lua_pushliteral(L, "main chunk");
  else if (*ar->what == 'C') {
    if (compat53_pushglobalfuncname(L, ar)) {
      lua_pushfstring(L, "function " LUA_QS, lua_tostring(L, -1));
      lua_remove(L, -2);  /* remove name */
    }
    else
      lua_pushliteral(L, "?");
  }
  else
    lua_pushfstring(L, "function <%s:%d>", ar->short_src, ar->linedefined);
}

#define COMPAT53_LEVELS1 12  /* size of the first part of the stack */
#define COMPAT53_LEVELS2 10  /* size of the second part of the stack */

COMPAT53_API void luaL_traceback (lua_State *L, lua_State *L1,
                                  const char *msg, int level) {
  lua_Debug ar;
  int top = lua_gettop(L);
  int numlevels = compat53_countlevels(L1);
  int mark = (numlevels > COMPAT53_LEVELS1 + COMPAT53_LEVELS2) ? COMPAT53_LEVELS1 : 0;
  if (msg) lua_pushfstring(L, "%s\n", msg);
  lua_pushliteral(L, "stack traceback:");
  while (lua_getstack(L1, level++, &ar)) {
    if (level == mark) {  /* too many levels? */
      lua_pushliteral(L, "\n\t...");  /* add a '...' */
      level = numlevels - COMPAT53_LEVELS2;  /* and skip to last ones */
    }
    else {
      lua_getinfo(L1, "Slnt", &ar);
      lua_pushfstring(L, "\n\t%s:", ar.short_src);
      if (ar.currentline > 0)
        lua_pushfstring(L, "%d:", ar.currentline);
      lua_pushliteral(L, " in ");
      compat53_pushfuncname(L, &ar);
      lua_concat(L, lua_gettop(L) - top);
    }
  }
  lua_concat(L, lua_gettop(L) - top);
}


COMPAT53_API int luaL_fileresult (lua_State *L, int stat, const char *fname) {
  const char* s = NULL;
  int en = errno;  /* calls to Lua API may change this value */
#if (defined(COMPAT53_HAVE_STRERROR_R) && COMPAT53_HAVE_STRERROR_R) || (defined(COMPAT53_HAVE_STRERROR_S) && COMPAT53_HAVE_STRERROR_S)
  char buf[512] = { 0 };
#endif /* buffer for threadsafe variants of strerror if possible */
  if (stat) {
    lua_pushboolean(L, 1);
    return 1;
  }
  else {
    lua_pushnil(L);
#if (defined(COMPAT53_HAVE_STRERROR_R) && COMPAT53_HAVE_STRERROR_R)
    /* use strerror_r here, because it's available on these specific platforms */
#if defined(COMPAT53_HAVE_STRERROR_R_XSI)
    /* XSI Compliant */
    strerror_r(en, buf, sizeof(buf));
    s = buf;
#else
    /* GNU-specific which returns const char* */
    s = strerror_r(en, buf, sizeof(buf));
#endif
#elif (defined(COMPAT53_HAVE_STRERROR_S) && COMPAT53_HAVE_STRERROR_S)
    /* for MSVC and other C11 implementations, use strerror_s 
     * since it's provided by default by the libraries 
     */
    strerror_s(buf, sizeof(buf), en);
    s = buf;
#else
    /* fallback, but
     * strerror is not guaranteed to be threadsafe due to modifying
     * errno itself and some impls not locking a static buffer for it
     * ... but most known systems have threadsafe errno: this might only change
     * if the locale is changed out from under someone while this function is being called
     */ 
    s = strerror(en);
#endif
    if (fname)
      lua_pushfstring(L, "%s: %s", fname, s);
    else
      lua_pushstring(L, s);
    lua_pushnumber(L, (lua_Number)en);
    return 3;
  }
}


static const char* compat53_f_parser_handler (lua_State *L, void *data, size_t *size) {
  (void)L; /* better fix for unused parameter warnings */
  struct compat53_f_parser_buffer *p = (struct compat53_f_parser_buffer*)data;
  if (feof(p->file) != 0) {
	  *size = 0;
	  return NULL;
  }
  size_t readcount = fread(p->buffer + p->start, sizeof(*p->buffer), sizeof(p->buffer), p->file);
  if (ferror(p->file) != 0) {
    *size = 0;
    return NULL;
  }
  *size = readcount + p->start;
  p->start = 0;
  return p->buffer;
}


static int checkmode (lua_State *L, const char *mode, const char *modename, int err) {
  if (mode && strchr(mode, modename[0]) == NULL) {
    lua_pushfstring(L, "attempt to load a %s chunk (mode is '%s')", modename, mode);
    return err;
  }
  return LUA_OK;
}


COMPAT53_API int luaL_loadfilex (lua_State *L, const char *filename, const char *mode) {
  FILE* fp;
  int c;
  int status = LUA_OK;
#if defined(_MSC_VER)
  int err = 0;
  err = fopen_s(&fp, filename, "rb");
  if (err != 0) {
    fclose(fp);
    lua_pushfstring(L, "cannot open file '%s'", filename);
    return LUA_ERRFILE;
  }
#else /* fopen error on Visual C */
  fp = fopen(filename, "rb");
  if (fp == NULL) {
    fclose(fp);
    lua_pushfstring(L, "cannot open file '%s'", filename);
    return LUA_ERRFILE;
  }
#endif /* fopen error on Visual C */
  c = fgetc(fp);
  if (ferror(fp) != 0) {
    /* can't read even a single character */
    fclose(fp);
    lua_pushfstring(L, "cannot read from file '%s'", filename);
    return LUA_ERRFILE;
  }
  if (c == '\x1b') {
    status = checkmode(L, mode, "binary", LUA_ERRFILE);
  }
  else {
    status = checkmode(L, mode, "text", LUA_ERRFILE);
  }
  if (status != LUA_OK) {
    fclose(fp);
    return status;
  }
  struct compat53_f_parser_buffer fbuf;
  fbuf.file = fp;
  fbuf.start = 1;
  fbuf.buffer[0] = (char)c;
  status = lua_load(L, &compat53_f_parser_handler, &fbuf, filename);
  fclose(fp);
  return status;
}


COMPAT53_API int luaL_loadbufferx (lua_State *L, const char *buff, size_t sz, const char *name, const char *mode) {
  int status = LUA_OK;
  if (buff[0] == '\x1b') {
    status = checkmode(L, mode, "binary", LUA_ERRSYNTAX);
  }
  else {
    status = checkmode(L, mode, "text", LUA_ERRSYNTAX);
  }
  if (status != LUA_OK)
    return status;
  return luaL_loadbuffer(L, buff, sz, name);
}

#if !defined(l_inspectstat) && \
    (defined(unix) || defined(__unix) || defined(__unix__) || \
     defined(__TOS_AIX__) || defined(_SYSTYPE_BSD) || \
     (defined(__APPLE__) && defined(__MACH__)))
/* some form of unix; check feature macros in unistd.h for details */
#  include <unistd.h>
/* check posix version; the relevant include files and macros probably
 * were available before 2001, but I'm not sure */
#  if defined(_POSIX_VERSION) && _POSIX_VERSION >= 200112L
#    include <sys/wait.h>
#    define l_inspectstat(stat,what) \
  if (WIFEXITED(stat)) { stat = WEXITSTATUS(stat); } \
  else if (WIFSIGNALED(stat)) { stat = WTERMSIG(stat); what = "signal"; }
#  endif
#endif

/* provide default (no-op) version */
#if !defined(l_inspectstat)
#  define l_inspectstat(stat,what) ((void)0)
#endif


COMPAT53_API int luaL_execresult (lua_State *L, int stat) {
  const char *what = "exit";
  if (stat == -1)
    return luaL_fileresult(L, 0, NULL);
  else {
    l_inspectstat(stat, what);
    if (*what == 'e' && stat == 0)
      lua_pushboolean(L, 1);
    else
      lua_pushnil(L);
    lua_pushstring(L, what);
    lua_pushinteger(L, stat);
    return 3;
  }
}


COMPAT53_API void luaL_buffinit (lua_State *L, luaL_Buffer_53 *B) {
  /* make it crash if used via pointer to a 5.1-style luaL_Buffer */
  B->b.p = NULL;
  B->b.L = NULL;
  B->b.lvl = 0;
  /* reuse the buffer from the 5.1-style luaL_Buffer though! */
  B->ptr = B->b.buffer;
  B->capacity = LUAL_BUFFERSIZE;
  B->nelems = 0;
  B->L2 = L;
}


COMPAT53_API char *luaL_prepbuffsize (luaL_Buffer_53 *B, size_t s) {
  if (B->capacity - B->nelems < s) { /* needs to grow */
    char* newptr = NULL;
    size_t newcap = B->capacity * 2;
    if (newcap - B->nelems < s)
      newcap = B->nelems + s;
    if (newcap < B->capacity) /* overflow */
      luaL_error(B->L2, "buffer too large");
    newptr = (char*)lua_newuserdata(B->L2, newcap);
    memcpy(newptr, B->ptr, B->nelems);
    if (B->ptr != B->b.buffer)
      lua_replace(B->L2, -2); /* remove old buffer */
    B->ptr = newptr;
    B->capacity = newcap;
  }
  return B->ptr+B->nelems;
}


COMPAT53_API void luaL_addlstring (luaL_Buffer_53 *B, const char *s, size_t l) {
  memcpy(luaL_prepbuffsize(B, l), s, l);
  luaL_addsize(B, l);
}


COMPAT53_API void luaL_addvalue (luaL_Buffer_53 *B) {
  size_t len = 0;
  const char *s = lua_tolstring(B->L2, -1, &len);
  if (!s)
    luaL_error(B->L2, "cannot convert value to string");
  if (B->ptr != B->b.buffer)
    lua_insert(B->L2, -2); /* userdata buffer must be at stack top */
  luaL_addlstring(B, s, len);
  lua_remove(B->L2, B->ptr != B->b.buffer ? -2 : -1);
}


void luaL_pushresult (luaL_Buffer_53 *B) {
  lua_pushlstring(B->L2, B->ptr, B->nelems);
  if (B->ptr != B->b.buffer)
    lua_replace(B->L2, -2); /* remove userdata buffer */
}


#endif /* Lua 5.1 */



/* definitions for Lua 5.1 and Lua 5.2 */
#if defined( LUA_VERSION_NUM ) && LUA_VERSION_NUM <= 502


COMPAT53_API int lua_geti (lua_State *L, int index, lua_Integer i) {
  index = lua_absindex(L, index);
  lua_pushinteger(L, i);
  lua_gettable(L, index);
  return lua_type(L, -1);
}


COMPAT53_API int lua_isinteger (lua_State *L, int index) {
  if (lua_type(L, index) == LUA_TNUMBER) {
    lua_Number n = lua_tonumber(L, index);
    lua_Integer i = lua_tointeger(L, index);
    if (i == n)
      return 1;
  }
  return 0;
}


static void compat53_reverse (lua_State *L, int a, int b) {
  for (; a < b; ++a, --b) {
    lua_pushvalue(L, a);
    lua_pushvalue(L, b);
    lua_replace(L, a);
    lua_replace(L, b);
  }
}


COMPAT53_API void lua_rotate (lua_State *L, int idx, int n) {
  int n_elems = 0;
  idx = lua_absindex(L, idx);
  n_elems = lua_gettop(L)-idx+1;
  if (n < 0)
    n += n_elems;
  if ( n > 0 && n < n_elems) {
    luaL_checkstack(L, 2, "not enough stack slots available");
    n = n_elems - n;
    compat53_reverse(L, idx, idx+n-1);
    compat53_reverse(L, idx+n, idx+n_elems-1);
    compat53_reverse(L, idx, idx+n_elems-1);
  }
}


COMPAT53_API void lua_seti (lua_State *L, int index, lua_Integer i) {
  luaL_checkstack(L, 1, "not enough stack slots available");
  index = lua_absindex(L, index);
  lua_pushinteger(L, i);
  lua_insert(L, -2);
  lua_settable(L, index);
}


#if !defined(lua_str2number)
#  define lua_str2number(s, p)  strtod((s), (p))
#endif

COMPAT53_API size_t lua_stringtonumber (lua_State *L, const char *s) {
  char* endptr;
  lua_Number n = lua_str2number(s, &endptr);
  if (endptr != s) {
    while (*endptr != '\0' && isspace((unsigned char)*endptr))
      ++endptr;
    if (*endptr == '\0') {
      lua_pushnumber(L, n);
      return endptr - s + 1;
    }
  }
  return 0;
}


COMPAT53_API const char *luaL_tolstring (lua_State *L, int idx, size_t *len) {
  if (!luaL_callmeta(L, idx, "__tostring")) {
    int t = lua_type(L, idx), tt = 0;
    char const* name = NULL;
    switch (t) {
      case LUA_TNIL:
        lua_pushliteral(L, "nil");
        break;
      case LUA_TSTRING:
      case LUA_TNUMBER:
        lua_pushvalue(L, idx);
        break;
      case LUA_TBOOLEAN:
        if (lua_toboolean(L, idx))
          lua_pushliteral(L, "true");
        else
          lua_pushliteral(L, "false");
        break;
      default:
        tt = luaL_getmetafield(L, idx, "__name");
        name = (tt == LUA_TSTRING) ? lua_tostring(L, -1) : lua_typename(L, t);
        lua_pushfstring(L, "%s: %p", name, lua_topointer(L, idx));
        if (tt != LUA_TNIL)
          lua_replace(L, -2);
        break;
    }
  } else {
    if (!lua_isstring(L, -1))
      luaL_error(L, "'__tostring' must return a string");
  }
  return lua_tolstring(L, -1, len);
}


COMPAT53_API void luaL_requiref (lua_State *L, const char *modname,
                                 lua_CFunction openf, int glb) {
  luaL_checkstack(L, 3, "not enough stack slots available");
  luaL_getsubtable(L, LUA_REGISTRYINDEX, "_LOADED");
  if (lua_getfield(L, -1, modname) == LUA_TNIL) {
    lua_pop(L, 1);
    lua_pushcfunction(L, openf);
    lua_pushstring(L, modname);
    lua_call(L, 1, 1);
    lua_pushvalue(L, -1);
    lua_setfield(L, -3, modname);
  }
  if (glb) {
    lua_pushvalue(L, -1);
    lua_setglobal(L, modname);
  }
  lua_replace(L, -2);
}


#endif /* Lua 5.1 and 5.2 */


#endif /* COMPAT53_C_ */


/*********************************************************************
* This file contains parts of Lua 5.2's and Lua 5.3's source code:
*
* Copyright (C) 1994-2014 Lua.org, PUC-Rio.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*********************************************************************/

