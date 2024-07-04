#include "utils.h"
#include "hooking.h"
#include "logging.h"
#include "lua.h"

#include <Windows.h>
#include <string>
#include <string_view>
#include <TlHelp32.h>
#include <vector>
#include <iostream>

#pragma comment(lib, "../lib/libMinHook.x64.lib")

using namespace std::literals;

static constexpr auto test_lua = R"(
print("Test?")

function writeAllGlobals()	
	local file = io.open("out.txt", "w+")

    local seen={}
    local function dump(t,i)
        seen[t]=true
        local s={}
        local n=0
        for k, v in pairs(t) do
            n=n+1
			s[n]=tostring(k)
        end
        table.sort(s)
        for k,v in ipairs(s) do
            file:write(i .. v .. "\n")
            v=t[v]
            if type(v)=="table" and not seen[v] then
                dump(v,i.."\t")
            end
        end
    end

    dump(_G,"")
	file:close()
end

function test_func()
    writeAllGlobals()
end

function reload(module)
        package.loaded[module] = nil
        return require(module)
end

function reload_test()
    a, b = pcall( function() reload("test") end )
    print(tostring(b))
    error("test")
end
)"sv;


lua::lua_State* lua_state = NULL;

bool CheckLuaErr(lua::lua_State* state, int err) {
    if (err != 0) {
        FATAL("[LUA] %s", lua::tostring(state, 1));
        return false;
    }
    return true;
} 

MAKE_HOOK(int, luaL_loadbuffer, (lua::lua_State* state, const char* buff, size_t sz, const char* name), {
    lua_state = state;
    return luaL_loadbuffer_Call(state, buff, sz, name);
})

MAKE_HOOK(int, lua_print, (lua::lua_State* state), {
    int num = (int)(*(int*)(state + 0x48) - *(int*)(state + 0x50) >> 4);
    if (num < 0)
        return 0;

    lua::getglobal(state, "tostring");
    lua::pushvalue(state, 1);
    lua::call(state, 1, 1);
    if (
        *(int*)(state + 0x48) - 0x10 < 
        *(unsigned int*)(state + 0x50)
    ) {
        WARN("[LUA] tostring didnt return a string to print");
        return -1;
    }
    DEBUG("[LUA] %s", lua::tostring(state, 1));
    return 0;
})

void init() {
    while (!lua_state) {
        Sleep(100);
    }

    DEBUG("Calling loadbuffer...");
    if (!CheckLuaErr(lua_state, lua::l_loadbuffer(lua_state, test_lua.data(), test_lua.length(), "test")))
        return;


    DEBUG("Calling pcall...");
    if (!CheckLuaErr(lua_state, lua::pcall(lua_state, 0, 0, 0)))
        return;

    while (true) {
        if (GetAsyncKeyState(VK_PRIOR) & 1) {
            DEBUG("[LUA] Reloading...");
            lua::getglobal(lua_state, "reload_test");
            if (!CheckLuaErr(lua_state, lua::pcall(lua_state, 0, 0, 0))) {
                WARN("[LUA] Failed to call reload_test!");
            }
        }
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
    	case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(hModule);

            // config()

            // if (lua::loadbuffer(lua_state, lua_func.data(), lua_func.length(), "line") != 0) {
            //     FATAL("Failed loading buffer!");
            // }


            // lua::loadbuffer()

            utils::create_console();
            DEBUG("Console created.");
            if (MH_Initialize() != MH_OK) {
                FATAL("Failed to intialize MinHook!");
                return false;
            }
    
            init_lua();

            INSTALL_AND_ENABLE(
                lua_print,
                lua::print,
                FATAL("Failed to install/enable hook for smth."),
                DEBUG("Installed hook for smth.")
            );

            INSTALL_AND_ENABLE(
                luaL_loadbuffer,
                lua::l_loadbuffer,
                FATAL("Failed to install/enable hook for luaL_loadbuffer."),
                DEBUG("Installed hook for luaL_loadbuffer.")
            );


            CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)init, hModule, 0, nullptr);
            break;
    }
    return TRUE;
}