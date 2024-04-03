#pragma once

#include "AbstractWrapper.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "vector"
#include "Bvh.h"
//useful structs for creating datastructure

class RayTraceWrapper : AbstractWrapper {
private:
    unsigned int VAO, VBO, texture;
    unsigned int ObjSSBO, BvhSSBO;
    int imgSize, imgWidth, imgHeight, pixelOffset, rowsPerFrame, imgNum;
    float pixelDelta;
    glm::vec3 lowerLeft;
    std::vector<BVH::CylinderGPU> obj;
    std::vector<BVH::BVHNodeGPU> bvh;

    void createCylinders();

    void createSSBO();

    void loadSSBO();

public:
    RayTraceWrapper();

    void initBVH();

    void init() override;

    void draw() override;

    void cleanup() override;

    void resetImg();

    void resetCamera();
};
