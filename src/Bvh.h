#pragma once

#include "algorithm"
#include "stack"
#include "vector"
#include "glm/glm.hpp"
#include "logger.h"

namespace BVH {
    struct CylinderGPU {
        float cap[3];
        float dir[3];
        float col1[3];
        float col2[3];
        float r;
        float length;
    };

    struct Cylinder {
        int index;
        CylinderGPU c;
    };

    struct AABB {
        glm::vec3 min;
        glm::vec3 max;
    };

    struct BVHNodeGPU {
        float min[3];
        float max[3];
        int leftNodeIndex = -1;
        int rightNodeIndex = -1;
        int objectIndex = -1;
    };


    struct BVHNode {
        AABB box;
        int index = -1;
        int leftNodeIndex = -1;
        int rightNodeIndex = -1;
        int axis = 0;
        std::vector<Cylinder> objects;

        int sortingAxis() {
            axis = (axis + 1) % 3;
            return axis;
        }

        BVHNodeGPU gpuNode() {
            BVHNodeGPU output;
            bool leaf = leftNodeIndex == -1 && rightNodeIndex == -1;
            for (int i = 0; i < 3; i++) {
                output.min[i] = box.min[i];
                output.max[i] = box.max[i];
            }
            output.leftNodeIndex = leftNodeIndex;
            output.rightNodeIndex = rightNodeIndex;

            if (objects.empty()) { return output; }
            if (leaf) { output.objectIndex = objects[0].index; }

            return output;
        }
    };

    AABB aabbFromObject(CylinderGPU c);

    AABB aabbFromBoxes(AABB b0, AABB b1);

    AABB aabbFromList(std::vector<Cylinder> objects);

    bool boxCompare(Cylinder a, Cylinder b, int axis);

    bool boxXCompare(Cylinder a, Cylinder b);

    bool boxYCompare(Cylinder a, Cylinder b);

    bool boxZCompare(Cylinder a, Cylinder b);

    bool nodeCompare(BVHNode a, BVHNode b);

//heavily inspired by https://github.com/grigoryoskin/vulkan-compute-ray-tracing/tree/master
    std::vector<BVHNodeGPU> createBHV(std::vector<Cylinder> objects);
}

