#ifdef _WIN32
#define _WIN32_WINNT 0x0501

#include <windows.h>

#endif

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <logger.h>
#include <learnopengl/shader_s.h>
#include <path.h>


const unsigned int INITIAL_WIDTH = 800;
const unsigned int INITIAL_HEIGHT = 600;
std::string path;

void processInput(GLFWwindow *window);

void render(GLFWwindow *window);

GLFWwindow *init();

void cleanup();

int main() {
    std::atexit(cleanup);

#ifdef _WIN32
    // Attach output to console if called from console.
    AttachConsole(-1);
    freopen("CONIN$", "r", stdin);
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
#endif

    path = getPath();

    std::cout << "Tract-based 3D animation tool by Willem, Finnean and Aloys\n" << std::endl;

    std::cout
            << "For optimal output in consoles, use the following:\n"
            << " PowerShell:    .\\Start-Process -Wait .\\Brain.exe\n"
            << " CMD:           start /wait Brain.exe\n"
            << std::endl;

    Info("Attempting to initialize window");
    // Initialize GLFW and GLAD
    GLFWwindow *window = init();
    Info("Successfully initialized window");

    Info("Starting render");
    render(window);

    return 0;
}

// Main render loop
void render(GLFWwindow *window) {
    float vertices[] = {
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            0.0f, 0.5f, 0.0f
    };

    Shader shader(path + "basic.vsh",
                  path + "basic.fsh");
    unsigned int VAO, VBO;

    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    // 1. bind Vertex Array Object
    glBindVertexArray(VAO);
    // 2. copy our vertices array in a buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 3. then set our vertex attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

// Close window if ESC is pressed.
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// Initializes a GLFW window
GLFWwindow *init() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // for MAC OS

    GLFWwindow *window = glfwCreateWindow(INITIAL_WIDTH, INITIAL_HEIGHT, "Tract-based 3D animation tool", NULL, NULL);

    if (window == NULL) {
        Error("Failed to create GLFW window");
        exit(-1);
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        Error("Failed to initialize GLAD");
        exit(-1);
    }

    // Set OpenGL size = window size
    glViewport(0, 0, INITIAL_WIDTH, INITIAL_HEIGHT);
    // Update on resize:
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int width, int height) {
        glViewport(0, 0, width, height);
    });

    return window;
}

// Called before exiting
void cleanup() {
    Info("Attempting to clean up before exiting");
    glfwTerminate();
    Info("Successfully cleaned up");
    Info("Exiting...");

#ifdef _WIN32
    FreeConsole();
#endif
}
