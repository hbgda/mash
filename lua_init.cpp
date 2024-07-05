#include "lua.h"
#include "lua_init.h"

#include <Windows.h>

#define LUA_FUNC(o, r, n, p) r (*n) p
namespace lua {
    #include "lua_functions.h"
}
#undef LUA_FUNC

void init_lua() {
    uintptr_t baseAddress = (uintptr_t)GetModuleHandleA("mafiadefinitiveedition.exe");

    using namespace lua;

    #define LUA_FUNC(o, r, n, p) n = (r (*) p)(baseAddress + o)
    #include "lua_functions.h"
    #undef LUA_FUNC
}