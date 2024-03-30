//
// Created by wille on 27/03/2024.
//

#include "RayTraceWrapper.h"
#include "RenderSettings.h"

void RayTraceWrapper::init() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenTextures(1, &texture);

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
//    Info(lowerLeft.x << ", " << lowerLeft.y << ", " << lowerLeft.z);
//    Info(settings.camera.Position.x << ", " << settings.camera.Position.y << ", " << settings.camera.Position.z);
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
    int size = 0;
    for (auto& ds: settings.datasets) {
        ds->bindSSBO();
        size += ds->getVertexNum();
    }
    Info(pixelOffset);
    settings.rtComputeShader.setInt("bufferSize", size);

    glDispatchCompute(imgWidth, rowsPerFrame, 1);

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    settings.rtRenderShader.use();
    settings.rtRenderShader.setInt("windowW", imgWidth);
    settings.rtRenderShader.setInt("windowH", imgHeight);
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    int newOffset = pixelOffset + rowsPerFrame;
    if (newOffset >= imgHeight - 1) {
        imgNum++;
    }
    pixelOffset = (newOffset) % imgHeight;
}

void RayTraceWrapper::cleanup() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteTextures(1, &texture);
}

RayTraceWrapper::RayTraceWrapper() {
    settings.rt = this;
}
