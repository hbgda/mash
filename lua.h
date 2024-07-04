#include <Windows.h>

namespace lua {
    typedef int lua_State;
}

#define LUA_FUNC(o, r, n, p) extern r (*n) p = nullptr
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