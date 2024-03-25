#pragma once

#include "glm/glm.hpp"
#include <vector>
#include "AbstractWrapper.h"
#include "Ray.h"
#include <random>

class RayTracer : AbstractWrapper {
    unsigned int VAO, VBO, texture;

    int renderIndex;//current index we are calculating
    int imgSize, imgWidth, imgHeight;
    float dX, dY;
    std::vector<glm::vec3> colours;//list of colours, this will be the image send to GPU
    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution = std::uniform_real_distribution<float>(0.0, 1.0);

public:
    void init() override;

    void reset();

    void renderPixel();

    void draw() override;

    void cleanup() override {};

    glm::vec3 getRayColour(Ray ray);

    Ray generateRay(glm::vec2 pixelCoord);

    RayTracer();

    ~RayTracer();

};