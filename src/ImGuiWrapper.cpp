#include "ImGuiWrapper.h"
#include "logger.h"
#include "TractDataWrapper.h"
#include "path.h"

void ImGuiWrapper::init() {
    settings.imgui = this;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
//    (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    const char* glsl_version = "#version 420";
    ImGui_ImplOpenGL3_Init(glsl_version);
}

void ImGuiWrapper::draw() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (settings.show_demo_window)
        ImGui::ShowDemoWindow(&settings.show_demo_window);

    {
        static int counter = 0;

        ImGui::Begin(
                "Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text(
                "This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window",
                        &settings.show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &settings.show_another_window);

        for (auto dataset: settings.datasets) {
            ImGui::SliderInt("Tract Count", &dataset->showTractCount, 1, dataset->tractCount);
        }
        ImGui::ColorEdit3("clear color", (float*) &settings.clear_color); // Edit 3 floats representing a color

        if (ImGui::Checkbox("Anti Aliasing", &settings.MSAA)) {
            settings.MSAA ? glEnable(GL_MULTISAMPLE) : glDisable(GL_MULTISAMPLE);
        }
        if (ImGui::Checkbox("V-Sync", &settings.vsync)) {
            glfwSwapInterval((int) settings.vsync);
        }
        if (ImGui::Checkbox("Draw Tubes", &settings.drawTubes)) {
            auto path = getPath();
            if (settings.drawTubes) {
                settings.shader = Shader(path + "tubes.vsh", path + "tubes.fsh", path + "tubes.geom");
            } else {
                settings.shader = Shader(path + "basic.vsh", path + "basic.fsh");
            }
        }

        ImGuiIO& io = ImGui::GetIO();
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}

void ImGuiWrapper::cleanup() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

