#pragma once

#include <imgui.h>
#include "learnopengl/camera.h"
#include <vector>
#include <learnopengl/shader_s.h>
#include <memory>
#include "GLFWWrapper.h"
#include "ImGuiWrapper.h"
#include "TractDataWrapper.h"
#include "PathTraceWrapper.h"

enum rendererType {
    UNSHADED_LINES,
    SHADED_LINES,
    SHADED_TUBES,
    PATH_TRACING
};

class RenderSettings {
public:
    GLFWWrapper* glfw = nullptr;
    ImGuiWrapper* imgui = nullptr;
    PathTraceWrapper* pt = nullptr;
    std::vector<std::shared_ptr<TractDataWrapper>> datasets;
    std::vector<std::shared_ptr<TractDataWrapper>> examples;
    rendererType renderer = UNSHADED_LINES;

    bool show_demo_window = false;
    bool MSAA = true;
    bool vsync = true;
    bool fullScreen = false;
    bool highlightEnabled = false;
    float viewExpansionFactor = 0.0f;
    float expansionFactor = 0.0f;
    float lineWidth = 1.0f;
    bool blurEnabled = false;
    bool rotatingLight = false;
    bool drawPoints = false;
    bool rotateData = true;
    bool smoothCap = false;
    bool neuronSim = false;
    bool blackSim = false;
    int particleDens = 50;
    float particleSize = 50;
    bool effectCASplaying = false;
    std::shared_ptr<TractDataWrapper> EmptyTractData = std::make_shared<TractDataWrapper>("None");
    std::shared_ptr<TractDataWrapper> CASBundle = EmptyTractData;
    std::shared_ptr<TractDataWrapper> highlightedBundle = EmptyTractData;
    float highlightAlpha = 0.1f;
    float generalAlpha = 1.0f;
    int nrOfSides = 3;
    float tubeDiameter = 0.1f;
    int ptBounceNr = 10;
    // vector that determines how many tracts to show per file
    std::vector<int> show_tract_count = {1};
    // vector that stores the total number of tracts per file
    std::vector<int> tract_count = {1};
    ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    Camera camera = Camera(glm::vec3(0.0f, -25.0f, 100.0f));
    Shader shader;
    Shader defaultShader;
    Shader lineShadingShader;
    Shader ptComputeShader;
    Shader ptRenderShader;

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
