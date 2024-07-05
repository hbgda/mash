using namespace lua;

#define LUA_POP(L,n) lua::settop(L, -(n)-1)

LUA_FUNC(0x0e4df10, const char*, tostring, (lua_State* state, int idx));
LUA_FUNC(0x4911ab0, void, call, (lua_State* state, int nargs, int nresults));
LUA_FUNC(0x1ac6ce0, void, pushvalue, (lua_State* state, int idx));
LUA_FUNC(0x4923030, void, settop, (lua_State* state, int idx));
LUA_FUNC(0x3d829d0, int, getglobal, (lua_State* state, const char* global));
LUA_FUNC(0x491cab0, int, l_loadbuffer, (lua_State* state, const char* buf, size_t sz, const char* name));
LUA_FUNC(0x4942fe0, int, b_pcall, (lua_State* state));
LUA_FUNC(0x4912960, int, pcall, (lua_State* state, int nargs, int nresults, int errfunc));
LUA_FUNC(0x4911640, void, newstate, (lua_State* state, void* _));
LUA_FUNC(0x49425d0, int, print, (lua_State* state));
LUA_FUNC(0x491f210, void, openlib, (lua_State* state, const char* name, const lua::luaL_reg lib[], int upvalues));
LUA_FUNC(0x49127a0, void*, newuserdata, (lua_State* state, size_t sz));
// LUA_FUNC(0x3811340, void*, ??, (lua_State* state, int idx));
LUA_FUNC(0x491cc60, int, l_newmetatable, (lua_State* state, const char* name));
LUA_FUNC(0x491c400, void, l_getmetatable, (lua_State* state, const char* name));
LUA_FUNC(0x491ff50, int, setmetatable, (lua_State* state, int objindex));
LUA_FUNC(0x0e4da50, void, pushstring, (lua_State* state, const char* str, size_t len));
LUA_FUNC(0x491fb00, int, rawset, (lua_State* state, int index));
LUA_FUNC(0x0e4dcd0, void, setfield, (lua_State* state, int index, const char* field));
LUA_FUNC(0x49136e0, void, pushnamedcclosure, (lua_State* state, lua_CFunction fn, int n, const char* name, int unk));
LUA_FUNC(0x491bd40, void*, checkudata, (lua_State* state, int index, const char* name));