#include "RayTracer.h"
#include "Ray.h"

void RayTracer::init() {
    //should be called everytime camera is moved since image will not be valid anymore, and thus needs to be reset

    Camera cam = RenderSettings::getInstance().camera;//used to get window sizes
    float vertices[]{
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            0.0f, 0.5f, 0.0f,

            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f
    };

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    //reset and bind texture
    colours.clear();
    imgWidth = cam.windowHeight;
    imgHeight = cam.windowWidth;
    imgSize = imgWidth * imgHeight;
    colours.resize(imgSize, glm::vec3(0.0f));

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cam.windowWidth, cam.windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE,
                 &colours);//should be empty image, might actually fuck up some shit
    glGenerateMipmap(GL_TEXTURE_2D);

    renderIndex = 0;//start from the first pixel
    //precompute pixel deltas
    float w = 2 * glm::tan(glm::radians(cam.FOV / 2)) * cam.NearPlane;
    float h = w * ((float) imgHeight / (float) imgWidth);
    dX = w / imgWidth;
    dY = h / imgHeight;

}

void RayTracer::renderPixel() {
    Camera cam = RenderSettings::getInstance().camera;

    //first get pixel coords in range [0,imgWidth-1], [0, imgHeight-1]
    glm::vec2 pixelCoord = glm::vec2(float(renderIndex % imgWidth),
                                     float(renderIndex / imgHeight));
    // then transform coords in range [-imgWidth/2, imgWidth/2], [-imgHeight/2, imgHeight/2]
    pixelCoord -= glm::vec2(imgWidth / 2.0f,
                            imgHeight / 2.0f);

    glm::vec3 screenCentre = cam.Position + cam.Front;
    glm::vec3 screenCoord = screenCentre + pixelCoord.x * cam.Right + pixelCoord.y * cam.Up;
    glm::vec3 dir = screenCoord - cam.Position;//no need to normalize since ray constructor will do this

    //finally create our ray
    Ray ray = Ray(cam.Position, dir);

    //update pixel index so next call does next pixel
    renderIndex++;
}

void RayTracer::draw() {
    RenderSettings::getInstance().rayTracingShader.use();
    glBindVertexArray(VAO);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void RayTracer::cleanup() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

RayTracer::RayTracer() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
}

RayTracer::~RayTracer() {
    cleanup();
}