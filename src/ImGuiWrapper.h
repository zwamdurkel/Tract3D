#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "AbstractWrapper.h"


class ImGuiWrapper : AbstractWrapper {
public:
    ImGuiWrapper(RenderSettings& settings, GLFWwindow* window) : AbstractWrapper(settings), window(window) {}

    void init() override;

    void use() override;

    void cleanup() override;

private:
    GLFWwindow* window;
};
