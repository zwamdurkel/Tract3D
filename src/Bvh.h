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

    AABB aabbFromObject(CylinderGPU c) {
        glm::vec3 a(c.cap[0], c.cap[1], c.cap[2]);
        glm::vec3 dir(c.dir[0], c.dir[1], c.dir[2]);
        glm::vec3 b = a + dir * c.length;
        glm::vec3 tmp = glm::vec3(c.r);

        return AABB(glm::min(a, b) - tmp, glm::max(a, b) + tmp);
    }

    AABB aabbFromBoxes(AABB b0, AABB b1) {
        return AABB(glm::min(b0.min, b1.min), glm::max(b0.max, b1.max));
    }

    AABB aabbFromList(std::vector<Cylinder> objects) {
        AABB temp;
        AABB output;
        bool first = true;

        for (auto obj: objects) {
            temp = aabbFromObject(obj.c);
            output = first ? temp : aabbFromBoxes(temp, output);
            first = false;
        }

        return output;
    }

    bool boxCompare(Cylinder a, Cylinder b, int axis) {
        AABB box0 = aabbFromObject(a.c);
        AABB box1 = aabbFromObject(b.c);

        return box0.min[axis] < box1.min[axis];
    }

    bool boxXCompare(Cylinder a, Cylinder b) {
        return boxCompare(a, b, 0);
    }

    bool boxYCompare(Cylinder a, Cylinder b) {
        return boxCompare(a, b, 1);
    }

    bool boxZCompare(Cylinder a, Cylinder b) {
        return boxCompare(a, b, 2);
    }

    bool nodeCompare(BVHNode a, BVHNode b) {
        return a.index < b.index;
    }

//heavily inspired by https://github.com/grigoryoskin/vulkan-compute-ray-tracing/tree/master
    std::vector<BVHNodeGPU> createBHV(std::vector<Cylinder> objects) {
        std::vector<BVHNode> intermediate;
        int nodeCounter = 0;
        std::stack<BVHNode> nodeStack;

        BVHNode root;
        root.index = nodeCounter;
        root.objects = objects;
        nodeCounter++;
        nodeStack.push(root);

        while (!nodeStack.empty()) {
            BVHNode curr = nodeStack.top();
            nodeStack.pop();

            curr.box = aabbFromList(curr.objects);

            int axis = curr.sortingAxis();
            auto comparator = (axis == 0) ? boxXCompare
                                          : (axis == 1) ? boxYCompare
                                                        : boxZCompare;

            int objectNum = curr.objects.size();
            std::sort(curr.objects.begin(), curr.objects.end(), comparator);

            if (objectNum > 1) {
                int mid = objectNum / 2;
                BVHNode left;
                left.index = nodeCounter++;
                left.axis = curr.axis;
                for (int i = 0; i < mid; i++) {
                    left.objects.push_back(curr.objects[i]);
                }
                nodeStack.push(left);

                BVHNode right;
                right.index = nodeCounter++;
                right.axis = curr.axis;
                for (int i = mid; i < objectNum; i++) {
                    right.objects.push_back(curr.objects[i]);
                }
                nodeStack.push(right);

                curr.leftNodeIndex = left.index;
                curr.rightNodeIndex = right.index;
            }
            intermediate.push_back(curr);
        }
        std::sort(intermediate.begin(), intermediate.end(), nodeCompare);
        auto node = intermediate[0];
        std::vector<BVHNodeGPU> output;
        output.reserve(intermediate.size());

        for (auto node: intermediate) {
            output.push_back(node.gpuNode());
        }
        return output;
    }
}

