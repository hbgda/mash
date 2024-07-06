#include "ui.h"
#include "../logging.h"
#include "../imgui_dx11_hook/imgui_dx11.h"

ui::Window* get_window(lua::lua_State* state) {
    return (ui::Window*)lua::checkudata(state, 1, "ui::Window");
}

void ui::Window::Render() {
    for (auto &menu : m_menus) {
        DEBUG("menu->m_title = %s", menu->m_title);
        if (ImGui::CollapsingHeader(menu->m_title)) {
            for (int i = 0; i < menu->m_btns.size(); i++) {
                if (ImGui::Button(menu->m_btns[i]->label)) {
                    menu->Click(lua::state, i);
                }
            }
        }
    }
}

static int window_new(lua::lua_State* state) {
    ui::Window* window = (ui::Window*)lua::newuserdata(state, sizeof(ui::Window));
    new (window) ui::Window();

    const char* title = lua::tostring(state, 1);
    strcpy(window->m_title, title);

    lua::l_getmetatable(state, "ui::Window");
    lua::setmetatable(state, -2);
    DEBUG("[ui::Window] Created new window %s.", title);
    gui::register_window(window);
    return 1;
};

static int window_addmenu(lua::lua_State* state) {
    ui::Window* window = get_window(state);
    if (!window) {
        FATAL("[ui::Window] AddMenu - window is null!");
        return -1;   
    }

    ui::Menu* menu = (ui::Menu*)lua::checkudata(state, 2, "ui::Menu");
    if (!menu) {
        FATAL("[ui::Window] (%s) AddMenu - menu is null!", window->m_title);
        return -1;
    }

    window->m_menus.push_back(menu);

    DEBUG("[ui::Window] (%s) AddMenu - %s", window->m_title, menu->m_title);
    return 0;
}

static constexpr lua::luaL_reg window_funcs[] = {
    { "AddMenu", window_addmenu },
    { 0, 0}
};