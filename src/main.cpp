#ifdef _WIN32
#define _WIN32_WINNT 0x0501 // enable AttachConsole command

#include <windows.h>
#include <string>
#include <iostream>
#include <filesystem>
#include <algorithm>

// Tell GPU drivers to use Dedicated GPU instead of iGPU (mostly for laptops)
extern "C" {
__declspec(dllexport) DWORD NvOptimusEnablement = 1;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

#include <iostream>
#include <glad/glad.h> // this is important
#include <GLFW/glfw3.h>
#include "logger.h"
#include "path.h"
#include "GLFWWrapper.h"
#include "ImGuiWrapper.h"
#include "TractDataWrapper.h"
#include "learnopengl/camera.h"
#include "learnopengl/shader_s.h"

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

    Info("Successfully initialized window");

    GLFWwindow* window = glfw.getWindow();

    namespace fs = std::filesystem;
    auto path = getPath();

    for (const auto& entry: fs::directory_iterator(path + "examples")) {
        std::string filePath = entry.path().string();
        std::replace(filePath.begin(), filePath.end(), '\\', '/');
        auto td = std::make_shared<TractDataWrapper>(filePath.c_str(), entry.path().filename().string());
        settings.examples.push_back(td);
        //Info(filePath);
    }

    // Import vertex and fragment shaders
    Shader& shader = settings.shader;
    shader = Shader(path + "basic.vsh", path + "basic.fsh");//draw only lines

    Info("Starting render");
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        shader.use();
        glm::mat4 modelMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        shader.setMat4("uModelMatrix", modelMatrix);
        shader.setMat4("uViewMatrix", settings.camera.GetViewMatrix());
        shader.setMat4("uProjectionMatrix", settings.camera.GetProjectionMatrix());
        shader.setVec3("uViewPos", settings.camera.Position);
        shader.setBool("uDrawTubes", settings.drawTubes);

        glfw.draw();
        for (auto& dataset: settings.datasets) {
            if (dataset->enabled) {
                dataset->draw();
            }
        }
        for (auto& dataset: settings.examples) {
            if (dataset->enabled) {
                dataset->draw();
            }
        }
        imgui.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
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
    float deltaTime = currentFrameTime - lastFrameTime;;
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
