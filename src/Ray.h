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
    float intersectCylinder(glm::vec3 pos, float r);
};


