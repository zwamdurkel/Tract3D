#pragma once

#include "AbstractWrapper.h"
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
};

