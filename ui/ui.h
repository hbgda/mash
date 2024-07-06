#pragma once

#include "../lua.h"
#include <vector>

namespace ui {
    void register_lua(lua::lua_State* state);

    typedef struct {
        const char* label;
        int lua_ref;
    } MenuButton;

    class Menu {
    public:
        char* m_title;
        std::vector<MenuButton*> m_btns;
        Menu() {};
        bool Click(lua::lua_State* state, int idx);
        void AddButton(const char* label, int lua_ref);
    };

    class Window {
    public:
        char* m_title;
        std::vector<Menu*> m_menus;
        Window() {};
        void Render();
    };
}