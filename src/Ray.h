#pragma once

#include <glm/glm.hpp>


class Ray {

private:
    glm::vec3 origin;
    glm::vec3 direction;

public:
    Ray(glm::vec3 o, glm::vec3 d);

    glm::vec3 rayAt(float t);

    //for debugging purposes
    float intersectSphere(glm::vec3 pos, float r);

    //actually needed
    float intersectCylinder(glm::vec3 cap, glm::vec3 dir, float r, float length, glm::vec3* normal);

    float intersectCylinder(glm::vec3 p1, glm::vec3 p2, float r, glm::vec3* normal);


    bool solveQuadratic(float a, float b, float c, float* t0, float* t1);

    bool solveHalfQuadratic(float a, float half_b, float c, float* t0, float* t1);

};


