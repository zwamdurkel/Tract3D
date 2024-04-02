#ifdef _WIN32
#define _WIN32_WINNT 0x0501 // enable AttachConsole command

#include <windows.h>

// Tell GPU drivers to use Dedicated GPU instead of iGPU (mostly for laptops)
extern "C" {
__declspec(dllexport) DWORD NvOptimusEnablement = 1;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

#include <glad/glad.h> // this is important
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <algorithm>
#include <filesystem>
#include "logger.h"
#include "path.h"
#include "GLFWWrapper.h"
#include "ImGuiWrapper.h"
#include "TractDataWrapper.h"
#include "learnopengl/camera.h"
#include "learnopengl/shader_s.h"
#include "RenderSettings.h"

void run();

void processInput(GLFWwindow* window);

int main() {
#ifdef _WIN32
    // Attach output to console if called from console.
    AttachConsole(-1);
    freopen("CONIN$", "r", stdin);
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
#endif

    std::cout << "Tract-based 3D animation tool by Willem, Finnean and Aloys\n" << std::endl;

#ifdef _WIN32
    auto exec = getExec();
    std::cout
            << "For optimal output in consoles, use the following:\n"
            << " PowerShell:    Start-Process -Wait .\\" << exec << "\n"
            << " CMD:           start /wait " << exec << "\n"
            << std::endl;
#endif

    // Run program :)
    run();

    return 0;
}

void computeInfo() {
    // show compute shader related info
    // work group handling
    // work group count
    GLint work_group_count[3];
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_group_count[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_group_count[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_group_count[2]);
    std::cout << "total work group count x: " << work_group_count[0] << std::endl;
    std::cout << "total work group count y: " << work_group_count[1] << std::endl;
    std::cout << "total work group count z: " << work_group_count[2] << std::endl;

    // work group size
    GLint work_group_size[3];
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_group_size[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_group_size[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_group_size[2]);
    std::cout << "total work group size x: " << work_group_size[0] << std::endl;
    std::cout << "total work group size y: " << work_group_size[1] << std::endl;
    std::cout << "total work group size z: " << work_group_size[2] << std::endl;
    // global work group size is 512 * 512 == texture width * texture height
    // local work group size is 1 since 1 pixel at a time

    // work group invocation
    GLint work_group_inv;
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_group_inv);
    std::cout << "max work group invocation: " << work_group_inv << std::endl;
    // end of work group
}

bool getExamples() {
    RenderSettings& settings = RenderSettings::getInstance();
    namespace fs = std::filesystem;
    auto path = getPath();

    for (const auto& entry: fs::directory_iterator(path + "examples")) {
        std::string filePath = entry.path().string();
        std::replace(filePath.begin(), filePath.end(), '\\', '/');
        settings.examples.emplace_back(std::make_unique<TractDataWrapper>(entry.path().filename().string(), filePath));
    }

    return true;
}

void run() {
    RenderSettings& settings = RenderSettings::getInstance();
    Info("Attempting to initialize window");

    Info("Creating GLFW Wrapper");
    GLFWWrapper glfw;
    Info("Initializing GLFW");
    glfw.init();
    Info("Creating ImGUI wrapper");
    ImGuiWrapper imgui(glfw.getWindow());
    Info("Initializing ImGUI");
    imgui.init();
    RayTraceWrapper rt;
    rt.init();
    //computeInfo();
    Info("Successfully initialized window");

    GLFWwindow* window = glfw.getWindow();

    namespace fs = std::filesystem;
    auto path = getPath();

    // Import vertex and fragment shaders

    settings.defaultShader = Shader(path + "basic.vsh", path + "basic.fsh");//draw only lines
    settings.lineShadingShader = Shader(path + "basic.vsh", path + "LineShading.fsh");//draw only lines
    settings.rtComputeShader = Shader(path + "rtCompute.comp");
    settings.rtRenderShader = Shader(path + "rtRender.vsh", path + "rtRender.fsh");

    settings.shader = settings.defaultShader;//draw only lines

    Info("Starting render");

    glm::mat4 modelMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::vec4 lightPos = glm::normalize(glm::vec4(1.0f, 1.0f, 0.0f, 0.0f));

    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        static float lastFrameTime;
        float currentFrameTime = glfwGetTime();
        float deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        if (settings.rotatingLight) {
            glm::mat4 lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(float(90.0f * deltaTime)),
                                                  glm::vec3(0.0f, 1.0f, 0.0f));
            lightPos = lightRotation * lightPos;
        }

        settings.shader.use();
        settings.shader.setVec3("lightDir", lightPos.x, lightPos.y, lightPos.z);
        settings.shader.setMat4("uModelMatrix", modelMatrix);
        settings.shader.setMat4("uViewMatrix", settings.camera.GetViewMatrix());
        settings.shader.setMat4("uProjectionMatrix", settings.camera.GetProjectionMatrix());
        settings.shader.setVec3("uViewPos", settings.camera.Position);
        settings.shader.setBool("uDrawTubes", settings.renderer == SHADED_TUBES);
        settings.shader.setInt("uNrOfSides", settings.nrOfSides);
        settings.shader.setFloat("uTubeDiameter", settings.tubeDiameter);

        glfw.draw();
        if (settings.renderer != rendererType::RAY_TRACING) {
            auto dataList = {std::cref(settings.datasets), std::cref(settings.examples)};
            [&] {
                for (const auto& datasets: dataList) {
                    for (auto& d: datasets.get()) {
                        if (d->name == settings.highlightedBundle && d->enabled) {
                            d->draw();
                            return;
                        }
                    }
                }
            }();
            for (const auto& datasets: dataList) {
                for (auto& d: datasets.get()) {
                    if (d->name != settings.highlightedBundle && d->enabled) {
                        d->draw();
                    }
                }
            }
        } else {
            settings.rt->draw();
        }

        imgui.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
        static bool t = getExamples(); // Get examples once (because static) after initial paint.
    }
    Info("User requested to close");

    Info("Attempting to clean up before closing");
    imgui.cleanup();
    glfw.cleanup();
    Info("Cleaned up successfully, exiting...");
}

// Close window if ESC is pressed.
void processInput(GLFWwindow* window) {
    static float lastFrameTime;
    float currentFrameTime = glfwGetTime();
    float deltaTime = currentFrameTime - lastFrameTime;
    lastFrameTime = currentFrameTime;

    static Camera& cam = RenderSettings::getInstance().camera;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cam.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cam.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cam.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cam.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        cam.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cam.ProcessKeyboard(DOWN, deltaTime);


}
