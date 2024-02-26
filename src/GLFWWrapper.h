#pragma once

#include "AbstractWrapper.h"
#include "learnopengl/shader_s.h"
#include <GLFW/glfw3.h>

class GLFWWrapper : AbstractWrapper {
public:
    GLFWWrapper() = default;

    GLFWwindow* getWindow() {
        return window;
    }

    void init() override;

    void use() override;

    void cleanup() override;

private:
    GLFWwindow* window;
    Shader shader;
    unsigned int VAO, VBO, VCO;
    int tractsize;
    std::vector<int> tractsizes, tractfirst;
};

