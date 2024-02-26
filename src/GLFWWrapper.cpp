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

    glEnable(GL_MULTISAMPLE);

    // Run program :)
    TractData td;
    td.parse("whole_brain.tck", true);
//    std::vector<float> vertices = td.data[0].vertices;
    std::vector<float> vertices;
    int count = 0;
    for (int i = 0; i < 2; i++) {
        vertices.insert(vertices.begin(), td.data[i].vertices.begin(), td.data[i].vertices.end());
        count += td.data[i].vertices.size();
        tractsizes.push_back(td.data[i].vertices.size());
        tractfirst.push_back(count - td.data[i].vertices.size());
    }
    tractsize = tractsizes.size();
//    tractsizes = td.sizes;
//    tractfirst = td.first;

    auto path = getPath();

    shader = Shader(path + "basic.vsh",
                    path + "basic.fsh");

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &VCO);

    // 1. bind Vertex Array Object
    glBindVertexArray(VAO);
    // 2. copy our vertices array in a buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, tractsize, (void*) &vertices[0], GL_STATIC_DRAW);
    // 3. then set our vertex attributes pointers
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);
//    // 4. copy our vertices array in a buffer for OpenGL to use
//    glBindBuffer(GL_ARRAY_BUFFER, VCO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
//    // color attribute
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
//    glEnableVertexAttribArray(1);
}

void GLFWWrapper::use() {
    glClearColor(settings.clear_color.x, settings.clear_color.y, settings.clear_color.z, settings.clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);


    shader.use();
    shader.setMat4("uViewMatrix", settings.camera.GetViewMatrix());
    shader.setMat4("uProjectionMatrix", settings.camera.GetProjectionMatrix());

    glBindVertexArray(VAO);
    glMultiDrawArrays(GL_LINE_STRIP, (int*) &tractfirst[0], (int*) &tractsizes[0], tractsize);
}

void GLFWWrapper::cleanup() {
    glfwTerminate();
}
