#pragma once

#include <imgui.h>
#include "learnopengl/camera.h"

struct RenderSettings {
    bool show_demo_window = false;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.2f, 0.3f, 0.3f, 1.0f);
    Camera* camera;
};
