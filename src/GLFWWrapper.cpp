#include "GLFWWrapper.h"
#include "logger.h"
#include "RenderSettings.h"

const unsigned int INITIAL_WIDTH = 1280;
const unsigned int INITIAL_HEIGHT = 720;

//stuff for mouse location
float lastX;
float lastY;
bool firstMouse = true;

void GLFWWrapper::init() {
    settings.glfw = this;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
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
        settings.pt->resetImg();
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

        settings.shader.use();
        settings.shader.setMat4("uProjectionMatrix", settings.camera.GetProjectionMatrix());

        settings.glfw->draw();
        // Simplified dataset rendering
        for (const auto& datasets: {std::cref(settings.datasets), std::cref(settings.examples)}) {
            for (auto& d: datasets.get()) {
                if (d->enabled) {
                    d->draw();
                }
            }
        }
        settings.imgui->draw();

        glfwSwapBuffers(window);
        glFinish();
    });

    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        RenderSettings& settings = RenderSettings::getInstance();
        if (key == GLFW_KEY_F11 && action == GLFW_PRESS) {
            settings.fullScreen = !settings.fullScreen;
            settings.glfw->setFullScreen(settings.fullScreen);
        }
    });

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glPointSize(5);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void GLFWWrapper::draw() {
    // Clear and set Clear Color
    glClearColor(settings.clear_color.x, settings.clear_color.y, settings.clear_color.z, settings.clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLFWWrapper::cleanup() {
    glfwTerminate();
}

void GLFWWrapper::setFullScreen(bool enabled) {
    static int xpos, ypos, width, height;

    if (enabled) {
        glfwGetWindowPos(window, &xpos, &ypos);
        glfwGetWindowSize(window, &width, &height);

        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    } else {
        glfwSetWindowMonitor(window, NULL, xpos, ypos, width, height, 0);
    }
}
