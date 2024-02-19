#pragma once

#include "AbstractWrapper.h"
#include "learnopengl/shader_s.h"
#include <GLFW/glfw3.h>

class GLFWWrapper : AbstractWrapper {
public:
    explicit GLFWWrapper(RenderSettings& settings) : AbstractWrapper(settings) {}

    GLFWwindow* getWindow() {
        return window;
    }

    void init() override;

    void use() override;

    void cleanup() override;

private:
    GLFWwindow* window;
    Shader shader;
    unsigned int VAO, VBO, EBO;
};

