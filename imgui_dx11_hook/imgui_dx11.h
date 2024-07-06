#pragma once
#include "imgui/imgui.h"
#include "../ui/ui.h"

namespace gui {
    void init();
    void register_window(ui::Window* window);
}