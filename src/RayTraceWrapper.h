#pragma once

#include "AbstractWrapper.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class RayTraceWrapper : AbstractWrapper {
private:
    unsigned int VAO, VBO, texture;
    int imgSize, imgWidth, imgHeight;


public:
    RayTraceWrapper();
    
    void init() override;

    void draw() override;

    void cleanup() override;

    void reset();
};
