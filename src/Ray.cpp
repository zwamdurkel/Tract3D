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

    if (D < 0) {
        return -1;
    } else {
        return (-half_b - glm::sqrt(D)) / a;
    }
}

float Ray::intersectCylinder(glm::vec3 pos, float r) {
    //currently just a circle extended infinitely along z direction
    float a = direction.x * direction.x + direction.y + direction.y;
    float b = 2.0f * (direction.x * origin.x + direction.y * origin.y);
    float c = origin.x * origin.x + origin.y * origin.y - r * r;
    float D = b * b - 4 * a * c;
    return 0;
}