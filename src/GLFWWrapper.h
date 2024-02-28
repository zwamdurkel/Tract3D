#pragma once

#include "AbstractWrapper.h"
#include "learnopengl/shader_s.h"
#include "TractData.h"
#include <GLFW/glfw3.h>

class GLFWWrapper : AbstractWrapper {
public:
    GLFWWrapper() = default;

    GLFWwindow* getWindow() {
        return window;
    }

    void init() override;

    void draw() override;

    void cleanup() override;

private:
    GLFWwindow* window;
    Shader shader;
    TractData td;
    unsigned int VAO, VBO, VCO, EBO;
    float rainbow[21] = {
            1.0f, 0.15f, 0.15f, // red
            1.0f, 0.6f, 0.0f, // orange
            1.0f, 0.95f, 0.0f, // yellow
            0.2f, 1.0f, 0.2f, // green
            0.2f, 0.15f, 0.95f, // blue
            0.45f, 0.15f, 1.0f, //violet
            0.55f, 0.15f, 1.0f //purple
    };
};

