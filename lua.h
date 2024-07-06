#pragma once

#define LUA_REGISTRYINDEX (-10000)

namespace lua {
    typedef int lua_State;
    typedef int (*lua_CFunction)(lua_State* state);
    typedef struct {
        const char* name;
        int (*func)(lua_State* state);
    } luaL_reg;
}

#define LUA_FUNC(o, r, n, p) extern r (*n) p
namespace lua {
    #include "lua_functions.h"
}
#undef LUA_FUNC