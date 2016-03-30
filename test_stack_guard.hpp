#pragma once

struct stack_guard {
    lua_State* L;
    int& begintop;
    int& endtop;
    stack_guard(lua_State* L, int& begintop, int& endtop) : L(L), begintop(begintop), endtop(endtop) { 
        begintop = lua_gettop(L);
    }
    ~stack_guard() { endtop = lua_gettop(L); }
};
