#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <string>
#include <memory>
#include "AbstractWrapper.h"
#include "TractDataWrapper.h"


class ImGuiWrapper : AbstractWrapper {
public:
    explicit ImGuiWrapper(GLFWwindow* window) : window(window) {}

    void init() override;

    void draw() override;

    void cleanup() override;

private:
    GLFWwindow* window;
    std::string iniFile;

    // Perform `func` function on all datasets + `settings.EmptyTractData`
    static void forAllDatasets(void (* func)(const std::shared_ptr<TractDataWrapper>& dataset));
};
