//
// Created by wille on 27/03/2024.
//

#include "RayTraceWrapper.h"
#include "RenderSettings.h"


void RayTraceWrapper::init() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenTextures(1, &texture);
    glGenBuffers(1, &ObjSSBO);
    glGenBuffers(1, &BvhSSBO);
    resetImg();
    rowsPerFrame = 1;
    pixelOffset = imgHeight - 1;
    //quad vertices
    float vertices[]{//vertices of the quad on which we render the result
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f,

            1.0f, -1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f,
    };

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    //shader buffer objects
    initBVH();

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ObjSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, obj.size() * sizeof(BVH::CylinderGPU), &obj[0], GL_STATIC_DRAW);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, BvhSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, bvh.size() * sizeof(BVH::BVHNodeGPU), &bvh[0], GL_STATIC_DRAW);
}

void RayTraceWrapper::resetImg() {
    pixelOffset = 0;
    imgNum = 0;
    glDeleteTextures(1, &texture);
    glGenTextures(1, &texture);
    glfwGetWindowSize(settings.glfw->getWindow(), &imgWidth, &imgHeight);
    imgSize = imgWidth * imgHeight;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, imgWidth, imgHeight, 0,
                 GL_RGBA, GL_FLOAT, NULL);
    glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    glBindTexture(GL_TEXTURE_2D, 0); // unbind
}

void RayTraceWrapper::resetCamera() {
    float w = glm::tan(glm::radians(settings.camera.FOV / 2.0f)) * settings.camera.NearPlane;
    float h = w * ((float) imgHeight / (float) imgWidth);
    pixelDelta = w / imgWidth;
    glm::vec3 screenCentre = settings.camera.Position + settings.camera.Front * settings.camera.NearPlane;
    lowerLeft = screenCentre - w * settings.camera.Right - h * settings.camera.Up;
}


void RayTraceWrapper::createCylinders() {
    obj.clear();
    if (settings.datasets.empty() &&
        settings.examples.empty()) { return; }//if no dataset exists we cannot make any cylinders

    //we always take the first in the list, we maybe increase later to all of them
    std::vector<ssboUnit> ds;
    //find first enabled dataset
    auto dataList = {std::cref(settings.datasets), std::cref(settings.examples)};
    for (const auto& datasets: dataList) {
        for (auto& d: datasets.get()) {
            if (d->enabled) {
                ds = d->getSSBOData();
                break;
            }
        }
        if (!ds.empty()) { break; }
    }

    glm::vec3 min = glm::vec3(0);
    glm::vec3 max = glm::vec3(0);

    for (int i = 1; i < ds.size() - 1; i++) {
        ssboUnit ss0 = ds[i];
        ssboUnit ss1 = ds[i + 1];

        glm::vec3 pos0(ss0.position[0], ss0.position[1], ss0.position[2]);
        glm::vec3 pos1(ss1.position[0], ss1.position[1], ss1.position[2]);

        min = glm::min(min, pos0);
        min = glm::min(min, pos1);
        max = glm::max(max, pos0);
        max = glm::max(max, pos1);

        glm::vec3 grad(ss0.gradient[0], ss0.gradient[1], ss0.gradient[2]);

        glm::vec3 diff = pos1 - pos0;
        float length = glm::length(diff);
        diff /= length;//normalize it
        if (dot(diff, grad) < 0.75) { continue; }

        BVH::CylinderGPU c({pos0.x, pos0.y, pos0.z},
                           {diff.x, diff.y, diff.z},
                           {ss0.gradient[0], ss0.gradient[1], ss0.gradient[2]},
                           {ss1.gradient[0], ss1.gradient[1], ss1.gradient[2]},
                           0.1,
                           length);

        obj.push_back(c);
    }
}

void RayTraceWrapper::draw() {
    resetCamera();

    settings.rtComputeShader.use();
    settings.rtComputeShader.setVec3("eye", settings.camera.Position);
    settings.rtComputeShader.setVec3("up", settings.camera.Up);
    settings.rtComputeShader.setVec3("right", settings.camera.Right);
    settings.rtComputeShader.setVec3("lowerLeft", lowerLeft);
    settings.rtComputeShader.setFloat("pixelDelta", pixelDelta);
    settings.rtComputeShader.setInt("pixelYoffset", pixelOffset);
    settings.rtComputeShader.setInt("frameCount", imgNum);
    settings.rtComputeShader.setInt("depth", settings.rtBounceNr);
    if (settings.datasets.empty() && settings.examples.empty()) { return; }

    //bind buffers
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ObjSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, BvhSSBO);

    glDispatchCompute(imgWidth, rowsPerFrame, 1);

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    settings.rtRenderShader.use();
    settings.rtRenderShader.setInt("windowW", imgWidth);
    settings.rtRenderShader.setInt("windowH", imgHeight);
    settings.rtRenderShader.setInt("frameCount", imgNum);
    settings.rtRenderShader.setInt("blurEnabled", settings.blurEnabled);
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    int newOffset = pixelOffset + rowsPerFrame;
    if (newOffset >= imgHeight) {
        imgNum++;
        Info("Frame: " << imgNum);
    }
    pixelOffset = (newOffset) % imgHeight;
}

void RayTraceWrapper::cleanup() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &ObjSSBO);
    glDeleteBuffers(1, &BvhSSBO);
    glDeleteTextures(1, &texture);
}

RayTraceWrapper::RayTraceWrapper() {
    settings.rt = this;
}

void RayTraceWrapper::initBVH() {
    bvh.clear();
    createCylinders();
    std::vector<BVH::Cylinder> cylinders;
    for (int i = 0; i < obj.size(); i++) {
        BVH::Cylinder c(i, obj[i]);
        cylinders.push_back(c);
    }

    bvh = BVH::createBHV(cylinders);
}

//comparator functions for sorting


