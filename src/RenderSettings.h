#pragma once

#include <imgui.h>
#include "learnopengl/camera.h"
#include <vector>

class RenderSettings {
public:
    bool show_demo_window = false;
    bool show_another_window = false;
    // vector that determines how many tracts to show per file
    std::vector<int> show_tract_count = {1};
    // vector that stores the total number of tracts per file
    std::vector<int> tract_count = {1};
    ImVec4 clear_color = ImVec4(0.2f, 0.7f, 0.6f, 1.0f);
    Camera camera = Camera(glm::vec3(0.0f, -25.0f, 100.0f));

    // Singleton logic
    static RenderSettings& getInstance() {
        static RenderSettings instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

    RenderSettings(RenderSettings const&) = delete;

    void operator=(RenderSettings const&) = delete;

private:
    RenderSettings() = default;
};
