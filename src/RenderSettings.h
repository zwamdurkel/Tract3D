#pragma once

#include <imgui.h>
#include "learnopengl/camera.h"
#include <vector>
#include <learnopengl/shader_s.h>

// Forward declarations to use pointer to wrapper classes.
class AbstractWrapper;

class GLFWWrapper;

class ImGuiWrapper;

class TractDataWrapper;

class RenderSettings {
public:
    GLFWWrapper* glfw = nullptr;
    ImGuiWrapper* imgui = nullptr;
    std::vector<TractDataWrapper*> datasets;
    bool show_demo_window = false;
    bool show_another_window = false;
    bool MSAA = true;
    bool vsync = true;
    bool drawTubes = false;
    // vector that determines how many tracts to show per file
    std::vector<int> show_tract_count = {1};
    // vector that stores the total number of tracts per file
    std::vector<int> tract_count = {1};
    ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    Camera camera = Camera(glm::vec3(0.0f, -25.0f, 100.0f));
    Shader shader;

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
