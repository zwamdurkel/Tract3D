#include "RayTracer.h"
#include "Ray.h"
#include "RenderSettings.h"

void RayTracer::init() {
    //should be called everytime camera is moved since image will not be valid anymore, and thus needs to be reset
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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

    reset();
}

void RayTracer::reset() {
    //reset and bind texture
    Camera cam = settings.camera;//used to get window sizes

    colours.clear();
    glfwGetWindowSize(settings.glfw->getWindow(), &imgWidth, &imgHeight);
    imgSize = imgWidth * imgHeight;

    colours.resize(imgSize, glm::vec3(0.0f, 0.0f, 0.0f));

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, imgWidth, imgHeight, 0, GL_RGB, GL_FLOAT,
                 &colours[0]);//should be empty image
    glGenerateMipmap(GL_TEXTURE_2D);

    renderIndex = 0;//start from the first pixel
    //precompute pixel deltas
    float w = 2 * glm::tan(glm::radians(cam.FOV / 2)) * cam.NearPlane;
    float h = w * ((float) imgHeight / (float) imgWidth);
    dX = w / imgWidth;
    dY = h / imgHeight;//brother these two are the same
    settings.rayTracingShader.setInt("windowW", imgWidth);
    settings.rayTracingShader.setInt("windowH", imgHeight);
}

void RayTracer::renderPixel() {
    Camera cam = settings.camera;

    //first get pixel coords in range [0,imgWidth-1], [0, imgHeight-1]
    glm::vec2 pixelCoord = glm::vec2(float(renderIndex % imgWidth),
                                     float(renderIndex / imgWidth));//integer division so no need for floor()
    // then transform coords in range [-imgWidth/2, imgWidth/2], [-imgHeight/2, imgHeight/2]
    pixelCoord -= glm::vec2(imgWidth / 2.0f,
                            imgHeight / 2.0f);
    pixelCoord = dX * pixelCoord;//dX and dY are the same so this works

    glm::vec3 screenCentre = cam.Position + cam.Front * cam.NearPlane;
    glm::vec3 screenCoord = screenCentre + pixelCoord.x * cam.Right + pixelCoord.y * cam.Up;
    glm::vec3 dir = glm::normalize(screenCoord - cam.Position);//no need to normalize since ray constructor will do this
    //finally create our ray
    Ray ray = Ray(cam.Position, dir);

    //Initial list of sphere data for debugging purposes
    glm::vec3 centers[] = {glm::vec3(0.0f, -25.0f, 95.0f),
                           glm::vec3(0.0f, 1.0f, 5.0f),
                           glm::vec3(5.0f, 1.0f, 5.0f),
                           glm::vec3(0.0f, -1.0f, 0.0f)};
    float radii[] = {0.5f, 1, 1, 1};
    //--------------------------------------------

    glm::vec3 colour = glm::vec3(1, 1, 1);//colour of the ray

    //-------------------------------------------
    //----insert lighting calculation here ------
    float t = -1;
    const float minRayDist = 0.001;

    for (int i = 0; i < 4; i++) {
        float newT = ray.intersectCylinder(centers[i], glm::vec3(0.0f, 1.0f, 0.0f), radii[i], 3);
//        float newT = ray.intersectSphere(centers[i], radii[i]);
        if (newT < minRayDist) { continue; }
        if (t < 0) { t = newT; }
        else if (newT < t) {
            //get information about hit point
            t = newT;
        }
    }
    if (t >= minRayDist) {
        colour = glm::vec3(t, 0.0f, 0.0f);
    }


    //-------------------------------------------

    colours[renderIndex] = colour;//TODO: Add averaging
    //update pixel index so next call does next pixel
    renderIndex++;
    renderIndex = renderIndex % imgSize;//Make a loop
}

void RayTracer::draw() {
    settings.rayTracingShader.use();
    glBindVertexArray(VAO);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, imgWidth, imgHeight, 0, GL_RGB, GL_FLOAT,
                 &colours[0]);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

RayTracer::RayTracer() {

}

RayTracer::~RayTracer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteTextures(1, &texture);
}