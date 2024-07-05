#include "ui.h"
#include "../logging.h"

#include <format>

static int menu_new(lua::lua_State* state) {
    ui::Menu* menu = (ui::Menu*)lua::newuserdata(state, sizeof(ui::Menu));
    new (menu) ui::Menu();
    lua::l_getmetatable(state, "ui::Menu");
    lua::setmetatable(state, -2);
    DEBUG("menu = %p", menu);
    return 1;
}

static int menu_set(lua::lua_State* state) {
    ui::Menu* menu = (ui::Menu*)lua::checkudata(state, 1, "ui::Menu");
    const char* x = lua::tostring(state, 2);
    DEBUG("Menu:Test('%s')", x);
    DEBUG("menu = %p", menu);
    menu->x = x;
    return 0;
}

static int menu_get(lua::lua_State* state) {
    ui::Menu* menu = (ui::Menu*)lua::checkudata(state, 1, "ui::Menu");
    DEBUG("menu = %p", menu);
    lua::pushstring(state, "test thing idk", 14);
    return 1;
}

static constexpr lua::luaL_reg menu_funcs[] = {
    { "Set", menu_set },
    { "Get", menu_get },
    { 0, 0 }
};
static constexpr lua::luaL_reg ui_lib[] = {
    { "Menu", menu_new },
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

        lua::openlib(state, "ui", ui_lib, 0);

        LUA_POP(state, 1);
    }
}