#include "Ray.h"
#include <glm/gtc/matrix_transform.hpp>
#include "logger.h"

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

    //rotate space such that
    glm::vec3 oc = origin - cap;

    float a = direction.x * direction.x + direction.z * direction.z;
    float half_b = direction.x * oc.x + direction.z * oc.z;
    float c = oc.x * oc.x + oc.z * oc.z - r * r;

    float t0, t1;
    if (!solveHalfQuadratic(a, half_b, c, &t0, &t1)) {
        return -1;
    } else {
        glm::vec3 p0 = rayAt(t0) - cap;//vector from cap to point on infinite cylinder
        glm::vec3 p1 = rayAt(t1) - cap;

        float d0 = glm::dot(p0, dir);
        float d1 = glm::dot(p1, dir);

        if (d0 < 0) {
            //we are looking at the bottom of the cylinder or missing it completely
            if (d1 < 0 || d1 > length) { return -1; }//we missed it
            return t1;
            //we need to find intersection with the circle that forms the cap i guess
        } else if (d0 > length) {
            //we are looking at the top of the cylinder or missing it completely
            if (d1 < 0 || d1 > length) { return -1; }//we missed it
            return t1;
        }

        return glm::min(t0, t1);
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
    float tmin = (-half_b - glm::sqrt(D)) / a;
    float tmax = (-half_b + glm::sqrt(D)) / a;

    if (tmin > tmax) {
        float temp = tmax;
        tmax = tmin;
        tmin = temp;
    }
    *t0 = tmin;
    *t1 = tmax;
    return true;
}

glm::mat4 Ray::matchVectorRotationsMatrix(glm::vec3 axis, float sinT, float cosT) {
    //asin is very computationally expensive, currently already doubles computation time with 4 cyllinders
    //so to skip calculating the angle we can use the fact that dot(v,u) = cos(theta), cross(v,u) = sin(theta) * n
    glm::vec4 c1 = glm::vec4(cosT + axis.x * axis.x * (1 - cosT),
                             axis.y * axis.x * (1 - cosT) + axis.z * sinT,
                             axis.z * axis.x * (1 - cosT) - axis.y * sinT,
                             0.0f);
    glm::vec4 c2 = glm::vec4(axis.x * axis.y * (1 - cosT) - axis.z * sinT,
                             cosT + axis.y * axis.y * (1 - cosT),
                             axis.z * axis.y * (1 - cosT) + axis.x * sinT,
                             0.0f);
    glm::vec4 c3 = glm::vec4(axis.x * axis.z * (1 - cosT) + axis.y * sinT,
                             axis.y * axis.z * (1 - cosT) - axis.x * sinT,
                             cosT + axis.z * axis.z * (1 - cosT),
                             0.0f);
    glm::vec4 c4 = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);


    return glm::mat4(c1, c2, c3, c4);
}