#include "Bvh.h"

BVH::AABB BVH::aabbFromObject(BVH::CylinderGPU c) {
    glm::vec3 a(c.cap[0], c.cap[1], c.cap[2]);
    glm::vec3 dir(c.dir[0], c.dir[1], c.dir[2]);
    glm::vec3 b = a + dir * c.length;
    glm::vec3 tmp = glm::vec3(c.r);

    return AABB(glm::min(a, b) - tmp, glm::max(a, b) + tmp);
}

BVH::AABB BVH::aabbFromBoxes(BVH::AABB b0, BVH::AABB b1) {
    return AABB(glm::min(b0.min, b1.min), glm::max(b0.max, b1.max));
}

BVH::AABB BVH::aabbFromList(std::vector<BVH::Cylinder> objects) {
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

bool BVH::boxCompare(BVH::Cylinder a, BVH::Cylinder b, int axis) {
    AABB box0 = aabbFromObject(a.c);
    AABB box1 = aabbFromObject(b.c);

    return box0.min[axis] < box1.min[axis];
}

bool BVH::boxXCompare(BVH::Cylinder a, BVH::Cylinder b) {
    return boxCompare(a, b, 0);
}

bool BVH::boxYCompare(BVH::Cylinder a, BVH::Cylinder b) {
    return boxCompare(a, b, 1);
}

bool BVH::boxZCompare(BVH::Cylinder a, BVH::Cylinder b) {
    return boxCompare(a, b, 2);
}

bool BVH::nodeCompare(BVH::BVHNode a, BVH::BVHNode b) {
    return a.index < b.index;
}

//heavily inspired by https://github.com/grigoryoskin/vulkan-compute-ray-tracing/tree/master
std::vector<BVH::BVHNodeGPU> BVH::createBHV(std::vector<BVH::Cylinder> objects) {
    Info("Data size: " << objects.size());
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