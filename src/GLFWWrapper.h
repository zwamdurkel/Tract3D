#pragma once

#include "AbstractWrapper.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class GLFWWrapper : AbstractWrapper {
public:
    GLFWWrapper() = default;

    GLFWwindow* getWindow() {
        return window;
    }

    void setFullScreen(bool enabled);

    void init() override;

    void draw() override;

    void cleanup() override;

private:
    GLFWwindow* window;
};

