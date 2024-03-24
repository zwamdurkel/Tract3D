#pragma once

#include "glm/glm.hpp"
#include <vector>
#include "AbstractWrapper.h"

class RayTracer : AbstractWrapper {
public:
    unsigned int VAO, VBO, texture;

    int renderIndex;//current index we are calculating
    int imgSize, imgWidth, imgHeight;
    float dX, dY;
    std::vector<glm::vec3> colours;//list of colours, this will be the image send to GPU


    void init() override;

    void reset();

    void renderPixel();

    void draw() override;

    void cleanup() override {};

    RayTracer();

    ~RayTracer();

};