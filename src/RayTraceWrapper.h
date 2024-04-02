#pragma once

#include "AbstractWrapper.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"

class RayTraceWrapper : AbstractWrapper {
private:
    unsigned int VAO, VBO, texture;
    int imgSize, imgWidth, imgHeight, pixelOffset, rowsPerFrame, imgNum;
    float pixelDelta;
    glm::vec3 lowerLeft;
public:
    RayTraceWrapper();

    void init() override;

    void draw() override;

    void cleanup() override;

    void resetImg();

    void resetCamera();
};
