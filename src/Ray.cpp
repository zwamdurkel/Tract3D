#include "Ray.h"

Ray::Ray(glm::vec3 o, glm::vec3 d) {
    origin = o;
    direction = glm::normalize(d);
}

glm::vec3 Ray::rayAt(float t) {
    return origin + t * direction;
}

float Ray::intersectSphere(glm::vec3 pos, float r) {
    glm::vec3 oc = origin - pos;
    float a = glm::dot(direction, direction);
    float half_b = dot(oc, direction);
    float c = dot(oc, oc) - r * r;
    float D = half_b * half_b - a * c;

    float t1, t2;
    if (!solveHalfQuadratic(a, half_b, c, &t1, &t2)) {
        return -1;
    } else {
        return glm::min(t1, t2);
    }
}

float Ray::intersectCylinder(glm::vec3 cap, glm::vec3 dir, float r, float length) {
    //currently just a circle extended infinitely along z direction
    glm::vec3 oc = origin - cap;
    float a = direction.x * direction.x + direction.z * direction.z;
    float half_b = direction.x * oc.x + direction.z * oc.z;
    float c = oc.x * oc.x + oc.z * oc.z - r * r;

    float t1, t2;
    if (!solveHalfQuadratic(a, half_b, c, &t1, &t2)) {
        return -1;
    } else {
        return glm::min(t1, t2);
    }
}

bool Ray::solveQuadratic(float a, float b, float c, float* t0, float* t1) {
    float D = b * b - 4 * a * c;
    if (D < 0) {
        return false;
    }
    *t0 = (-b - glm::sqrt(D)) / (2 * a);
    *t1 = (-b + glm::sqrt(D)) / (2 * a);
    return true;
}

//can save a few multiplication operations if the definition of b allows for it.
//if b is defined as "b = 2*g" for some value g then solveQuadratic(a,b,c) == solveHalfQuadratic(a,g,c)
bool Ray::solveHalfQuadratic(float a, float half_b, float c, float* t0, float* t1) {
    float D = half_b * half_b - a * c;
    if (D < 0) {
        return false;
    }
    *t0 = (-half_b - glm::sqrt(D)) / a;
    *t1 = (-half_b + glm::sqrt(D)) / a;
    return true;
}