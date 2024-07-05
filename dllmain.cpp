#include "utils.h"
#include "hooking.h"
#include "logging.h"
#include "ui/ui.h"

#include "lua.h"
#include "lua_init.h"

#include <Windows.h>
#include <string>
#include <string_view>

#include <Tlhelp32.h>
#include <iostream>
#include <vector>

bool suspendOtherThreads(DWORD currentThreadId) {
    HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hThreadSnap == INVALID_HANDLE_VALUE) {
        std::cerr << "CreateToolhelp32Snapshot failed.\n";
        return false;
    }

    THREADENTRY32 te32;
    te32.dwSize = sizeof(THREADENTRY32);

    if (!Thread32First(hThreadSnap, &te32)) {
        CloseHandle(hThreadSnap);
        std::cerr << "Thread32First failed.\n";
        return false;
    }

    do {
        if (te32.th32ThreadID != currentThreadId && te32.th32OwnerProcessID == GetCurrentProcessId()) {
            HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te32.th32ThreadID);
            if (hThread != nullptr) {
                SuspendThread(hThread);
            }
        }
    } while (Thread32Next(hThreadSnap, &te32));

    CloseHandle(hThreadSnap);

    return true;
}

bool resumeOtherThreads(DWORD currentThreadId) {
    HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hThreadSnap == INVALID_HANDLE_VALUE) {
        std::cerr << "CreateToolhelp32Snapshot failed.\n";
        return false;
    }

    THREADENTRY32 te32;
    te32.dwSize = sizeof(THREADENTRY32);

    if (!Thread32First(hThreadSnap, &te32)) {
        CloseHandle(hThreadSnap);
        std::cerr << "Thread32First failed.\n";
        return false;
    }


    do {
        if (te32.th32ThreadID != currentThreadId && te32.th32OwnerProcessID == GetCurrentProcessId()) {
            HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te32.th32ThreadID);
            if (hThread != nullptr) {
                ResumeThread(hThread);
            }
        }
    } while (Thread32Next(hThreadSnap, &te32));

    CloseHandle(hThreadSnap);
    return true;
}


#pragma comment(lib, "../lib/libMinHook.x64.lib")



using namespace std::literals;

static constexpr auto test_lua = R"(
print("Hello from injected lua")

menu = ui.Menu()

local function dump (  value , call_indent)

    if not call_indent then 
      call_indent = ""
    end
  
    local indent = call_indent .. "  "
  
    local output = ""
  
    if type(value) == "table" then
        output = output .. "{"
        local first = true
        for inner_key, inner_value in pairs ( value ) do
          if not first then 
            output = output .. ", "
          else
            first = false
          end
          output = output .. "\n" .. indent
          output = output  .. inner_key .. " = " .. tostring(dump ( inner_value, indent ) )
        end
        output = output ..  "\n" .. call_indent .. "}"
  
    elseif type (value) == "userdata" then
      output = "userdata"
    else 
      output =  value
    end
    return output 
  end
  
function getAllData(t, prevData)
    -- if prevData == nil, start empty, otherwise start with prevData
    local data = prevData or {}
  
    -- copy all the attributes from t
    for k,v in pairs(t) do
        data[k] = data[k] or v
    end
  
    -- get t's metatable, or exit if not existing
    local mt = getmetatable(t)
    if type(mt)~='table' then return data end
  
    -- get the __index from mt, or exit if not table
    local index = mt.__index
    if type(index)~='table' then return data end
  
    -- include the data from index into data, recursively, and return
    return getAllData(index, data)
end
print(dump(getAllData(getmetatable(menu))))

print(tostring(menu))

menu:Set("Test")
print(menu:Get())

function reload(module)
        package.loaded[module] = nil
        return require(module)
end

function reload_test()
    a, b = pcall( function() reload("test") end )
    print(tostring(b))
    --error("test")
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
    while (!lua_state) 
        Sleep(100);

    if (!luaL_loadbuffer_Hook.Disable())
        WARN("Could'nt disable loadbuffer hook?");

    suspendOtherThreads(GetCurrentThreadId());

    ui::register_lua(lua_state);

    DEBUG("Calling loadbuffer...");
    if (!CheckLuaErr(lua_state, lua::l_loadbuffer(lua_state, test_lua.data(), test_lua.length(), "test")))
        return;

    DEBUG("Calling pcall...");
    if (!CheckLuaErr(lua_state, lua::pcall(lua_state, 0, 0, 0))) {
        FATAL("[LUA] Error in loaded Lua!");
        return;
    }

    resumeOtherThreads(GetCurrentThreadId());

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
                FATAL("Failed to install/enable hook for lua_print."),
                DEBUG("Installed hook for lua_print.")
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