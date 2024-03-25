#include "RayTracer.h"
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

    //first get pixel coords in range [0,imgWidth-1], [0, imgHeight-1]
    glm::vec2 pixelCoord = glm::vec2(float(renderIndex % imgWidth),
                                     float(renderIndex / imgWidth));//integer division so no need for floor()
    // then transform coords in range [-imgWidth/2, imgWidth/2], [-imgHeight/2, imgHeight/2]
    pixelCoord -= glm::vec2(imgWidth / 2.0f,
                            imgHeight / 2.0f);
    pixelCoord = dX * pixelCoord;//dX and dY are the same so this works



    //-------------------------------------------
    //----insert lighting calculation here ------
    glm::vec3 colour = glm::vec3(0.0f);

    int nrays = 4;
    for (int i = 0; i < nrays; i++)
        colour += getRayColour(generateRay(pixelCoord));


    //-------------------------------------------

    colours[renderIndex] = colour * 0.25f;//TODO: Add averaging
    //update pixel index so next call does next pixel
    renderIndex++;
    renderIndex = renderIndex % imgSize;//Make a loop
}

Ray RayTracer::generateRay(glm::vec2 pixelCoord) {
    Camera cam = settings.camera;
    glm::vec2 delta = glm::vec2(distribution(generator), distribution(generator)) * (dX * 0.5f);

    glm::vec3 screenCentre = cam.Position + cam.Front * cam.NearPlane;
    glm::vec3 screenCoord = screenCentre + pixelCoord.x * cam.Right + pixelCoord.y * cam.Up;
    glm::vec3 dir = glm::normalize(screenCoord - cam.Position);//no need to normalize since ray constructor will do this
    //finally create our ray
    return Ray(cam.Position, dir);
}

glm::vec3 RayTracer::getRayColour(Ray ray) {
    float t = -1;
    const float minRayDist = 0.001;
    const float tuberadius = 0.1f;
    glm::vec3 n;
    glm::vec3 col = glm::vec3(0.0f, 0.0f, 0.0f);
    //auto data = settings.datasets[0]->data[0];
    for (auto data: settings.datasets[0]->data) {
        for (int i = 0; i < data.vertices.size() - 1; i++) {
            glm::vec3 newN;
            float newT = ray.intersectCylinder(data.vertices[i], data.vertices[i + 1], tuberadius, &newN);
//        float newT = ray.intersectSphere(centers[i], radii[i]);
            if (newT < minRayDist) { continue; }
            else if (t < 0 || newT < t) {
                //get information about hit point
                t = newT;
                n = newN;
                col = glm::abs(glm::normalize(data.vertices[i + 1] - data.vertices[i]));
            }
        }
    }
    if (t >= minRayDist) {
        col = col * glm::vec3(glm::dot(n, glm::vec3(0.0f, 1.0f, 0.0f)));
    }
    return col;
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