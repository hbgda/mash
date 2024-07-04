using namespace lua;

#define POP(L,n) lua::settop(L, -(n)-1)

LUA_FUNC(0xe4df10, const char*, tostring, (lua_State* state, int idx));
LUA_FUNC(0x4911ab0, void, call, (lua_State* state, int nargs, int nresults));
LUA_FUNC(0x1ac6ce0, void, pushvalue, (lua_State* state, int idx));
LUA_FUNC(0x4923030, void, settop, (lua_State* state, int idx));
LUA_FUNC(0x3d829d0, int, getglobal, (lua_State* state, const char* global));
LUA_FUNC(0x3d829d0, void, getfield, (lua_State* state, const char* field));
LUA_FUNC(0x491cab0, int, l_loadbuffer, (lua_State* state, const char* buf, size_t sz, const char* name));
LUA_FUNC(0x4942fe0, int, b_pcall, (lua_State* state));
LUA_FUNC(0x4912960, unsigned int, pcall, (lua_State* state, int nargs, int nresults, int errfunc));
LUA_FUNC(0x4911640, void, newstate, (lua_State* state, void* _));
LUA_FUNC(0x49425d0, int, print, (lua_State* state));