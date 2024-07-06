#include "ui.h"
#include "menu.h"
#include "window.h"
#include "../logging.h"
#include "../imgui_dx11_hook/imgui_dx11.h"

static constexpr lua::luaL_reg ui_lib[] = {
    { "Menu", menu_new },
    { "Window", window_new },
    { 0, 0 }
};

namespace ui {
    void register_lua(lua::lua_State* state) {
        DEBUG("Registering ui lib...");

        lua::openlib(state, "ui::Menu", menu_funcs, 0);
        lua::l_newmetatable(state, "ui::Menu");
        
        lua::pushstring(state, "__index", 7);
        lua::pushvalue(state, -3);
        lua::rawset(state, -3);

        LUA_POP(state, 1);

        lua::openlib(state, "ui::Window", window_funcs, 0);
        lua::l_newmetatable(state, "ui::Window");

        lua::pushstring(state, "__index", 7);
        lua::pushvalue(state, -3);
        lua::rawset(state, -3);

        LUA_POP(state, 1);

        lua::openlib(state, "ui", ui_lib, 0);

        LUA_POP(state, 1);
    }
}