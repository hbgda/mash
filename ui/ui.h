#include "../lua.h"

namespace ui {
    void register_lua(lua::lua_State* state);

    class Menu {
    public:
        const char* x;
        Menu() {
            x = "test";
        }
    };
}