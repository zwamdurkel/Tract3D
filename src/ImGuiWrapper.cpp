#include <algorithm>
#include "ImGuiWrapper.h"
#include "logger.h"
#include "TractDataWrapper.h"
#include "path.h"
#include "nativefiledialog/src/include/nfd.hpp"
#include "RenderSettings.h"
#include "GLFWWrapper.h"

void ImGuiWrapper::init() {
    settings.imgui = this;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
//    (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    auto path = getPath();
    io.Fonts->AddFontFromFileTTF((path + "Ruda-Regular.ttf").c_str(), 16);
//    io.Fonts->AddFontFromFileTTF("Ruda-Bold.ttf", 10);
//    io.Fonts->AddFontFromFileTTF("Ruda-Bold.ttf", 14);
//    io.Fonts->AddFontFromFileTTF("Ruda-Bold.ttf", 18);

    ImGuiStyle* style = &ImGui::GetStyle();

    style->WindowPadding = ImVec2(15, 15);
    style->WindowRounding = 5.0f;
    style->FramePadding = ImVec2(5, 5);
    style->FrameRounding = 4.0f;
    style->ItemSpacing = ImVec2(12, 10);
    style->ItemInnerSpacing = ImVec2(8, 6);
    style->IndentSpacing = 25.0f;
    style->ScrollbarSize = 15.0f;
    style->ScrollbarRounding = 9.0f;
    style->GrabMinSize = 5.0f;
    style->GrabRounding = 3.0f;

    style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
    style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    style->Colors[ImGuiCol_ChildBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
    style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
    style->Colors[ImGuiCol_Border] = ImVec4(0.52f, 0.50f, 0.55f, 0.88f);
    style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
    style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
    style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
    style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    style->Colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.18f, 0.21f, 1.00f);
    style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_Header] = ImVec4(0.32f, 0.30f, 0.34f, 1.00f);
    style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
    style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
    style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
    style->Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    const char* glsl_version = "#version 420";
    ImGui_ImplOpenGL3_Init(glsl_version);
}

void ImGuiWrapper::draw() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    // windows settings
    static bool no_move = false;
    static bool no_resize = false;
    ImGuiWindowFlags window_flags = 0;
    //window_flags |= ImGuiWindowFlags_NoCollapse;
    if (no_move) window_flags |= ImGuiWindowFlags_NoMove;
    if (no_resize) window_flags |= ImGuiWindowFlags_NoResize;

    if (settings.show_demo_window)
        ImGui::ShowDemoWindow(&settings.show_demo_window);

    {
        ImGui::Begin("Tract 3D", nullptr, window_flags);

        ImGui::Text("Version 1.0.0");

        if (ImGui::CollapsingHeader("Rendering Options")) {

            ImGui::SeparatorText("General Options");

            ImVec2 avail_size = ImGui::GetContentRegionAvail();
            ImGui::ColorEdit3("Background Color", (float*) &settings.clear_color, ImGuiColorEditFlags_NoInputs);

            if (ImGui::Checkbox("Anti Aliasing", &settings.MSAA)) {
                settings.MSAA ? glEnable(GL_MULTISAMPLE) : glDisable(GL_MULTISAMPLE);
            }
            if (ImGui::Checkbox("V-Sync", &settings.vsync)) {
                glfwSwapInterval((int) settings.vsync);
            }

            if (ImGui::Checkbox("Full Screen", &settings.fullScreen)) {
                settings.glfw->setFullScreen(settings.fullScreen);
            }

            ImGui::SeparatorText("Tract Options");

            if (ImGui::Checkbox("Line Shading", &settings.shadedLines)) {
                if (settings.shadedLines) { settings.shader = settings.lineShadingShader; }
                else { settings.shader = settings.defaultShader; }
            }

            if (ImGui::Checkbox("Draw Tubes", &settings.drawTubes)) {
                for (auto& dataset: settings.datasets) {
                    dataset->init();
                }
                for (auto& dataset: settings.examples) {
                    dataset->init();
                }
            }
            if (settings.drawTubes) {
                if (ImGui::SliderInt("Tube Sides", &settings.nrOfSides, 3, 8)) {
                    for (auto& dataset: settings.datasets) {
                        dataset->init();
                    }
                    for (auto& dataset: settings.examples) {
                        dataset->init();
                    }
                }
            }

            if (ImGui::Checkbox("Enable Highlight", &settings.highlightEnabled)) {
                for (auto& dataset: settings.datasets) {
                    if (!settings.highlightEnabled) {
                        dataset->alpha = settings.generalAlpha;
                    }
                }
                for (auto& dataset: settings.examples) {
                    if (!settings.highlightEnabled) {
                        dataset->alpha = settings.generalAlpha;
                    }
                }
            }
            if (settings.highlightEnabled) {
                if (ImGui::SliderFloat("Highlight Alpha", &settings.highlightAlpha, 0.0f, 1.0f, "%.2f")) {
                    for (auto& data: settings.datasets) {
                        if (data->name != settings.highlightedBundle) {
                            data->alpha = settings.highlightAlpha;
                        }
                    }
                    for (auto& data: settings.examples) {
                        if (data->name != settings.highlightedBundle) {
                            data->alpha = settings.highlightAlpha;
                        }
                    }
                }


                if (ImGui::BeginCombo("Highlighted Bundle", settings.highlightedBundle.c_str())) {
                    for (auto& dataset: settings.datasets) {
                        if (dataset->enabled) {
                            ImGui::PushID(&dataset);
                            if (ImGui::Selectable(dataset->name.c_str(), dataset->name == settings.highlightedBundle)) {
                                settings.highlightedBundle = dataset->name;
                                for (auto& data: settings.datasets) {
                                    if (data->name != settings.highlightedBundle) {
                                        data->alpha = settings.highlightAlpha;
                                    } else {
                                        data->alpha = settings.generalAlpha;
                                    }
                                }
                                for (auto& data: settings.examples) {
                                    if (data->name != settings.highlightedBundle) {
                                        data->alpha = settings.highlightAlpha;
                                    } else {
                                        data->alpha = settings.generalAlpha;
                                    }
                                }
                            }
                            ImGui::PopID();
                        }
                    }
                    for (auto& dataset: settings.examples) {
                        if (dataset->enabled) {
                            ImGui::PushID(&dataset);
                            if (ImGui::Selectable(dataset->name.c_str(), dataset->name == settings.highlightedBundle)) {
                                settings.highlightedBundle = dataset->name;
                                for (auto& data: settings.datasets) {
                                    if (data->name != settings.highlightedBundle) {
                                        data->alpha = settings.highlightAlpha;
                                    } else {
                                        data->alpha = settings.generalAlpha;
                                    }
                                }
                                for (auto& data: settings.examples) {
                                    if (data->name != settings.highlightedBundle) {
                                        data->alpha = settings.highlightAlpha;
                                    } else {
                                        data->alpha = settings.generalAlpha;
                                    }
                                }
                            }
                            ImGui::PopID();
                        }
                    }
                    ImGui::EndCombo();
                }
            }

//            if (ImGui::SliderFloat("Alpha", &settings.generalAlpha, 0.0f, 1.0f, "%.2f")) {
//                for (auto& dataset: settings.datasets) {
//                    dataset->alpha = settings.generalAlpha;
//                }
//                for (auto& dataset: settings.examples) {
//                    dataset->alpha = settings.generalAlpha;
//                }
//            }

            ImGui::SeparatorText("Tract Count");

            for (auto& dataset: settings.datasets) {
                if (dataset->enabled) {
                    std::string name = dataset->name;
                    ImGui::SliderInt((name + " ").c_str(), &dataset->showTractCount, 1, dataset->tractCount);
                }
            }

            if (ImGui::TreeNode("Examples Tract Count")) {
                for (auto& dataset: settings.examples) {
                    if (dataset->enabled) {
                        std::string name = dataset->name;
                        ImGui::SliderInt((name + " ").c_str(), &dataset->showTractCount, 1, dataset->tractCount);
                    }
                }

                ImGui::TreePop();
            }
        }

        if (ImGui::CollapsingHeader("Dataset options")) {
            static std::string message = "Select Dataset";
            if (ImGui::Button("Browse")) {
                NFD::Init();
                char* outPath;
                nfdfilteritem_t filterItem[1] = {{"MRI files", "tck"}};
                nfdresult_t result = NFD::OpenDialog(outPath, filterItem, 1, nullptr);
                if (result == NFD_OKAY) {
                    Info("Selected file: " << outPath);
                    NFD::FreePath(outPath);
                    auto name = std::string(outPath);
                    name.erase(0, name.find_last_of('\\') + 1);
                    bool duplicate = false;
                    for (auto& dataset: settings.datasets) {
                        if (dataset->name == name) {
                            duplicate = true;
                            message = "Duplicate Rejected";
                        }
                    }
                    for (auto& dataset: settings.examples) {
                        if (dataset->name == name) {
                            duplicate = true;
                            message = "Example Duplicate";
                        }
                    }
                    if (!duplicate) {
                        auto td = std::make_unique<TractDataWrapper>(name, outPath);
                        settings.datasets.push_back(std::move(td));
                        message = "Successfully Added";
                    }
                } else if (result == NFD_CANCEL) {
                    Info("User pressed cancel.");
                    message = "User Cancelled";
                } else {
                    Info("Error: %s\n" << NFD::GetError());
                    message = "Select Dataset";
                }
                NFD::Quit();

            }
            ImGui::SameLine();
            ImGui::Text(message.c_str());

            ImGui::SeparatorText("Datasets");

            for (auto it = settings.datasets.begin(); it != settings.datasets.end();) {
                ImGui::Checkbox((*it)->name.c_str(), &(*it)->enabled);
                ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 60);
                if (ImGui::Button("Remove")) {
                    it = settings.datasets.erase(it);
                } else {
                    ++it;
                }
            }

            if (ImGui::TreeNode("Examples")) {
                if (ImGui::Button("Enable All")) {
                    for (auto& dataset: settings.examples) {
                        dataset->enabled = true;
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button("Disable All")) {
                    for (auto& dataset: settings.examples) {
                        dataset->enabled = false;
                    }
                }

                for (auto& dataset: settings.examples) {
                    ImGui::Checkbox(dataset->name.c_str(), &dataset->enabled);
                }
                ImGui::TreePop();
            }
        }

        if (ImGui::CollapsingHeader("Camera Options")) {
            ImGui::SliderFloat("FOV", &settings.camera.FOV, 30.0f, 150.0f, "%.0f");
            ImGui::SliderFloat("Speed", &settings.camera.MovementSpeed, 1.0f, 100.0f, "%.0f");
            ImGui::SliderFloat("Draw Distance", &settings.camera.FarPlane, 1.0f, 500.0f, "%.0f");
        }

        if (ImGui::CollapsingHeader("Development Options")) {
            ImGui::TableNextColumn();
            ImGui::Checkbox("No Move", &no_move);
            ImGui::TableNextColumn();
            ImGui::Checkbox("No Resize", &no_resize);
//            ImGui::Checkbox("Demo Window",
//                            &settings.show_demo_window);      // Edit bools storing our window open/close state
        }

        ImGuiIO& io = ImGui::GetIO();
        ImGui::Text("Application Framerate %.1f FPS", io.Framerate);
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

