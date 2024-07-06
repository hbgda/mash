#include "ui.h"
#include "../logging.h"

void ui::Menu::AddButton(const char* label, int lua_ref) {
    ui::MenuButton* btn = new ui::MenuButton { label, lua_ref };
    m_btns.push_back(btn);
}

bool ui::Menu::Click(lua::lua_State* state, int idx) {
    if (idx >= m_btns.size()) {
        return false;
    }

    MenuButton* btn = m_btns[idx];
    lua::rawgeti(state, LUA_REGISTRYINDEX, btn->lua_ref);
    lua::pushvalue(state, 3);

    int err = lua::pcall(state, 0, 0, 0);
    if (err != 0) {
        WARN("Failed to call lua function for MenuButton %s, %d, %s", btn->label, err, lua::tostring(state, -1));
        return false;
    }
    return true;
}

ui::Menu* get_menu(lua::lua_State* state) {
    return (ui::Menu*)lua::checkudata(state, 1, "ui::Menu");
}

static int menu_new(lua::lua_State* state) {
    ui::Menu* menu = (ui::Menu*)lua::newuserdata(state, sizeof(ui::Menu));
    new (menu) ui::Menu();
    lua::l_getmetatable(state, "ui::Menu");
    lua::setmetatable(state, -2);
    DEBUG("[ui::Menu] Created new menu.");
    return 1;
}

static int menu_addbutton(lua::lua_State* state) {
    ui::Menu* menu = get_menu(state);
    if (!menu) {
        WARN("[ui::Menu] AddButton - menu is null!");
        return -1;
    }

    const char* label = lua::tostring(state, 2);
    if (!label) {
        WARN("[ui::Menu] AddButton - label is null!");
        return -1;
    }

    int lua_ref = lua::ref(state, LUA_REGISTRYINDEX);
    
    menu->AddButton(label, lua_ref);
    DEBUG("[ui::Menu] AddButton - { label = %s, lua_ref = %d }", label, lua_ref);
    return 0;
}

static int menu_click(lua::lua_State* state) {
    ui::Menu* menu = get_menu(state);
    if (!menu) {
        WARN("[ui::Menu] Click - menu is null!");
        return -1;
    }

    int idx = lua::checkinteger(state, 2);
    if (idx < 0 || idx >= menu->m_btns.size()) {
        WARN("[ui::Menu] Click - idx invalid!");
        return -1;
    }

    bool success = menu->Click(state, idx);
    DEBUG("[ui::Menu] Click - idx = %d, success = %d", idx, success);
    return success ? 0 : -1;
}

static constexpr lua::luaL_reg menu_funcs[] = {
    { "AddButton", menu_addbutton },
    { "Click", menu_click },
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