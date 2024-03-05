#include "GLFWWrapper.h"
#include "path.h"
#include "logger.h"
#include "ImGuiWrapper.h"
#include "TractDataWrapper.h"

const unsigned int INITIAL_WIDTH = 800;
const unsigned int INITIAL_HEIGHT = 600;

//stuff for mouse location
float lastX;
float lastY;
bool firstMouse = true;

void GLFWWrapper::init() {
    settings.glfw = this;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // for MAC OS
    glfwWindowHint(GLFW_SAMPLES, 4);

    window = glfwCreateWindow(INITIAL_WIDTH, INITIAL_HEIGHT, "Tract-based 3D animation tool", NULL, NULL);

    if (window == NULL) {
        Error("Failed to create GLFWWrapper window");
        exit(-1);
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        Error("Failed to initialize GLAD");
        exit(-1);
    }

    // Update on resize:
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
        RenderSettings& settings = RenderSettings::getInstance();
        // Make sure we don't get divide by 0.
        settings.camera.windowWidth = width ? width : 1;
        settings.camera.windowHeight = height ? height : 1;
    });

    // Mouse movement callback
    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xposIn, double yposIn) {
        RenderSettings& settings = RenderSettings::getInstance();
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);

        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT))
            settings.camera.ProcessMouseMovement(xoffset, yoffset);
    });

    // Hide cursor while Panning
    glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    });

    // Mouse scroll callback
    glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
        RenderSettings& settings = RenderSettings::getInstance();
        if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
            settings.camera.ProcessMouseScroll(yoffset);
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            settings.camera.ChangeFOV(yoffset);
    });

    // Keep rendering while resizing
    glfwSetWindowRefreshCallback(window, [](GLFWwindow* window) {
        RenderSettings& settings = RenderSettings::getInstance();
        settings.glfw->draw();
        for (auto& dataset: settings.datasets) {
            dataset->draw();
        }
        settings.imgui->draw();

        glfwSwapBuffers(window);
    });

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
    // Used for drawing lines from one buffer.
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(0xFFFFFFFF);
}

void GLFWWrapper::draw() {
    // Clear and set Clear Color
    glClearColor(settings.clear_color.x, settings.clear_color.y, settings.clear_color.z, settings.clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLFWWrapper::cleanup() {
    glfwTerminate();
}
