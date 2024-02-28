#include "GLFWWrapper.h"
#include "learnopengl/shader_s.h"
#include "path.h"
#include "logger.h"
#include "TractData.h"

const unsigned int INITIAL_WIDTH = 800;
const unsigned int INITIAL_HEIGHT = 600;

//stuff for mouse location
float lastX;
float lastY;
bool firstMouse = true;

void GLFWWrapper::init() {
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
    // Disable vsync
    glfwSwapInterval(0);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        Error("Failed to initialize GLAD");
        exit(-1);
    }

    // Update on resize:
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
        RenderSettings& settings = RenderSettings::getInstance();
        settings.camera.windowWidth = width;
        settings.camera.windowHeight = height;
    });

    // mouse movement callback
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

        settings.camera.ProcessMouseMovement(xoffset, yoffset);
    });

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    // Used for drawing lines from one buffer.
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(0xFFFFFFFF);

    // Import vertex and fragment shaders
    auto path = getPath();
    Info(path);
    shader = Shader(path + "basic.vsh",
                    path + "basic.fsh");

    // Parse dataset into a single vertex array
    td.parse("whole_brain.tck", false);
    std::vector<float> vertices = td.data[0].vertices;

    // Set the number of tracts we have in the settings
    settings.tract_count[0] = td.tractEndIndex.size();
    settings.show_tract_count[0] = td.tractEndIndex.size();

    // Color Array
    std::vector<float> colors;
    for (int i: td.tractSizes) {
        int first = round(i / (sizeof(rainbow) / sizeof(float) / 3));
        int last = i - (first * (sizeof(rainbow) / sizeof(float) / 3 - 1));
        for (int j = 0; j < (sizeof(rainbow) / sizeof(float) - 3); j += 3) {
            for (int k = 0; k < first; k++) {
                colors.push_back(rainbow[j]);
                colors.push_back(rainbow[j + 1]);
                colors.push_back(rainbow[j + 2]);
            }
        }
        for (int k = 0; k < last; k++) {
            colors.push_back(rainbow[(sizeof(rainbow) / sizeof(float)) - 3]);
            colors.push_back(rainbow[(sizeof(rainbow) / sizeof(float)) - 2]);
            colors.push_back(rainbow[(sizeof(rainbow) / sizeof(float)) - 1]);
        }
    }

    // Generate buffers. VAO = Vertex Array Object, VBO = Vertex Buffer Object, VCO = Vertex Color Object
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &VCO);

    // Bind Vertex Array Object
    glBindVertexArray(VAO);
    // Copy Vertices Array to a Buffer for OpenGL
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, (long) (vertices.size() * sizeof(float)), (void*) &vertices[0], GL_STATIC_DRAW);
    // Then set our Vertex Attributes Pointers
    // Position Attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * td.tractIndices.size(), &td.tractIndices[0], GL_STATIC_DRAW);

    // Copy Colors Array to a Buffer for OpenGL
    glBindBuffer(GL_ARRAY_BUFFER, VCO);
    glBufferData(GL_ARRAY_BUFFER, (long) (colors.size() * sizeof(float)), (void*) &colors[0], GL_STATIC_DRAW);
    // Color Attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(1);
}

void GLFWWrapper::use() {
    // Clear and set Clear Color
    glClearColor(settings.clear_color.x, settings.clear_color.y, settings.clear_color.z, settings.clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Define Shaders
    shader.use();
    shader.setMat4("uViewMatrix", settings.camera.GetViewMatrix());
    shader.setMat4("uProjectionMatrix", settings.camera.GetProjectionMatrix());

    // Draw all tracts using a line strip primitive per tract
    glBindVertexArray(VAO);
    glDrawElements(GL_LINE_STRIP, (int) td.tractEndIndex[settings.show_tract_count[0] - 1], GL_UNSIGNED_INT,
                   (GLvoid*) 0);
}

void GLFWWrapper::cleanup() {
    glfwTerminate();
}
